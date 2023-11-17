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

// Precompiled header
#include "ecrInputPCH.h"

#include "InputService.h"
#include "InputServiceEvent.h"
#include "efd/MessageService.h"
#include <efd/ParseHelper.h>
#include <efd/ecrLogIDs.h>

using namespace ecrInput;

ecrInput::FlagMap InputServiceEvent::ms_eventFlagsMap;
ecrInput::FlagMap InputServiceEvent::ms_actionClsIDMap;

//--------------------------------------------------------------------------------------------------
InputServiceEvent::~InputServiceEvent()
{
    RemoveAllActions();
}

//--------------------------------------------------------------------------------------------------
// Input processing
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceEvent::ProcessInput(
    InputService* pInput,
    const efd::utf8string& eventName,
    efd::Float32 currentTime,
    efd::set<efd::UInt32>& skipList,
    efd::set<efd::UInt32>& execList)
{
    // Exit if no actions defined
    if (m_actions.empty())
        return false;

    bool send = false;
    efd::UInt32 appData = 0;
    efd::Float32 magnitude = 0.0f;
    efd::Float32 x = 0.0f;
    efd::Float32 y = 0.0f;
    efd::Float32 z = 0.0f;

    if (m_flags & InputService::ACTION_COMBO)
    {
        // Reset combo if it is started and has expired
        if (m_currentAction && currentTime - m_lastActionTime > m_timeout)
        {
            while (m_currentAction)
            {
                // Fill execution list with all completed actions from combo
                if (!(m_flags & InputService::COMBO_SEND_ACTIONS))
                {
                    execList.insert(m_actions[m_currentAction - 1]->GetHash());
                }
                m_currentAction--;
            }
        }

        if (m_actions[m_currentAction]->ProcessInput(pInput, appData, magnitude, x, y, z))
        {
            // Advance combo if pending action occurs, save start & last time
            if (!m_currentAction)
                m_comboStartTime = currentTime;

            m_lastActionTime = currentTime;

            // Store occurred action in skip list
            if (!(m_flags & InputService::COMBO_SEND_ACTIONS))
                skipList.insert(m_actions[m_currentAction]->GetHash());

            m_currentAction++;

            if (m_currentAction == m_actions.size())
            {
                // We have reached the end of combo sequence! Reset and report.
                m_currentAction = 0;

                // Send total combo time
                send = true;
                magnitude = currentTime - m_comboStartTime;
            }
        }
    }
    else
    {
        // This is a OR or AND actions, go through them
        for (efd::UInt32 i = 0; i < m_actions.size(); i++)
        {
            InputServiceActionBase* pAction = m_actions[i];
            bool skip = skipList.find(pAction->GetHash()) != skipList.end();
            bool exec = execList.find(pAction->GetHash()) != execList.end();

            bool result;
            if (!skip && !exec)
            {
                result = pAction->ProcessInput(pInput, appData, magnitude, x, y, z);
            }
            else
            {
                result = !skip || exec;
            }

            if (m_flags & InputService::ACTION_AND)
            {
                // Exit if at least one AND action is not triggered
                if (!result)
                    return false;
            }
            else
            {
                send |= result;

                // Exit loop if OR action triggered
                if (send)
                    break;
            }
        }

        if (m_flags & InputService::ACTION_AND)
            send = true;
    }

    if (send)
    {
        if (m_flags & InputService::RETURN_COORD)
        {
            pInput->FireStickAction(eventName, appData, magnitude, m_msgCategory, x, y, z);
        }
        else
        {
            pInput->FireButtonAction(eventName, appData, magnitude, m_msgCategory);
        }

        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
// XML serialization
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceEvent::SaveXml(efd::TiXmlElement* pElement)
{
    if (m_msgCategory.IsValid())
    {
        efd::utf8string catStr;
        efd::ParseHelper<efd::UInt64>::ToString(m_msgCategory.GetValue(), catStr);
        pElement->SetAttribute("EventCategory", catStr.c_str());
    }
    WriteFlagsToAttribute(pElement, "EventFlags", ms_eventFlagsMap, m_flags);
    pElement->SetDoubleAttribute("Timeout", m_timeout);

    efd::TiXmlElement* pActionsList = EE_EXTERNAL_NEW efd::TiXmlElement("ActionsList");
    pElement->LinkEndChild(pActionsList);

    for (efd::UInt32 i = 0; i < m_actions.size(); i++)
    {
        efd::TiXmlElement* pActionElement = EE_EXTERNAL_NEW efd::TiXmlElement("Action");
        pActionsList->LinkEndChild(pActionElement);

        m_actions[i]->SaveXml(pActionElement);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceEvent::LoadXml(efd::TiXmlElement* pElement)
{
    const char* eventCategory = pElement->Attribute("EventCategory");
    if (eventCategory)
    {
        efd::UInt64 value;
        efd::ParseHelper<efd::UInt64>::FromString(eventCategory, value);
        m_msgCategory = value;
        if (!m_msgCategory.IsValid())
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                ("Couldn't retrieve attribute 'EventCategory' in XML file."));
            return false;
        }
    }

    // Test for an int value first, it is possible to have an int field that is either
    // generated by the SaveXML() call or by the user manually calculating the field
    // or it could be a text entry from InputService.h EventFlags enum
    if (!ReadFlagsFromAttribute(pElement, "EventFlags", ms_eventFlagsMap, m_flags))
    {
        return false;
    }

    pElement->QueryFloatAttribute("Timeout", &m_timeout);
    // Check for presence of actions list
    efd::TiXmlElement* pActionElement = pElement->FirstChildElement("ActionsList");

    if (!pActionElement)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Couldn't find element 'ActionsList' in XML file."));
        return false;
    }

    RemoveAllActions();

    for (pActionElement = pActionElement->FirstChildElement();
        pActionElement;
        pActionElement = pActionElement->NextSiblingElement())
    {
        // As above, this may be an int value or a string
        efd::UInt32 actionClsID;
        ReadEnumFromAttribute(pActionElement, "ActionClsID", ms_actionClsIDMap, actionClsID);

        InputServiceActionBase* pAction = InputService::CreateAction(actionClsID);

        if (!pAction)
            continue;

        if (!pAction->LoadXml(pActionElement))
        {
            EE_DELETE pAction;
            continue;
        }

        m_actions.push_back(pAction);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void InputServiceEvent::LoadTranslationMap()
{
    // This must be kept in sync with InputService.h EventFlags enum
    ms_eventFlagsMap["RETURN_MAGNITUDE"] = InputService::RETURN_MAGNITUDE;
    ms_eventFlagsMap["RETURN_COORD"] = InputService::RETURN_COORD;
    ms_eventFlagsMap["RETURN_MASK"] = InputService::RETURN_MASK;
    ms_eventFlagsMap["ACTION_AND"] = InputService::ACTION_AND;
    ms_eventFlagsMap["ACTION_COMBO"] = InputService::ACTION_COMBO;
    ms_eventFlagsMap["COMBO_SEND_ACTIONS"] = InputService::COMBO_SEND_ACTIONS;
    ms_eventFlagsMap["ACTION_MASK"] = InputService::ACTION_MASK;

    ms_actionClsIDMap["ACTION"] = EE_NIS_ACTION_CLSID_ACTION;
    ms_actionClsIDMap["DPAD"] = EE_NIS_ACTION_CLSID_DPAD;
    ms_actionClsIDMap["STICK"] = EE_NIS_ACTION_CLSID_STICK;
    ms_actionClsIDMap["MOUSE"] = EE_NIS_ACTION_CLSID_MOUSE;
}

//--------------------------------------------------------------------------------------------------

void InputServiceEvent::UnloadTranslationMap()
{
    ms_eventFlagsMap.clear();
    ms_actionClsIDMap.clear();
}

//--------------------------------------------------------------------------------------------------
