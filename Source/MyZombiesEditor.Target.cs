// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyZombiesEditorTarget : TargetRules
{
	public MyZombiesEditorTarget(TargetInfo Target) : base(Target)
	{
       	Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest; // Updated to Latest or Unreal5_2
        ExtraModuleNames.Add("MyZombies");

	}
}
