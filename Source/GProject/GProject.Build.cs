// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GProject : ModuleRules
{
	public GProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", /*"HeadMountedDisplay",*/ 
			"NavigationSystem", "AIModule", "GameplayTags"/*, "GameplayTasks", "OnlineSubsystem", "UMG"*/ });
    }
}
