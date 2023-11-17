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

#include "ControlBuiltins.h"
#include <egf/ScriptContext.h>

//--------------------------------------------------------------------------------------------------
efd::UInt32 ecrInput::ListenForInputActionEvent(
    egf::EntityID entityID,
    efd::utf8string inputEventName,
    efd::utf8string behaviorName,
    efd::UInt32 callbackType)
{
    ecrInput::InputService* pInputService =
        egf::g_bapiContext.GetSystemServiceAs<ecrInput::InputService>();
    EE_ASSERT(pInputService);

    return pInputService->ListenForInputEvent(
        inputEventName,
        entityID,
        behaviorName,
        static_cast<ecrInput::InputService::CallbackType>(callbackType));
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 ecrInput::ListenForInputStickEvent(
    egf::EntityID entityID,
    efd::utf8string inputEventName,
    efd::utf8string behaviorName,
    efd::UInt32 callbackType)
{
    ecrInput::InputService* pInputService =
        egf::g_bapiContext.GetSystemServiceAs<ecrInput::InputService>();
    EE_ASSERT(pInputService);

    return pInputService->ListenForInputEvent(
        inputEventName,
        entityID,
        behaviorName,
        static_cast<ecrInput::InputService::CallbackType>(callbackType));
}

//--------------------------------------------------------------------------------------------------
void ecrInput::ClearRegisteredInputEvents(egf::EntityID entityID)
{
    ecrInput::InputService* pInputService =
        egf::g_bapiContext.GetSystemServiceAs<ecrInput::InputService>();
    EE_ASSERT(pInputService);

    pInputService->ClearRegisteredInputEvents(entityID);
}

//--------------------------------------------------------------------------------------------------
void ecrInput::ClearRegisteredInputEvent(efd::UInt32 callbackID)
{
    ecrInput::InputService* pInputService =
        egf::g_bapiContext.GetSystemServiceAs<ecrInput::InputService>();
    EE_ASSERT(pInputService);

    pInputService->ClearRegisteredInputEvent(callbackID);
}

//--------------------------------------------------------------------------------------------------
