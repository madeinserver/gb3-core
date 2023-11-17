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
#include "NiTerrainFileVersion1.h"
#include "NiTerrainXMLHelpers.h"
#include "NiTerrainSectorFileVersion5.h"

//--------------------------------------------------------------------------------------------------
NiTerrainFile::FileVersion NiTerrainFileVersion1::DetectFileVersion(
    const char* pcTerrainArchive)
{
    FileVersion eVersion = 0;

    // Attempt to access the file
    NiString kSectorPath;
    kSectorPath.Format("%s%s", pcTerrainArchive, "\\TerrainConfig.xml");

    // Attempt to open the file:
    efd::TiXmlDocument kFile(kSectorPath);
    if (!NiTerrainXMLHelpers::LoadXMLFile(&kFile))
    {
        // File does not exist, so attempt loading using the oldest style
        eVersion = NiTerrainFile::ms_kFileVersion0;
    }
    else
    {
        // Reset to the initial tag
        efd::TiXmlElement* pkCurElement = kFile.FirstChildElement("Terrain");            
        if (!pkCurElement)
            return NULL;

        // Inspect the file version
        const char* pcVersion = pkCurElement->Attribute("Version");
        if (pcVersion)
        {
            NiUInt32 uiFileVersion = 0;
            if (NiString(pcVersion).ToUInt(uiFileVersion))
                eVersion = uiFileVersion;
        }
    }

    return eVersion;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion1::Close()
{
    WriteFileHeader();
    WriteSurfaceIndex();

    NiTerrainFile::Close();
}
//--------------------------------------------------------------------------------------------------
NiString NiTerrainFileVersion1::GenerateTerrainConfigFilename()
{
    NiString kString = m_kTerrainArchive;
    kString += "\\TerrainConfig.xml";
    return kString;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion1::GetFilePaths(efd::set<efd::utf8string>& kFilePaths)
{
    kFilePaths.insert((const char*)GenerateTerrainConfigFilename());
}
//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion1::ReadConfiguration(efd::UInt32& uiSectorSize, efd::UInt32& uiNumLOD, 
    efd::UInt32& uiMaskSize, efd::UInt32& uiLowDetailSize, float& fMinElevation, 
    float& fMaxElevation, float& fVertexSpacing, float& fLowDetailSpecularPower, 
    float& fLowDetailSpecularIntensity, efd::UInt32& uiSurfaceCount)
{
    // Read these values out from Sector 0,0 since there are no other files around
    NiTerrainSectorFile* pkFile = NiTerrainSectorFile::Open(
            m_kTerrainArchive, 0, 0, false);
    if (!pkFile)
        return false;

    // Make sure we are dealing with an old terrain asset
    if (pkFile->GetFileVersion() > NiTerrainSectorFile::FileVersion(5))
    {
        pkFile->Close();
        EE_DELETE pkFile;
        return false;
    }
    NiTerrainSectorFileVersion5* pkOldFile = (NiTerrainSectorFileVersion5*)pkFile;

    // Fetch the basic configuration values
    uiNumLOD = pkOldFile->GetNumLOD();
    uiSectorSize = ((pkOldFile->GetBlockWidthInVerts() - 1) << uiNumLOD) + 1;
    
    // Precache the height data
    pkOldFile->Precache(0, uiNumLOD, NiTerrainSectorFile::DataField::HEIGHTS);

    // Figure out the min/max height of the terrain
    fMinElevation = FLT_MAX;
    fMaxElevation = FLT_MIN;
    pkOldFile->GetMinMaxHeight(fMinElevation, fMaxElevation);
    EE_ASSERT(fMinElevation <= fMaxElevation);

    // Set vertex spacing to default
    fVertexSpacing = 1.0f;

    // Set the low detail specular values to defaults
    fLowDetailSpecularPower = 10.0f;
    fLowDetailSpecularIntensity = 0.5f;

    // Figure out the size of the overall mask
    uiMaskSize = pkOldFile->GetBlendMaskSize();
    // Figure out the size of the low detail diffuse texture
    uiLowDetailSize = pkOldFile->GetLowDetailTextureSize();
    
    // Close off the file
    pkOldFile->Close();
    EE_DELETE(pkOldFile);

    uiSurfaceCount = GetNumSurfaces();
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion1::ReadSurface(NiUInt32 uiSurfaceIndex, NiFixedString& kPackageID, 
    NiFixedString& kSurfaceID)
{   
    if (uiSurfaceIndex < m_kSurfacePackageArray.GetSize())
    {
        kPackageID = GetSurfacePackage(uiSurfaceIndex);
        kSurfaceID = GetSurfaceName(uiSurfaceIndex);
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
//------------------------------------  OLD INTERFACE  ---------------------------------------------
//--------------------------------------------------------------------------------------------------
NiTerrainFileVersion1::NiTerrainFileVersion1(
    const char* pcTerrainFile,
    bool bWriteAccess):
    NiTerrainFileVersion2(pcTerrainFile, bWriteAccess)
{
}
//---------------------------------------------------------------------------
NiTerrainFileVersion1::~NiTerrainFileVersion1()
{
    // Release the stream buffers:
    if (IsWritable())
    {
        m_kFile.SaveFile();
    }
}
//---------------------------------------------------------------------------
bool NiTerrainFileVersion1::Initialize()
{
    m_kFile = efd::TiXmlDocument(GenerateTerrainConfigFilename());
    if (!m_bWriteAccess && !NiTerrainXMLHelpers::LoadXMLFile(&m_kFile))
        return false;

    // Initialize the variables:
    m_kSurfaceNameArray.RemoveAll();
    m_kSurfacePackageArray.RemoveAll();

    // If we are reading from the file, then begin setting up:
    if (!m_bWriteAccess)
    {
        // Reset to the initial tag
        efd::TiXmlElement* pkCurElement = m_kFile.FirstChildElement("Terrain");            
        if (!pkCurElement)
            return NULL;

        // Read the file version
        m_kFileVersion = 0;
        if (!NiString(pkCurElement->Attribute("Version")).ToUInt(m_kFileVersion))
            return false;
        
        // Read the sector index
        if (!ReadSurfaceIndex(pkCurElement))
            return false;
    }
    else
    {
        m_kFileVersion = ms_kFileVersion1;
    }

    return NiTerrainFile::Initialize();
}
//---------------------------------------------------------------------------
bool NiTerrainFileVersion1::ReadSurfaceIndex(const efd::TiXmlElement* pkDocument)
{
    const efd::TiXmlElement* pkCurElement = pkDocument->FirstChildElement("SurfaceList");
    pkCurElement = pkCurElement->FirstChildElement("Surface");
    if(pkCurElement)
    {
        const char* pcPackage = 0;
        const char* pcName = 0;
        const char* pcLayerNum = 0;

        // Loop through all surfaces in the list and collect the information
        while (pkCurElement)
        {
            pcPackage = pkCurElement->Attribute("package");
            if (pcPackage == 0)
                pcPackage = "";

            pcName = pkCurElement->Attribute("name");
            if (pcName == 0)
                pcName = "";

            pcLayerNum = pkCurElement->Attribute("position");
            if (pcLayerNum == 0)
                pcLayerNum = "";

            // Convert the layer number into an index
            NiUInt32 uiSurfaceIndex = 0;
            if (NiString(pcLayerNum).ToUInt(uiSurfaceIndex))
            {
                // Add this surface to the list
                m_kSurfacePackageArray.SetAtGrow(uiSurfaceIndex, pcPackage);
                m_kSurfaceNameArray.SetAtGrow(uiSurfaceIndex, pcName);
            }

            // Move onto the next surface
            pkCurElement = pkCurElement->NextSiblingElement();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainFileVersion1::WriteFileHeader()
{
    if (!IsReady() || !IsWritable())
        return false;

    NiTerrainXMLHelpers::WriteXMLHeader(&m_kFile);
    efd::TiXmlElement* pkTerrainElement = NiTerrainXMLHelpers::CreateElement("Terrain", NULL);
    pkTerrainElement->SetAttribute("Version", m_kFileVersion);
    m_kFile.LinkEndChild(pkTerrainElement);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainFileVersion1::WriteSurfaceIndex()
{
    if (!IsReady() || !IsWritable())
        return false;

    efd::TiXmlElement* pkRootElement = m_kFile.FirstChildElement("Terrain");
    efd::TiXmlElement* pkSurfaceListElement = NiTerrainXMLHelpers::CreateElement(
        "SurfaceList", pkRootElement);

    // Write each surface into the file
    NiUInt32 uiNumSurfaces = GetNumSurfaces();
    for (NiUInt32 uiIndex = 0; uiIndex < uiNumSurfaces; ++uiIndex)
    {   
        efd::TiXmlElement* pkSurfaceElement = NiTerrainXMLHelpers::CreateElement(
            "Surface", pkSurfaceListElement);
        
        // Write this surface's data to the file
        pkSurfaceElement->SetAttribute("package", GetSurfacePackage(uiIndex));
        pkSurfaceElement->SetAttribute("name", GetSurfaceName(uiIndex));
        pkSurfaceElement->SetAttribute("position", uiIndex);
    }

    return true;
}
//---------------------------------------------------------------------------
