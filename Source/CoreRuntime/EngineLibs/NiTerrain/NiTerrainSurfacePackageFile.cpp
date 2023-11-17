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
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiTerrainPCH.h"
#include "NiTerrainSurfacePackageFile.h"

#include "NiTerrainSurfacePackageFileVersion0.h"
#include "NiTerrainSurfacePackageFileVersion1.h"
//#include "NiTerrainSurfacePackageFileVersion2.h"

//--------------------------------------------------------------------------------------------------
NiTerrainSurfacePackageFile *NiTerrainSurfacePackageFile::Open(const efd::utf8string& kFilename, 
    bool bWriteAccess)
{
    NiTerrainSurfacePackageFile* pkResult = NULL;

    // Check the filename
    if (kFilename.empty())
        return NULL;

    // Determine the version of the data
    FileVersion kVersion = ms_kFileVersion;
    if (!bWriteAccess)
    {
        // We are asked to read the data to figure out the fileversion
        kVersion = NiTerrainSurfacePackageFileVersion1::DetectFileVersion(kFilename);
    }

    // Instantiate the correct version of the file
    switch (kVersion)
    {
    case ms_kFileVersion0:
        pkResult = EE_NEW NiTerrainSurfacePackageFileVersion0(kFilename, bWriteAccess);
        break;
    case ms_kFileVersion1:
        pkResult = EE_NEW NiTerrainSurfacePackageFileVersion1(kFilename, bWriteAccess);
        break;
    case ms_kFileVersion2:
        //pkResult = EE_NEW NiTerrainSurfacePackageFileVersion2(kFilename, bWriteAccess);
        break;
    default:
        EE_FAIL("NiTerrainSurfacePackageFile::Open failed, unable to determine the file version");
    }

    // Make sure we managed to load an appropriate parser
    if (pkResult == NULL)
        return pkResult;

    // Make sure the parser initializes ok!
    if (!pkResult->Initialize())
    {
        // Failed to initialize properly
        NiDelete pkResult;
        pkResult = NULL;
    }

    return pkResult;
}

//--------------------------------------------------------------------------------------------------
NiTerrainSurfacePackageFile::NiTerrainSurfacePackageFile(efd::utf8string kFilename,
    bool bWriteAccess)
    : m_kPackageFilename(kFilename)
    , m_bWriteAccess(bWriteAccess)
    , m_bOpen(false)
    , m_kFileVersion(0)
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainSurfacePackageFile::~NiTerrainSurfacePackageFile()
{
    EE_ASSERT(!m_bOpen);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::Initialize()
{
    m_bOpen = true;
    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::Precache(efd::UInt32 uiDataFields)
{
    EE_UNUSED_ARG(uiDataFields);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::Close()
{
    m_bOpen = false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadPackageConfig(efd::utf8string& kPackageName,
    efd::UInt32& uiIteration)
{
    EE_UNUSED_ARG(kPackageName);
    EE_UNUSED_ARG(uiIteration);
    
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WritePackageConfig(const efd::utf8string& kPackageName,
    efd::UInt32 uiIteration)
{
    EE_UNUSED_ARG(kPackageName);
    EE_UNUSED_ARG(uiIteration);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadNumSurfaces(efd::UInt32& uiNumSurfaces)
{
    EE_UNUSED_ARG(uiNumSurfaces);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WriteNumSurfaces(const efd::UInt32& uiNumSurfaces)
{
    EE_UNUSED_ARG(uiNumSurfaces);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadSurfaceConfig(efd::UInt32 uiSurfaceIndex,
    efd::utf8string& kName, 
    efd::Float32& fTextureTiling,
    efd::Float32& fDetailTiling,
    efd::Float32& fRotation,
    efd::Float32& fParallaxStrength,
    efd::Float32& fDistributionMaskStrength,
    efd::Float32& fSpecularPower,
    efd::Float32& fSpecularIntensity,
    efd::UInt32& uiNumDecorationLayers)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(kName);
    EE_UNUSED_ARG(fTextureTiling);
    EE_UNUSED_ARG(fDetailTiling);
    EE_UNUSED_ARG(fRotation);
    EE_UNUSED_ARG(fParallaxStrength);
    EE_UNUSED_ARG(fDistributionMaskStrength);
    EE_UNUSED_ARG(fSpecularPower);
    EE_UNUSED_ARG(fSpecularIntensity);
    EE_UNUSED_ARG(uiNumDecorationLayers);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WriteSurfaceConfig(efd::UInt32 uiSurfaceIndex,
    const efd::utf8string& kName, 
    efd::Float32 fTextureTiling,
    efd::Float32 fDetailTiling,
    efd::Float32 fRotation,
    efd::Float32 fParallaxStrength,
    efd::Float32 fDistributionMaskStrength,
    efd::Float32 fSpecularPower,
    efd::Float32 fSpecularIntensity,
    efd::UInt32 uiNumDecorationLayers)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(kName);
    EE_UNUSED_ARG(fTextureTiling);
    EE_UNUSED_ARG(fDetailTiling);
    EE_UNUSED_ARG(fRotation);
    EE_UNUSED_ARG(fParallaxStrength);
    EE_UNUSED_ARG(fDistributionMaskStrength);
    EE_UNUSED_ARG(fSpecularPower);
    EE_UNUSED_ARG(fSpecularIntensity);
    EE_UNUSED_ARG(uiNumDecorationLayers);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadSurfaceSlot(efd::UInt32 uiSurfaceIndex,
    efd::UInt32 uiSlotID,
    NiTerrainAssetReference* pkReference)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(uiSlotID);
    EE_UNUSED_ARG(pkReference);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WriteSurfaceSlot(efd::UInt32 uiSurfaceIndex,
    efd::UInt32 uiSlotID,
    const NiTerrainAssetReference* pkReference)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(uiSlotID);
    EE_UNUSED_ARG(pkReference);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadSurfaceMetadata(efd::UInt32 uiSurfaceIndex,
    NiMetaData& kMetaData)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(kMetaData);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WriteSurfaceMetadata(efd::UInt32 uiSurfaceIndex,
    const NiMetaData& kMetaData)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(kMetaData);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadSurfaceCompiledTextures(efd::UInt32 uiSurfaceIndex,
    NiTexturePtr* aspTextures, efd::UInt32 uiNumTextures)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(aspTextures);
    EE_UNUSED_ARG(uiNumTextures);

    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFile::ReadSurfaceCompiledTextures(efd::utf8string kSurfaceName, 
    NiTexturePtr* aspTextures, efd::UInt32 uiNumTextures)
{
    EE_UNUSED_ARG(kSurfaceName);
    EE_UNUSED_ARG(aspTextures);
    EE_UNUSED_ARG(uiNumTextures);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFile::WriteSurfaceCompiledTextures(efd::UInt32 uiSurfaceIndex,
    NiTexturePtr* aspTextures, efd::UInt32 uiNumTextures)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(aspTextures);
    EE_UNUSED_ARG(uiNumTextures);
}

//--------------------------------------------------------------------------------------------------