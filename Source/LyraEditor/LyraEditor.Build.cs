using UnrealBuildTool;

public class LyraEditor: ModuleRules
{
    public LyraEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "LyraEditor"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core", 
                "CoreUObject", 
                "Engine"
            }
        );
    }
}
