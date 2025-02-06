// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperMind.h"
#include "MinesweeperMindStyle.h"
#include "MinesweeperMindCommands.h"
#include "SMinesweeperMindWindow.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName MinesweeperMindTabName("MinesweeperMind");

#define LOCTEXT_NAMESPACE "FMinesweeperMindModule"

void FMinesweeperMindModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
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