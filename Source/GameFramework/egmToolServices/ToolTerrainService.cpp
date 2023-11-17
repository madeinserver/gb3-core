// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#include "egmToolServicesPCH.h"

#include <efd/ServiceManager.h>

#include <egf/egfLogIDs.h>

#include <NiTerrainStreamLocks.h>
#include <NiTerrainXMLHelpers.h>
#include <NiTerrainSDM.h>
#include <egmTerrain/TerrainService.h>
#include "TerrainPaintingGuide.h"
#include "ToolTerrainService.h"
#include "EntityGizmoPolicy.h"

#include "TerrainPhysXSaveDataPolicy.h"
#include "NiTerrainSurfacePackageFile.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;
using namespace egmTerrain;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolTerrainService);

//------------------------------------------------------------------------------------------------
ToolTerrainService::ToolTerrainService() :
    k_numInnerSegments(32),
    k_numOuterSegments(64),
    k_numBrushVertices(k_numInnerSegments + k_numOuterSegments + 2),
    m_callbackPackageLoadedCallback(NULL),
    m_callbackCheckAssetMigration(NULL),
    m_renderLowDetailTextures(false),
    m_invalidateRenderContexts(false)
{
    // Override the handling of asset changes. We don't want to handle them in tool mode
    m_handleAssetChanges = false;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::OnServiceRegistered(efd::IAliasRegistrar* pAliasRegistrar)
{
    pAliasRegistrar->AddIdentity<egmTerrain::TerrainService>();
    egmTerrain::TerrainService::OnServiceRegistered(pAliasRegistrar);
}

//------------------------------------------------------------------------------------------------
efd::SyncResult ToolTerrainService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<GizmoService>();
    return egmTerrain::TerrainService::OnPreInit(pDependencyRegistrar);
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolTerrainService::OnInit()
{
    NiTerrainSDM::Init();

    if (TerrainService::OnInit() == efd::AsyncResult_Failure)
        return efd::AsyncResult_Failure;

    NiTerrain::SetInToolMode(true);

    // Add Standard Terrain Gizmos
    GizmoService* pGizmoService = m_pServiceManager->GetSystemServiceAs<GizmoService>();

    m_spTerrainEditGizmo = EE_NEW TerrainEditGizmo(m_pServiceManager);
    EntityGizmoPolicy* pEGP = pGizmoService->GetPolicy<EntityGizmoPolicy>();
    pEGP->AddGizmo("TerrainEdit", m_spTerrainEditGizmo);

    return efd::AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolTerrainService::OnShutdown()
{
    // Shutdown the terrain service
    efd::AsyncResult eResult = TerrainService::OnShutdown();
    NiTerrainSDM::Shutdown();

    // Destroy the Gizmo
    m_spTerrainEditGizmo = NULL;
    m_overlayMeshes.clear();

    return eResult;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolTerrainService::OnTick()
{
    if (m_renderLowDetailTextures)
    {
        ForceUpdateLowDetailTextures();
    }

    if (m_invalidateRenderContexts)
    {
        ForceUpdateRenderContexts();
    }

    return TerrainService::OnTick();
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::RemoveTerrainEntity(egf::Entity* pEntity, efd::Bool removeEntity)
{
    TerrainService::RemoveTerrainEntity(pEntity, removeEntity);

    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::UpdateTransformation(NiTerrain* pTerrain, egf::Entity* pEntity)
{
    TerrainService::UpdateTransformation(pTerrain, pEntity);
    m_spTerrainEditGizmo->UpdateGizmoTransform(pTerrain);
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::AttachTerrainToEntity(NiTerrain* pTerrain, egf::Entity* pEntity)
{
    TerrainService::AttachTerrainToEntity(pTerrain, pEntity);

    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::ForceUpdateLowDetailTextures()
{
    // Update terrain's textures
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        // The terrain was probably just painted upon. Lets make sure it gets an update
        float currTime = (float)m_pServiceManager->GetServiceManagerTime();
        pTerrain->Update(currTime);

        EE_VERIFY(pTerrain->RenderLowDetailTextures());
        pTerrain->Update(currTime);
    }
    m_renderLowDetailTextures = false;

    // We should really re-render the render contexts now
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::ForceUpdateRenderContexts()
{
    // Force the render service to render it's contexts immediately
    RenderService* pRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(pRenderService);
    pRenderService->InvalidateRenderContexts();

    efd::Float32 currTime = (efd::Float32)m_pServiceManager->GetTime(kCLASSID_GameTimeClock);
    efd::UInt32 contextCount = pRenderService->GetRenderContextCount();
    for (efd::UInt32 index = 0; index < contextCount; ++index)
    {
        RenderContext* pContext = pRenderService->GetRenderContextAt(index);
        if (pContext)
            pContext->Draw(currTime);
    }
    m_invalidateRenderContexts = false;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::CreateNewMaterialPackage(const efd::utf8string& packageName,
    const efd::utf8string& packageLocation)
{
    NiSurfacePackagePtr spNewPackage = NiSurfacePackage::Create(packageName.c_str());

    efd::utf8string fullPath = efd::PathUtils::PathCombine(packageLocation, packageName);
    fullPath += ".tmpkg";
    spNewPackage->SetFileName(fullPath.c_str());
    if (!m_spSurfaceLibrary->SavePackage(spNewPackage))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not write material package file."));

        return false;
    }
    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::CreateNewMaterial(const efd::utf8string& packageName,
    const efd::utf8string& materialName)
{
    NiSurfacePackage* pSurfacePkg = FindMaterialPackage(packageName);
    if (pSurfacePkg == NULL)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not locate the currently selected package."));

        return false;
    }

    NiSurface* pNewSurface = NULL;
    pSurfacePkg->CreateSurface(materialName.c_str(), pNewSurface);
    if (!pNewSurface)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not create new terrain material."));

        return false;
    }

    pNewSurface->MarkResolved(true);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::ChangeMaterialName(const efd::utf8string& packageName,
    const efd::utf8string& oldMaterialName, const efd::utf8string& newMaterialName)
{
    NiSurfacePackage* pSurfacePkg = NULL;
    NiSurface* pSurface = NULL;

    if (GetPackageAndSurface(packageName, oldMaterialName, pSurfacePkg, pSurface))
    {
        bool res = pSurfacePkg->RenameSurface(pSurface, newMaterialName.c_str());

        // Update the references in the loaded terrains
        const char* packageAssetId = pSurfacePkg->GetAssetID();
        efd::set<NiTerrain*> terrainSet;
        efd::set<NiTerrain*>::iterator setIter;
        FetchTerrainSet(terrainSet);
        for (setIter = terrainSet.begin(); setIter != terrainSet.end(); ++setIter)
        {
            NiTerrain* pTerrain = *setIter;
            if (pTerrain)
            {
                // Cover the two states the terrain may be in with regard to this texture:
                efd::SInt32 surfaceIndex = pTerrain->GetSurfaceIndex(pSurface);
                bool bOldSurfaceNameReferenced = surfaceIndex >= 0;
                bool bNewSurfaceNameReferenced =
                    pTerrain->GetSurfaceIndex(packageAssetId, newMaterialName.c_str()) >= 0;

                if (bNewSurfaceNameReferenced)
                {
                    // Force the terrain to reload it's package completely. This will cause the
                    // surface reference to swap to the new one.
                    pTerrain->NotifySurfacePackageLoaded(packageAssetId, pSurfacePkg);
                }
                else if (bOldSurfaceNameReferenced)
                {
                    // Simply rename the current reference on the terrain
                    pTerrain->UpdateSurfaceEntry(surfaceIndex, packageAssetId,
                        newMaterialName.c_str(), pSurfacePkg->GetIteration());
                }
            }
        }

        return res;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::RemoveMaterial(const efd::utf8string& packageName,
    const efd::utf8string& materialName)
{
    NiSurfacePackage* pSurfacePkg = NULL;
    NiSurface* pSurface = NULL;

    if (GetPackageAndSurface(packageName, materialName, pSurfacePkg, pSurface))
    {
        if (IsMaterialUsed(pSurface))
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Deleting a material that is used on a terrain is not possible."));
            return false;
        }

        pSurfacePkg->UnloadSurface(materialName.c_str());

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::CanDeleteMaterial(const efd::utf8string& packageNameUTF8,
    const efd::utf8string& materialNameUTF8)
{
    NiSurfacePackage* pSurfacePkg = NULL;
    NiSurface* pSurface = NULL;

    if (!GetPackageAndSurface(packageNameUTF8, materialNameUTF8, pSurfacePkg, pSurface))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
        ("ToolTerrainService: The material could not be found."));
        return false;
    }

    if (IsMaterialUsed(pSurface))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
        ("ToolTerrainService: Deleting a material that is used on a terrain is not possible."));
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::IsMaterialUsed(const NiSurface* pSurface)
{
    EntityList::iterator iter = m_terrainEntities.begin();

    while (iter != m_terrainEntities.end())
    {
        NiTerrain* pTerrain = m_entityToTerrainMap[(*iter)->GetEntityID()];
        if (pTerrain->GetSurfaceIndex(pSurface) != -1)
            return true;
        ++iter;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::AssignMaterialTexture(const efd::utf8string& packageName,
    const efd::utf8string& materialName, NiSurface::SurfaceMapID tMapType,
    const efd::utf8string& fullPath, const efd::utf8string& assetId)
{
    NiSurfacePackage* pSurfacePkg = NULL;
    NiSurface* pSurface = NULL;

    if (GetPackageAndSurface(packageName, materialName, pSurfacePkg, pSurface))
    {
        NiSurface::TextureSlotEntry* pSlot = const_cast<NiSurface::TextureSlotEntry*>(
            pSurface->GetTextureSlotEntry(tMapType));
        if (!pSlot)
        {
            pSlot = EE_NEW NiSurface::TextureSlotEntry();
            pSurface->SetTextureSlotEntry(tMapType, pSlot);
        }

        if (!assetId.empty() && !fullPath.empty())
        {
            pSlot->m_kAssetId = assetId;
            pSlot->m_kFilePath = fullPath.c_str();
        }
        else
        {
            pSurface->SetTextureSlotEntry(tMapType, NULL);
        }

        // Go ahead and mark the surface as resolved and compile the surface since we already know
        // the file path of the texture.
        pSurface->MarkResolved(true); 
        pSurface->CompileSurface(); 

        NotifySurfaceChanged(pSurface);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::AddBrushTypeOverlay(const efd::utf8string& brushTypeName,
    NiNode* pOverlayObj)
{
    OverlayMeshMap::iterator itor = m_overlayMeshes.find(brushTypeName);
    if (itor != m_overlayMeshes.end())
        return;

    m_overlayMeshes[brushTypeName] = pOverlayObj;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::SetBrushTypeOverlayActive(const efd::utf8string& brushTypeName)
{
    OverlayMeshMap::iterator itor = m_overlayMeshes.find(brushTypeName);
    if (itor != m_overlayMeshes.end())
    {
        EE_ASSERT(m_spTerrainEditGizmo);
        m_spTerrainEditGizmo->SetGizmoMesh(itor->second);
        m_activeGizmoName = itor->first;
        return true;
    }
    else
    {
        m_activeGizmoName = efd::utf8string::NullString();
        return false;
    }
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::UpdateStandardBrushParams(efd::Float32 innerRadius,
    efd::Float32 outerRadius, efd::Float32 brushSize)
{
    if (m_activeGizmoName != "Standard")
        return;

    EE_ASSERT(m_spTerrainEditGizmo);

    efd::Float32 brushRadius = brushSize / 2.0f;
    m_spTerrainEditGizmo->SetBrushRadius(brushRadius);
    NiNode* pNode = (NiNode*)m_spTerrainEditGizmo->GetGizmoMesh();
    NiMesh* pMesh = (NiMesh*)pNode->GetAt(0);

    NiDataStreamElementLock positionLock(pMesh, NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    EE_ASSERT(positionLock.IsLocked());
    NiTStridedRandomAccessIterator<NiPoint3> positionItor =
        positionLock.begin<NiPoint3>();

    efd::Float32 worldInnerRadius = innerRadius * brushRadius;
    efd::Float32 worldOuterRadius = outerRadius * brushRadius;

    efd::SInt32 count = -1;
    efd::Float32 sinCos[2];
    for (efd::UInt32 ui = 0; ui < k_numBrushVertices; ++ui)
    {
        count++;

        // Inner circle
        if (count <= k_numInnerSegments)
        {
            efd::SinCos((float)count / (float)k_numInnerSegments * efd::EE_TWO_PI,
                sinCos[0], sinCos[1]);
            positionItor[ui] = NiPoint3(worldInnerRadius * sinCos[1], worldInnerRadius * sinCos[0],
                0.0f);
        }
        else // Outer circle
        {
            efd::Float32 outerAngle = ((float)count - (k_numInnerSegments + 1))/
                (float)k_numOuterSegments * efd::EE_TWO_PI;
            efd::SinCos(outerAngle, sinCos[0], sinCos[1]);

            positionItor[ui] = NiPoint3(worldOuterRadius * sinCos[1], worldOuterRadius * sinCos[0],
                0.0f);
        }
    }

    positionLock.Unlock();
    pNode->Update(0.0f);
}

//------------------------------------------------------------------------------------------------
NiNode* ToolTerrainService::CreateStandardOverlayMesh()
{
    NiMesh* pMesh = EE_NEW NiMesh();
    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    pMesh->SetTranslate(0,0,0);

    // Create vertex position stream.
    NiDataStreamElementLock positionLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3,
        k_numBrushVertices,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, false, true);

    // Create the vertex color stream
    NiDataStreamElementLock colorLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::COLOR(), 0,
        NiDataStreamElement::F_NORMUINT8_4,
        k_numBrushVertices,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, false, true);

    // Create the index buffer.
    NiDataStreamElementLock indicesLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1,
        2 * (k_numInnerSegments + k_numOuterSegments),
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX, false, true);

    NiTStridedRandomAccessIterator<NiPoint3> positionItor = positionLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiUInt16> indIter = indicesLock.begin<NiUInt16>();
    NiTStridedRandomAccessIterator<NiRGBA> colorsIter = colorLock.begin<NiRGBA>();

    efd::SInt32 count = -1;
    efd::SInt32 count2 = 0;
    for (efd::UInt32 ui = 0; ui < k_numBrushVertices; ++ui)
    {
        count++;

        if (count2 < 2 * (k_numInnerSegments + k_numOuterSegments))
        {
            // The lines from the inner to outer circles and end to beginning should not be
            // connected.
            if (count != k_numInnerSegments && count != (k_numBrushVertices - 1))
            {
                indIter[count2] = (efd::UInt16)ui;
                count2++;
                indIter[count2] = (efd::UInt16)(ui + 1);
                count2++;
            }
        }

        NiColorA innerColor = NiColorA(0.0f, 1.0f, 0.0f, 1.0f);
        NiColorA outerColor = NiColorA(0.0f, 0.75f, 0.0f, 1.0f);
        efd::Float32 sinCos[2];

        // Inner circle
        if (count <= k_numInnerSegments)
        {
            efd::SinCos((float)count / (float)k_numInnerSegments * efd::EE_TWO_PI,
                sinCos[0], sinCos[1]);

            innerColor.GetAs(colorsIter[ui]);
            positionItor[ui] = NiPoint3(sinCos[1], sinCos[0], 0.0f);
        }
        else // Outer circle
        {
            efd::Float32 outerAngle = ((float)count - (k_numInnerSegments + 1))/
                (float)k_numOuterSegments * efd::EE_TWO_PI;
            efd::SinCos(outerAngle, sinCos[0], sinCos[1]);

            outerColor.GetAs(colorsIter[ui]);
            positionItor[ui] = NiPoint3(sinCos[1], sinCos[0], 0.0f);
        }
    }

    positionLock.Unlock();
    indicesLock.Unlock();
    colorLock.Unlock();

    // Set the bound
    NiBound bound;
    bound.SetCenterAndRadius(NiPoint3(0,0,0), 1.0f);
    pMesh->SetModelBound(bound);
    pMesh->SetSubmeshCount(1);

    NiVertexColorProperty *pVertexColorProperty = EE_NEW NiVertexColorProperty();
    pVertexColorProperty->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pVertexColorProperty->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pMesh->AttachProperty(pVertexColorProperty);

    NiZBufferProperty* pZBufferProperty = EE_NEW NiZBufferProperty();
    pZBufferProperty->SetZBufferTest(false);
    pZBufferProperty->SetZBufferWrite(true);
    pMesh->AttachProperty(pZBufferProperty);

    NiNode* pNode = EE_NEW NiNode();
    pNode->SetName("Standard_BrushGizmo");

    pNode->AttachChild(pMesh);
    pNode->UpdateProperties();
    pNode->UpdateEffects();
    pNode->Update(0.0f);

    return pNode;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 ToolTerrainService::GetNumActiveSurfaces() const
{
    return m_activeSurfaceMaterials.size();
}

//------------------------------------------------------------------------------------------------
NiSurface* ToolTerrainService::GetActiveSurfaceAt(efd::UInt32 index)
{
    efd::UInt32 currIndex = 0;
    SurfaceMaterialList::iterator itor;
    for (itor = m_activeSurfaceMaterials.begin(); itor != m_activeSurfaceMaterials.end();
        ++itor, currIndex++)
    {
        if (currIndex == index)
            return *itor;
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::AddActiveSurface(NiSurface* pSurfaceMaterial)
{
    EE_ASSERT(pSurfaceMaterial);
    m_activeSurfaceMaterials.push_back(pSurfaceMaterial);
    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::RemoveActiveSurface(NiSurface* pSurfaceMaterial)
{
    SurfaceMaterialList::iterator itor;
    for (itor = m_activeSurfaceMaterials.begin(); itor != m_activeSurfaceMaterials.end(); itor++)
    {
        if (*itor == pSurfaceMaterial)
            break;
    }

    if (itor != m_activeSurfaceMaterials.end())
        m_activeSurfaceMaterials.erase(itor);

    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::ClearActiveSurfaces()
{
    m_activeSurfaceMaterials.clear();
    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::SaveMaterialPackage(const efd::utf8string& packageName)
{
    NiSurfacePackage* pSurfacePkg = FindMaterialPackage(packageName);
    if (!pSurfacePkg)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Unable to locate material package."));
        return false;
    }

    return SaveMaterialPackage(pSurfacePkg);
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::SaveMaterialPackage(NiSurfacePackage* pPackage)
{
    EE_ASSERT(pPackage);
    if (!m_spSurfaceLibrary->SavePackage(pPackage))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not write material package file."));
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::ApplyToHeightMap(HeightMapBuffer* pHeightMap)
{
    NiTerrain* pTerrain = GetNiTerrain();

    // Apply the changes
    EE_ASSERT(pHeightMap && pTerrain);
    pTerrain->SetHeightMap(pHeightMap);
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::ApplyMaterialMask(const efd::utf8string& surfaceName,
    const efd::utf8string& packageName, SurfaceMaskBuffer* pMaskBuffer,
    SurfaceMaskBuffer* pMaskSumBuffer)
{
    // Fetch the surface
    NiSurface* pSurface = NULL;
    NiSurfacePackage* pSurfacePkg = NULL;
    GetPackageAndSurface(packageName, surfaceName, pSurfacePkg, pSurface);

    // Apply the changes
    NiTerrain* pTerrain = GetNiTerrain();
    EE_ASSERT(pMaskBuffer && pTerrain);
    pTerrain->SetSurfaceMask(pSurface, pMaskBuffer, pMaskSumBuffer);

    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetNewMinMaxElevation(efd::Float32 minElevation, efd::Float32 maxElevation)
{
    // Rebuild terrain geometry based on these new settings
    NiTerrain* pTerrain = GetNiTerrain();
    pTerrain->SetMinHeight(minElevation);
    pTerrain->SetMaxHeight(maxElevation);
    pTerrain->RebuildGeometry();
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::CreateTerrainAsset(efd::utf8string archivePath,
    efd::UInt32 terrainSize, efd::UInt32 cellSize, efd::UInt32 materialMaskSize,
    efd::UInt32 lowDetailTextureSize, efd::Float32 minElevation, efd::Float32 maxElevation,
    efd::Float32 vertexSpacing, efd::utf8string& terrainAssetPath)
{
    bool bSuccess = true;

    // Create a terrain asset based on the given data
    NiTerrain* pTerrain = CreateBlankTerrain(terrainSize, cellSize, materialMaskSize,
        lowDetailTextureSize, minElevation, maxElevation, vertexSpacing);
    EE_ASSERT(pTerrain);

    // Wait for the terrain data to be generated
    pTerrain->GetStreamingManager()->WaitForAllTasksCompleted();

    // Save the terrain asset
    pTerrain->SetArchivePath(archivePath.c_str());
    if (!pTerrain->Save())
    {
        bSuccess = false;
    }

    // Destroy the terrain
    EE_DELETE pTerrain;

    // Calculate the new terrain's asset path
    terrainAssetPath = archivePath + "\\root.terrain";
    terrainAssetPath = efd::PathUtils::PathMakeNative(terrainAssetPath);

    return bSuccess;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::SaveTerrain(egf::Entity* pEntity, const efd::utf8string& terrainPath)
{
    // Do we have a specific folder we need to save into?
    efd::utf8string fullPath;
    const char* pArchivePath = 0;
    if (!terrainPath.empty())
    {
        fullPath = efd::PathUtils::PathMakeNative(terrainPath);
        fullPath = efd::PathUtils::PathRemoveFileName(fullPath);
        pArchivePath = fullPath.c_str();
    }

    // Fetch the terrain object
    NiTerrain* pTerrain = GetTerrainForEntity(pEntity);
    if (!pTerrain)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Invalid terrain entity."));
        return false;
    }

    // We can not resolved using the FlatModelID as the physX related libraries are not loaded
    if (pEntity->GetModel()->ContainsModel("PhysXTerrain"))
    {
        pTerrain->SetCustomDataPolicy(EE_NEW TerrainPhysXSaveDataPolicy());
    }

    // Tell the terrain to save the data it has loaded
    efd::UInt32 errorCode;
    if (!pTerrain->Save(pArchivePath, &errorCode))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Error writing sector file."));
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetSurfacePackageLoadedCallback(PackageLoadedCallbackType callback)
{
    m_callbackPackageLoadedCallback = callback;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetCheckAssetMigrationCallback(CheckAssetMigrationCallbackType callback)
{
    m_callbackCheckAssetMigration = callback;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetTerrainDirtyCallback(TerrainDirtyCallbackType callback)
{
    m_callbackTerrainDirty = callback;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::NotifySurfacePackageLoaded(const efd::utf8string& assetID, 
    NiSurfacePackage* pPackage)
{
    TerrainService::NotifySurfacePackageLoaded(assetID, pPackage);

    // Since we are notified of a loaded package even in the event of loading failure, we need
    // to make sure to make sure we only trigger our callback when it was a successful load
    if (m_callbackPackageLoadedCallback && pPackage)
    {
        efd::utf8string kPackageName = (const char*)pPackage->GetName();
        (*m_callbackPackageLoadedCallback)(kPackageName);
    }

    // Check each terrain to see if it's package references matched appropriately (iteration counts
    // must match for the runtime to look correct).
    efd::set<NiTerrain*> terrainSet;
    FetchTerrainSet(terrainSet);

    efd::set<NiTerrain*>::iterator iter;
    for (iter = terrainSet.begin(); iter != terrainSet.end(); ++iter)
    {
        NiTerrain* pTerrain = *iter;
        if (!pTerrain->CheckPackageIteration(pPackage))
        {
            // Send out a warning message
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kLVL2,
                ("ToolTerrainService: The loaded terrain's package references are have been updated"
                " and should be saved. The document has been marked dirty."));
    
            // Mark the terrain document as dirty
            NotifyTerrainOutOfDate(pTerrain);

            break;
        }
    }

    // Check to see if the package itself is out of date
    NiTerrainSurfacePackageFile* pFile = NiTerrainSurfacePackageFile::Open(
        pPackage->GetFilename(), false);
    if (pFile)
    {
        if (pFile->GetFileVersion() < pFile->GetCurrentVersion())
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("ToolTerrainService: The package (%s) needs updating to the latest file format. "
                "Please save your document in toolbench.", (const char*)pPackage->GetName()));

            NotifyTerrainOutOfDate(NULL);
        }
        
        pFile->Close();
        NiDelete pFile;
    }

    // Schedule a re-render of the low detail texture
    m_renderLowDetailTextures = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::NotifyTerrainOutOfDate(NiTerrain* pTerrain)
{   
    EE_UNUSED_ARG(pTerrain);

    // Execute a callback to force the document to dirty.
    if (m_callbackTerrainDirty)
        (*m_callbackTerrainDirty)();
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::NotifySurfaceChanged(NiSurface* pSurface)
{
    // Fetch all the terrains in the system
    efd::set<NiTerrain*> terrains;
    FetchTerrainSet(terrains);

    // Tell the package that it's data is dirty
    pSurface->GetPackage()->MarkDirty(true);

    // Loop over all terrains
    efd::set<NiTerrain*>::iterator iter;
    for (iter = terrains.begin(); iter != terrains.end(); ++iter)
    {
        if (*iter)
        {
            (*iter)->NotifySurfaceChanged(pSurface);
            m_renderLowDetailTextures = true;
        }
    }
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetDebugMode(efd::UInt32 debugMode)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        pTerrain->SetDebugMode((NiTerrainCellShaderData::DebugMode)debugMode);
        m_renderLowDetailTextures = true;
    }
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::GetTerrainMaterialIntersectionData(efd::Point2& sectorID,
    efd::UInt32& leafID, efd::vector<efd::utf8string>& materialList)
{
    if (!IsTerrainEditGizmoActive())
        return false;

    NiTerrain* pTerrain = GetNiTerrain();
    if (!pTerrain)
        return false;

    const NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)
        m_spTerrainEditGizmo->GetLastIntersectedCell();

    if (pLeaf)
    {
        leafID = pLeaf->GetCellID();
        NiInt16 sectorX, sectorY;
        pLeaf->GetContainingSector()->GetSectorIndex(sectorX, sectorY);
        sectorID.x = sectorX;
        sectorID.y = sectorY;

        for (efd::UInt32 ui = 0; ui < pLeaf->GetSurfaceCount(); ++ui)
        {
            NiInt32 index = pLeaf->GetSurfaceIndex(ui);
            NiFixedString packageID;
            NiFixedString surfaceID;
            const NiSurface* pSurface = pTerrain->GetSurfaceAt(index);

            if (pSurface)
            {
                NiSurfacePackage* pPackage = pSurface->GetPackage();
                efd::utf8string materialName = "";
                if (pPackage)
                {
                    materialName = pSurface->GetPackage()->GetName();
                    materialName.append('.');
                    materialName.append(pSurface->GetName());
                }
                else
                {
                    materialName = "[ Special: ";
                    materialName.append(pSurface->GetName());
                    materialName.append(" ]");
                }

                materialList.push_back(materialName);
            }
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::RemoveMaterialFromLeaf(efd::Point2 sectorID, efd::UInt32 leafID,
    efd::utf8string materialName)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        NiTerrainSector* pSector = pTerrain->GetSector((efd::UInt16)sectorID.GetX(),
            (efd::UInt16)sectorID.GetY());

        if (!pSector)
            return;

        NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(leafID);

        if (!pLeaf)
            return;

        efd::vector<efd::utf8string> splitMaterialName;
        if (materialName.split(".", splitMaterialName) != 2)
            return;

        NiSurface* pSurface = NULL;
        NiSurfacePackage* pSurfacePkg = NULL;
        if (!GetPackageAndSurface(splitMaterialName[0], splitMaterialName[1], pSurfacePkg,
            pSurface))
        {
            return;
        }

        pLeaf->RemoveSurface(pTerrain->GetSurfaceIndex(pSurface));
        m_spTerrainEditGizmo->GetPaintingGuide()->AddChangedLeaf(pLeaf);

        m_renderLowDetailTextures = true;
    }
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::RemoveMaterialFromTerrain(efd::utf8string packageNameUTF8,
    efd::utf8string materialNameUTF8)
{
    NiSurfacePackage* pSurfacePkg = NULL;
    NiSurface* pSurface = NULL;

    if (GetPackageAndSurface(packageNameUTF8, materialNameUTF8, pSurfacePkg, pSurface))
    {
        RemoveMaterialFromTerrain(pSurface);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::RemoveMaterialFromTerrain(const NiSurface* pSurface)
{
    EntityList::iterator iter = m_terrainEntities.begin();

    while (iter != m_terrainEntities.end())
    {
        NiTerrain* pkTerrain = m_entityToTerrainMap[(*iter)->GetEntityID()];
        pkTerrain->RemoveSurface(pSurface);
        ++iter;
    }

    m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_renderLowDetailTextures = true;

    return true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::DisplayPaintingGuide(bool enable)
{
    m_spTerrainEditGizmo->GetPaintingGuide()->DisplayPaintingGuide(enable);
    m_renderLowDetailTextures = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::RequestPaintingGuideRebuild()
{
    if (m_spTerrainEditGizmo)
        m_spTerrainEditGizmo->GetPaintingGuide()->RequestFullRebuild();
    m_invalidateRenderContexts = true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::GetSurfaceMaskData(efd::Point2 sectorIndex, efd::UInt32 leafID,
    efd::utf8string materialNameUTF8, SurfaceMaskBuffer* pMaskBuffer,
    SurfaceMaskBuffer* pSumBuffer)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        NiTerrainSector* pSector = pTerrain->GetSector((efd::UInt16)sectorIndex.GetX(),
            (efd::UInt16)sectorIndex.GetY());

        if (!pSector)
            return;

        NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(leafID);

        if (!pLeaf)
            return;

        efd::vector<efd::utf8string> splittedMaterialName;
        if (materialNameUTF8.split(".", splittedMaterialName) != 2)
            return;

        NiSurface* pSurface = NULL;
        NiSurfacePackage* pSurfacePkg = NULL;
        if (!GetPackageAndSurface(splittedMaterialName[0], splittedMaterialName[1], pSurfacePkg,
            pSurface))
        {
            return;
        }

        NiRect<efd::SInt32> worldSpaceRegion;
        NiIndex ind;
        pLeaf->GetBottomLeftIndex(ind);
        efd::UInt32 cellSize = pLeaf->GetCellSize();
        efd::UInt32 sectorSize = pTerrain->GetCalcSectorSize() - 1;

        worldSpaceRegion.m_bottom = ind.y + (efd::SInt32)sectorIndex.y * sectorSize;
        worldSpaceRegion.m_left = ind.x + (efd::SInt32)sectorIndex.x * sectorSize;
        worldSpaceRegion.m_top = worldSpaceRegion.m_bottom + cellSize;
        worldSpaceRegion.m_right = worldSpaceRegion.m_left + cellSize;

        pTerrain->GetSurfaceMask(pSurface, worldSpaceRegion, pMaskBuffer, pSumBuffer);
    }
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::SetSurfaceMaskData(efd::Point2 sectorIndex, efd::UInt32 leafID,
    efd::utf8string materialNameUTF8,
    SurfaceMaskBuffer* pBuffer, SurfaceMaskBuffer* pSumBuffer)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        NiTerrainSector* pSector = pTerrain->GetSector((efd::UInt16)sectorIndex.GetX(),
            (efd::UInt16)sectorIndex.GetY());

        if (!pSector)
            return;

        NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(leafID);

        if (!pLeaf)
            return;

        efd::vector<efd::utf8string> splittedMaterialName;
        if (materialNameUTF8.split(".", splittedMaterialName) != 2)
            return;

        NiSurface* pSurface = NULL;
        NiSurfacePackage* pSurfacePkg = NULL;
        if (!GetPackageAndSurface(splittedMaterialName[0], splittedMaterialName[1], pSurfacePkg,
            pSurface))
        {
            return;
        }

        pTerrain->SetSurfaceMask(pSurface, pBuffer, pSumBuffer);

        // Update the painting guide display for that leaf
        m_spTerrainEditGizmo->GetPaintingGuide()->AddChangedLeaf(pLeaf);
        m_renderLowDetailTextures = true;
    }
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::SetPaintingGuideTexture(efd::utf8string fileName)
{
    return m_spTerrainEditGizmo->GetPaintingGuide()->SetDisplayTexture(fileName);
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::GetLastIntersectedSector(efd::SInt16& sectorX, efd::SInt16& sectorY)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (!pTerrain)
        return false;

    // Check to see if the gizmo is currently active
    if (!IsTerrainEditGizmoActive())
        return false;

    // Figure out what sector was intersected
    efd::Point3 intersectionPt = m_spTerrainEditGizmo->GetLastIntersectionPt();

    // Transform intersection point into a terrain space point
    float terrainScale = pTerrain->GetWorldScale();
    NiPoint3 terrainPos = pTerrain->GetWorldTranslate();
    intersectionPt /= terrainScale;
    intersectionPt -= terrainPos;

    // Calculate which sector this point would be
    float sectorWidth = float(pTerrain->GetCalcSectorSize() - 1);
    intersectionPt /= sectorWidth;
    // Offset the sector center
    intersectionPt += NiPoint3(0.5f, 0.5f, 0.5f);

    sectorX = efd::SInt16(efd::Floor(intersectionPt.x));
    sectorY = efd::SInt16(efd::Floor(intersectionPt.y));

    return true;
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::UnloadSector(NiTerrain* pTerrain, efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    // Make sure this sector has been saved out (so any changes to it are remembered)
    EE_ASSERT(pTerrain);

    NiTerrainSector* pSector = pTerrain->GetSector(sectorX, sectorY);
    if (pSector)
    {
        efd::UInt32 errorCode = 0;
        if (!pSector->Save(0, &errorCode))
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::LoadSector: Could not save the sector."
                "The file may be read-only or open"));
        }
    }

    // Now unload the sector
    return TerrainService::UnloadSector(pTerrain, sectorX, sectorY);
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::CreateSector(NiTerrain* pTerrain, efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    if (!pTerrain)
        return;

    // Create the sector
    pTerrain->CreateBlankSector(sectorX, sectorY, true);

    // Fetch the sector selector object from the terrain. If the right type isn't there
    // then this operation isn't supported.
    SectorSelector* pSectorSelector =
        NiDynamicCast(SectorSelector, pTerrain->GetSectorSelector());
    if (!pSectorSelector)
        return;

    // Tell the sector selector that this sector has been loaded
    pSectorSelector->NotifySectorLOD(sectorX, sectorY, pTerrain->GetNumLOD());
    pSectorSelector->LockSectorLOD(sectorX, sectorY);
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::IsSectorLODLoaded(efd::SInt16 sectorX, efd::SInt16 sectorY,
    efd::SInt32 lod)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        // Set the default test for LOD
        if (lod < 0)
            lod = pTerrain->GetNumLOD();

        NiTerrainSector* pSector = pTerrain->GetSector(sectorX, sectorY);
        if (pSector)
        {
            // Test if the sector is loaded to the specified level of detail
            return pSector->GetSectorData()->GetHighestLoadedLOD() >= lod;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ToolTerrainService::IsSectorOnDisk(efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    NiTerrain* pTerrain = GetNiTerrain();
    if (pTerrain)
    {
        return pTerrain->IsSectorOnDisk(sectorX, sectorY);
    }
    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool ToolTerrainService::LoadTerrainAsset(const efd::utf8string& assetID,
    const efd::utf8string& fileName)
{
    // Check if this terrain asset requires migration
    if (m_callbackCheckAssetMigration)
    {
        if ((*m_callbackCheckAssetMigration)(assetID, fileName))
        {
            Entity* pEntity = FindEntityByAssetLLID(assetID);
            if (pEntity)
            {
                LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("TerrainService::LoadTerrainAsset: Could not load the asset "
                    "bound to this entity because it requires migration. (%s).", 
                    fileName.c_str()), false);

                // Send out a message to trigger migration of this asset
                TerrainAssetMigrationRequestMessagePtr spMessage =
                    EE_NEW TerrainAssetMigrationRequestMessage(pEntity->GetDataFileID(),
                    assetID, fileName);
                m_pMessageService->SendLocal(spMessage);
            }

            SkipPreloadingAssets(false);

            return false;
        }
    }

    return TerrainService::LoadTerrainAsset(assetID, fileName);
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::NotifyMigrationComplete(egf::Entity* pEntity)
{
    // Remove the entity from our control, so that all of its out of date contents is destroyed.
    DetachSceneGraph(pEntity);
    RemoveTerrainEntity(pEntity, true);

    efd::utf8string assetID;
    if (RequestLoadTerrain(pEntity, assetID))
    {
        // Add the entity to our list of managed terrain entities
        m_terrainEntities.push_back(pEntity);
    }
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::InitializeTerrain(NiTerrain* pkTerrain, egf::Entity* pEntity)
{
    TerrainService::InitializeTerrain(pkTerrain, pEntity);

    // Register the streaming listener
    pkTerrain->GetStreamingManager()->SetListener(EE_NEW StreamingListener(this, pkTerrain));
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
ToolTerrainService::StreamingListener::StreamingListener(ToolTerrainService* pService,
    NiTerrain* pTerrain):
    m_pService(pService),
    m_pTerrain(pTerrain)
{
    EE_ASSERT(m_pService);
    EE_ASSERT(m_pTerrain);
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::StreamingListener::IncRefCount()
{
    return NiRefObject::IncRefCount();
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::StreamingListener::DecRefCount()
{
    return NiRefObject::DecRefCount();
}

//------------------------------------------------------------------------------------------------
void ToolTerrainService::StreamingListener::ReportFinishTask(TaskID taskID,
    TaskType::VALUE taskType, efd::UInt32 errorCode)
{
    EE_UNUSED_ARG(taskID);
    EE_UNUSED_ARG(taskType);
    EE_UNUSED_ARG(errorCode);

    if (taskType != TaskType::REBUILDING)
    {
        m_pService->m_invalidateRenderContexts = true;
        m_pService->m_renderLowDetailTextures = true;
        m_pService->RequestPaintingGuideRebuild();
    }

    switch (taskType)
    {
    case TaskType::REBUILDING:
    case TaskType::LOADING:
        if ((errorCode & NiTerrain::EC_SECTOR_LOADED) == 0)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::LoadSector: Could not load the sector."
                "The file may be corrupt or incompatible with this terrain"));
        }
        break;

    case TaskType::UNLOADING:
        if ((errorCode & NiTerrain::EC_SECTOR_UNLOADED) == 0)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::UnloadSector: Could not unload the sector."));
        }
        break;
    }
}

//------------------------------------------------------------------------------------------------
