// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class DSUDatabase : ModuleRules
{
	public string ProjectDirectory
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../.."));
		}
	}

	private void CopyToProjectBinaries(string FilePathName, ReadOnlyTargetRules Target)
	{
		string BineriesDirectory = Path.Combine(ProjectDirectory, "Binaries", Target.Platform.ToString());
		string Filename = Path.GetFileName(FilePathName);

		if (!Directory.Exists(BineriesDirectory))
		{
			Directory.CreateDirectory(BineriesDirectory);
		}

		string ProjectFileFullName = Path.Combine(BineriesDirectory, Filename);
		if (!File.Exists(ProjectFileFullName))
		{
			File.Copy(FilePathName, ProjectFileFullName, true);
		}

		RuntimeDependencies.Add(ProjectFileFullName);
	}

	public DSUDatabase(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableUndefinedIdentifierWarnings = false;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Win64)
		{
			string PluginsDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "../.."));
			string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
			string LibMySQLDirectory = Path.Combine(PluginsDirectory, "ExtraLib", "MySQL_8_12", PlatformString);

			PublicIncludePaths.Add(LibMySQLDirectory);

			RuntimeDependencies.Add(Path.Combine(LibMySQLDirectory, "libmysql.dll"));
			PublicAdditionalLibraries.Add(Path.Combine(LibMySQLDirectory, "libmysql.lib"));

			CopyToProjectBinaries(Path.Combine(LibMySQLDirectory, "libmysql.dll"), Target);
			CopyToProjectBinaries(Path.Combine(LibMySQLDirectory, "libmysql.lib"), Target);
		}
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
			
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Sockets",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		// 如果是在项目里面想用到该内容，就把它打开
		// PublicDefinitions.Add("PLATFORM_PROJECT");
		if (PublicDefinitions.Contains("PLATFORM_PROJECT"))
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Engine"
				}
				);
		}
	}
}
