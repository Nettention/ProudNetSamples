// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SimpleUE4 : ModuleRules
{
	public SimpleUE4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Preprocessor를 여기에 하기와 같이 선언하셔야 됩니다.
            PublicDefinitions.Add("UNICODE");
            PublicDefinitions.Add("_UNICODE");

            // 헤더파일 Include 경로를 여기에 하기와 같이 설정합니다.
            PublicIncludePaths.Add(@"E:\engine\dev003_3478\R2\ProudNet\include");

            // 라이브러리 참조 경로를 하기와 같이 설정합니다.
            PublicAdditionalLibraries.Add(@"E:\engine\dev003_3478\R2\ProudNet\lib\x64\v140\Release\ProudNetClient.lib");
            PublicAdditionalLibraries.Add(@"E:\engine\dev003_3478\R2\ProudNet\Sample\bin\Release\SimpleCommon.lib");
        }
    }
}
