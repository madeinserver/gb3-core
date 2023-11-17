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
#include "NiTerrainSector.h"
#include "NiTerrainSectorFile.h"
#include "NiTerrainSectorFileVersion1.h"
#include "NiTerrainSectorFileVersion2.h"
#include "NiTerrainSectorFileVersion3.h"
#include "NiTerrainSectorFileVersion4.h"
#include "NiTerrainSectorFileVersion5.h"
#include "NiTerrainSectorFileVersion6.h"

#include <efd/File.h>
#include <NiBound.h>

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFile* NiTerrainSectorFile::Open(const char* pcTerrainArchive, NiInt32 iSectorX, 
    NiInt32 iSectorY, bool bWriteAccess)
{
    NiTerrainSectorFile* pkFile = NULL;

    // Work out the access mode
    efd::File::OpenMode eAccessMode = efd::File::READ_ONLY;
    if (bWriteAccess)
        eAccessMode = efd::File::WRITE_ONLY;

    // Determine the version of the data
    FileVersion kVersion = ms_kFileVersion;
    if (!bWriteAccess)
    {
        // We are reading data, so read the file version from the data
        kVersion = NiTerrainSectorFileVersion6::DetectFileVersion(pcTerrainArchive, 
            iSectorX, iSectorY);
    }

    // Create the appropriate object for the version
    switch (kVersion)
    {
    case ms_kFileVersion1:
        pkFile = NiTerrainSectorFileVersion5::Create<NiTerrainSectorFileVersion1>(
            pcTerrainArchive, iSectorX, iSectorY, eAccessMode);
        break;
    case ms_kFileVersion2:
        pkFile = NiTerrainSectorFileVersion5::Create<NiTerrainSectorFileVersion2>(
            pcTerrainArchive, iSectorX, iSectorY, eAccessMode);
        break;
    case ms_kFileVersion3:
        pkFile = NiTerrainSectorFileVersion5::Create<NiTerrainSectorFileVersion3>(
            pcTerrainArchive, iSectorX, iSectorY, eAccessMode);
        break;
    case ms_kFileVersion4:
        pkFile = NiTerrainSectorFileVersion5::Create<NiTerrainSectorFileVersion4>(
            pcTerrainArchive, iSectorX, iSectorY, eAccessMode);
        break;
    case ms_kFileVersion5:
        pkFile = NiTerrainSectorFileVersion5::Create<NiTerrainSectorFileVersion5>(
            pcTerrainArchive, iSectorX, iSectorY, eAccessMode);
        break;
    case ms_kFileVersion:
        pkFile = EE_NEW NiTerrainSectorFileVersion6(pcTerrainArchive, iSectorX, iSectorY, 
            eAccessMode);
        break;
    default:
        break;
    }

    // Initialize the file object
    if (pkFile && !pkFile->Initialize())
    {
        pkFile->Close();
        EE_DELETE pkFile;
        pkFile = NULL;
    }

    return pkFile;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::IsReady() const
{
    return m_bOpen;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::IsWritable() const
{
    return m_kAccessMode & efd::File::WRITE_ONLY;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::Close()
{
    m_bOpen = false;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFile::NiTerrainSectorFile(const char* pcTerrainArchive, NiInt32 iSectorX, 
    NiInt32 iSectorY, efd::File::OpenMode kAccessMode):
    m_bOpen(false),
    m_kAccessMode(kAccessMode),
    m_kTerrainArchive(pcTerrainArchive),
    m_iSectorX(iSectorX),
    m_iSectorY(iSectorY),
    m_eCachedData(0)
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFile::~NiTerrainSectorFile()
{
    EE_ASSERT(!m_bOpen);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::Initialize()
{
    m_bOpen = true;
    return IsReady();
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFile::FileVersion NiTerrainSectorFile::GetCurrentVersion()
{
    return ms_kFileVersion;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::Precache(efd::UInt32 uiBeginLevel, efd::UInt32 uiEndLevel, 
    efd::UInt32 eData)
{
    EE_UNUSED_ARG(uiBeginLevel);
    EE_UNUSED_ARG(uiEndLevel);
    EE_UNUSED_ARG(eData);
    m_eCachedData = eData;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::IsDataReady(DataField::Value eDataField)
{
    return (m_eCachedData & eDataField) != 0;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadSectorConfig(efd::UInt32& uiSectorWidthInVerts, efd::UInt32& uiNumLOD)
{
    EE_UNUSED_ARG(uiSectorWidthInVerts);
    EE_UNUSED_ARG(uiNumLOD);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteSectorConfig(efd::UInt32 uiSectorWidthInVerts, efd::UInt32 uiNumLOD)
{
    EE_UNUSED_ARG(uiSectorWidthInVerts);
    EE_UNUSED_ARG(uiNumLOD);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadHeights(efd::UInt16* pusHeights, efd::UInt32 uiLength)
{
    EE_UNUSED_ARG(pusHeights);
    EE_UNUSED_ARG(uiLength);
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadNormals(efd::Point2* pkNormals, efd::UInt32 uiDataLength)
{
    EE_UNUSED_ARG(pkNormals);
    EE_UNUSED_ARG(uiDataLength);
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadTangents(efd::Point2* pkTangents, efd::UInt32 uiDataLength)
{
    EE_UNUSED_ARG(pkTangents);
    EE_UNUSED_ARG(uiDataLength);
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadLowDetailDiffuseMap(NiPixelData*& pkLowDetailDiffuse)
{
    EE_UNUSED_ARG(pkLowDetailDiffuse);
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadLowDetailNormalMap(NiPixelData*& pkLowDetailNormal)
{
    EE_UNUSED_ARG(pkLowDetailNormal);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteHeights(efd::UInt16* pusHeights, efd::UInt32 uiDataLength)
{
    EE_UNUSED_ARG(pusHeights);
    EE_UNUSED_ARG(uiDataLength);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteNormals(efd::Point2* pkNormals, efd::UInt32 uiDataLength)
{
    EE_UNUSED_ARG(pkNormals);
    EE_UNUSED_ARG(uiDataLength);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteTangents(efd::Point2* pkTangents, efd::UInt32 uiDataLength)
{
    EE_UNUSED_ARG(pkTangents);
    EE_UNUSED_ARG(uiDataLength);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteLowDetailDiffuseMap(NiPixelData* pkLowDetailDiffuse)
{
    EE_UNUSED_ARG(pkLowDetailDiffuse);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteLowDetailNormalMap(NiPixelData* pkLowDetailNormal)
{
    EE_UNUSED_ARG(pkLowDetailNormal);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadCellSurfaceIndex(efd::UInt32 uiCellRegionID, efd::UInt32 uiNumCells, 
    LeafData* pkCellData)
{
    EE_UNUSED_ARG(uiCellRegionID);
    EE_UNUSED_ARG(uiNumCells);
    EE_UNUSED_ARG(pkCellData);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteCellSurfaceIndex(efd::UInt32 uiCellRegionID, efd::UInt32 uiNumCells, 
    LeafData* pkCellData)
{
    EE_UNUSED_ARG(uiCellRegionID);
    EE_UNUSED_ARG(uiNumCells);
    EE_UNUSED_ARG(pkCellData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadCellBoundData(efd::UInt32 uiCellRegionID, efd::UInt32 uiNumCells, 
    CellData* pkCellData)
{
    EE_UNUSED_ARG(uiCellRegionID);
    EE_UNUSED_ARG(uiNumCells);
    EE_UNUSED_ARG(pkCellData);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteCellBoundData(efd::UInt32 uiCellRegionID, efd::UInt32 uiNumCells, 
    CellData* pkCellData)
{
    EE_UNUSED_ARG(uiCellRegionID);
    EE_UNUSED_ARG(uiNumCells);
    EE_UNUSED_ARG(pkCellData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadBlendMask(NiPixelData*& pkBlendMask)
{
    EE_UNUSED_ARG(pkBlendMask);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteBlendMask(NiPixelData* pkBlendMask)
{
    EE_UNUSED_ARG(pkBlendMask);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFile::ReadTerrainSectorPhysXData(
    efd::map<efd::UInt32, NiPhysXMaterialMetaData>& kMaterialMap, 
    NiTerrainSectorPhysXData*& pkSampleData)
{
    EE_UNUSED_ARG(kMaterialMap);
    EE_UNUSED_ARG(pkSampleData);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFile::WriteTerrainSectorPhysXData(
    efd::map<efd::UInt32, NiPhysXMaterialMetaData> kMaterialMap, 
    NiTerrainSectorPhysXData* pkSampleData)
{
    EE_UNUSED_ARG(kMaterialMap);
    EE_UNUSED_ARG(pkSampleData);
}

//--------------------------------------------------------------------------------------------------
