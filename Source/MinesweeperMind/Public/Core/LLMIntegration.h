#pragma once

#include "CoreMinimal.h"

class MINESWEEPERMIND_API LLMIntegration
{
public:
 // Calls the Python script and returns a JSON string
 static FString GetMinesweeperDimensions();
};
