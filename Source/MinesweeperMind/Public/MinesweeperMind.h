#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FMinesweeperMindModule : public IModuleInterface
{
public:
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
