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
#include "TimeOfDayFileVersion1.h"

using namespace egmTerrain;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(TimeOfDayFileVersion1);

//--------------------------------------------------------------------------------------------------
TimeOfDayFileVersion1::TimeOfDayFileVersion1(const char* pFileName, bool writeAccess) : 
    TimeOfDayFile(pFileName, writeAccess)
{

}

//--------------------------------------------------------------------------------------------------
bool TimeOfDayFileVersion1::ReadPropertySet(const efd::TiXmlElement* pRootElement)
{
    const efd::TiXmlElement* pEntElement = pRootElement->FirstChildElement("Entity");

    if (!pEntElement)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToDFile: Attempting to read a file using an invalid format as the Entity node "
            "could not be found. This entity will be skipped. Saving the file will solve "
            "the issue but might cause the loss of the file's content."));
    }    

    while (pEntElement)
    {
        if (!pEntElement->Attribute("EntityName"))
        {
            // Can not add a property without a name!
            pEntElement = pEntElement->NextSiblingElement("Entity");
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("ToDFile: Attempting to read an entity without a name. "
                "This entity will be skipped. Saving the file will solve the issue."));
            continue;
        }

        efd::utf8string entityName = pEntElement->Attribute("EntityName");

        // Find the first entity with this name
        egf::EntityManager::EntityMap::const_iterator entityIter =
            m_spEntityManager->GetFirstEntityPos();
        bool found = false;
        egf::Entity* pEntity = NULL;
        while (m_spEntityManager->GetNextEntity(entityIter, pEntity))
        {
            if (pEntity->GetModel()->GetName() == entityName)
            {
                found = true;
                break;
            }
        }
        
        if (!found)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("ToDFile: Could not find an existing entity implementing the model %s.", 
                entityName.c_str()));
            pEntElement = pEntElement->NextSiblingElement("Entity");
            continue;
        }

        efd::utf8string entityID;
        efd::ParseHelper<efd::ID128>::ToString(pEntity->GetDataFileID(), entityID);
        AddEntity(pEntity->GetDataFileID(), entityName);

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
                    if (newProperty.m_propertyName.substr(0, entityName.length()) != entityName)
                    {
                        pCurElement = pCurElement->NextSiblingElement("Property");
                        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                            ("ToDFile: Attempting to read a property with a name that does not "
                            "correspond to the entity being loaded. this property will be skipped. "
                            "Saving the file will solve the issue."));
                        continue;
                    }

                    newProperty.m_propertyName.replace(0, entityName.length(), entityID);
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

                AddProperty(pEntity->GetDataFileID(), newProperty);
                
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