using UnrealBuildTool;

public class MetaHumanMeshToolsEditor : ModuleRules
{
    public MetaHumanMeshToolsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "MeshDescription",
            "StaticMeshDescription"
        });

        // Only add these dependencies if we are building for the editor.
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",               // Editor functionality.
                "BlueprintEditorLibrary", // For UBlueprintEditorLibrary.
                "Slate",
                "SlateCore",
                "EditorFramework",
                "DesktopPlatform"
            });
        }
    }
}
