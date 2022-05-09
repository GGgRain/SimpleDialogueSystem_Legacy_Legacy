// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#undef UCONFIG_NO_TRANSLITERATION

using UnrealBuildTool;

public class SimpleDialogueSystem : ModuleRules
{
	public SimpleDialogueSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;




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
				"ICU",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"ICU",
				"FlowControlSystem",
				"SaveExtension",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.bCompileICU == true)
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target, "ICU");
		}

		//PublicDefinitions.Remove("UE_ENABLE_ICU");
		//PublicDefinitions.Add("UE_ENABLE_ICU=" + (Target.bCompileICU ? "1" : "0")); // Enable/disable (=1/=0) ICU usage in the codebase. NOTE: This flag is for use while integrating ICU and will be removed afterward.

		//PublicDefinitions.Remove("UCONFIG_NO_TRANSLITERATION");
		//PublicDefinitions.Add("UCONFIG_NO_TRANSLITERATION=" + (Target.bCompileICU ? "0" : "1")); // Enable/disable (=0 /=1) ICU usage in the codebase. NOTE: This flag is for use while integrating ICU and will be removed afterward.

	}
}
