using UnrealBuildTool;

public class MetaHumanMeshTools : ModuleRules
{
    public MetaHumanMeshTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
        {
            // Add public include paths here if needed.
        });

        PrivateIncludePaths.AddRange(new string[]
        {
            // Add private include paths here if needed.
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            // Other runtime dependencies, if any.
        });

        // Runtime modules – only include modules that are safe for runtime.
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "MeshDescription",
            "StaticMeshDescription"
            // Removed "DesktopPlatform", "EditorFramework", "Slate", "SlateCore"
        });

        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
            // Add any dynamically loaded modules here.
        });
    }
}
