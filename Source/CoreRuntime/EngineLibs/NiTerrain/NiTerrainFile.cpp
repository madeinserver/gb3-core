// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiTerrainPCH.h"
#include "NiTerrainFile.h"
#include "NiTerrainXMLHelpers.h"

// File versions
#include "NiTerrainFileVersion0.h"
#include "NiTerrainFileVersion1.h"
#include "NiTerrainFileVersion2.h"
#include "NiTerrainFileVersion3.h"

//--------------------------------------------------------------------------------------------------
NiTerrainFile *NiTerrainFile::Open(const char* pcArchivePath, bool bWriteAccess)
{
    NiTerrainFile *pkResult = NULL;

    // Check the filename:
    if (!pcArchivePath)
        return NULL;

    // Determine the version of the data
    FileVersion kVersion = ms_kFileVersion;
    if (!bWriteAccess)
    {
        // We are reading data, so read the file version from the data
        kVersion = NiTerrainFileVersion3::DetectFileVersion(pcArchivePath);
    }

    // Instantiate the correct version of the file
    switch(kVersion)
    {
    case ms_kFileVersion0:
        pkResult = EE_NEW NiTerrainFileVersion0(pcArchivePath, bWriteAccess);
        break;
    case ms_kFileVersion1:
        pkResult = EE_NEW NiTerrainFileVersion1(pcArchivePath, bWriteAccess);
        break;
    case ms_kFileVersion2:
        pkResult = EE_NEW NiTerrainFileVersion2(pcArchivePath, bWriteAccess);
        break;
    case ms_kFileVersion3:
        pkResult = EE_NEW NiTerrainFileVersion3(pcArchivePath, bWriteAccess);
        break;
    default:
        EE_FAIL("NiTerrainFile::Open failed, unable to determine the file version");
    }

    // Make sure we managed to load an appropriate parser
    if (pkResult == NULL)    
        return pkResult;

    // Make sure the parser initializes ok!
    if (!pkResult->Initialize())
    {
        // The parser failed to initialize properly!
        NiDelete pkResult;
        pkResult = NULL;
    }

    return pkResult;
}

//--------------------------------------------------------------------------------------------------
NiTerrainFile::NiTerrainFile(
    const char* pcTerrainArchive,
    bool bWriteAccess) :
    m_kTerrainArchive(pcTerrainArchive),
    m_bWriteAccess(bWriteAccess),
    m_bOpen(false),
    m_kFileVersion(0)
{
}
//--------------------------------------------------------------------------------------------------
NiTerrainFile::~NiTerrainFile()
{
    EE_ASSERT(!m_bOpen);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFile::Initialize()
{
    m_bOpen = true;
    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFile::Close()
{
    m_bOpen = false;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFile::ReadConfiguration(efd::UInt32& uiSectorSize, efd::UInt32& uiNumLOD, 
    efd::UInt32& uiMaskSize, efd::UInt32& uiLowDetailSize, float& fMinElevation, 
    float& fMaxElevation, float& fVertexSpacing, float& fLowDetailSpecularPower, 
    float& fLowDetailSpecularIntensity, efd::UInt32& uiSurfaceCount)
{
    EE_UNUSED_ARG(uiSectorSize);
    EE_UNUSED_ARG(uiNumLOD);
    EE_UNUSED_ARG(uiMaskSize);
    EE_UNUSED_ARG(uiLowDetailSize);
    EE_UNUSED_ARG(fMinElevation);
    EE_UNUSED_ARG(fMaxElevation);
    EE_UNUSED_ARG(fVertexSpacing);
    EE_UNUSED_ARG(uiSurfaceCount);
    EE_UNUSED_ARG(fLowDetailSpecularPower);
    EE_UNUSED_ARG(fLowDetailSpecularIntensity);

    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFile::WriteConfiguration(efd::UInt32 uiSectorSize, efd::UInt32 uiNumLOD, 
    efd::UInt32 uiMaskSize, efd::UInt32 uiLowDetailSize, float fMinElevation, 
    float fMaxElevation, float fVertexSpacing, float fLowDetailSpecularPower, 
    float fLowDetailSpecularIntensity, efd::UInt32 uiSurfaceCount)
{
    EE_UNUSED_ARG(uiSectorSize);
    EE_UNUSED_ARG(uiNumLOD);
    EE_UNUSED_ARG(uiMaskSize);
    EE_UNUSED_ARG(uiLowDetailSize);
    EE_UNUSED_ARG(fMinElevation);
    EE_UNUSED_ARG(fMaxElevation);
    EE_UNUSED_ARG(fVertexSpacing);
    EE_UNUSED_ARG(uiSurfaceCount);
    EE_UNUSED_ARG(fLowDetailSpecularPower);
    EE_UNUSED_ARG(fLowDetailSpecularIntensity);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFile::ReadSurface( NiUInt32 uiSurfaceIndex, NiTerrainAssetReference* pkPackageRef, 
    NiFixedString& kSurfaceID, efd::UInt32& uiIteration)
{   
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(pkPackageRef);
    EE_UNUSED_ARG(kSurfaceID);
    EE_UNUSED_ARG(uiIteration);
    return false;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFile::WriteSurface(NiUInt32 uiSurfaceIndex, NiTerrainAssetReference* pkPackageRef, 
    NiFixedString kSurfaceID, efd::UInt32 uiIteration)
{
    EE_UNUSED_ARG(uiSurfaceIndex);
    EE_UNUSED_ARG(pkPackageRef);
    EE_UNUSED_ARG(kSurfaceID);
    EE_UNUSED_ARG(uiIteration);
}

//--------------------------------------------------------------------------------------------------
