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
#include <NiTerrain.h>
#include "TerrainPhysXSaveDataPolicy.h"

using namespace egf;
using namespace efd;
using namespace ecr;
using namespace egmToolServices;

static const char* STATIC_FRICTION = "NX_STATICFRICTION";
static const char* DYNAMIC_FRICTION = "NX_DYNAMICFRICTION";
static const char* RESTITUTION = "NX_RESTITUTION";

//--------------------------------------------------------------------------------------------------
bool TerrainPhysXSaveDataPolicy::SaveCustomData(NiTerrainSector* pkSector, 
    NiTerrainSectorFile* pkFile)
{
    return TerrainPhysXSaveDataPolicy::SavePhysXData(pkSector, pkFile);
}

//--------------------------------------------------------------------------------------------------
bool TerrainPhysXSaveDataPolicy::SavePhysXData(NiTerrainSector* pkSector, 
    NiTerrainSectorFile* pkFile)
{
    if (pkSector->GetSectorData()->GetHighestLoadedLOD() != 
        (NiInt32)pkSector->GetSectorData()->GetNumLOD())
    {
        return false;
    }

    efd::map<efd::UInt32, NiPhysXMaterialMetaData> kMetaMap;    
    if (!BuildMaterialMap(pkSector->GetTerrain(), kMetaMap))
        return false;
    
    NiTerrainSectorPhysXData* pkData = BuildPhysXData(pkSector);    
    if (!pkData)
        return false;    

    pkFile->WriteTerrainSectorPhysXData(kMetaMap, pkData);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool TerrainPhysXSaveDataPolicy::BuildMaterialMap(NiTerrain* pkTerrain, 
    efd::map<efd::UInt32, NiPhysXMaterialMetaData>& kMaterialIndex)
{
    if (!pkTerrain)
        return false;

    NiUInt32 uiNumMats = pkTerrain->GetNumSurfaces();

    for (NiUInt32 ui = 0; ui < uiNumMats; ++ui)
    {
        // Check if this is a gap in the surface index
        NiFixedString kDummyString;
        if (!pkTerrain->GetSurfaceEntry(ui, kDummyString, kDummyString))
            continue;

        const NiSurface* pkSurface = pkTerrain->GetSurfaceAt(ui);
        NiPhysXMaterialMetaData kMeta;

        float fValue = 0;
        float fWeight = 0;
        pkSurface->GetMetaData().Get(RESTITUTION, fValue, fWeight);
        kMeta.SetRestitution(efd::Clamp(fValue, 0.0f, 1.0f));

        fValue = 0;
        pkSurface->GetMetaData().Get(DYNAMIC_FRICTION, fValue, fWeight);
        kMeta.SetDynamicFriction(fValue);        

        fValue = 0;
        pkSurface->GetMetaData().Get(STATIC_FRICTION, fValue, fWeight);
        kMeta.SetStaticFriction(fValue);

        kMaterialIndex[ui] = kMeta;
    }
    
    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorPhysXData* TerrainPhysXSaveDataPolicy::BuildPhysXData(const NiTerrainSector* pkSector)
{
    NiUInt32 uiSectorSize = pkSector->GetTerrain()->GetCalcSectorSize();
    NiTerrainSectorPhysXData* pkResult =
        EE_NEW NiTerrainSectorPhysXData(uiSectorSize, uiSectorSize);

    NiUInt32 uiNumLOD = pkSector->GetSectorData()->GetNumLOD();
    NiUInt32 uiNumBlocks = (NiUInt32)(NiPow(2.0f, 2.0f * (float)uiNumLOD));
    
    for (NiUInt32 uiLeafID = 0; uiLeafID < uiNumBlocks; ++uiLeafID)
    {
        // we should only save leafs
        NiTerrainCellLeaf* pkLeaf = NiDynamicCast(NiTerrainCellLeaf,
            pkSector->GetCell(pkSector->GetCellOffset(uiNumLOD) + uiLeafID));
        
        if (!pkLeaf)
            continue;

        if (!BuildLeafData(pkResult, pkLeaf))
        {
            EE_DELETE pkResult;
            return NULL;
        }
    }

    return pkResult;
}

//--------------------------------------------------------------------------------------------------
bool TerrainPhysXSaveDataPolicy::BuildLeafData(NiTerrainSectorPhysXData*& pkPhysXData, 
    NiTerrainCellLeaf* pkLeaf)
{
    if (!pkLeaf)
        return false;
    NiUInt8 ucMask = NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ;
    NiTerrainStreamLocks kLocks;

    NiTStridedRandomAccessIterator<NiUInt16> kOriIndIter16;
    NiTStridedRandomAccessIterator<NiUInt32> kOriIndIter32;
    bool bUseIndex16 = false;
    NiTerrainSector* pkSector = pkLeaf->GetContainingSector();

    if (pkSector->GetUsingShortIndexBuffer())
    {
        kLocks.GetIndexIterator(pkLeaf, ucMask, kOriIndIter16);
        bUseIndex16 = true;
    }
    else
    {
        kLocks.GetIndexIterator(pkLeaf, ucMask, kOriIndIter32);
    }

    NiUInt32 uiBlockSize = pkLeaf->GetWidthInVerts();
    NiUInt32 uiBlockWidth = pkLeaf->GetCellSize();
    NiUInt32 uiSectorSize = pkSector->GetSectorData()->GetSectorWidthInVerts();

    NiIndex kLeafMinIndex;
    pkLeaf->GetBottomLeftIndex(kLeafMinIndex);  
   
    for (NiUInt32 y = 0; y < uiBlockSize; ++y)
    {
        for (NiUInt32 x = 0; x < uiBlockSize; ++x)
        {
            // Work out the sample tessalation
            // We don't deal with the border points as they would be outside
            // the sample tile.
            bool bTessFlag = false;
            if (y != uiBlockWidth && x != uiBlockWidth)
            {
                NiUInt32 uiIndexValue[3];
                NiUInt32 uiIndStreamIndex = (y * uiBlockWidth + x) * 6;
                if (bUseIndex16)
                {
                    uiIndexValue[0] = kOriIndIter16[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter16[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter16[uiIndStreamIndex + 2];
                }
                else
                {
                    uiIndexValue[0] = kOriIndIter32[uiIndStreamIndex];
                    uiIndexValue[1] = kOriIndIter32[uiIndStreamIndex + 1];
                    uiIndexValue[2] = kOriIndIter32[uiIndStreamIndex + 2];
                }

                // now that we have the indexvalue for the tile points we can
                // work out the tesselation
                NiUInt32 uiCount = 0;
                while (uiIndexValue[0] + 1 != uiIndexValue[1] && uiCount < 3)
                {
                    NiUInt32 uiTemp = uiIndexValue[0];
                    uiIndexValue[0] = uiIndexValue[1];
                    uiIndexValue[1] = uiIndexValue[2];
                    uiIndexValue[2] = uiTemp;
                    uiCount++;
                }

                if (uiIndexValue[0] + 1 == uiIndexValue[1])
                {
                    if (uiIndexValue[0] + uiBlockSize == uiIndexValue[2])
                        bTessFlag = true;
                    else
                        bTessFlag = false;
                }
                else
                {
                    // This is a default case that should not be hit
                    bTessFlag = true;
                }
            }

            NiUInt16 usMatIndex0, usMatIndex1;
            GetDominantSurface(pkLeaf, x, y, bTessFlag, usMatIndex0, usMatIndex1);

            NiUInt32 uiDataIndex = (kLeafMinIndex.y + y) * uiSectorSize + kLeafMinIndex.x + x;
            pkPhysXData->m_pkSamples[uiDataIndex] = 
                NiTerrainSectorPhysXSampleData(usMatIndex0, usMatIndex1, bTessFlag);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void TerrainPhysXSaveDataPolicy::GetDominantSurface(NiTerrainCellLeaf* pkLeaf, NiInt32 iX, NiInt32 iY, 
    bool bTessFlag, NiUInt16& usMatIndex0, NiUInt16& usMatIndex1)
{
    if (!pkLeaf)
        return;

    // Find out the coordinates to read surface values from using the 
    // NiTextureRegion
    const NiTextureRegion kTextureRegion = 
        pkLeaf->GetTextureRegion(NiTerrainResourceManager::TextureType::BLEND_MASK);
    NiSourceTexture* pkTexture = NiDynamicCast(NiSourceTexture, kTextureRegion.GetTexture());

    if (!pkTexture)
        return;

    NiPixelData* pkPixelData = pkTexture->GetSourcePixelData();

    NiPoint2 kOffset = kTextureRegion.GetTextureOffset();
    float fScale = kTextureRegion.GetTextureScale();
    float fTexWidth = (float)pkPixelData->GetWidth();
    float fCellTexRange = 1.0f / fScale;

    NiIndex kEndPoint; 
    kEndPoint.x = (NiUInt32)((kOffset.x + fCellTexRange) * fTexWidth);
    kEndPoint.y = (NiUInt32)((kOffset.y + fCellTexRange) * fTexWidth);

    float fRatio = 1.0f / (float)pkLeaf->GetWidthInVerts();

    NiPoint2 kCurrentIndex((float)iX, (float)iY);

    float fParentWidth = (float)((pkLeaf->GetWidthInVerts() - 1));
    if (kCurrentIndex.x >= fParentWidth || kCurrentIndex.y >= fParentWidth)
        return;

    float fMaxSum0 = 0.0f;
    float fCurrentBlendedValue0 = 0.0f;
    float fMaxSum1 = 0.0f;
    float fCurrentBlendedValue1 = 0.0f;
    efd::UInt32 uiCellWidth = (efd::UInt32)fTexWidth - 1;
    for (NiUInt32 ui = 0; ui < pkLeaf->GetSurfaceCount(); ++ui)
    {
        NiUInt16 usFloorX = (NiUInt16)((kCurrentIndex.x * fRatio * kEndPoint.x));
        NiUInt16 usFloorY = (NiUInt16)((kCurrentIndex.y * fRatio * kEndPoint.y));

        NiUInt16 usCeilX = (NiUInt16)(((kCurrentIndex.x + 1) * fRatio * kEndPoint.x));
        NiUInt16 usCeilY = (NiUInt16)(((kCurrentIndex.y + 1) * fRatio * kEndPoint.y));

        if (usFloorX == usCeilX)
            ++usCeilX;

        if (usFloorY == usCeilY)
            ++usCeilY;

        NiUInt32 uiSum0 = 0;
        NiUInt32 uiSum1 = 0;
        NiUInt8 ucCount0 = 0;
        NiUInt8 ucCount1 = 0;
        for (NiInt32 uj = usFloorY; uj < usCeilY; ++uj)
        {
            for (NiInt32 uk = usFloorX; uk < usCeilX; ++uk)
            {
                if (!bTessFlag)
                {
                    // in this case the line equation is y = x therefore
                    // is y > to x then we are in triangle 1 else we are in
                    // triangle 0;
                    if (usFloorY >= usFloorX)
                    {
                        uiSum1 += pkLeaf->GetPixelAt(
                            NiTerrainResourceManager::TextureType::BLEND_MASK, 
                            NiIndex(uk, uiCellWidth - uj), ui);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkLeaf->GetPixelAt(
                            NiTerrainResourceManager::TextureType::BLEND_MASK, 
                            NiIndex(uk, uiCellWidth - uj), ui);
                        ++ucCount0;
                    }
                }
                else
                {
                    // in this case the line equation is y = -x + 1 therefore
                    // is y > to -x + 1 then we are in triangle 0 else we are
                    // in triangle 1;
                    if (usFloorY < - usFloorX + 1)
                    {
                        uiSum1 += pkLeaf->GetPixelAt(
                            NiTerrainResourceManager::TextureType::BLEND_MASK, 
                            NiIndex(uk, uiCellWidth - uj), ui);
                        ++ucCount1;
                    }
                    else
                    {
                        uiSum0 += pkLeaf->GetPixelAt(
                            NiTerrainResourceManager::TextureType::BLEND_MASK, 
                            NiIndex(uk, uiCellWidth - uj), ui);
                        ++ucCount0;
                    }
                }
            }
        }

        // if one ucCount = 0 then the mask is smaller than the terrain and
        // one of the triangles will not be given a material therefore both
        // triangle should have the same material
        if (ucCount0 == 0)
        {
            uiSum0 = uiSum1;
            ucCount0 = ucCount1;
        }
        else if (ucCount1 == 0)
        {
            uiSum1 = uiSum0;
            ucCount1 = ucCount0;
        }

        float fSum0 = (float)(uiSum0) / (ucCount0);
        float fSum1 = (float)(uiSum1) / (ucCount1);

        float fAvailValue0 = 255 - fCurrentBlendedValue0;
        if (fSum0 > fAvailValue0)
            fSum0 = fAvailValue0;

        fCurrentBlendedValue0 += fSum0;

        float fAvailValue1 = 255 - fCurrentBlendedValue1;
        if (fSum1 > fAvailValue1)
            fSum1 = fAvailValue1;

        fCurrentBlendedValue1 += fSum1;

        efd::SInt32 matIndex = pkLeaf->GetSurfaceIndex(ui);
        if (fSum0 >= fMaxSum0)
        {
            fMaxSum0 = fSum0;            
            if (matIndex >= 0)
                usMatIndex0 = (NiUInt16)matIndex;
        }

        if (fSum1 >= fMaxSum1)
        {
            fMaxSum1 = fSum1;
            if (matIndex >= 0)
                usMatIndex1 = (NiUInt16)matIndex;
        }
    }
}

//--------------------------------------------------------------------------------------------------

