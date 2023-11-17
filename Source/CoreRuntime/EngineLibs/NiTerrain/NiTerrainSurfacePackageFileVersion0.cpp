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
#include "NiTerrainSurfacePackageFileVersion0.h"
#include "NiTerrainXMLHelpers.h"
#include <efd/ecrLogIDs.h>

static const char* XML_ELEMENT_PACKAGE = "Package";
static const char* XML_ELEMENT_SURFACE = "Surface";
static const char* XML_ELEMENT_METADATA = "MetaData";
static const char* XML_ATTRIBUTE_PACKAGENAME = "name";
static const char* XML_ATTRIBUTE_SURFACENAME = "name";
static const char* XML_ATTRIBUTE_UVSCALE = "UVScaleModifier";
static const char* XML_ATTRIBUTE_DIFFUSEMAP = "DiffuseMap";
static const char* XML_ATTRIBUTE_NORMALMAP = "NormalMap";

//--------------------------------------------------------------------------------------------------
NiTerrainSurfacePackageFileVersion0::NiTerrainSurfacePackageFileVersion0(efd::utf8string kFilename,
    bool bWriteAccess)
    : NiTerrainSurfacePackageFile(kFilename, bWriteAccess)
    , m_bConfigurationValid(false)
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainSurfacePackageFileVersion0::~NiTerrainSurfacePackageFileVersion0()
{
    // Release the collection of surface information
    efd::vector<SurfaceData*>::iterator kIter;
    for (kIter = m_kSurfaces.begin(); kIter != m_kSurfaces.end(); ++kIter)
    {
        NiDelete(*kIter);
    }
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFileVersion0::GetFilePaths(efd::set<efd::utf8string>& kFilePaths)
{
    kFilePaths.insert(m_kPackageFilename);
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::Initialize()
{
    m_kFile = efd::TiXmlDocument(m_kPackageFilename.c_str());
    if (!m_bWriteAccess && !NiTerrainXMLHelpers::LoadXMLFile(&m_kFile))
        return false;

    // Initialize the variables:
    m_kSurfaces.clear();

    // If we are reading from the file, then begin setting up:
    if (!m_bWriteAccess)
    {
        // Reset to the initial tag
        efd::TiXmlElement* pkCurElement = m_kFile.FirstChildElement(XML_ELEMENT_PACKAGE);            
        if (!pkCurElement)
            return NULL;

        // Read the file version
        m_kFileVersion = 0;

        // Read the configuration data
        if (!ReadOldPackage(pkCurElement))
            return false;
    }
    else
    {
        m_kFileVersion = ms_kFileVersion0;
        /// This version does not support saving any more
        return false;
    }

    bool bResult = NiTerrainSurfacePackageFile::Initialize();
    return bResult;
}

//--------------------------------------------------------------------------------------------------
void NiTerrainSurfacePackageFileVersion0::Close()
{
    NiTerrainSurfacePackageFile::Close();
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadOldPackage(const efd::TiXmlElement* pkRootElement)
{
    const char* pcPackageName = 0;

    // Extract the package name
    pcPackageName = pkRootElement->Attribute(XML_ATTRIBUTE_PACKAGENAME);
    if (pcPackageName == 0)
        pcPackageName = "";
    m_kPackageName = pcPackageName;
    m_bConfigurationValid = true;

    // Extract the iteration count
    m_uiIteration = 0;

    // Extract all the surfaces from the file
    const efd::TiXmlElement* pkCurElement = pkRootElement->FirstChildElement(XML_ELEMENT_SURFACE);
    if (pkCurElement)
    {
        // Loop through all surfaces in the list and collect the information
        while (pkCurElement)
        {
            SurfaceData kTempSurfaceData;
            if (ReadOldSurface(pkCurElement, kTempSurfaceData))
            {
                // Add the surface to the list
                SurfaceData* pkSurfaceData = NiNew SurfaceData();
                *pkSurfaceData = kTempSurfaceData;
                m_kSurfaces.push_back(pkSurfaceData);
            }

            // Move onto the next surface
            pkCurElement = pkCurElement->NextSiblingElement();
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadOldSurface(const efd::TiXmlElement* pkSurfaceElement,
    SurfaceData& kTempSurfaceData)
{
    // Read the name of the surface:
    const char* pcName = pkSurfaceElement->Attribute(XML_ATTRIBUTE_SURFACENAME);
    if (!pcName)
        return false;
    kTempSurfaceData.m_kName = pcName;

    // Read the surface attributes:
    NiPoint2 kScaleModifier;
    NiTerrainXMLHelpers::ReadElement(pkSurfaceElement, XML_ATTRIBUTE_UVSCALE, kScaleModifier);
    kTempSurfaceData.m_fTextureTiling = kScaleModifier.x;
    
    // Read in the different textures
    const char* pucDiffuseMap = NULL;
    NiTerrainXMLHelpers::ReadElement(pkSurfaceElement, XML_ATTRIBUTE_DIFFUSEMAP, pucDiffuseMap);
    if (!pucDiffuseMap)
        pucDiffuseMap = "";
    kTempSurfaceData.m_akTextureSlots[NiSurface::SURFACE_MAP_DIFFUSE].m_kLastRelativePath = 
        pucDiffuseMap;

    const char* pucNormalMap = NULL;
    NiTerrainXMLHelpers::ReadElement(pkSurfaceElement, XML_ATTRIBUTE_NORMALMAP, pucNormalMap);
    if (!pucNormalMap)
        pucNormalMap = "";
    kTempSurfaceData.m_akTextureSlots[NiSurface::SURFACE_MAP_NORMAL].m_kLastRelativePath = 
        pucNormalMap;

    // Load in all the meta data for the surface
    const efd::TiXmlElement* pkMetaDataElement = 
        pkSurfaceElement->FirstChildElement(XML_ELEMENT_METADATA);
    if (pkMetaDataElement)
        kTempSurfaceData.m_kMetaData.Load(pkMetaDataElement);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadPackageConfig(efd::utf8string& kPackageName,
    efd::UInt32& uiIteration)
{
    if (!m_bConfigurationValid)
        return false;

    kPackageName = m_kPackageName;
    uiIteration = m_uiIteration;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadNumSurfaces(efd::UInt32& uiNumSurfaces)
{
    if (!m_bConfigurationValid)
        return false;
    
    uiNumSurfaces = m_kSurfaces.size();

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadSurfaceConfig(efd::UInt32 uiSurfaceIndex,
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
    if (uiSurfaceIndex >= m_kSurfaces.size())
        return false;
    SurfaceData* pkSurfaceData = m_kSurfaces[uiSurfaceIndex];

    kName = 
        pkSurfaceData->m_kName;
    fTextureTiling = 
        pkSurfaceData->m_fTextureTiling;
    fDetailTiling = 
        pkSurfaceData->m_fDetailTiling;
    fRotation = 
        pkSurfaceData->m_fRotation;
    fParallaxStrength = 
        pkSurfaceData->m_fParallaxStrength;
    fDistributionMaskStrength = 
        pkSurfaceData->m_fDistributionMaskStrength;
    fSpecularPower = 
        pkSurfaceData->m_fSpecularPower;
    fSpecularIntensity = 
        pkSurfaceData->m_fSpecularIntensity;
    uiNumDecorationLayers = 
        pkSurfaceData->m_uiNumDecorationLayers;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadSurfaceSlot(efd::UInt32 uiSurfaceIndex,
    efd::UInt32 uiSlotID,
    NiTerrainAssetReference* pkReference)
{
    EE_ASSERT(pkReference);
    if (uiSurfaceIndex >= m_kSurfaces.size())
        return false;
    SurfaceData* pkSurfaceData = m_kSurfaces[uiSurfaceIndex];

    if (uiSlotID >= NiSurface::NUM_SURFACE_MAPS)
        return false;

    pkReference->SetReferringAssetLocation(m_kPackageFilename);
    pkReference->SetAssetID(pkSurfaceData->m_akTextureSlots[uiSlotID].m_kAssetID);
    pkReference->SetRelativeAssetLocation(
        pkSurfaceData->m_akTextureSlots[uiSlotID].m_kLastRelativePath);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiTerrainSurfacePackageFileVersion0::ReadSurfaceMetadata(efd::UInt32 uiSurfaceIndex,
    NiMetaData& kMetaData)
{
    if (uiSurfaceIndex >= m_kSurfaces.size())
        return false;
    SurfaceData* pkSurfaceData = m_kSurfaces[uiSurfaceIndex];

    kMetaData = pkSurfaceData->m_kMetaData;

    return true;
}

//--------------------------------------------------------------------------------------------------


