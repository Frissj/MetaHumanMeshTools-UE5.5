// File: FaceMorphTargetCopy_Editor.h
#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditorLibrary.h"
#include "FaceMorphTargetCopy_Editor.generated.h"

/**
 * This class contains blueprint callable utility functions to copy morph target data
 * from one MetaHuman face mesh to another.
 * (Editor-only functionality.)
 */
UCLASS()
class METAHUMANMESHTOOLSEDITOR_API UFaceMorphTargetCopy_Editor : public UBlueprintEditorLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Mesh Tools")
    static void CopyFaceMorphTargets(USkeletalMesh* sourceMesh, USkeletalMesh* targetMesh);

    UFUNCTION(BlueprintCallable, Category = "MetaHuman Mesh Tools")
    static void CopyFaceMorphTargetsUsingReference(USkeletalMesh* originalSourceMesh, USkeletalMesh* vertexPositionReferenceMesh, USkeletalMesh* targetMesh);
};
