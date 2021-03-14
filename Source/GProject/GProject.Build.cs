// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GProject : ModuleRules
{
	public GProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivatePCHHeaderFile = "Public/GProject.h";

		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			/*"HeadMountedDisplay",*/ 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTags", 
			"GameplayTasks", 
			"GameplayAbilities",
			"SlateCore"
			/*,"OnlineSubsystem",
			 * "UMG"*/ 
		});
    }
}
