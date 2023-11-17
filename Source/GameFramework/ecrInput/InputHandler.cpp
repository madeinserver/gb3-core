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

#include "ecrInputPCH.h"

#include "InputHandler.h"

using namespace efd;
using namespace egf;
using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ecrInput::InputHandler);
EE_IMPLEMENT_BUILTINMODEL_PROPERTIES(ecrInput::InputHandler);

//--------------------------------------------------------------------------------------------------
InputHandler::InputHandler()
: m_enableInput(true)
, m_pInputService(NULL)
{
}

//--------------------------------------------------------------------------------------------------
InputHandler::~InputHandler()
{
}

//--------------------------------------------------------------------------------------------------
void InputHandler::OnEndLifecycle(efd::UInt32 lifecycle)
{
    switch (lifecycle)
    {
    case Entity::lifecycle_OnEntitySetFinished:
        m_pInputService = GetServiceManager()->GetSystemServiceAs<ecrInput::InputService>();
        if (m_enableInput)
        {
            EnableAllEvents();
        }
        break;

    case Entity::lifecycle_OnDestroy:
        if (m_enableInput)
        {
            DisableAllEvents();
        }
        m_pInputService = NULL;
        break;
    }
}

//--------------------------------------------------------------------------------------------------
/* static */
egf::IBuiltinModel* InputHandler::Factory()
{
    return EE_NEW InputHandler();
}

//--------------------------------------------------------------------------------------------------
egf::PropertyResult InputHandler::GetNormalEvent(const efd::utf8string& i_key,
                                   efd::utf8string& o_value) const
{
    EventMap::const_iterator iter = m_NormalEvents.find(i_key);
    if (iter != m_NormalEvents.end())
    {
        o_value = iter->second.first;
        return PropertyResult_OK;
    }
    return PropertyResult_KeyNotFound;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::SetNormalEvent(const efd::utf8string& i_key, const efd::utf8string& i_value)
{
    // If previous value, unregister that behavior
    EventMap::iterator iter = m_NormalEvents.find(i_key);
    if (iter != m_NormalEvents.end())
    {
        UnregisterBehavior(iter->second);
    }
    // then add the new behavior
    efd::UInt32 cbID = RegisterBehavior(ecrInput::InputService::CALLBACK_NORMAL, i_key, i_value);
    m_NormalEvents[i_key] = EventMapData(i_value, cbID);
}

//--------------------------------------------------------------------------------------------------
PropertyResult InputHandler::RemoveNormalEvent(const efd::utf8string& i_key)
{
    EventMap::iterator iter = m_NormalEvents.find(i_key);
    if (iter != m_NormalEvents.end())
    {
        UnregisterBehavior(iter->second);
        m_NormalEvents.erase(iter);
        return PropertyResult_OK;
    }
    return PropertyResult_KeyNotFound;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputHandler::GetNormalEventSize() const
{
    return m_NormalEvents.size();
}

//--------------------------------------------------------------------------------------------------
void InputHandler::GetNextNormalEventKey(const efd::utf8string& i_prevKey,
                           efd::utf8string& o_nextKey) const
{
    EventMap::const_iterator iter;
    if (i_prevKey.empty())
    {
        iter = m_NormalEvents.begin();
    }
    else
    {
        iter = m_NormalEvents.find(i_prevKey);
        if (iter == m_NormalEvents.end())
        {
            return /*PropertyResult_KeyNotFound*/;
        }

        ++iter;
    }

    if (iter == m_NormalEvents.end())
    {
        o_nextKey.clear();
    }
    else
    {
        o_nextKey = iter->first;
    }

    return /*PropertyResult_OK*/;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::ClearNormalEvents()
{
    if (m_enableInput)
    {
        for (EventMap::iterator iter = m_NormalEvents.begin();
            iter != m_NormalEvents.end();
            ++iter)
        {
            UnregisterBehavior(iter->second);
        }
    }
    m_NormalEvents.clear();
}

//--------------------------------------------------------------------------------------------------
egf::PropertyResult InputHandler::GetImmediateEvent(const efd::utf8string& i_key,
    efd::utf8string& o_value) const
{
    EventMap::const_iterator iter = m_ImmediateEvents.find(i_key);
    if (iter != m_ImmediateEvents.end())
    {
        o_value = iter->second.first;
        return PropertyResult_OK;
    }
    return PropertyResult_KeyNotFound;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::SetImmediateEvent(const efd::utf8string& i_key, const efd::utf8string& i_value)
{
    // If previous value, unregister that behavior
    EventMap::iterator iter = m_ImmediateEvents.find(i_key);
    if (iter != m_ImmediateEvents.end())
    {
        UnregisterBehavior(iter->second);
    }
    // then add the new behavior
    efd::UInt32 cbID = RegisterBehavior(ecrInput::InputService::CALLBACK_IMMEDIATE, i_key, i_value);
    m_ImmediateEvents[i_key] = EventMapData(i_value, cbID);
}

//--------------------------------------------------------------------------------------------------
PropertyResult InputHandler::RemoveImmediateEvent(const efd::utf8string& i_key)
{
    EventMap::iterator iter = m_ImmediateEvents.find(i_key);
    if (iter != m_ImmediateEvents.end())
    {
        UnregisterBehavior(iter->second);
        m_ImmediateEvents.erase(iter);
        return PropertyResult_OK;
    }
    return PropertyResult_KeyNotFound;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputHandler::GetImmediateEventSize() const
{
    return m_ImmediateEvents.size();
}

//--------------------------------------------------------------------------------------------------
void InputHandler::GetNextImmediateEventKey(const efd::utf8string& i_prevKey,
                              efd::utf8string& o_nextKey) const
{
    EventMap::const_iterator iter;
    if (i_prevKey.empty())
    {
        iter = m_ImmediateEvents.begin();
    }
    else
    {
        iter = m_ImmediateEvents.find(i_prevKey);
        if (iter == m_ImmediateEvents.end())
        {
            return /*PropertyResult_KeyNotFound*/;
        }

        ++iter;
    }

    if (iter == m_ImmediateEvents.end())
    {
        o_nextKey.clear();
    }
    else
    {
        o_nextKey = iter->first;
    }

    return /*PropertyResult_OK*/;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::ClearImmediateEvents()
{
    if (m_enableInput)
    {
        for (EventMap::iterator iter = m_ImmediateEvents.begin();
            iter != m_ImmediateEvents.end();
            ++iter)
        {
            UnregisterBehavior(iter->second);
        }
    }
    m_ImmediateEvents.clear();
}

//--------------------------------------------------------------------------------------------------
bool InputHandler::GetInputsEnabled() const
{
    return m_enableInput;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::SetInputsEnabled(const bool& value)
{
    if (value != m_enableInput)
    {
        m_enableInput = value;
        if (m_enableInput)
        {
            EnableAllEvents();
        }
        else
        {
            DisableAllEvents();
        }
    }
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputHandler::RegisterBehavior(ecrInput::InputService::CallbackType type,
    const utf8string& eventName,
    const utf8string& behaviorName)
{
    if (m_enableInput && m_pInputService)
    {
        efd::UInt32 cbid = m_pInputService->ListenForInputEvent(
            eventName,
            m_pOwningEntity->GetEntityID(),
            behaviorName,
            type);
        return cbid;
    }
    return ecrInput::kINVALID_CALLBACK;
}

//--------------------------------------------------------------------------------------------------
void InputHandler::UnregisterBehavior(EventMapData& io_data)
{
    if (m_enableInput && m_pInputService)
    {
        m_pInputService->ClearRegisteredInputEvent(io_data.second);
        io_data.second = ecrInput::kINVALID_CALLBACK;
    }
}

//--------------------------------------------------------------------------------------------------
void InputHandler::EnableAllEvents()
{
    if (m_pInputService)
    {
        for (EventMap::iterator iter = m_NormalEvents.begin();
            iter != m_NormalEvents.end();
            ++iter)
        {
            iter->second.second = RegisterBehavior(ecrInput::InputService::CALLBACK_NORMAL,
                iter->first,
                iter->second.first);
        }
        for (EventMap::iterator iter = m_ImmediateEvents.begin();
            iter != m_ImmediateEvents.end();
            ++iter)
        {
            iter->second.second = RegisterBehavior(ecrInput::InputService::CALLBACK_IMMEDIATE,
                iter->first,
                iter->second.first);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void InputHandler::DisableAllEvents()
{
    if (m_pInputService)
    {
        m_pInputService->ClearRegisteredInputEvents(m_pOwningEntity->GetEntityID());
    }
}
