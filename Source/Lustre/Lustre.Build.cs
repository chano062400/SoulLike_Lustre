// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lustre : ModuleRules
{
	public Lustre(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" , "GeometryCollectionEngine", "UMG", "AIModule", "Niagara", "Paper2D","Slate","SlateCore", "AnimGraphRuntime" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
