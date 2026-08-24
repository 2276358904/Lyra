// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lyra : ModuleRules
{
	public Lyra(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
				"Lyra"
			}
		);

        PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject",
                "CommonUI",
                "Engine",
                "EnhancedInput",
                "GameplayTags",
                "GameplayAbilities",
                "GameplayTasks",
                "HeadMountedDisplay",
                "InputCore", 
                "ModularGameplay",
            }
		);
	}
}
