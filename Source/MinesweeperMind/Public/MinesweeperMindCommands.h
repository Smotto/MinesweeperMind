#pragma once

#include "Framework/Commands/Commands.h"
#include "MinesweeperMindStyle.h"

class MINESWEEPERMIND_API FMinesweeperMindCommands : public TCommands<FMinesweeperMindCommands>
{
public:
	FMinesweeperMindCommands()
		: TCommands<FMinesweeperMindCommands>(TEXT("MinesweeperMind"), NSLOCTEXT("Contexts", "MinesweeperMind", "MinesweeperMind Plugin"), NAME_None, FMinesweeperMindStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
