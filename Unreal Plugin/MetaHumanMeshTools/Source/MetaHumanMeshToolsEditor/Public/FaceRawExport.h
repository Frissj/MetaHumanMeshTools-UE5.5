// File: FaceRawExport.h
#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "BlueprintEditorLibrary.h"
#include "IDesktopPlatform.h"
#include "FaceRawExport.generated.h"

/**
 * This class contains blueprint callable functions to export the raw mesh data of a MetaHuman face as JSON.
 * The raw mesh data is the mesh as it was imported from FBX, before it is optimized by Unreal.
 * Morph targets are not included in the raw export.
 */
UCLASS()
class METAHUMANMESHTOOLSEDITOR_API UFaceRawExport : public UBlueprintEditorLibrary
{
    GENERATED_BODY()

public:
    /**
     * Exports the raw mesh data of the given SkeletalMesh to a JSON file at the specified path.
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Mesh Tools")
    static void FaceRawExport(USkeletalMesh* mesh, FString path);

    /**
     * Opens a save file dialog and returns the chosen file path.
     */
    UFUNCTION(BlueprintCallable, Category = "MetaHuman Mesh Tools")
    static FString SaveFileDialog(const FString& meshFileName);

    /**
     * Utility function: copies the bytes of the given data array into a raw buffer and returns a Base64 string.
     */
    template <class T>
    static FString getBase64EncodedData(const TArray<T>& data);

    /**
     * Utility function: encodes an array of FColor into a Base64 string (converting from BGRA to RGBA).
     */
    static FString getBase64EncodedDataColor(const TArray<FColor>& data);
};
#else
// For non-editor builds, define empty stubs.
class METAHUMANMESHTOOLSEDITOR_API UFaceRawExport
{
public:
    static void FaceRawExport(USkeletalMesh* /*mesh*/, FString /*path*/) {}
    static FString SaveFileDialog(const FString& /*meshFileName*/) { return FString(); }

    template <class T>
    static FString getBase64EncodedData(const TArray<T>& /*data*/) { return FString(); }

    static FString getBase64EncodedDataColor(const TArray<FColor>& /*data*/) { return FString(); }
};
#endif
