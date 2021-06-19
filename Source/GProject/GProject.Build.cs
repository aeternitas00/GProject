// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GProject : ModuleRules
{
	public GProject(ReadOnlyTargetRules Target) : base(Target)
	{
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivatePCHHeaderFile = "Public/GProject.h";

		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTags", 
			"GameplayTasks", 
			"GameplayAbilities",
			"SlateCore",
			"Niagara",
            "Http",
            "Json",
            "JsonUtilities"
        });
		//PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities" });
    }
}
