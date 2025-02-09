#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class MINESWEEPERMIND_API FMinesweeperMindModule : public IModuleInterface
{
public:
	static void ExecutePythonScriptFromFile(const FString& ScriptFileRelativePath);
	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:
	TWeakPtr<SWindow> MinesweeperWindow;
	
	void RegisterMenus();
	
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
