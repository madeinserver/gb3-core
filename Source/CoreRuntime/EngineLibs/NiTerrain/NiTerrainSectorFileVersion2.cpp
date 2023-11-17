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
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#include "NiTerrainPCH.h"
#include "NiTerrainSectorFileVersion2.h"
#include "NiTerrainUtils.h"
#include "NiIndex.h"

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion2::NiTerrainSectorFileVersion2(
    const char* pcSectorFile, efd::File::OpenMode kAccessMode): 
    NiTerrainSectorFileVersion3(pcSectorFile, kAccessMode)
{
    
}

//--------------------------------------------------------------------------------------------------
NiTerrainSectorFileVersion2::~NiTerrainSectorFileVersion2()
{

}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSectorFileVersion2::Initialize()
{
    // Attempt to gain access to the file in this mode!
    m_pkFile = efd::File::GetFile(m_kSectorFile, m_kAccessMode);
    if (!m_pkFile)
        return false;

    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkFile->SetEndianSwap(!bPlatformLittle);

    // Return null if it was not possible
    if (!m_pkFile)
    {
        return false;
    }

    // Initialize the variables:
    m_ulFilePosition = 0;
    m_iCurrentBlockID = -1;
    m_iCurrentBlockLevel = 0;
    m_iCurrentBlockLevelIndex = 0;
    m_kPositionStack.RemoveAll();

    // If we are reading from the file, then begin setting up:
    if (m_kAccessMode == efd::File::READ_ONLY)
    {
        // Read the file header
        LoadLegacyFileHeaderBinary(m_kFileHeader, *m_pkFile);
        m_ulFilePosition += sizeof(m_kFileHeader) -
            sizeof(m_kFileHeader.m_uiNumLOD);

        // Read the first block's data:
        if (!NextBlock())
            return false;

        // Calculate the number of LOD stored in this file (it is not in
        // the header of this version).
        m_kFileHeader.m_uiNumLOD = 0;
        while (PushBlock(0))
            m_kFileHeader.m_uiNumLOD++;

        while (PopBlock());
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSectorFileVersion2::LoadLegacyFileHeaderBinary(
    FileHeader& kFileHeader, efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, kFileHeader.m_kVersion);
    NiStreamLoadBinary(kStream, kFileHeader.m_uiVertsPerBlock);
    kFileHeader.m_uiNumLOD = 0;
}
