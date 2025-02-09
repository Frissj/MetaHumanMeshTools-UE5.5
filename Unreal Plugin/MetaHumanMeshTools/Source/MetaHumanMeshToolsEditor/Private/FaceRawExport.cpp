// Disable the new include order if needed.
#define UE_ENABLE_INCLUDE_ORDER_DEPRECATED_IN_5_2 0

#include "FaceRawExport.h"

#include "Engine/SkeletalMesh.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"  // For attribute names (e.g. MeshAttribute::Vertex::Position)
#include "EditorFramework/AssetImportData.h"  // (Legacy; not used here)

#include <Misc/FileHelper.h>
#include <Misc/Base64.h>
#include <DesktopPlatformModule.h>
#include <EditorDirectories.h>

void UFaceRawExport::FaceRawExport(USkeletalMesh* mesh, FString path)
{
    if (!mesh || path.IsEmpty())
    {
        return;
    }

    // Get the MeshDescription for LOD 0.
    const FMeshDescription* MeshDesc = mesh->GetMeshDescription(0);
    if (!MeshDesc)
    {
        return;
    }

    // Retrieve vertex positions.
    TVertexAttributesConstRef<FVector3f> VertexPositions =
        MeshDesc->VertexAttributes().GetAttributesRef<FVector3f>(MeshAttribute::Vertex::Position);

    TArray<FVector3f> Vertices;
    for (const FVertexID VertexID : MeshDesc->Vertices().GetElementIDs())
    {
        Vertices.Add(VertexPositions[VertexID]);
    }

    // Retrieve triangle indices.
    TArray<uint32> Indices;
    // Iterate over all polygons.
    for (const FPolygonID PolygonID : MeshDesc->Polygons().GetElementIDs())
    {
        // Get the triangles (FTriangleID array) in this polygon.
        TArrayView<const FTriangleID> TriangleIDs = MeshDesc->GetPolygonTriangles(PolygonID);
        for (const FTriangleID TriangleID : TriangleIDs)
        {
            // Get the vertex instance IDs for this triangle.
            TArrayView<const FVertexInstanceID> VertexInstanceIDs = MeshDesc->GetTriangleVertexInstances(TriangleID);
            for (const FVertexInstanceID& VertexInstanceID : VertexInstanceIDs)
            {
                // Map the vertex instance to its corresponding vertex.
                FVertexID VertexID = MeshDesc->GetVertexInstanceVertex(VertexInstanceID);
                Indices.Add(VertexID.GetValue());
            }
        }
    }

    // Create JSON output containing vertex and index data.
    FString output;
    output += "{\n";
    output += "\t\"num_vertices\": " + FString::Printf(TEXT("%i"), Vertices.Num()) + ",\n";
    output += "\t\"vertices\": \"" + getBase64EncodedData<FVector3f>(Vertices) + "\",\n";
    output += "\t\"num_faces\": " + FString::Printf(TEXT("%i"), Indices.Num() / 3) + ",\n";
    output += "\t\"face_indices\": \"" + getBase64EncodedData<uint32>(Indices) + "\"\n";
    output += "}\n";

    FFileHelper::SaveStringToFile(FStringView(output), *path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

FString UFaceRawExport::SaveFileDialog(const FString& meshFileName)
{
    TArray<FString> fileNames;
    const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    FString LastUsedPath = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::FBX);
    if (FDesktopPlatformModule::Get()->SaveFileDialog(ParentWindowHandle,
        "Save MetaHuman Raw Face Mesh: " + meshFileName,
        LastUsedPath,
        FPaths::GetBaseFilename(meshFileName),
        "Raw Face Mesh|*.json",
        EFileDialogFlags::None,
        fileNames))
    {
        if (fileNames.Num() == 1)
        {
            FEditorDirectories::Get().SetLastDirectory(ELastDirectory::FBX, FPaths::GetPath(fileNames[0]));
            return fileNames[0];
        }
    }
    return "";
}

template <class T>
FString UFaceRawExport::getBase64EncodedData(const TArray<T>& data)
{
    TArray<uint8> rawData;
    rawData.SetNumUninitialized(data.Num() * sizeof(T));
    FMemory::Memcpy(rawData.GetData(), data.GetData(), data.Num() * sizeof(T));
    return FBase64::Encode(rawData);
}

FString UFaceRawExport::getBase64EncodedDataColor(const TArray<FColor>& data)
{
    TArray<uint8> rawData;
    rawData.Reserve(data.Num() * 4);
    for (const FColor& color : data)
    {
        rawData.Add(color.R);
        rawData.Add(color.G);
        rawData.Add(color.B);
        rawData.Add(color.A);
    }
    return FBase64::Encode(rawData);
}

// Explicit template instantiation
template FString UFaceRawExport::getBase64EncodedData<FVector3f>(const TArray<FVector3f>& data);
template FString UFaceRawExport::getBase64EncodedData<uint32>(const TArray<uint32>& data);
