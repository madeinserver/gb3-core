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

#include "NiTerrainPCH.h"
#include "NiTerrain.h"
#include "NiTerrainSectorSelector.h"
#include "NiTerrainSectorPager.h"

//--------------------------------------------------------------------------------------------------
NiImplementRTTI(NiTerrainSectorPager, NiTerrainSectorSelector);
//--------------------------------------------------------------------------------------------------
NiTerrainSectorPager::NiTerrainSectorPager(NiTerrain* pkTerrain):
    NiTerrainSectorSelector(pkTerrain),
    m_fLoadDistance(10.0f),
    m_fMaxLODLoadDistance(10.0f),
    m_fUnloadDistanceTolerance(0.0f),
    m_uiCurrentRefObject(0)
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorPager::~NiTerrainSectorPager()
{

}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorPager::NiTerrainSectorPager(NiTerrain* pkTerrain, NiAVObject* pkRefObject) :
    NiTerrainSectorSelector(pkTerrain),
    m_fLoadDistance(10.0f),
    m_fMaxLODLoadDistance(10.0f),
    m_fUnloadDistanceTolerance(0.0f),
    m_uiCurrentRefObject(0)
{
    // Associate the reference object with this manager
    AddReferenceObject(pkRefObject);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorPager::UpdateSectorSelection()
{
    FindNextSectorsToUnload();
    FindNextSectorsToLoad();

    return m_kSelectedSector.size() > 0;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorPager::FindNextSectorsToLoad()
{
    // Ensure we have what we need to calculate the next sector to load
    const NiAVObject* pkReferenceObject = GetCurrentReferenceObject();
    if (!m_pkTerrain || !pkReferenceObject)
        return;

    // Calculate some basic factors.
    float fSectorWidth = (float)m_pkTerrain->GetCalcSectorSize() - 1.0f;

    // Convert the ref point to terrain space
    NiTransform kTerrainTransform = m_pkTerrain->GetWorldTransform();
    NiTransform kInverseTerrainTransform;
    kTerrainTransform.Invert(kInverseTerrainTransform);
    NiPoint3 kRefPoint3 = kInverseTerrainTransform * 
        pkReferenceObject->GetWorldTranslate();
    NiPoint2 kRefPoint = NiPoint2(kRefPoint3.x, kRefPoint3.y);
    NiPoint2 kCenterSectorID = NiPoint2(
        NiFloor(kRefPoint.x / fSectorWidth + 0.5f),
        NiFloor(kRefPoint.y / fSectorWidth + 0.5f));

    // parse through the potential sectors
    NiUInt32 uiNumSpiralLevels = NiUInt32(m_fLoadDistance / fSectorWidth) + 1;
    for (NiUInt32 uiSpiralID = 0; uiSpiralID <= uiNumSpiralLevels; ++uiSpiralID)
    {
        // We check the sectors in a spiral arround the current page node.
        // The number of sectors to be checked for each iteration depends on 
        // uiXLevels which represents the number of spirals arround the center 
        // sector.
        NiPoint2 kCurrentIndex(
            kCenterSectorID.x + uiSpiralID, kCenterSectorID.y + uiSpiralID);
        for (NiUInt32 uiSpiralIndex = 0; uiSpiralIndex <= uiSpiralID * 8; ++uiSpiralIndex)
        {
            // This code will offset the current sector index stepping along 
            // the side the spiral.
            if (uiSpiralIndex != 0)
            {
                if (uiSpiralIndex < uiSpiralID * 2 + 1)
                {
                    // Going from right to left as we started from the sector 
                    // which is top right of the center sector.
                    kCurrentIndex.x--; 
                }
                else if (uiSpiralIndex < uiSpiralID * 4 + 1)
                {
                    // We are now going down on the left hand side of the 
                    // center sector. we have by now checked all the 
                    // sectors above the center sector for this spiral
                    kCurrentIndex.y--;
                }
                else if (uiSpiralIndex < uiSpiralID * 6 + 1)
                {
                    // We have checked the sectors above and on the left of 
                    // the center sector. We will now step through the sectors 
                    // that are below the center sector
                    kCurrentIndex.x++; 
                }
                else
                {
                    // Finally we check the sectors that are on the right hand 
                    // side of the center sector. once we have stepped all the 
                    // way we will be starting a new spiral further away from 
                    // the center sector
                    kCurrentIndex.y++;
                } 
            }

            NiInt16 sIndexX = (NiInt16)kCurrentIndex.x;
            NiInt16 sIndexY = (NiInt16)kCurrentIndex.y;

            // Check that this sector is available for paging at this point in
            // time
            if (!CheckSectorIsOnDisk(sIndexX, sIndexY))
                continue;
            
            // Calculate the LoD that sector should be loaded at
            float fDistance = CalculateMinDistanceToSector(sIndexX, sIndexY, kRefPoint);
            NiInt32 iLODToLoad = CalculateMinSectorLOD(fDistance);
            // We should not load a sector with a negative lod
            if (iLODToLoad < 0)
                continue;

            // Now that we have the sector ID, check if it should be loaded
            NiTerrainSector* pkCurrentSector = m_pkTerrain->GetSector(sIndexX, sIndexY);
            NiInt32 iTargetLoD = 0;
            if (pkCurrentSector)
            {
                NiInt32 iSectorLoadedLevel = 
                    pkCurrentSector->GetSectorData()->GetHighestLoadedLOD();

                // Is the sector already loaded at this level?
                if (iSectorLoadedLevel >= iLODToLoad)
                    continue;

                // We will want to increase the LOD by 1
                iTargetLoD = iSectorLoadedLevel + 1;

                // Check that its adjacent sectors are loaded to
                // at least the same amount as this sector (preventing cracks)
                const NiInt16 asAdjacentSectorOffsetX[] = {0, 0, 1, -1};
                const NiInt16 asAdjacentSectorOffsetY[] = {1, -1, 0, 0};
                bool bAdjacentSectorsLoaded = true;

                for (NiUInt32 uiAdjSector = 0; uiAdjSector < 4; ++uiAdjSector)
                {
                    NiInt16 sAdjacentIndexX = sIndexX + asAdjacentSectorOffsetX[uiAdjSector];
                    NiInt16 sAdjacentIndexY = sIndexY + asAdjacentSectorOffsetY[uiAdjSector];

                    // Check that this sector is even available to be paged
                    if (!CheckSectorIsOnDisk(sAdjacentIndexX, sAdjacentIndexY))
                        continue;

                    // Check if the adjacent sector requires more detail
                    fDistance = CalculateMinDistanceToSector( 
                        sAdjacentIndexX, sAdjacentIndexY, kRefPoint);
                    NiInt32 iAdjacentLODToLoad = CalculateMinSectorLOD(fDistance);

                    // Does the adjacent sector want to be unloaded?
                    if (iAdjacentLODToLoad == -1)
                        continue;

                    // Check that the adjacent sector is already loaded to the required LOD
                    NiTerrainSector* pkAdjSector = m_pkTerrain->GetSector(sAdjacentIndexX, 
                        sAdjacentIndexY);

                    if (pkAdjSector && 
                        pkAdjSector->GetSectorData()->GetHighestLoadedLOD() < iSectorLoadedLevel)
                    {
                        bAdjacentSectorsLoaded = false;
                        break;
                    }

                    // Check that the sector has actually been created. If it hasn't been created, 
                    // then we can only load our sector to level 0
                    if (!pkAdjSector && (iSectorLoadedLevel > 0))
                    {
                        bAdjacentSectorsLoaded = false;
                        break;
                    }

                }

                if (!bAdjacentSectorsLoaded)
                    continue;
            }

            AddToSelection(sIndexX, sIndexY, iTargetLoD);
        }
    }

    // Set to look around the next page node on the next iteration.
    if (m_uiCurrentRefObject < m_kReferenceObjects.GetSize() - 1)
        ++m_uiCurrentRefObject;
    else
        m_uiCurrentRefObject = 0;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorPager::FindNextSectorsToUnload()
{
    const NiAVObject* pkReferenceObject = GetCurrentReferenceObject();
    if (!m_pkTerrain || !pkReferenceObject)
        return;

    // Convert the ref point to terrain space
    NiTransform kTerrainTransform = m_pkTerrain->GetWorldTransform();
    NiTransform kInverseTerrainTransform;
    kTerrainTransform.Invert(kInverseTerrainTransform);
    NiPoint3 kRefPoint3 = kInverseTerrainTransform * pkReferenceObject->GetWorldTranslate();
    NiPoint2 kRefPoint = NiPoint2(kRefPoint3.x, kRefPoint3.y);

    // Parse through the loaded sector and check if they should be unloaded
    const NiTMap<NiUInt32, NiTerrainSector*>& kSectorMap = m_pkTerrain->GetLoadedSectors();
    NiTMapIterator kIter = kSectorMap.GetFirstPos();
    while(kIter)
    {
        NiTerrainSector* pkSector;
        NiUInt32 uiKey;
        kSectorMap.GetNext(kIter, uiKey, pkSector);
        if (pkSector->GetSectorData()->GetHighestLoadedLOD() < 0)
            continue;

        NiInt16 sIndexX, sIndexY;
        pkSector->GetSectorIndex(sIndexX, sIndexY);

        // Find the shortest distance to a reference object
        float fDistance = FLT_MAX;
        for(NiUInt32 ui = 0; ui < m_kReferenceObjects.GetSize(); ++ui)
        {
            // Only unload lods according to the closest ref point
            const NiAVObject* pkTempReferenceObject = 
                m_kReferenceObjects.GetAt(ui);

            NiPoint3 kTempRefPoint3 = kInverseTerrainTransform * 
                pkTempReferenceObject->GetWorldTranslate();
            NiPoint2 kTempRefPoint = NiPoint2(kTempRefPoint3.x, 
                kTempRefPoint3.y);
            
            // Calculate the distance to this point
            float fTempDistance = CalculateMinDistanceToSector(sIndexX, sIndexY, 
                kTempRefPoint);

            // Select the shortest distance
            if (fTempDistance < fDistance)
                fDistance = fTempDistance;     
        }

        // Fetch the maximum that this sector should be loaded to
        NiInt32 iLODToLoad = CalculateMaxSectorLOD(fDistance);

        // only worry about levels of detail to unload
        if (pkSector->GetSectorData()->GetHighestLoadedLOD() <= iLODToLoad)
            continue;

        AddToSelection(sIndexX, sIndexY, iLODToLoad);
    }
}

//--------------------------------------------------------------------------------------------------
NiInt32 NiTerrainSectorPager::CalculateMinSectorLOD(float fDistanceToSector)
{
    // Calculate some useful constants
    NiInt32 iNumLOD = m_pkTerrain->GetNumLOD();
    float fSectorDiameter = float(m_pkTerrain->GetCalcSectorSize()) - 1.0f;
    fSectorDiameter *= NiSqrt(2.0f);

    // If the sector is beyond the maximum load distance then don't load
    // any of it
    if (fDistanceToSector > m_fLoadDistance)
        return -1;

    // Count the number of sectors past the MAXLOD load line this sector is
    float fDistancePastMaxLOD = fDistanceToSector - m_fMaxLODLoadDistance;
    NiInt32 iNumSectorsOut = NiInt32(fDistancePastMaxLOD / fSectorDiameter);

    // Reduce the proposed LOD for every sector past the MAXLOD load line.
    NiInt32 iProposedLOD = iNumLOD - iNumSectorsOut;
    return NiClamp(iProposedLOD, 0, iNumLOD);
}

//--------------------------------------------------------------------------------------------------
NiInt32 NiTerrainSectorPager::CalculateMaxSectorLOD(float fDistanceToSector)
{
    // The Max Sector LOD is governed by the same rules as the minimum sector
    // LOD, except the range over which an LOD may remain loaded is extended
    // according to the tolerance setting. 
    return CalculateMinSectorLOD(fDistanceToSector - m_fUnloadDistanceTolerance);
}

//--------------------------------------------------------------------------------------------------
float NiTerrainSectorPager::CalculateMinDistanceToSector(
    NiInt16 sIndexX, NiInt16 sIndexY, NiPoint2 kRefPoint)
{
    // Calculate the center of the sector
    float fSectorWidth = (float)(m_pkTerrain->GetCalcSectorSize() - 1);
    NiPoint2 kSectorCenter((float)sIndexX * fSectorWidth, (float)sIndexY * fSectorWidth);

    // Calculate the distance from the ref point to the sector's center
    float fSectorDistance = (kSectorCenter - kRefPoint).Length();

    // Subtract the possible radius that the sector represents
    static const float SQRT2on2 = NiSqrt(2.0f) * 0.5f;
    fSectorDistance -= fSectorWidth * SQRT2on2;

    return fSectorDistance;
}

//--------------------------------------------------------------------------------------------------