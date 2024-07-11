// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectNo1 : ModuleRules
{
	public ProjectNo1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "GeometryCollectionEngine", "Niagara" , "NavigationSystem" ,"AIModule", "Slate", "SlateCore" });

    }
}
