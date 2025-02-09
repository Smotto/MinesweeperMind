// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperMind.h"

#include "IPythonScriptPlugin.h"
#include "MinesweeperMindStyle.h"
#include "MinesweeperMindCommands.h"
#include "SMinesweeperMindWindow.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Interfaces/IPluginManager.h"

static const FName MinesweeperMindTabName("MinesweeperMind");

#define LOCTEXT_NAMESPACE "FMinesweeperMindModule"
void FMinesweeperMindModule::ExecutePythonScriptFromFile(const FString& ScriptFileRelativePath)
{
    TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("MinesweeperMind"));
    if (!Plugin.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Plugin 'MinesweeperMind' not found."));
        return;
    }
    
    FString PluginRoot = Plugin->GetBaseDir();
    FString FullScriptPath = FPaths::Combine(*PluginRoot, *ScriptFileRelativePath);
    
    if (!FPaths::FileExists(FullScriptPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Python script file not found: %s"), *FullScriptPath);
        return;
    }
    
    FString PythonScriptContent;
    if (!FFileHelper::LoadFileToString(PythonScriptContent, *FullScriptPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Python script file: %s"), *FullScriptPath);
        return;
    }
    
    if (!FModuleManager::Get().IsModuleLoaded("PythonScriptPlugin"))
    {
        UE_LOG(LogTemp, Error, TEXT("PythonScriptPlugin module is not loaded."));
        return;
    }
    
    IPythonScriptPlugin* PythonScriptPlugin = FModuleManager::GetModulePtr<IPythonScriptPlugin>("PythonScriptPlugin");
    if (!PythonScriptPlugin)
    {
        UE_LOG(LogTemp, Error, TEXT("PythonScriptPlugin pointer is null."));
        return;
    }

    auto ExecutePython = [PythonScriptContent]()
    {
        IPythonScriptPlugin* PythonScriptPluginInner = FModuleManager::Get().GetModulePtr<IPythonScriptPlugin>("PythonScriptPlugin");
        if (PythonScriptPluginInner)
        {
            UE_LOG(LogTemp, Log, TEXT("Executing Python script..."));
            PythonScriptPluginInner->ExecPythonCommand(*PythonScriptContent);
            UE_LOG(LogTemp, Log, TEXT("Executed Python agent script successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PythonScriptPlugin pointer is invalid during execution."));
        }
    };

	PythonScriptPlugin->OnPythonInitialized().AddLambda([ExecutePython]()
	{
		ExecutePython();
	});
}

void FMinesweeperMindModule::StartupModule()
{
    // Load the LLM on startup
    ExecutePythonScriptFromFile(TEXT("Content/Scripts/minesweeper_agent.py"));

    FMinesweeperMindStyle::Initialize();
    FMinesweeperMindStyle::ReloadTextures();
    FMinesweeperMindCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FMinesweeperMindCommands::Get().PluginAction,
        FExecuteAction::CreateRaw(this, &FMinesweeperMindModule::PluginButtonClicked),
        FCanExecuteAction());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperMindModule::RegisterMenus));
}

void FMinesweeperMindModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperMindStyle::Shutdown();

	FMinesweeperMindCommands::Unregister();
}

void FMinesweeperMindModule::PluginButtonClicked()
{
    if (MinesweeperWindow.IsValid())
    {
    	TSharedPtr<SWindow> ExistingWindow = MinesweeperWindow.Pin();
    	if (ExistingWindow.IsValid())
    	{
    		if (ExistingWindow->IsWindowMinimized())
    		{
    			ExistingWindow->Restore();
    		}

    		FSlateApplication::Get().SetUserFocus(FSlateApplication::Get().GetUserIndexForKeyboard(), ExistingWindow, EFocusCause::SetDirectly);
			ExistingWindow->BringToFront();
    	}
        return;
    }

	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(FText::FromString(TEXT("Minesweeper Mind")))
		.ClientSize(FVector2D(800, 600))
		.MinWidth(800)
		.MinHeight(600)
		.SizingRule(ESizingRule::UserSized);

	Window->SetContent(SNew(SMinesweeperMindWindow));

	MinesweeperWindow = Window;

	FSlateApplication::Get().AddWindow(Window);
}


void FMinesweeperMindModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperMindCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperMindCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	 
IMPLEMENT_MODULE(FMinesweeperMindModule, MinesweeperMind)