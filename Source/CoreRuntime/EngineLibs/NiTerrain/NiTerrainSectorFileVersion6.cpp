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
#include "NiTerrainSectorFileVersion6.h"
#include "NiTerrainSectorFileVersion5.h"

#include <efd/File.h>
#include <NiBound.h>

const char* NiTerrainSectorFileVersion6::ms_pcSectorFilename = ".dof";
//--------------------------------------------------------------------------------------------------
NiTerrainSectorFile::FileVersion NiTerrainSectorFileVersion6::DetectFileVersion(
    const char* pcTerrainArchive, NiInt32 iSectorX, NiInt32 iSectorY)
{
    FileVersion eVersion = 0;

    // Attempt to access the file
    efd::utf8string kFilename;
    kFilename.sprintf("%s\\Sector_%d_%d%s", pcTerrainArchive, iSectorX, iSectorY,
        ms_pcSectorFilename);
    if (efd::File::Access(kFilename.c_str(), efd::File::READ_ONLY))
        eVersion = ms_kFileVersion6;

    if (!eVersion)
        return NiTerrainSectorFileVersion5::DetectFileVersion(pcTerrainArchive, iSectorX, iSectorY);

    return eVersion;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::FileVersion NiTerrainSectorFileVersion6::GetFileVersion() const
{
    return ms_kFileVersion6;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::Close()
{
    if (IsWritable())
    {
        WriteFile();
    }

    m_bOpen = false;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::NiTerrainSectorFileVersion6(const char* pcTerrainArchive, 
    NiInt32 iSectorX, NiInt32 iSectorY, efd::File::OpenMode kAccessMode):
NiTerrainSectorFile(pcTerrainArchive, iSectorX, iSectorY, kAccessMode)
{
    // Initialize the block data pointers
    for (efd::UInt32 uiIndex = 0; uiIndex < DataBlockType::NUM_BLOCK_TYPES; ++uiIndex)
    {
        m_apkDataBlocks[uiIndex] = 0;
    }
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::~NiTerrainSectorFileVersion6()
{
    // Free the block data pointers
    for (efd::UInt32 uiIndex = 0; uiIndex < DataBlockType::NUM_BLOCK_TYPES; ++uiIndex)
    {
        EE_DELETE m_apkDataBlocks[uiIndex];
    }
}

//--------------------------------------------------------------------------------------------------
efd::utf8string NiTerrainSectorFileVersion6::GenerateFilename()
{
    efd::utf8string kFilename;
    kFilename.sprintf("%s\\Sector_%d_%d%s", (const char*)m_kTerrainArchive, m_iSectorX, m_iSectorY, 
        ms_pcSectorFilename);
    return kFilename;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::Initialize()
{
    bool bResult = true;

    // Check that we have the correct type of access to the files we need
    bool bAccess = true;
    efd::utf8string kFilename = GenerateFilename();
    if (m_kAccessMode == efd::File::READ_ONLY)
    {
        // Must at least have access to the heightmap of this sector
        bAccess &= efd::File::Access(kFilename.c_str(), m_kAccessMode);
    }
    else
    {
        efd::utf8string kPath = efd::PathUtils::PathRemoveFileName(kFilename);
        bAccess &= efd::File::CreateDirectoryRecursive(kPath.c_str());
    }
    bResult &= bAccess;

    // Return false if we have failed
    if (!bResult)
        return false;

    return NiTerrainSectorFile::Initialize();
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::FileVersion NiTerrainSectorFileVersion6::GetCurrentVersion()
{
    return ms_kFileVersion;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::GetFilePaths(efd::set<efd::utf8string>& kFilePaths)
{
    kFilePaths.insert(GenerateFilename());
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::Precache(efd::UInt32 uiBeginLevel, efd::UInt32 uiEndLevel, 
    efd::UInt32 uiData)
{
    bool bResult = true;
    efd::File* pkFile = efd::File::GetFile(GenerateFilename().c_str(), efd::File::READ_ONLY);
    if (!pkFile)
        return false;

    // Setup the endianess
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    pkFile->SetEndianSwap(!bPlatformLittle);

    // Read the file header
    FileHeader kFileHeader;
    bResult &= ReadFileHeader(*pkFile, kFileHeader);

    // Refine which data we will load based on the present data and the requested data
    uiData = uiData & kFileHeader.m_uiPresentData;
    m_eCachedData = uiData;

    // Read the data stored in the file
    bResult &= ReadDataBlocks(*pkFile, uiBeginLevel, uiEndLevel, uiData);

    EE_DELETE pkFile;
    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadSectorConfig(efd::UInt32& uiSectorWidthInVerts, 
    efd::UInt32& uiNumLOD)
{
    ConfigDataBlock* pkConfig = (ConfigDataBlock*)m_apkDataBlocks[DataBlockType::CONFIG];
    if (!pkConfig)
        return false;

    uiSectorWidthInVerts = pkConfig->m_uiSectorWidthInVerts;
    uiNumLOD = pkConfig->m_uiNumLOD;

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteSectorConfig(efd::UInt32 uiSectorWidthInVerts, 
    efd::UInt32 uiNumLOD)
{
    ConfigDataBlock* pkConfig = EE_NEW ConfigDataBlock();
    pkConfig->m_uiDataLength = sizeof(efd::UInt32) * 2;
    pkConfig->m_eBlockType = DataBlockType::CONFIG;
    pkConfig->m_uiSectorWidthInVerts = uiSectorWidthInVerts;
    pkConfig->m_uiNumLOD = uiNumLOD;

    m_apkDataBlocks[pkConfig->m_eBlockType] = pkConfig;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadHeights(efd::UInt16* pusHeights, efd::UInt32 uiLength)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::HEIGHTS];
    if (!pkImage)
        return false;
    efd::UInt32 uiNumBytes = uiLength * sizeof(efd::UInt16);
    EE_ASSERT(pkImage->m_uiHeight * pkImage->m_uiWidth * pkImage->m_usNumChannels * 
        pkImage->m_usBytesPerChannel == uiNumBytes);
    return pkImage->DecompressToStream((efd::UInt8*)pusHeights, uiNumBytes);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadNormals(efd::Point2* pkNormals, efd::UInt32 uiDataLength)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::NORMALS];
    if (!pkImage)
        return false;
    efd::UInt32 uiNumBytes = uiDataLength * sizeof(efd::Point2);
    EE_ASSERT(pkImage->m_uiHeight * pkImage->m_uiWidth * pkImage->m_usNumChannels * 
        pkImage->m_usBytesPerChannel == uiNumBytes);
    return pkImage->DecompressToStream((efd::UInt8*)pkNormals, uiNumBytes);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadTangents(efd::Point2* pkTangents, efd::UInt32 uiDataLength)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::TANGENTS];
    if (!pkImage)
        return false;
    efd::UInt32 uiNumBytes = uiDataLength * sizeof(efd::Point2);
    EE_ASSERT(pkImage->m_uiHeight * pkImage->m_uiWidth * pkImage->m_usNumChannels *
        pkImage->m_usBytesPerChannel == uiNumBytes);
    return pkImage->DecompressToStream((efd::UInt8*)pkTangents, uiNumBytes);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadLowDetailDiffuseMap(NiPixelData*& pkLowDetailDiffuse)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::LOWDETAIL_DIFFUSE];
    if (!pkImage)
        return false;
    return pkImage->DecompressToImageData(pkLowDetailDiffuse);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadLowDetailNormalMap(NiPixelData*& pkLowDetailNormal)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::LOWDETAIL_NORMALS];
    if (!pkImage)
        return false;
    return pkImage->DecompressToImageData(pkLowDetailNormal);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteHeights(efd::UInt16* pusHeights, efd::UInt32 uiDataLength)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();

    efd::UInt32 uiWidth = efd::UInt32(efd::Sqrt((float)uiDataLength));
    EE_ASSERT(uiWidth * uiWidth == uiDataLength);

    efd::UInt32 uiStride = sizeof(efd::UInt16);
    efd::UInt32 uiNumBytes = uiDataLength * uiStride;
    pkImage->m_eBlockType = DataBlockType::HEIGHTS;
    pkImage->CompressFromStream(ImageCompressionMode::NONE, (efd::UInt8*)pusHeights, uiNumBytes, 
        uiWidth, uiWidth, 1, sizeof(efd::UInt16));

    m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteNormals(efd::Point2* pkNormals, efd::UInt32 uiDataLength)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();

    efd::UInt32 uiWidth = efd::UInt32(efd::Sqrt((float)uiDataLength));
    EE_ASSERT(uiWidth * uiWidth == uiDataLength);

    efd::UInt32 uiStride = sizeof(efd::Point2);
    efd::UInt32 uiNumBytes = uiDataLength * uiStride;
    pkImage->m_eBlockType = DataBlockType::NORMALS;
    pkImage->CompressFromStream(ImageCompressionMode::NONE, (efd::UInt8*)pkNormals, uiNumBytes, 
        uiWidth, uiWidth, 2, sizeof(float));

    m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteTangents(efd::Point2* pkTangents, efd::UInt32 uiDataLength)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();

    efd::UInt32 uiWidth = efd::UInt32(efd::Sqrt((float)uiDataLength));
    EE_ASSERT(uiWidth * uiWidth == uiDataLength);

    efd::UInt32 uiStride = sizeof(efd::Point2);
    efd::UInt32 uiNumBytes = uiDataLength * uiStride;
    pkImage->m_eBlockType = DataBlockType::TANGENTS;
    pkImage->CompressFromStream(ImageCompressionMode::NONE, (efd::UInt8*)pkTangents, uiNumBytes, 
        uiWidth, uiWidth, 2, sizeof(float));

    m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteLowDetailDiffuseMap(NiPixelData* pkLowDetailDiffuse)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();
    pkImage->m_eBlockType = DataBlockType::LOWDETAIL_DIFFUSE;
    if (pkImage->CompressFromImageData(ImageCompressionMode::NONE, pkLowDetailDiffuse))
        m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteLowDetailNormalMap(NiPixelData* pkLowDetailNormal)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();
    pkImage->m_eBlockType = DataBlockType::LOWDETAIL_NORMALS;
    if (pkImage->CompressFromImageData(ImageCompressionMode::NONE, pkLowDetailNormal))
        m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadCellSurfaceIndex(efd::UInt32 uiCellRegionID, 
    efd::UInt32 uiNumCells, LeafData* pkLeafData)
{
    SurfaceIndexBlock* pkSurfaceIndex = (SurfaceIndexBlock*)
        m_apkDataBlocks[DataBlockType::SURFACE_INDEXES];
    if (!pkSurfaceIndex)
        return false;

    EE_ASSERT(uiNumCells <= pkSurfaceIndex->m_uiNumLeaves);
    EE_ASSERT(uiCellRegionID >= pkSurfaceIndex->m_uiStartLeaf);

    for (efd::UInt32 uiIndex = 0; uiIndex < uiNumCells; ++uiIndex)
    {
        pkLeafData[uiIndex] = pkSurfaceIndex->m_pkSurfaceIndexData[
            uiIndex + (uiCellRegionID - pkSurfaceIndex->m_uiStartLeaf)];
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteCellSurfaceIndex(efd::UInt32 uiCellRegionID, 
    efd::UInt32 uiNumCells, LeafData* pkCellData)
{
    SurfaceIndexBlock* pkSurfaceIndex = EE_NEW SurfaceIndexBlock();
    EE_ASSERT(uiCellRegionID == 0);

    pkSurfaceIndex->m_uiDataLength = uiNumCells * SurfaceIndexBlock::ms_uiLeafDataSize;
    pkSurfaceIndex->m_eBlockType = DataBlockType::SURFACE_INDEXES;
    pkSurfaceIndex->m_uiNumLeaves = uiNumCells;
    pkSurfaceIndex->m_uiStartLeaf = uiCellRegionID;
    pkSurfaceIndex->m_pkSurfaceIndexData = EE_ALLOC(LeafData, uiNumCells);

    efd::Memcpy(pkSurfaceIndex->m_pkSurfaceIndexData, pkCellData, uiNumCells * sizeof(LeafData));
    m_apkDataBlocks[pkSurfaceIndex->m_eBlockType] = pkSurfaceIndex;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadCellBoundData(efd::UInt32 uiCellRegionID, 
    efd::UInt32 uiNumCells, CellData* pkCellData)
{
    BoundingDataBlock* pkBounds = (BoundingDataBlock*)m_apkDataBlocks[DataBlockType::BOUNDS];
    if (!pkBounds)
        return false;

    EE_ASSERT(uiNumCells <= pkBounds->m_uiNumCells);
    EE_ASSERT(uiCellRegionID >= pkBounds->m_uiStartCell);

    for (efd::UInt32 uiIndex = 0; uiIndex < uiNumCells; ++uiIndex)
    {
        pkCellData[uiIndex] = 
            pkBounds->m_pkBoundingData[uiIndex + (uiCellRegionID - pkBounds->m_uiStartCell)];
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteCellBoundData(efd::UInt32 uiCellRegionID, 
    efd::UInt32 uiNumCells, CellData* pkCellData)
{
    BoundingDataBlock* pkBounds = EE_NEW BoundingDataBlock();
    EE_ASSERT(uiCellRegionID == 0);

    pkBounds->m_uiDataLength = uiNumCells * BoundingDataBlock::ms_uiCellDataSize;
    pkBounds->m_eBlockType = DataBlockType::BOUNDS;
    pkBounds->m_uiNumCells = uiNumCells;
    pkBounds->m_uiStartCell = uiCellRegionID;
    pkBounds->m_pkBoundingData = EE_ALLOC(CellData, uiNumCells);

    efd::Memcpy(pkBounds->m_pkBoundingData, pkCellData, uiNumCells * sizeof(CellData));
    m_apkDataBlocks[pkBounds->m_eBlockType] = pkBounds;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadBlendMask(NiPixelData*& pkBlendMask)
{
    ImageDataBlock* pkImage = (ImageDataBlock*)m_apkDataBlocks[DataBlockType::BLEND_MASK];
    if (!pkImage)
        return false;
    return pkImage->DecompressToImageData(pkBlendMask);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteBlendMask(NiPixelData* pkBlendMask)
{
    ImageDataBlock* pkImage = EE_NEW ImageDataBlock();
    pkImage->m_eBlockType = DataBlockType::BLEND_MASK;
    if (pkImage->CompressFromImageData(ImageCompressionMode::NONE, pkBlendMask))
        m_apkDataBlocks[pkImage->m_eBlockType] = pkImage;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadTerrainSectorPhysXData(
    efd::map<efd::UInt32, NiPhysXMaterialMetaData>& kMaterialMap, 
    NiTerrainSectorPhysXData*& pkSampleData)
{
    PhysXMaterialDataBlock* pkPhysXMatData = 
        (PhysXMaterialDataBlock*)m_apkDataBlocks[DataBlockType::PHYSXMATERIAL_DATA];

    if (!pkPhysXMatData)
        return false;

    kMaterialMap = pkPhysXMatData->m_kMaterialData;

    PhysXDataBlock* pkData = (PhysXDataBlock*)m_apkDataBlocks[DataBlockType::PHYSX_DATA];
    if (!pkData)
        return false;

    pkSampleData = pkData->m_pkPhysXData;

    return pkSampleData != NULL;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion6::WriteTerrainSectorPhysXData(
    efd::map<efd::UInt32, NiPhysXMaterialMetaData> kMaterialMap, 
    NiTerrainSectorPhysXData* pkSampleData)
{
    PhysXMaterialDataBlock* pkPhysXMatData = EE_NEW PhysXMaterialDataBlock();
    pkPhysXMatData->m_eBlockType = DataBlockType::PHYSXMATERIAL_DATA;
    pkPhysXMatData->m_kMaterialData = kMaterialMap;
    pkPhysXMatData->m_uiDataLength = sizeof(efd::UInt32) + kMaterialMap.size() * 
        PhysXMaterialDataBlock::ms_uiStaticDataSize;

    m_apkDataBlocks[pkPhysXMatData->m_eBlockType] = pkPhysXMatData;

    PhysXDataBlock* pkData = EE_NEW PhysXDataBlock();
    pkData->m_eBlockType = DataBlockType::PHYSX_DATA;
    pkData->m_pkPhysXData = pkSampleData;
    pkData->m_uiDataLength = 2 * sizeof(efd::UInt32) + 
        pkSampleData->m_uiNumSamples * PhysXDataBlock::ms_uiStaticDataSize;

    m_apkDataBlocks[pkData->m_eBlockType] = pkData;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::WriteFile()
{
    efd::File* pkFile = efd::File::GetFile(GenerateFilename().c_str(), efd::File::WRITE_ONLY);
    if (!pkFile)
        return false;

    // Setup the endianess
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    pkFile->SetEndianSwap(!bPlatformLittle);

    // Generate the header
    FileHeader kHeader;
    kHeader.m_kVersion = GetFileVersion();
    kHeader.m_uiPresentData = 0;
    for (efd::UInt32 uiField = 0; uiField < DataBlockType::NUM_BLOCK_TYPES; ++uiField)
    {
        if (m_apkDataBlocks[uiField])
            kHeader.m_uiPresentData |= (1 << uiField);
    }

    bool bResult = true;
    bResult &= WriteFileHeader(*pkFile, kHeader);
    bResult &= WriteDataBlocks(*pkFile);

    EE_DELETE pkFile;

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::WriteFileHeader(efd::BinaryStream& kStream, FileHeader& kHeader)
{
    efd::BinaryStreamSave(kStream, &kHeader.m_kVersion);
    efd::BinaryStreamSave(kStream, &kHeader.m_uiPresentData);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::WriteDataBlocks(efd::BinaryStream& kStream)
{
    bool bResult = true;
    for (efd::UInt32 uiField = 0; uiField < DataBlockType::NUM_BLOCK_TYPES; ++uiField)
    {
        DataBlock* pkBlock = m_apkDataBlocks[uiField];
        if (pkBlock)
        {
            bResult &= pkBlock->WriteBlockHeader(kStream);
            bResult &= pkBlock->WriteBlockData(kStream);
        }
    }

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadFileHeader(efd::BinaryStream& kStream, FileHeader& kHeader)
{
    efd::BinaryStreamLoad(kStream, &kHeader.m_kVersion);
    efd::BinaryStreamLoad(kStream, &kHeader.m_uiPresentData);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ReadDataBlocks(efd::BinaryStream& kStream, 
    efd::UInt32 uiBeginLevel, efd::UInt32 uiEndLevel, efd::UInt32 eSelectedData)
{
    // Read data blocks until the end of the file
    DataBlock kBlockHeader;
    while (DataBlock::ReadBlockHeader(kStream, kBlockHeader))
    {
        // Handle this block
        DataBlock* pkBlock = NULL;
        switch (kBlockHeader.m_eBlockType)
        {
        case DataBlockType::CONFIG:
            if (eSelectedData & DataField::CONFIG)
                pkBlock = EE_NEW ConfigDataBlock();
            break;
        case DataBlockType::HEIGHTS:
            if (eSelectedData & DataField::HEIGHTS)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::NORMALS:
            if (eSelectedData & DataField::NORMALS)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::TANGENTS:
            if (eSelectedData & DataField::TANGENTS)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::BLEND_MASK:
            if (eSelectedData & DataField::BLEND_MASK)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::LOWDETAIL_NORMALS:
            if (eSelectedData & DataField::LOWDETAIL_NORMALS)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::LOWDETAIL_DIFFUSE:
            if (eSelectedData & DataField::LOWDETAIL_DIFFUSE)
                pkBlock = EE_NEW ImageDataBlock();
            break;
        case DataBlockType::BOUNDS:
            if (eSelectedData & DataField::BOUNDS)
            {
                BoundingDataBlock* pkBounds = EE_NEW BoundingDataBlock();
                efd::UInt32 uiCellNumber = 0;
                pkBounds->m_uiStartCell = efd::UInt32(-1);
                pkBounds->m_uiNumCells = efd::UInt32(-1);
                for (efd::UInt32 uiLevel = 0; uiLevel <= uiEndLevel; ++uiLevel)
                {
                    efd::UInt32 uiNumCells = (1 << uiLevel) * (1 << uiLevel);

                    if (uiLevel == uiBeginLevel)
                        pkBounds->m_uiStartCell = uiCellNumber;
                    if (uiLevel == uiEndLevel)
                    {
                        pkBounds->m_uiNumCells = 
                            uiCellNumber + uiNumCells - pkBounds->m_uiStartCell;
                    }

                    uiCellNumber += uiNumCells;
                }
                EE_ASSERT(pkBounds->m_uiStartCell != efd::UInt32(-1));
                EE_ASSERT(pkBounds->m_uiNumCells != efd::UInt32(-1));
                pkBlock = pkBounds;
            }
            break;
        case DataBlockType::SURFACE_INDEXES:
            if (eSelectedData & DataField::SURFACE_INDEXES)
            {
                // Config block must have loaded
                ConfigDataBlock* pkConfigDataBlock = 
                    (ConfigDataBlock*)m_apkDataBlocks[DataBlockType::CONFIG];
                EE_ASSERT(pkConfigDataBlock);

                SurfaceIndexBlock* pkSurfaceIndex = EE_NEW SurfaceIndexBlock();
                pkSurfaceIndex->m_uiStartLeaf = 0;
                pkSurfaceIndex->m_uiNumLeaves = 0;
                efd::UInt32 uiNumLeaves = (1 << uiEndLevel) * (1 << uiEndLevel);
                if (uiEndLevel == pkConfigDataBlock->m_uiNumLOD)
                    pkSurfaceIndex->m_uiNumLeaves = uiNumLeaves;
                pkBlock = pkSurfaceIndex;
            }
            break;
        case DataBlockType::PHYSXMATERIAL_DATA:
            if (eSelectedData & DataField::PHYSXMATERIAL_DATA)
                pkBlock = EE_NEW PhysXMaterialDataBlock();
            break;
        case DataBlockType::PHYSX_DATA:
            if (eSelectedData & DataField::PHYSX_DATA)
                pkBlock = EE_NEW PhysXDataBlock();
            break;
        default:
            EE_ASSERT(false);
            break;
        }

        if (pkBlock)
        {
            // Read the data stored in this block
            *pkBlock = kBlockHeader;
            pkBlock->ReadBlockData(kStream);
            m_apkDataBlocks[kBlockHeader.m_eBlockType] = pkBlock;
        }
        else
        {
            // Skip the data stored in this block
            kStream.Seek(kBlockHeader.m_uiDataLength);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::DataBlock::~DataBlock()
{
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::DataBlock::ReadBlockHeader(efd::BinaryStream& kStream, 
                                                             DataBlock& kBlockHeader)
{
    // Attempt to read the block type from the stream (if failed then assume EOF)
    unsigned int uiBytesToRead = sizeof(kBlockHeader.m_eBlockType);
    if (kStream.BinaryRead(&kBlockHeader.m_eBlockType, uiBytesToRead, &uiBytesToRead, 1) ==
        uiBytesToRead)
    {
        efd::BinaryStreamLoad(kStream, &kBlockHeader.m_uiDataLength);
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::DataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    EE_UNUSED_ARG(kStream);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::DataBlock::WriteBlockHeader(efd::BinaryStream& kStream)
{
    efd::BinaryStreamSave(kStream, &m_eBlockType);
    efd::BinaryStreamSave(kStream, &m_uiDataLength);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::DataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    EE_UNUSED_ARG(kStream);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ConfigDataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    efd::BinaryStreamLoad(kStream, &m_uiSectorWidthInVerts);
    efd::BinaryStreamLoad(kStream, &m_uiNumLOD);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ConfigDataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    efd::BinaryStreamSave(kStream, &m_uiSectorWidthInVerts);
    efd::BinaryStreamSave(kStream, &m_uiNumLOD);

    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::BoundingDataBlock::~BoundingDataBlock()
{
    EE_FREE(m_pkBoundingData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::BoundingDataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    efd::UInt32 uiNumCellsStored = m_uiDataLength / ms_uiCellDataSize;
    EE_ASSERT(uiNumCellsStored >= m_uiNumCells);

    m_pkBoundingData = EE_ALLOC(CellData, m_uiNumCells);

    for (efd::UInt32 uiIndex = 0; uiIndex < (m_uiStartCell + m_uiNumCells); ++uiIndex)
    {
        if (uiIndex >= m_uiStartCell)
        {
            CellData& kCurrentData = m_pkBoundingData[uiIndex - m_uiStartCell];
            NiPoint3 kValue;
            float fValue;
            efd::UInt32 uiValue;

            efd::BinaryStreamLoad(kStream, &uiValue); // CellID
            EE_ASSERT(uiValue == uiIndex);

            efd::BinaryStreamLoad(kStream, &kValue.x);
            efd::BinaryStreamLoad(kStream, &kValue.y);
            efd::BinaryStreamLoad(kStream, &kValue.z);
            kCurrentData.m_kBound.SetCenter(kValue); // Bound Center
            efd::BinaryStreamLoad(kStream, &fValue);
            kCurrentData.m_kBound.SetRadius(fValue); // Bound Radius
            efd::BinaryStreamLoad(kStream, &kValue.x);
            efd::BinaryStreamLoad(kStream, &kValue.y);
            efd::BinaryStreamLoad(kStream, &kValue.z);
            kCurrentData.m_kBox.SetCenter(kValue); // Box Center
            efd::BinaryStreamLoad(kStream, &kValue.x);
            efd::BinaryStreamLoad(kStream, &kValue.y);
            efd::BinaryStreamLoad(kStream, &kValue.z);
            kCurrentData.m_kBox.SetAxis(0, kValue); // Box X Axis
            efd::BinaryStreamLoad(kStream, &kValue.x);
            efd::BinaryStreamLoad(kStream, &kValue.y);
            efd::BinaryStreamLoad(kStream, &kValue.z);
            kCurrentData.m_kBox.SetAxis(1, kValue); // Box Y Axis
            efd::BinaryStreamLoad(kStream, &kValue.x);
            efd::BinaryStreamLoad(kStream, &kValue.y);
            efd::BinaryStreamLoad(kStream, &kValue.z);
            kCurrentData.m_kBox.SetAxis(2, kValue); // Box Z Axis
            efd::BinaryStreamLoad(kStream, &fValue);
            kCurrentData.m_kBox.SetExtent(0, fValue); // Box X Extent
            efd::BinaryStreamLoad(kStream, &fValue);
            kCurrentData.m_kBox.SetExtent(1, fValue); // Box Y Extent
            efd::BinaryStreamLoad(kStream, &fValue);
            kCurrentData.m_kBox.SetExtent(2, fValue); // Box Z Extent
        }
        else
        {
            kStream.Seek(ms_uiCellDataSize);
        }
    }

    kStream.Seek((uiNumCellsStored - (m_uiStartCell + m_uiNumCells)) * ms_uiCellDataSize);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::BoundingDataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    EE_ASSERT(m_uiStartCell == 0);

    for (efd::UInt32 uiIndex = 0; uiIndex < m_uiNumCells; ++uiIndex)
    {
        if (uiIndex >= m_uiStartCell)
        {
            CellData& kCurrentData = m_pkBoundingData[uiIndex - m_uiStartCell];
            NiPoint3 kValue;
            float fValue;
            efd::UInt32 uiValue;

            uiValue = uiIndex + m_uiStartCell; // CellID
            efd::BinaryStreamSave(kStream, &uiValue);
            kValue = kCurrentData.m_kBound.GetCenter(); // Bound Center
            efd::BinaryStreamSave(kStream, &kValue.x);
            efd::BinaryStreamSave(kStream, &kValue.y);
            efd::BinaryStreamSave(kStream, &kValue.z);
            fValue = kCurrentData.m_kBound.GetRadius(); // Bound Radius
            efd::BinaryStreamSave(kStream, &fValue);
            kValue = kCurrentData.m_kBox.GetCenter(); // Box Center
            efd::BinaryStreamSave(kStream, &kValue.x);
            efd::BinaryStreamSave(kStream, &kValue.y);
            efd::BinaryStreamSave(kStream, &kValue.z);
            kValue = kCurrentData.m_kBox.GetAxis(0); // Box X Axis
            efd::BinaryStreamSave(kStream, &kValue.x);
            efd::BinaryStreamSave(kStream, &kValue.y);
            efd::BinaryStreamSave(kStream, &kValue.z);
            kValue = kCurrentData.m_kBox.GetAxis(1); // Box Y Axis
            efd::BinaryStreamSave(kStream, &kValue.x);
            efd::BinaryStreamSave(kStream, &kValue.y);
            efd::BinaryStreamSave(kStream, &kValue.z);
            kValue = kCurrentData.m_kBox.GetAxis(2); // Box Z Axis
            efd::BinaryStreamSave(kStream, &kValue.x);
            efd::BinaryStreamSave(kStream, &kValue.y);
            efd::BinaryStreamSave(kStream, &kValue.z);
            fValue = kCurrentData.m_kBox.GetExtent(0); // Box X Extent
            efd::BinaryStreamSave(kStream, &fValue);
            fValue = kCurrentData.m_kBox.GetExtent(1); // Box Y Extent
            efd::BinaryStreamSave(kStream, &fValue);
            fValue = kCurrentData.m_kBox.GetExtent(2); // Box Z Extent
            efd::BinaryStreamSave(kStream, &fValue);
        }
        else
        {
            kStream.Seek(sizeof(CellData));
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::SurfaceIndexBlock::~SurfaceIndexBlock()
{
    EE_FREE(m_pkSurfaceIndexData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::SurfaceIndexBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    efd::UInt32 uiNumCellsStored = m_uiDataLength / ms_uiLeafDataSize;
    EE_ASSERT(uiNumCellsStored >= m_uiNumLeaves);

    m_pkSurfaceIndexData = EE_ALLOC(LeafData, m_uiNumLeaves);

    for (efd::UInt32 uiIndex = 0; uiIndex < (m_uiStartLeaf + m_uiNumLeaves); ++uiIndex)
    {
        if (uiIndex >= m_uiStartLeaf)
        {
            LeafData& kCurrentData = m_pkSurfaceIndexData[uiIndex - m_uiStartLeaf];
            NiPoint3 kValue;
            efd::UInt32 uiValue;

            uiValue = uiIndex + m_uiStartLeaf; // CellID
            efd::BinaryStreamLoad(kStream, &uiValue);
            EE_ASSERT(uiValue == uiIndex + m_uiStartLeaf);
            efd::BinaryStreamLoad(kStream, &uiValue); 
            kCurrentData.m_uiNumSurfaces = uiValue; // Num Surfaces

            for (efd::UInt32 uiSlot = 0; uiSlot < NiTerrainCellLeaf::MAX_NUM_SURFACES; ++uiSlot)
            {
                efd::BinaryStreamLoad(kStream, &uiValue);
                kCurrentData.m_auiSurfaceIndex[uiSlot] = uiValue;
            }
        }
        else
        {
            kStream.Seek(ms_uiLeafDataSize);
        }
    }

    kStream.Seek((uiNumCellsStored - (m_uiStartLeaf + m_uiNumLeaves)) * ms_uiLeafDataSize);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::SurfaceIndexBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    EE_ASSERT(m_uiStartLeaf == 0);

    for (efd::UInt32 uiIndex = 0; uiIndex < m_uiNumLeaves; ++uiIndex)
    {
        LeafData& kCurrentData = m_pkSurfaceIndexData[uiIndex - m_uiStartLeaf];
        NiPoint3 kValue;
        efd::UInt32 uiValue;

        uiValue = uiIndex + m_uiStartLeaf; // CellID
        efd::BinaryStreamSave(kStream, &uiValue);
        uiValue = kCurrentData.m_uiNumSurfaces; // Num Surfaces
        efd::BinaryStreamSave(kStream, &uiValue); 

        for (efd::UInt32 uiSlot = 0; uiSlot < NiTerrainCellLeaf::MAX_NUM_SURFACES; ++uiSlot)
        {
            uiValue = kCurrentData.m_auiSurfaceIndex[uiSlot];
            efd::BinaryStreamSave(kStream, &uiValue);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::ImageDataBlock::~ImageDataBlock()
{
    EE_FREE(m_pucCompressedData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    // Read in the image header
    efd::BinaryStreamLoad(kStream, &m_uiWidth);
    efd::BinaryStreamLoad(kStream, &m_uiHeight);
    efd::BinaryStreamLoad(kStream, &m_usNumChannels);
    efd::BinaryStreamLoad(kStream, &m_usBytesPerChannel);
    efd::BinaryStreamLoad(kStream, &m_eCompressionMode);

    // Read in the image data
    efd::UInt32 uiImageDataSize = m_uiDataLength - (4 + 4 + 2 + 2 + 4);
    m_pucCompressedData = EE_ALLOC(efd::UInt8, uiImageDataSize);

    efd::UInt32 uiCompressedStride = CalculateCompressionStride();
    EE_ASSERT(uiCompressedStride);
    efd::BinaryStreamLoad(
        kStream, 
        m_pucCompressedData, 
        uiImageDataSize / uiCompressedStride, 
        &uiCompressedStride);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    // Write out the image header
    efd::BinaryStreamSave(kStream, &m_uiWidth);
    efd::BinaryStreamSave(kStream, &m_uiHeight);
    efd::BinaryStreamSave(kStream, &m_usNumChannels);
    efd::BinaryStreamSave(kStream, &m_usBytesPerChannel);
    efd::BinaryStreamSave(kStream, &m_eCompressionMode);

    // Write out the image data
    efd::UInt32 uiImageDataSize = m_uiDataLength - (4 + 4 + 2 + 2 + 4);
    efd::UInt32 uiCompressedStride = CalculateCompressionStride();
    EE_ASSERT(uiCompressedStride);

    efd::BinaryStreamSave(
        kStream, 
        m_pucCompressedData, 
        uiImageDataSize / uiCompressedStride, 
        &uiCompressedStride,
        1);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 NiTerrainSectorFileVersion6::ImageDataBlock::CalculateCompressionStride()
{
    switch(m_eCompressionMode)
    {
    case ImageCompressionMode::NONE:
        return m_usBytesPerChannel;
    default:
        EE_FAIL("Unrecognised image compression mode");
        return 1;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::CompressFromStream(
    ImageCompressionMode::VALUE eCompressionMode,
    efd::UInt8* pucBuffer, efd::UInt32 uiBufferLength, 
    efd::UInt32 uiWidth, efd::UInt32 uiHeight, efd::UInt16 usNumChannels, 
    efd::UInt16 usBytesPerChannel)
{
    // Save the relevant information
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    m_usNumChannels = usNumChannels;
    m_usBytesPerChannel = usBytesPerChannel;
    m_eCompressionMode = eCompressionMode;

    // Compress the data:
    efd::UInt8* pucCompressedData = NULL;
    efd::UInt32 uiCompressedDataLength = 0;
    switch(eCompressionMode)
    {
    case ImageCompressionMode::NONE:
        // Simply copy the data out
        pucCompressedData = EE_ALLOC(efd::UInt8, uiBufferLength);
        efd::Memcpy(pucCompressedData, pucBuffer, uiBufferLength);
        uiCompressedDataLength = uiBufferLength;
        break;
    default:
        EE_FAIL("Unrecognised image compression mode");
    }
    m_pucCompressedData = pucCompressedData;
    m_uiDataLength = uiCompressedDataLength + ms_uiStaticDataSize;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::CompressFromImageData(
    ImageCompressionMode::VALUE eCompressionMode, 
    NiPixelData* pkSource)
{
    if (!pkSource)
        return false;
    NiPixelDataPtr spSource = pkSource;

    // Convert the image to be RGB or RGBA - not BGR/BGRA - rendered textures tend to be swapped
    efd::UInt8 ucBPP;
    NiPixelFormat::Component eComponent = NiPixelFormat::COMP_EMPTY;
    NiPixelFormat::Representation eRepresentation;
    bool bSigned;
    const NiPixelFormat kSrcFormat = spSource->GetPixelFormat();
    kSrcFormat.GetComponent(0, eComponent, eRepresentation, ucBPP, bSigned);
    if (eComponent != NiPixelFormat::COMP_RED)
    {
        // Work out if the fourth channel is worth keeping?
        kSrcFormat.GetComponent(3, eComponent, eRepresentation, ucBPP, bSigned);
        NiPixelFormat kDstFormat = NiPixelFormat::RGBA32;
        if (eComponent != NiPixelFormat::COMP_ALPHA)
            kDstFormat = NiPixelFormat::RGB24;

        // Perform the conversion
        NiDevImageConverter kConverter;
        spSource = kConverter.ConvertPixelData(*spSource, kDstFormat, NULL, false);
        EE_ASSERT(spSource);
    }

    efd::UInt32 uiWidth = spSource->GetWidth();
    efd::UInt32 uiHeight = spSource->GetHeight();
    efd::UInt16 usNumChannels = efd::Int32ToUInt16(spSource->GetPixelFormat().GetNumComponents());
    efd::UInt16 usBytesPerChannel = spSource->GetPixelFormat().GetBitsPerPixel() / 
        (8 * usNumChannels);
    efd::UInt8* pucBuffer = spSource->GetPixels();
    efd::UInt32 uiBufferLength = spSource->GetSizeInBytes();

    return CompressFromStream(eCompressionMode, pucBuffer, uiBufferLength, uiWidth, 
        uiHeight, usNumChannels, usBytesPerChannel);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::DecompressToImageData(NiPixelData*& pkSource)
{
    bool bResult = true;

    // Figure out what format to use
    NiPixelFormat kFormat;
    if (m_usBytesPerChannel == 1 && m_usNumChannels == 3)
    {
        kFormat = NiPixelFormat::RGB24;
    }
    else if (m_usBytesPerChannel == 1 && m_usNumChannels == 4)
    {
        kFormat = NiPixelFormat::RGBA32;
    }
    else
    {
        EE_FAIL("Unsupported format");
    }

    // Create a pixel data
    EE_ASSERT(kFormat.GetBitsPerPixel() / (8 * m_usNumChannels) == m_usBytesPerChannel);
    NiPixelData* pkPixelData = EE_NEW NiPixelData(m_uiWidth, m_uiHeight, kFormat);

    efd::UInt8* pucBuffer = pkPixelData->GetPixels();
    efd::UInt32 uiBufferLength = pkPixelData->GetSizeInBytes();

    bResult &= DecompressToStream(pucBuffer, uiBufferLength);
    if (!bResult)
        EE_DELETE pkPixelData;
    else
        pkSource = pkPixelData;
    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::ImageDataBlock::DecompressToStream(efd::UInt8* pucBuffer, 
    efd::UInt32 uiBufferLength)
{
#ifndef EE_ASSERTS_ARE_ENABLED
    EE_UNUSED_ARG(uiBufferLength);
#endif

    switch (m_eCompressionMode)
    {
    case ImageCompressionMode::NONE:
        {
            efd::UInt32 uiCompressedDataSize = m_uiDataLength - ms_uiStaticDataSize;
            EE_ASSERT(uiBufferLength >= uiCompressedDataSize);
            efd::Memcpy(pucBuffer, m_pucCompressedData, uiCompressedDataSize);
        }
        break;
    default:
        EE_FAIL("Unsupported image compression mode");
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::PhysXMaterialDataBlock::~PhysXMaterialDataBlock()
{
    m_kMaterialData.clear();
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::PhysXMaterialDataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    NiUInt32 uiSize;
    NiUInt32 uiIndex;
    float fRestitution;
    float fSFriction;
    float fDFriction;
    NiPhysXMaterialMetaData kData;


    efd::BinaryStreamLoad(kStream, &uiSize);

    for (NiUInt32 ui = 0; ui < uiSize; ++ui)
    {
        efd::BinaryStreamLoad(kStream, &uiIndex);
        efd::BinaryStreamLoad(kStream, &fRestitution);
        efd::BinaryStreamLoad(kStream, &fSFriction);
        efd::BinaryStreamLoad(kStream, &fDFriction);

        kData = NiPhysXMaterialMetaData(fRestitution, fSFriction, fDFriction);
        m_kMaterialData[uiIndex] = kData;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::PhysXMaterialDataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    efd::map<efd::UInt32, NiPhysXMaterialMetaData>::iterator kIter = m_kMaterialData.begin();

    NiUInt32 uiSize = m_kMaterialData.size();
    efd::BinaryStreamSave(kStream, &uiSize);
    while (kIter != m_kMaterialData.end())
    {
        NiUInt32 uiIndex = kIter->first;
        float fRestitution = kIter->second.GetRestitution();
        float fSFriction = kIter->second.GetStaticFriction();
        float fDFriction = kIter->second.GetDynamicFriction();

        efd::BinaryStreamSave(kStream, &uiIndex);
        efd::BinaryStreamSave(kStream, &fRestitution);
        efd::BinaryStreamSave(kStream, &fSFriction);
        efd::BinaryStreamSave(kStream, &fDFriction);
        ++kIter;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion6::PhysXDataBlock::~PhysXDataBlock()
{
    EE_DELETE m_pkPhysXData;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::PhysXDataBlock::ReadBlockData(efd::BinaryStream& kStream)
{
    NiUInt32 uiWidth;
    NiUInt32 uiHeight;
    efd::BinaryStreamLoad(kStream, &uiWidth);
    efd::BinaryStreamLoad(kStream, &uiHeight);

    m_pkPhysXData = EE_NEW NiTerrainSectorPhysXData(uiWidth, uiHeight);

    for (NiUInt32 ui = 0; ui < m_pkPhysXData->m_uiNumSamples; ++ui)
    {        
        NiUInt16 usMat1 = 0;
        NiUInt16 usMat2 = 0;
        bool bTess = false;

        efd::BinaryStreamLoad(kStream, &usMat1);
        efd::BinaryStreamLoad(kStream, &usMat2);
        efd::BinaryStreamLoad(kStream, &bTess);


        m_pkPhysXData->m_pkSamples[ui] = NiTerrainSectorPhysXSampleData(usMat1, usMat2, bTess);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion6::PhysXDataBlock::WriteBlockData(efd::BinaryStream& kStream)
{
    NiUInt32 uiWidth = m_pkPhysXData->m_uiWidth;
    NiUInt32 uiHeight = m_pkPhysXData->m_uiHeight;
    efd::BinaryStreamSave(kStream, &uiWidth);
    efd::BinaryStreamSave(kStream, &uiHeight);

    for (NiUInt32 ui = 0; ui < m_pkPhysXData->m_uiNumSamples; ++ui)
    {
        NiTerrainSectorPhysXSampleData kSample = m_pkPhysXData->m_pkSamples[ui];
        NiUInt16 usMat1 = kSample.GetMaterialIndex1();
        NiUInt16 usMat2 = kSample.GetMaterialIndex2();
        bool bTess = kSample.GetTesselation();

        efd::BinaryStreamSave(kStream, &usMat1);
        efd::BinaryStreamSave(kStream, &usMat2);
        efd::BinaryStreamSave(kStream, &bTess);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
