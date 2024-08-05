// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BodyCamShooter : ModuleRules
{
	public BodyCamShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
