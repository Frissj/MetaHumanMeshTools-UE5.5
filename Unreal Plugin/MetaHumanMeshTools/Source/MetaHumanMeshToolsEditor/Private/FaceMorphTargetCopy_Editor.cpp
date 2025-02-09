// File: FaceMorphTargetCopy_Editor.cpp
#include "FaceMorphTargetCopy_Editor.h"
#include <Rendering/SkeletalMeshModel.h>

void UFaceMorphTargetCopy_Editor::CopyFaceMorphTargets(USkeletalMesh* sourceMesh, USkeletalMesh* targetMesh)
{
    auto sourceModel = sourceMesh->GetImportedModel();
    auto targetModel = targetMesh->GetImportedModel();

    if (sourceModel->LODModels.Num() < 1 || targetModel->LODModels.Num() < 1)
    {
        UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargets: Missing LOD data in supplied meshes!"));
        return;
    }
    if (sourceModel->LODModels[0].NumVertices != targetModel->LODModels[0].NumVertices)
    {
        UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargets: Target and reference meshes have mismatching vertex counts!"));
        return;
    }

    auto& sourceMorphTargets = sourceMesh->GetMorphTargets();
    TArray<UMorphTarget*> newMorphTargets;
    for (auto morphTarget : sourceMorphTargets)
    {
        if (!morphTarget || morphTarget->GetMorphLODModels().Num() == 0)
            continue;

        UMorphTarget* newMorphTarget = NewObject<UMorphTarget>(targetMesh, morphTarget->GetFName());
        newMorphTarget->BaseSkelMesh = targetMesh;
        newMorphTarget->GetMorphLODModels().AddDefaulted(1);
        auto& deltas = morphTarget->GetMorphLODModels()[0].Vertices;
        auto& sections = targetModel->LODModels[0].Sections;
        newMorphTarget->PopulateDeltas(deltas, 0, sections, true, false, 0.0f);
        newMorphTargets.Add(newMorphTarget);
    }

    targetMesh->Modify();
    targetMesh->SetMorphTargets(newMorphTargets);
    targetMesh->InitMorphTargetsAndRebuildRenderData();
}

void UFaceMorphTargetCopy_Editor::CopyFaceMorphTargetsUsingReference(USkeletalMesh* originalSourceMesh, USkeletalMesh* vertexPositionReferenceMesh, USkeletalMesh* targetMesh)
{
    auto sourceModel = originalSourceMesh->GetImportedModel();
    auto referenceModel = vertexPositionReferenceMesh->GetImportedModel();
    auto targetModel = targetMesh->GetImportedModel();

    if (sourceModel->LODModels.Num() < 1 || referenceModel->LODModels.Num() < 1 || targetModel->LODModels.Num() < 1)
    {
        UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargetsUsingReference: Missing LOD data in supplied meshes!"));
        return;
    }
    if (referenceModel->LODModels[0].NumVertices != targetModel->LODModels[0].NumVertices)
    {
        UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargetsUsingReference: Target and reference meshes have mismatching vertex counts!"));
        return;
    }
    if (sourceModel->LODModels[0].Sections.Num() != referenceModel->LODModels[0].Sections.Num())
    {
        UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargetsUsingReference: Source and reference meshes have mismatching section counts!"));
        return;
    }

    int numSections = sourceModel->LODModels[0].Sections.Num();

    TArray<FSoftSkinVertex> sourceVertices;
    TArray<FSoftSkinVertex> referenceVertices;
    sourceModel->LODModels[0].GetVertices(sourceVertices);
    referenceModel->LODModels[0].GetVertices(referenceVertices);

    TMap<uint32, uint32> targetToSourceVertexMap;
    TMultiMap<uint32, uint32> sourceToTargetVertexMap;

    for (int s = 0; s < numSections; ++s)
    {
        auto& sourceSection = sourceModel->LODModels[0].Sections[s];
        auto& referenceSection = referenceModel->LODModels[0].Sections[s];

        for (uint32 rvi = 0; rvi < (uint32)referenceSection.NumVertices; ++rvi)
        {
            float minDistance = 0.01f;
            TArray<uint32> sviMatches;
            bool foundMatch = false;
            for (uint32 svi = 0; svi < (uint32)sourceSection.NumVertices; ++svi)
            {
                float distance = FVector3f::Distance(sourceSection.SoftVertices[svi].Position, referenceSection.SoftVertices[rvi].Position);
                distance += FVector3f::Distance(sourceSection.SoftVertices[svi].TangentZ, referenceSection.SoftVertices[rvi].TangentZ);
                if (distance == minDistance)
                {
                    sviMatches.Add(svi);
                }
                else if (distance < minDistance)
                {
                    sviMatches.Empty();
                    sviMatches.Add(svi);
                    foundMatch = true;
                    minDistance = distance;
                }
            }
            if (!foundMatch)
                continue;
            targetToSourceVertexMap.Add(rvi + sourceSection.BaseVertexIndex, sviMatches[0] + sourceSection.BaseVertexIndex);
            for (uint32 svi : sviMatches)
            {
                sourceToTargetVertexMap.AddUnique(svi + sourceSection.BaseVertexIndex, rvi + sourceSection.BaseVertexIndex);
            }
        }
    }

    TArray<UMorphTarget*> newMorphTargets;
    for (auto morphTarget : originalSourceMesh->GetMorphTargets())
    {
        if (!morphTarget || morphTarget->GetMorphLODModels().Num() == 0)
            continue;

        TSet<uint32> affectedTargetVertices;
        auto& morphLodModel = morphTarget->GetMorphLODModels()[0];
        for (int32 i = 0; i < morphLodModel.NumVertices; ++i)
        {
            if (!sourceToTargetVertexMap.Contains(morphLodModel.Vertices[i].SourceIdx))
            {
                UE_LOG(LogScript, Warning, TEXT("CopyFaceMorphTargetsUsingReference: Unable to find matching vertex in morph target."));
                return;
            }
            affectedTargetVertices.Add(sourceToTargetVertexMap.FindRef(morphLodModel.Vertices[i].SourceIdx));
        }

        TArray<FMorphTargetDelta> deltas;
        for (uint32 v : affectedTargetVertices)
        {
            uint32 vSource = targetToSourceVertexMap.FindRef(v);
            FMorphTargetDelta delta;
            delta.SourceIdx = v;
            for (FMorphTargetDelta& sourceDelta : morphTarget->GetMorphLODModels()[0].Vertices)
            {
                if (sourceDelta.SourceIdx != vSource)
                    continue;
                delta.PositionDelta = sourceDelta.PositionDelta;
                delta.TangentZDelta = sourceDelta.TangentZDelta;
                break;
            }
            deltas.Add(delta);
        }

        UMorphTarget* newMorphTarget = NewObject<UMorphTarget>(targetMesh, morphTarget->GetFName());
        newMorphTarget->BaseSkelMesh = targetMesh;
        newMorphTarget->GetMorphLODModels().AddDefaulted(1);
        auto& sections = targetModel->LODModels[0].Sections;
        newMorphTarget->PopulateDeltas(deltas, 0, sections, true, false, 0.0f);
        newMorphTargets.Add(newMorphTarget);
    }

    targetMesh->Modify();
    targetMesh->SetMorphTargets(newMorphTargets);
    targetMesh->InitMorphTargetsAndRebuildRenderData();
}
