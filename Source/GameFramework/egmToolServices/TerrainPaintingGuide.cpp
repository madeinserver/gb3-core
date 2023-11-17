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
#include "egmToolServicesLibType.h"

#include "ToolTerrainService.h"
#include "TerrainPaintingGuide.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
TerrainPaintingGuide::TerrainPaintingGuide(efd::ServiceManager* pServiceManager) 
{
    m_pTerrainService = pServiceManager->GetSystemServiceAs<ToolTerrainService>();
    EE_ASSERT(m_pTerrainService);

    m_rebuildOverlay = true;
    m_isPaintingGuideDisplayed = false;
    m_isPaintingGuideEnabled = true;
    m_spGuideRoot = EE_NEW NiNode();
    m_spGuideRoot->SetName("PaintingGuideRoot");
    m_spGuideRoot->SetTranslate(NiPoint3::ZERO);
    m_spDisplayTexture = 0;
}

//--------------------------------------------------------------------------------------------------
TerrainPaintingGuide::~TerrainPaintingGuide()
{
    m_spGuideRoot->RemoveAllChildren();
    m_spGuideRoot = 0;
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::OnTick(NiNode* pParent)
{
    if (m_isPaintingGuideEnabled && !m_isPaintingGuideDisplayed)
    {
        m_isPaintingGuideDisplayed = true;
        pParent->AttachChild(m_spGuideRoot);
    }
    else if (m_isPaintingGuideDisplayed && !m_isPaintingGuideEnabled)
    {
        m_isPaintingGuideDisplayed = false;
        pParent->DetachChild(m_spGuideRoot);
    }
        
    Update();
}

//--------------------------------------------------------------------------------------------------
bool TerrainPaintingGuide::Update()
{
    if (m_rebuildOverlay)
    {
        ForceOverlayRebuild();
        m_rebuildOverlay = false;
    }

    if (m_changedLeaves.size() != 0)
        UpdatePaintingGuide();

    if (m_dirty)
    {        
        m_spGuideRoot->UpdateProperties();
        m_spGuideRoot->UpdateEffects();
        m_spGuideRoot->Update(0.0f);
        m_dirty = false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::AddChangedLeaf(efd::Point2 sectorID, efd::UInt32 leafID)
{
    InternalLeafID id = InternalLeafID(sectorID, leafID);
    efd::vector<InternalLeafID>::iterator foundAfPos = m_affectedLeaves.end();
    efd::vector<InternalLeafID>::iterator foundChPos = m_changedLeaves.end();

    foundAfPos = m_affectedLeaves.find(id);
    foundChPos = m_changedLeaves.find(id);

    // the leaf has already been added to the changed list no need to do so again
    if (foundChPos != m_changedLeaves.end())
        return;

    bool canPaint = CanPaintLeaf(id);

    if (!canPaint && foundAfPos == m_affectedLeaves.end())
    {
        // we need to add a new overlay
        m_changedLeaves.push_back(id);
    }
    else if (canPaint && foundAfPos != m_affectedLeaves.end())
    {
        // we need to remove an existing overlay
        for (efd::UInt32 ui = 0; ui < m_spGuideRoot->GetArrayCount(); ++ui)
        {
            NiMesh* pMesh = NiDynamicCast(NiMesh, m_spGuideRoot->GetAt(ui));

            if (pMesh && (*foundAfPos).m_internalID == pMesh->GetName())
            {
                m_spGuideRoot->DetachChildAt(ui);
                m_dirty = true;
                break;
            }
        }

        m_affectedLeaves.erase(foundAfPos);
    }    
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::AddChangedLeaf(NiTerrainCellLeaf* pLeaf)
{
    if (!pLeaf)
        return;

    NiTerrainSector* pSector = pLeaf->GetContainingSector();
    NiInt16 x, y;
    pSector->GetSectorIndex(x, y);
    
    AddChangedLeaf(efd::Point2(x,y), pLeaf->GetCellID());
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::RequestFullRebuild()
{
    m_rebuildOverlay = true;
}

//--------------------------------------------------------------------------------------------------
NiNode* TerrainPaintingGuide::GetPaintingGuideRoot()
{
    return m_spGuideRoot;
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::DisplayPaintingGuide(bool showGuide)
{
    m_isPaintingGuideEnabled = showGuide;
}

//--------------------------------------------------------------------------------------------------
bool TerrainPaintingGuide::SetDisplayTexture(efd::utf8string fileName)
{
    m_spDisplayTexture = NiSourceTexture::Create(fileName.c_str());
    RequestFullRebuild();

    return m_spDisplayTexture != 0;
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::UpdatePaintingGuide()
{
    efd::vector<InternalLeafID>::iterator iter = m_changedLeaves.begin();

    while (iter != m_changedLeaves.end())
    {   
        m_spGuideRoot->AttachChild(CreateLeafOverlayMesh(*iter), true);
        m_affectedLeaves.push_back(*iter);
        m_dirty = true;
        ++iter;
    }

    m_changedLeaves.clear();
}

//--------------------------------------------------------------------------------------------------
void TerrainPaintingGuide::ForceOverlayRebuild()
{
    NiTerrain* pCurrentTerrain = m_pTerrainService->GetNiTerrain();

    m_affectedLeaves.clear();
    m_changedLeaves.clear();
    m_spGuideRoot->RemoveAllChildren();

    if (!pCurrentTerrain)
    {
        return;
    }

    const NiTMap<NiUInt32, NiTerrainSector*>& sectorMap = pCurrentTerrain->GetLoadedSectors();
    NiTMapIterator pos = sectorMap.GetFirstPos();

    NiTerrainSector* pSector;
    efd::UInt32 key;
    while (pos)
    {        
        sectorMap.GetNext(pos, key, pSector);
        efd::UInt32 startCellID = pSector->GetCellOffset(pCurrentTerrain->GetNumLOD());
        efd::UInt32 endCellID = 0;
        if (!pSector->GetMaxLoadedCellIndex(endCellID))
            continue;

        for (efd::UInt32 ui = startCellID; ui <= endCellID; ++ui)
        {
            NiIndex sectorIndex;
            efd::SInt16 x, y;
            pSector->GetSectorIndex(x, y);

            sectorIndex.x = x;
            sectorIndex.y = y;
            NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(ui);
            AddChangedLeaf(pLeaf);
        }
    }
}

//--------------------------------------------------------------------------------------------------
bool TerrainPaintingGuide::CanPaintLeaf(InternalLeafID leaf)
{
    NiTerrain* pCurrentTerrain = m_pTerrainService->GetNiTerrain();

    if (!pCurrentTerrain)
        return false;

    NiTerrainSector* pSector =
        pCurrentTerrain->GetSector((NiInt16)leaf.m_sectorIndex.x, (NiInt16)leaf.m_sectorIndex.y);
    
    if (!pSector)
        return false;
    
    NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(leaf.m_leafID);

    if (pLeaf)
    {
        // If the leaf does not have the maximum number of surfaces then we can paint
        if (pLeaf->GetSurfaceCount() < NiTerrainCellLeaf::MAX_NUM_SURFACES)
            return true;

        efd::UInt32 numActiveMaterials = m_pTerrainService->GetNumActiveSurfaces();
        for (efd::UInt32 uiActive = 0; uiActive < numActiveMaterials; ++uiActive)
        {
            NiSurface* pActiveSurface = m_pTerrainService->GetActiveSurfaceAt(uiActive);
            bool found = false;
            for (efd::UInt32 uiLeaf = 0; uiLeaf < NiTerrainCellLeaf::MAX_NUM_SURFACES; ++uiLeaf)
            {
                const NiSurface* pSurface =
                    pCurrentTerrain->GetSurfaceAt(pLeaf->GetSurfaceIndex(uiLeaf));               

                // If one of the leaf's surfaces is a surface we are trying to paint then, we can
                // paint.
                if (pActiveSurface == pSurface)
                {
                    found = true;
                    break;
                }
                
            }
            if (!found)
                return false;
        }

        return true;

    }
    
    return false;
}

//--------------------------------------------------------------------------------------------------
NiMesh* TerrainPaintingGuide::CreateLeafOverlayMesh(InternalLeafID leaf)
{
    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    if (!pTerrain)
        return NULL;

    NiTerrainSector* pSector =
        pTerrain->GetSector((NiInt16)leaf.m_sectorIndex.x, (NiInt16)leaf.m_sectorIndex.y);

    if (!pSector)
        return NULL;

    NiTerrainCellLeaf* pLeaf = (NiTerrainCellLeaf*)pSector->GetCell(leaf.m_leafID);

    if (!pLeaf)
        return NULL;

    NiMesh* pLeafMesh = EE_NEW NiMesh();
    pLeafMesh->SetName(leaf.m_internalID.c_str());
    pLeafMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiIndex centerIndex;
    pLeaf->GetBottomLeftIndex(centerIndex);
    
    float sectorHalfSize = pSector->GetSectorData()->GetSectorSize() / 2.0f;
    efd::Point3 centerPoint = efd::Point3(
        (float)centerIndex.x - sectorHalfSize, 
        (float)centerIndex.y - sectorHalfSize,
        0.0f);
    centerPoint.x += (float)(pSector->GetSectorData()->GetSectorSize()) * 
        (NiInt16)leaf.m_sectorIndex.x;
    centerPoint.y += (float)(pSector->GetSectorData()->GetSectorSize()) * 
        (NiInt16)leaf.m_sectorIndex.y;
    
    pLeafMesh->SetTranslate(centerPoint);

    // Create vertex position stream.
    NiDataStreamElementLock positionLock = pLeafMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3,
        5,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX,
        false,
        true);
    
    // Create the index buffer.
    NiDataStreamElementLock indicesLock = pLeafMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1,
        12,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX_INDEX,
        false,
        true);

    // Create the texcoord stream
    NiDataStreamElementLock textCoordLock = pLeafMesh->AddStreamGetLock(
        NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2,
        5,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        false,
        true);
    
    NiTStridedRandomAccessIterator<NiPoint3> positionItor = positionLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint2> textCoordItor = textCoordLock.begin<NiPoint2>();
    NiTStridedRandomAccessIterator<NiUInt16> indIter = indicesLock.begin<NiUInt16>();
        
    efd::Point2 pointGuide[5] = {
        efd::Point2(0.0f, 0.0f),
        efd::Point2(1.0f, 0.0f), 
        efd::Point2(0.5f, 0.5f),
        efd::Point2(1.0f, 1.0f),
        efd::Point2(0.0f, 1.0f)};
    
    // Fetch the leaf transform:
    NiTransform transform = pLeaf->GetWorldTransform();
    NiTransform inverseTransform;
    transform.Invert(inverseTransform);

    float cellSize = (float)pLeaf->GetCellSize();
    
    EE_ASSERT(pTerrain);
    efd::Point3 cellCenter;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    for (efd::UInt32 ui = 0; ui < 5; ++ui)
    {
        // Local point. belongs to cell space
        efd::Point3 local = efd::Point3(pointGuide[ui].x * cellSize, 
            pointGuide[ui].y * cellSize, 0.0f);
        
        // Bring the borders in slightly for ray collision to avoid colliding with borders
        efd::Point3 localModifier = efd::Point3::ZERO;
        if (ui != 2)
        {
            efd::Point2 mod = efd::Point2(1.0f, 1.0f) - 2.0f * pointGuide[ui];
            localModifier = efd::Point3(0.01f * mod.x, 0.01f * mod.y, 0.0f) ;
        }

        // Convert to terrain space (does not take the terrain transformation in consideration)
        efd::Point3 rayOrigin =  pLeafMesh->GetLocalTransform() * (local + localModifier);

        // now convert to world space
        rayOrigin = pTerrain->GetWorldTransform() * rayOrigin;
        
        rayOrigin.z += (pTerrain->GetMaxHeight() + 1.0f) * pTerrain->GetWorldScale();
        NiRay ray(rayOrigin, NiPoint3(0.0f, 0.0f, -1.0f));

        if (pTerrain->Collide(ray))
        {
            NiPoint3 collisionPt, collisionNormal;
            ray.GetIntersection(collisionPt, collisionNormal);
            collisionPt = inverseTransform * collisionPt;
            local.z = collisionPt.z + 1.0f;
            
            if (local.z > maxZ)
                maxZ = local.z;
            if (local.z < minZ)
                minZ = local.z;
        }

        // This is the mesh's center point
        if (ui == 2)
            cellCenter = local;

        positionItor[ui] = local;
        textCoordItor[ui] = pointGuide[4 - ui];
    }

    // Fill in the index stream
    indIter[0] = 0;
    indIter[1] = 1;
    indIter[2] = 2;

    indIter[3] = 1;
    indIter[4] = 3;
    indIter[5] = 2;

    indIter[6] = 3;
    indIter[7] = 4;
    indIter[8] = 2;

    indIter[9] = 4;
    indIter[10] = 0;
    indIter[11] = 2;
    
    // unlock all streams
    positionLock.Unlock();
    indicesLock.Unlock();
    textCoordLock.Unlock();

    // Create a default red texture if no texture exist
    if (!m_spDisplayTexture)
    {
        NiPixelData* pPixels = EE_NEW NiPixelData(1,1, NiPixelFormat::RGBA32);
        unsigned char* pcPix = pPixels->GetPixels();

        pcPix[0] = 255;
        pcPix[1] = 0;
        pcPix[2] = 0;
        pcPix[3] = 64;

        m_spDisplayTexture = NiSourceTexture::Create(pPixels);        
    }
    
    // Set the bound
    NiBound bound;
    float radius = (cellSize / 2.0f) * efd::Sqrt(2.0f);
    if (radius < maxZ - minZ)
        radius = maxZ - minZ;

    bound.SetCenterAndRadius(cellCenter, radius);
    pLeafMesh->SetModelBound(bound);
    pLeafMesh->SetSubmeshCount(1);

    NiTexturingProperty* pTextProp = EE_NEW NiTexturingProperty();
    pTextProp->SetBaseFilterMode(NiTexturingProperty::FILTER_TRILERP);
    pTextProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pTextProp->SetBaseTexture(m_spDisplayTexture);
    pTextProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    pLeafMesh->AttachProperty(pTextProp);
    
    NiAlphaProperty* pAlphaProperty = EE_NEW NiAlphaProperty();
    pAlphaProperty->SetAlphaBlending(true);
    pLeafMesh->AttachProperty(pAlphaProperty);

    NiZBufferProperty* pZBufferProperty = EE_NEW NiZBufferProperty();
    pZBufferProperty->SetZBufferTest(false);
    pZBufferProperty->SetZBufferWrite(true);
    pLeafMesh->AttachProperty(pZBufferProperty);
    
    return pLeafMesh;
}

//--------------------------------------------------------------------------------------------------