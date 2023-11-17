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
#include "NiTerrainFileVersion3.h"
#include "NiTerrainFileVersion2.h"
#include "NiTerrainFileVersion1.h"
#include "NiTerrainXMLHelpers.h"

//--------------------------------------------------------------------------------------------------
const char* NiTerrainFileVersion3::ms_pcTerrainConfigFile = "\\root.terrain";
//--------------------------------------------------------------------------------------------------
NiTerrainFile::FileVersion NiTerrainFileVersion3::DetectFileVersion(
    const char* pcTerrainArchive)
{
    FileVersion eVersion = 0;

    // Attempt to access the file
    NiString kSectorPath;
    kSectorPath.Format("%s%s", pcTerrainArchive, ms_pcTerrainConfigFile);

    // Attempt to open the file:
    efd::TiXmlDocument kFile(kSectorPath);
    if (!NiTerrainXMLHelpers::LoadXMLFile(&kFile))
    {
        // File does not exist, so attempt loading using the oldest style
        eVersion = NiTerrainFileVersion1::DetectFileVersion(pcTerrainArchive);
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
NiTerrainFileVersion3::NiTerrainFileVersion3(const char* pcTerrainFile, bool bWriteAccess):
    NiTerrainFile(pcTerrainFile, bWriteAccess),
    m_bConfigurationValid(false)
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainFileVersion3::~NiTerrainFileVersion3()
{
    // Release the stream buffers:
    if (IsWritable())
    {
        m_kFile.SaveFile();
    }
}

//--------------------------------------------------------------------------------------------------
NiTerrainFileVersion3::SurfaceReference::SurfaceReference()
    : m_bValid(false) 
    , m_uiIteration(0)
{
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion3::GetFilePaths(efd::set<efd::utf8string>& kFilePaths)
{
    kFilePaths.insert((const char*)GenerateTerrainConfigFilename());
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::Initialize()
{
    m_kFile = efd::TiXmlDocument(GenerateTerrainConfigFilename());
    if (!m_bWriteAccess && !NiTerrainXMLHelpers::LoadXMLFile(&m_kFile))
        return false;

    // Initialize the variables:
    m_kSurfaceReferences.clear();

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
        
        // Read the surface index
        if (!ReadSurfaceIndex(pkCurElement))
            return false;

        // Read the configuration data
        if (!ReadConfiguration(pkCurElement))
            return false;
    }
    else
    {
        m_kFileVersion = ms_kFileVersion3;
    }

    m_bOpen = true;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::ReadConfiguration(efd::TiXmlElement* pkRootElement)
{
    efd::TiXmlElement* pkConfigElement = pkRootElement->FirstChildElement("Configuration");
    if (!pkConfigElement)
        return false;

    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "SectorSize", m_uiSectorSize);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "NumLOD", m_uiNumLOD);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "MaskSize", m_uiMaskSize);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "LowDetailTextureSize", m_uiLowDetailSize);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "MinElevation", m_fMinElevation);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "MaxElevation", m_fMaxElevation);
    if (!NiTerrainXMLHelpers::ReadElement(pkConfigElement, "VertexSpacing", m_fVertexSpacing))
    {
        // Default value for vertex spacing
        m_fVertexSpacing = 1.0f;
    }
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "NumSurfaces", m_uiSurfaceCount);

    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "LowDetailSpecularPower", 
        m_fLowDetailSpecularPower);
    NiTerrainXMLHelpers::ReadElement(pkConfigElement, "LowDetailSpecularIntensity", 
        m_fLowDetailSpecularIntensity);

    m_bConfigurationValid = true;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::ReadSurfaceIndex(efd::TiXmlElement* pkDocument)
{
    const efd::TiXmlElement* pkCurElement = pkDocument->FirstChildElement("SurfaceList");
    pkCurElement = pkCurElement->FirstChildElement("Surface");
    if(pkCurElement)
    {
        const char* pcName = 0;
        const char* pcLayerNum = 0;
        const char* pcPackageIteration = 0;
        const char* pcRelativePath = 0;
        const char* pcAssetID = 0;

        // Loop through all surfaces in the list and collect the information
        while (pkCurElement)
        {
            // Extract the values
            pcRelativePath = pkCurElement->Attribute("LastRelativePath");
            if (pcRelativePath == 0)
                pcRelativePath = "";

            pcAssetID = pkCurElement->Attribute("AssetID");
            if (pcAssetID == 0)
                pcAssetID = "";

            pcName = pkCurElement->Attribute("name");
            if (pcName == 0)
                pcName = "";

            pcLayerNum = pkCurElement->Attribute("position");
            if (pcLayerNum == 0)
                pcLayerNum = "";

            pcPackageIteration = pkCurElement->Attribute("packageIteration");
            if (pcPackageIteration == 0)
                pcPackageIteration = "0";

            // Convert the layer number into an index
            NiUInt32 uiSurfaceIndex = 0;
            NiUInt32 uiIteration = 0;
            if (NiString(pcLayerNum).ToUInt(uiSurfaceIndex) &&
                NiString(pcPackageIteration).ToUInt(uiIteration))
            {
                SurfaceReference kReference;
                kReference.m_kPackageAssetID = pcAssetID;
                kReference.m_kPackageRelativePath = pcRelativePath;
                kReference.m_kSurfaceName = pcName;
                kReference.m_uiIteration = uiIteration;
                kReference.m_bValid = true;

                // Add this surface to the list
                m_kSurfaceReferences[uiSurfaceIndex] = kReference;
            }

            // Move onto the next surface
            pkCurElement = pkCurElement->NextSiblingElement();
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion3::Close()
{
    WriteFileHeader();
    WriteConfiguration();
    WriteSurfaceIndex();

    NiTerrainFile::Close();
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::WriteFileHeader()
{
    if (!IsReady() || !IsWritable())
        return false;

    NiTerrainXMLHelpers::WriteXMLHeader(&m_kFile);
    efd::TiXmlElement* pkTerrainElement = NiTerrainXMLHelpers::CreateElement("Terrain", NULL);
    pkTerrainElement->SetAttribute("Version", m_kFileVersion);
    m_kFile.LinkEndChild(pkTerrainElement);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::WriteConfiguration()
{
    if (!IsReady() || !IsWritable() || !m_bConfigurationValid)
        return false;

    efd::TiXmlElement* pkRootElement = m_kFile.FirstChildElement("Terrain");
    efd::TiXmlElement* pkConfigElement = NiTerrainXMLHelpers::CreateElement(
        "Configuration", pkRootElement);

    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "SectorSize", m_uiSectorSize);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "NumLOD", m_uiNumLOD);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "MaskSize", m_uiMaskSize);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "LowDetailTextureSize", m_uiLowDetailSize);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "MinElevation", m_fMinElevation);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "MaxElevation", m_fMaxElevation);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "VertexSpacing", m_fVertexSpacing);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "NumSurfaces", m_uiSurfaceCount);

    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "LowDetailSpecularPower", 
        m_fLowDetailSpecularPower);
    NiTerrainXMLHelpers::WriteElement(pkConfigElement, "LowDetailSpecularIntensity", 
        m_fLowDetailSpecularIntensity);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::WriteSurfaceIndex()
{
    if (!IsReady() || !IsWritable())
        return false;

    efd::TiXmlElement* pkRootElement = m_kFile.FirstChildElement("Terrain");
    efd::TiXmlElement* pkSurfaceListElement = NiTerrainXMLHelpers::CreateElement(
        "SurfaceList", pkRootElement);

    // Write each surface into the file
    SurfaceReferenceMap::iterator kIter;
    for (kIter = m_kSurfaceReferences.begin(); kIter != m_kSurfaceReferences.end(); ++kIter)
    {   
        if (!kIter->second.m_bValid)
            continue;

        efd::TiXmlElement* pkSurfaceElement = NiTerrainXMLHelpers::CreateElement(
            "Surface", pkSurfaceListElement);
        
        // Save the surface's data into the attributes of the element
        SurfaceReference kReference = kIter->second;
        pkSurfaceElement->SetAttribute("LastRelativePath", 
            kReference.m_kPackageRelativePath.c_str());
        pkSurfaceElement->SetAttribute("AssetID", kReference.m_kPackageAssetID.c_str());
        pkSurfaceElement->SetAttribute("name", kReference.m_kSurfaceName.c_str());
        pkSurfaceElement->SetAttribute("position", kIter->first);
        pkSurfaceElement->SetAttribute("packageIteration", kReference.m_uiIteration);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::ReadConfiguration(efd::UInt32& uiSectorSize, efd::UInt32& uiNumLOD, 
    efd::UInt32& uiMaskSize, efd::UInt32& uiLowDetailSize, float& fMinElevation, 
    float& fMaxElevation, float& fVertexSpacing, float& fLowDetailSpecularPower, 
    float& fLowDetailSpecularIntensity, efd::UInt32& uiSurfaceCount)
{
    if (!m_bConfigurationValid)
        return false;

    uiSectorSize = m_uiSectorSize;
    uiNumLOD = m_uiNumLOD;
    uiMaskSize = m_uiMaskSize;
    uiLowDetailSize = m_uiLowDetailSize;
    fMinElevation = m_fMinElevation;
    fMaxElevation = m_fMaxElevation;
    fVertexSpacing = m_fVertexSpacing;
    uiSurfaceCount = m_uiSurfaceCount;
    fLowDetailSpecularPower = m_fLowDetailSpecularPower;
    fLowDetailSpecularIntensity = m_fLowDetailSpecularIntensity;

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion3::WriteConfiguration(efd::UInt32 uiSectorSize, efd::UInt32 uiNumLOD, 
    efd::UInt32 uiMaskSize, efd::UInt32 uiLowDetailSize, float fMinElevation, 
    float fMaxElevation, float fVertexSpacing, float fLowDetailSpecularPower, 
    float fLowDetailSpecularIntensity, efd::UInt32 uiSurfaceCount)
{
    m_bConfigurationValid = true;

    m_uiSectorSize = uiSectorSize;
    m_uiNumLOD = uiNumLOD;
    m_uiMaskSize = uiMaskSize;
    m_uiLowDetailSize = uiLowDetailSize;
    m_fMinElevation = fMinElevation;
    m_fMaxElevation = fMaxElevation;
    m_fVertexSpacing = fVertexSpacing;
    m_uiSurfaceCount = uiSurfaceCount;
    m_fLowDetailSpecularPower = fLowDetailSpecularPower;
    m_fLowDetailSpecularIntensity = fLowDetailSpecularIntensity;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainFileVersion3::ReadSurface(NiUInt32 uiSurfaceIndex, 
    NiTerrainAssetReference* pkPackageRef, NiFixedString& kSurfaceID, efd::UInt32& uiIteration)
{   
    EE_ASSERT(pkPackageRef);

    SurfaceReferenceMap::iterator kIter = m_kSurfaceReferences.find(uiSurfaceIndex);
    if (kIter == m_kSurfaceReferences.end())
        return false;

    SurfaceReference kReference = kIter->second;

    if (!kReference.m_bValid)
        return false;

    pkPackageRef->SetAssetID(kReference.m_kPackageAssetID);
    pkPackageRef->SetRelativeAssetLocation(kReference.m_kPackageRelativePath);
    pkPackageRef->SetReferringAssetLocation((const char*)GenerateTerrainConfigFilename());
    kSurfaceID = kReference.m_kSurfaceName.c_str();
    uiIteration = kReference.m_uiIteration;

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainFileVersion3::WriteSurface(NiUInt32 uiSurfaceIndex, 
    NiTerrainAssetReference* pkPackageRef, NiFixedString kSurfaceID, efd::UInt32 uiIteration)
{
    EE_ASSERT(pkPackageRef);

    pkPackageRef->SetReferringAssetLocation((const char*)GenerateTerrainConfigFilename());

    SurfaceReference kReference;
    kReference.m_kPackageAssetID = pkPackageRef->GetAssetID();
    kReference.m_kPackageRelativePath = pkPackageRef->GetLastRelativeLocation();
    kReference.m_kSurfaceName = (const char*)kSurfaceID;
    kReference.m_uiIteration = uiIteration;
    kReference.m_bValid = true;

    m_kSurfaceReferences[uiSurfaceIndex] = kReference;
}

//--------------------------------------------------------------------------------------------------
