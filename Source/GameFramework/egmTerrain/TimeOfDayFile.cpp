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

#include "egmTerrainPCH.h"

#include <NiPath.h>
#include <NiString.h>
#include <NiTerrainXMLHelpers.h>
#include "EnvironmentService.h"
#include "TimeOfDayFile.h"
#include "TimeOfDayFileVersion1.h"

using namespace egmTerrain;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(TimeOfDayFile);

//--------------------------------------------------------------------------------------------------

TimeOfDayFile* TimeOfDayFile::Open(const char* pInToDFile, 
    egf::EntityManager* pEntManager, bool writeAccess)
{
    TimeOfDayFile *pResult = NULL;

    // Check the filename:
    if (!pInToDFile)
        return NULL;

    // Allocate a temporary buffer for "Standardizing" the file path.
    size_t stLen = strlen(pInToDFile);
    char* pNonStandardToDFile = NiStackAlloc(char, stLen+1);

    // Standardize the file path.
    NiMemcpy(pNonStandardToDFile, pInToDFile, stLen+1);
    NiPath::Standardize(pNonStandardToDFile);

    // Set the Standardized file path and delete the temporary buffer.
    NiString toDFile(pNonStandardToDFile);
    NiStackFree(pNonStandardToDFile);

    // Attempt to load the file and determine the version
    efd::TiXmlDocument file(toDFile);
    TimeOfDayFile* pReadFile = NULL;
    if (NiTerrainXMLHelpers::LoadXMLFile(&file))
    {
        // Reset to the initial tag
        efd::TiXmlElement* pCurElement = file.FirstChildElement("TimeOfDay");
        if (!pCurElement)
            return NULL;

        // Inspect the file version
        const char* pVersion = pCurElement->Attribute("Version");
        if (!pVersion)
            return NULL;
        efd::UInt32 fileVersion = 0;
        if (!NiString(pVersion).ToUInt(fileVersion))
            return NULL;

        // Instantiate an appropriate reader for this type of file
        switch(fileVersion)
        {
        case ms_fileVersion:
            pReadFile = NiNew TimeOfDayFile(toDFile, false);
            break;
        case 1:
            pReadFile = NiNew TimeOfDayFileVersion1(toDFile, false);
        default:
            break;
        }

        if (!pReadFile->Initialize(pEntManager))
        {
            // The parser failed to initialize properly!
            NiDelete pReadFile;
            pReadFile = NULL;
        }
    }

    if (!writeAccess)
    {
        if (!pReadFile)
        {
            // File does not exist, so attempt loading using the old assets
            pResult = NULL;
        }
        else
        {
            pResult = pReadFile;
        }
    }
    else
    {
        // File does not exist, create a new one
        if (!pReadFile)
        {
            pResult = NiNew TimeOfDayFile(toDFile, writeAccess);
        }
        else
        {
            pResult = NiNew TimeOfDayFile(pReadFile, writeAccess);
            NiDelete pReadFile;
        }

        // Make sure the parser initializes ok!
        if (!pResult->Initialize(pEntManager))
        {
            // The parser failed to initialize properly!
            NiDelete pResult;
            pResult = NULL;
            return pResult;
        }

        pResult->WriteFileHeader();
    }

    return pResult;
}

//--------------------------------------------------------------------------------------------------
TimeOfDayFile::TimeOfDayFile(const char* fileName, bool writeAccess) : 
    m_writeAccess(false),
    m_ready(false),
    m_fileVersion(0)
{
    // Allocate a temporary buffer for "Standardizing" the file path.
    size_t stLen = strlen(fileName);
    char* pNonStandardToDFile = NiStackAlloc(char, stLen+1);

    // Standardize the file path.
    NiMemcpy(pNonStandardToDFile, fileName, stLen+1);
    NiPath::Standardize(pNonStandardToDFile);
    
    m_fileName = pNonStandardToDFile;
    m_writeAccess = writeAccess;

    // Free the temporary buffer.
    NiStackFree(pNonStandardToDFile);
}

//--------------------------------------------------------------------------------------------------
TimeOfDayFile::TimeOfDayFile(TimeOfDayFile* pFile, bool writeAccess) : 
    m_writeAccess(false),
    m_ready(false),
    m_fileVersion(0)
{
    // Allocate a temporary buffer for "Standardizing" the file path.
    size_t stLen = strlen(pFile->m_fileName.c_str());
    char* pNonStandardToDFile = NiStackAlloc(char, stLen+1);

    // Standardize the file path.
    NiMemcpy(pNonStandardToDFile, pFile->m_fileName.c_str(), stLen+1);
    NiPath::Standardize(pNonStandardToDFile);
    
    m_fileName = pNonStandardToDFile;
    m_writeAccess = writeAccess;

    // Free the temporary buffer.
    NiStackFree(pNonStandardToDFile);

    EntityToPropertyMap::iterator entityIter = pFile->m_entityToPropertyMap.begin();
    while (entityIter != pFile->m_entityToPropertyMap.end())
    {
        AddEntity(entityIter->first, pFile->m_registeredEntities[entityIter->first]);
        PropertyVector::iterator propIter = entityIter->second.begin();
        while (propIter != entityIter->second.end())
        {
            AddProperty(entityIter->first, *propIter);

            KeyframeVector keyframes;
            if (pFile->m_propertyMap.find(propIter->m_propertyName, keyframes))
            {
                KeyframeVector::iterator keyIter = keyframes.begin();
                while (keyIter != keyframes.end())
                {
                    AddKeyframe(propIter->m_propertyName, keyIter->time, keyIter->value);
                    ++keyIter;
                }
            }

            ++propIter;
        }

        ++entityIter;
    }
}

//--------------------------------------------------------------------------------------------------
TimeOfDayFile::~TimeOfDayFile()
{
    m_spEntityManager = 0;

    // Release the stream buffers:
    if (IsWritable())
    {
        m_file.SaveFile();
    }
}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFile::Initialize(egf::EntityManager* pEntManager)
{
    m_file = efd::TiXmlDocument(m_fileName.c_str());
    if (!m_writeAccess && !NiTerrainXMLHelpers::LoadXMLFile(&m_file) || !pEntManager)
        return false;

    m_spEntityManager = pEntManager;

    // If we are reading from the file, then begin setting up:
    if (!m_writeAccess)
    {
        // Reset to the initial tag
        efd::TiXmlElement* pCurElement = m_file.FirstChildElement("TimeOfDay");
        if (!pCurElement)
            return NULL;

        // Read the file version
        m_fileVersion = 0;
        if (!NiString(pCurElement->Attribute("Version")).ToUInt(m_fileVersion))
            return false;
        
        // Read the properties
        if (!ReadPropertySet(pCurElement))
            return false;
    }
    else
    {
        m_fileVersion = ms_fileVersion;
    }

    m_ready = true;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFile::WriteFileHeader()
{
    if (!IsReady() || !IsWritable())
        return false;

    NiTerrainXMLHelpers::WriteXMLHeader(&m_file);
    efd::TiXmlElement* pToDElement = NiTerrainXMLHelpers::CreateElement("TimeOfDay", NULL);
    pToDElement->SetAttribute("Version", m_fileVersion);
    m_file.LinkEndChild(pToDElement);

    return true;
}

//--------------------------------------------------------------------------------------------------
void TimeOfDayFile::WriteFileContent()
{
    EntityToPropertyMap::iterator entityIter = m_entityToPropertyMap.begin();
    
    while (entityIter != m_entityToPropertyMap.end())
    {
        WritePropertySet(entityIter->first);
        ++entityIter;
    }
}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFile::WritePropertySet(efd::ID128 entityID)
{    
    if (!IsReady() || !IsWritable())
        return false;

    efd::utf8string entityFileID;
    efd::ParseHelper<efd::ID128>::ToString(entityID, entityFileID);
    efd::utf8string entityName; 
    m_registeredEntities.find(entityID, entityName);

    efd::TiXmlElement* pRootElement = m_file.FirstChildElement("TimeOfDay");
    efd::TiXmlElement* pEntityElement = NiTerrainXMLHelpers::CreateElement("Entity", pRootElement);
    pEntityElement->SetAttribute("EntityName", entityName.c_str());
    pEntityElement->SetAttribute("EntityID",  entityFileID.c_str());

    // Parse the list of properties
    PropertyVector::iterator iter = m_entityToPropertyMap[entityID].begin();
    while (iter != m_entityToPropertyMap[entityID].end())
    {
        efd::TiXmlElement* pPropertyElement = NiTerrainXMLHelpers::CreateElement(
            "Property", pEntityElement);

        pPropertyElement->SetAttribute("Name", iter->m_propertyName.c_str());
        
        if (!iter->m_propertyType.empty())
            pPropertyElement->SetAttribute("Type", iter->m_propertyType.c_str());

        if (!iter->m_defaultValue.empty())
            pPropertyElement->SetAttribute("DefaultValue", iter->m_defaultValue.c_str());
        
        if (!iter->m_minValue.empty())
            pPropertyElement->SetAttribute("MinValue", iter->m_minValue.c_str());
        
        if (!iter->m_maxValue.empty())
            pPropertyElement->SetAttribute("MaxValue", iter->m_maxValue.c_str());

        // Parse all the keyframes associated with this property
        KeyframeVector keyframes = m_propertyMap[iter->m_propertyName];
        KeyframeVector::iterator keyframeIter = keyframes.begin();
        while (keyframeIter != keyframes.end())
        {
            efd::TiXmlElement* pKeyframeElement = NiTerrainXMLHelpers::CreateElement(
            "Keyframe", pPropertyElement);

            pKeyframeElement->SetAttribute("Time", keyframeIter->time.c_str());
            pKeyframeElement->SetAttribute("Value", keyframeIter->value.c_str());
            ++keyframeIter;
        }

        ++iter;
        
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFile::ReadPropertySet(const efd::TiXmlElement* pRootElement)
{
    const efd::TiXmlElement* pEntElement = pRootElement->FirstChildElement("Entity");

    while (pEntElement)
    {
        if (!pEntElement->Attribute("EntityID"))
        {
            // Can not add a property without a name!
            pEntElement = pEntElement->NextSiblingElement("Entity");
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("ToDFile: Attempting to read an entity without an entityID. "
                "This entity will be skipped. Saving the file will solve the issue."));
            continue;
        }

        efd::utf8string entityFileIDStr = pEntElement->Attribute("EntityID");
        efd::ID128 entityFileID;
        efd::ParseHelper<efd::ID128>::FromString(entityFileIDStr, entityFileID);
        egf::Entity* pEntity = m_spEntityManager->LookupEntityByDataFileID(entityFileID);

        efd::utf8string modelName = "";
        if (pEntElement->Attribute("EntityName") != NULL)
            modelName = pEntElement->Attribute("EntityName");
        
        if (!pEntity || (modelName.length() != 0 && pEntity->GetModelName() != modelName))
        {
            // Could not find the entity!
            pEntElement = pEntElement->NextSiblingElement("Entity");
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("ToDFile: The entity ID %s does not have any corresponding entity in the scene. "
                "The entity will be skipped. This is due to reading a .tod file that was created "
                "for another entity and is no longer in the scene. Saving the file will resolve "
                "the issue by removing the unused data.", entityFileIDStr));
            continue;
        }

        AddEntity(entityFileID, modelName);
        const efd::TiXmlElement* pCurElement = pEntElement->FirstChildElement("Property");
        if(pCurElement)
        {
            EnvironmentService::ToDProperty newProperty;
            
            // Loop through all the properties
            while (pCurElement)
            {
                if (pCurElement->Attribute("Name") != NULL)
                {
                    newProperty.m_propertyName = pCurElement->Attribute("Name");
                    if (newProperty.m_propertyName.substr(0, 
                        entityFileIDStr.length()) != entityFileIDStr)
                    {
                        pCurElement = pCurElement->NextSiblingElement("Property");
                        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                            ("ToDFile: Attempting to read a property with a name that does not "
                            "correspond to the entity being loaded. this property will be skipped. "
                            "Saving the file will solve the issue."));
                        continue;
                    }
                }
                else 
                {
                    // Can not add a property without a name!
                    pCurElement = pCurElement->NextSiblingElement("Property");
                    EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                        ("ToDFile: Attempting to read a property without a name. "
                        "This property will be skipped. Saving the file will solve the issue."));
                    continue;
                }

                if (pCurElement->Attribute("Type") != NULL)
                    newProperty.m_propertyType = pCurElement->Attribute("Type");
                else
                    newProperty.m_propertyType = "";
                
                if (pCurElement->Attribute("DefaultValue") != NULL)
                    newProperty.m_defaultValue = pCurElement->Attribute("DefaultValue");
                else
                    newProperty.m_defaultValue = "";

                if (pCurElement->Attribute("MinValue") != NULL)
                    newProperty.m_minValue = pCurElement->Attribute("MinValue");
                else
                    newProperty.m_minValue = "";

                if (pCurElement->Attribute("MaxValue") != NULL)
                    newProperty.m_maxValue = pCurElement->Attribute("MaxValue");
                else
                    newProperty.m_maxValue = "";

                AddProperty(entityFileID, newProperty);
                
                const efd::TiXmlElement* pKeyframe = pCurElement->FirstChildElement("Keyframe");
                efd::utf8string time;
                efd::utf8string value;
                
                // Loop through all the keyframes
                while (pKeyframe)
                {
                    if (pKeyframe->Attribute("Time") != NULL)
                    {
                        time = pKeyframe->Attribute("Time");
                    }
                    else
                    {

                        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                            ("ToDFile: Attempting to read a keyframe without a time for property %s. "
                            "This keyframe will be skipped. Saving the file will solve the issue.",
                            newProperty.m_propertyName.c_str()));
                        pKeyframe = pKeyframe->NextSiblingElement("Keyframe");
                        continue;
                    }

                    if (pKeyframe->Attribute("Value") != NULL)
                    {
                        value = pKeyframe->Attribute("Value");
                    }
                    else
                    {
                        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                            ("ToDFile: Attempting to read a keyframe without a value for property %s. "
                            "This keyframe will be skipped. Saving the file will solve the issue.",
                            newProperty.m_propertyName.c_str()));
                        pKeyframe = pKeyframe->NextSiblingElement("Keyframe");
                        continue;
                    }

                    AddKeyframe(newProperty.m_propertyName, time, value);
                    pKeyframe = pKeyframe->NextSiblingElement("Keyframe");
                }
                
                // Move onto the next property
                pCurElement = pCurElement->NextSiblingElement("Property");
            }
        }

        pEntElement = pEntElement->NextSiblingElement("Entity");
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFile::AddKeyframe(efd::utf8string propertyName, efd::utf8string time, 
    efd::utf8string value)
{
    PropertyToKeyframeMap::iterator it;
    it = m_propertyMap.find(propertyName);
    
    if (it == m_propertyMap.end())
    {
        KeyframeVector keyframeSequence;
        m_propertyMap[propertyName] = keyframeSequence;
    }    

    Keyframe toAdd;
    toAdd.time = time;
    toAdd.value = value;
    
    m_propertyMap[propertyName].push_back(toAdd);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::vector<TimeOfDayFile::Keyframe> TimeOfDayFile::GetPropertyKeyframes(
    efd::utf8string propertyName)
{
    return m_propertyMap[propertyName];
}

//--------------------------------------------------------------------------------------------------
void TimeOfDayFile::GetPropertyKeyframes(efd::utf8string propertyName, 
    efd::vector<efd::utf8string>& times,
    efd::vector<efd::utf8string>& values)
{
    KeyframeVector keyframes = m_propertyMap[propertyName];
    KeyframeVector::iterator iter = keyframes.begin();

    while (iter != keyframes.end())
    {
        times.push_back(iter->time);
        values.push_back(iter->value);
        ++iter;
    }
}

//--------------------------------------------------------------------------------------------------
efd::vector<EnvironmentService::ToDProperty> TimeOfDayFile::GetProperties(efd::ID128 entityID)
{
    return m_entityToPropertyMap[entityID];
}

//--------------------------------------------------------------------------------------------------
