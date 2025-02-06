// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperMindCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperMindModule"

void FMinesweeperMindCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MinesweeperMind", "Launch MinesweeperMind Window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
