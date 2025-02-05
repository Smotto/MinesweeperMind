// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperMindCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperMindModule"

void FMinesweeperMindCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MinesweeperMind", "Execute MinesweeperMind action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
