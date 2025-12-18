// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CenterServer : ModuleRules
{
	public CenterServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.Add("Runtime/Launch/Public");

		PrivateIncludePaths.Add("Runtime/Launch/Private");		// For LaunchEngineLoop.cpp include

		PrivateDependencyModuleNames.Add("Core");
		PrivateDependencyModuleNames.Add("CoreUObject");
		PrivateDependencyModuleNames.Add("ApplicationCore");
		PrivateDependencyModuleNames.Add("Projects");

		PrivateDependencyModuleNames.Add("DS_NetChannel");
	}
}
