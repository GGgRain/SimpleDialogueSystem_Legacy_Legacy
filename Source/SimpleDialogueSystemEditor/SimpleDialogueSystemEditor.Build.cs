// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SimpleDialogueSystemEditor : ModuleRules
{
	public SimpleDialogueSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {
			"SimpleDialogueSystemEditor",
			"SimpleDialogueSystemEditor/Private",
			"SimpleDialogueSystemEditor/Private/Factory",
			"SimpleDialogueSystemEditor/Private/AssetTool",
			"SimpleDialogueSystemEditor/Private/Style"
		});


		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"UnrealEd", 
			"SimpleDialogueSystem" ,
			"EditorStyle",                 
			"ContentBrowser",	
			"DesktopWidgets",		
			"InputCore",		
			"Projects",			
			"Slate",
			"SlateCore",
			"EditorWidgets",	
			"KismetWidgets",
		});

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
		});
	}
}
