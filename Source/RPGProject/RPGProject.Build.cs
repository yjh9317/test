// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGProject : ModuleRules
{
	public RPGProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"UMG", "AIModule", "Niagara", "NavigationSystem","NetCore",
			"GameplayTasks", "GameplayTags", "MotionWarping", "CableComponent",
			"MovieScene","LevelSequence", "MovieSceneTracks",
			"TemplateSequence", "AnimGraphRuntime", "AnimationModifiers", "GameplayCameras"
		});
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate","SlateCore","AnimationLocomotionLibraryRuntime", // "HeadMountedDisplay",
			"MovieSceneTools","SequencerScripting"
		});
		
		if (Target.Type == TargetType.Editor) 
		{
			PrivateDependencyModuleNames.AddRange(new string[] {
				"UnrealEd",
				"ToolMenus",
				"AssetTools"
			});
		}
		PrivateIncludePaths.Add("RPGProject");
	}
}
