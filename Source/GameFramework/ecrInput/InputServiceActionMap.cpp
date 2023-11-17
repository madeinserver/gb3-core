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

// Precompiled Header
#include "ecrInputPCH.h"

#include "InputService.h"
#include "InputServiceActionMap.h"

#include <efd/TinyXML.h>
#include <efd/ecrLogIDs.h>

using namespace ecrInput;

//--------------------------------------------------------------------------------------------------
InputServiceActionMap::~InputServiceActionMap()
{
    DeleteAllEvents();
}

//--------------------------------------------------------------------------------------------------
// Input processing
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceActionMap::ProcessInput(
    InputService* pInput,
    efd::Float32 currentTime)
{
    if (!m_events.size())
        return false;

#if defined(EE_PLATFORM_WIN32)
    // Ignore the input if the application window doesn't have focus.
    if (!GetFocus())
        return false;
#endif

    efd::map<efd::utf8string, InputServiceEvent*>::iterator itor;
    efd::set<efd::UInt32> skipList;
    efd::set<efd::UInt32> execList;

    // Process combos first. Combos will fill skipped actions list
    for (itor = m_events.begin(); itor != m_events.end(); ++itor)
    {
        InputServiceEvent* pEvent = itor->second;
        efd::UInt32 flags = pEvent->GetFlags();
        if (flags & InputService::ACTION_COMBO)
        {
            pEvent->ProcessInput(pInput, itor->first, currentTime, skipList, execList);
        }
    }

    // Then process all other events
    for (itor = m_events.begin(); itor != m_events.end(); ++itor)
    {
        InputServiceEvent* pEvent = itor->second;
        efd::UInt32 flags = pEvent->GetFlags();
        if (!(flags & InputService::ACTION_COMBO))
        {
            pEvent->ProcessInput(pInput, itor->first, currentTime, skipList, execList);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
// Serialization
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceActionMap::SaveXml(efd::TiXmlElement* pElement)
{
    pElement->SetAttribute("ActionMapName", m_name.c_str());

    efd::TiXmlElement* pEventsList = EE_EXTERNAL_NEW efd::TiXmlElement("EventsList");
    pElement->LinkEndChild(pEventsList);

    for (efd::map<efd::utf8string, InputServiceEvent*>::iterator itor = m_events.begin();
        itor != m_events.end();
        ++itor)
    {
        efd::TiXmlElement* pEventElement = EE_EXTERNAL_NEW efd::TiXmlElement("Event");
        pEventsList->LinkEndChild(pEventElement);

        pEventElement->SetAttribute("EventName", itor->first.c_str());

        itor->second->SaveXml(pEventElement);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceActionMap::LoadXml(efd::TiXmlElement* pElement)
{
    // Check for valid action map name and existence of event list element
    const efd::Char* pName = pElement->Attribute("ActionMapName");
    if (!pName)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Couldn't find attribute 'ActionMapName' in XML file."));
        return false;
    }

    m_name = pName;

    // Check for presence of event list
    efd::TiXmlElement* pEventElement = pElement->FirstChildElement("EventsList");

    if (!pEventElement)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Couldn't find element 'EventsList' in XML file."));
        return false;
    }

    // Remove all current events
    DeleteAllEvents();

    // Iterate through events list
    for (pEventElement = pEventElement->FirstChildElement();
        pEventElement;
        pEventElement = pEventElement->NextSiblingElement())
    {
        // Create new event instance
        InputServiceEvent* pEvent = EE_NEW InputServiceEvent(
            efd::kCAT_INVALID,
            InputService::RETURN_MAGNITUDE);
        efd::utf8string eventName = pEventElement->Attribute("EventName");

        if (eventName.empty())
        {
            EE_DELETE pEvent;
            continue;
        }

        // Load event and remove it if it has failed to load
        if (!pEvent->LoadXml(pEventElement))
        {
            EE_DELETE pEvent;
            continue;
        }

        InputServiceEvent* pOldEvent;
        if (m_events.find(eventName, pOldEvent))
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR3,
                ("Overriding event '%s' because of duplicate name.", eventName.c_str()));
            EE_DELETE pOldEvent;
        }

        // Register event
        m_events[eventName] = pEvent;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
