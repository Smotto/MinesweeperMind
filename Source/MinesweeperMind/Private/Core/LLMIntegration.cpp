#include "Core/LLMIntegration.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FString LLMIntegration::GetMinesweeperDimensions()
{
    FString PythonScriptPath = FPaths::ProjectPluginsDir() / TEXT("Content/Scripts/game_integration.py");
    FString Command = FString::Printf(TEXT("python \"%s\""), *PythonScriptPath);
    
    FString Output, Errors;
    int32 ReturnCode;
    FPlatformProcess::ExecProcess(*Command, TEXT(""), &ReturnCode, &Output, &Errors);

    if (ReturnCode != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Python execution failed: %s"), *Errors);
        return TEXT("");
    }

    return Output;
}
