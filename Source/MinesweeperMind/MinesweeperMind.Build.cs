// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MinesweeperMind : ModuleRules
{
	public MinesweeperMind(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"MinesweeperMind/Public/Core"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "MinesweeperMind/Private/Core",
                "MinesweeperMind/Private/UI",
                "MinesweeperMind/Private/Widgets"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"EditorFramework",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"Projects",
				"PythonScriptPlugin",
				"ToolMenus",
				"UnrealEd"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
