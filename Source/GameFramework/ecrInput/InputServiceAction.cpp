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

#include "InputServiceAction.h"
#include "InputService.h"

#include <efd/TinyXML.h>

using namespace ecrInput;

efd::map<efd::utf8string, efd::UInt32> InputServiceAction::ms_semanticMap;

//--------------------------------------------------------------------------------------------------
InputServiceAction::~InputServiceAction()
{
}

//--------------------------------------------------------------------------------------------------
// XML serialization methods
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::SaveXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::SaveXml(pElement);

    pElement->SetAttribute("ActionClsID", "ACTION");
    WriteEnumToAttribute(pElement, "Semantic", ms_semanticMap, m_semantic);
    pElement->SetDoubleAttribute("MinRange", m_minRange);
    pElement->SetDoubleAttribute("MaxRange", m_maxRange);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::LoadXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::LoadXml(pElement);

    if (!ReadEnumFromAttribute(pElement, "Semantic", ms_semanticMap, m_semantic))
    {
        return false;
    }

    pElement->QueryFloatAttribute("MinRange", &m_minRange);
    pElement->QueryFloatAttribute("MaxRange", &m_maxRange);

    // Update device control from semantic
    SetSemantic(m_semantic);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::ProcessInput(
    InputService* pInputService,
    efd::UInt32& appData,
    efd::Float32& magnitude,
    efd::Float32& x,
    efd::Float32& y,
    efd::Float32& z)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(z);

    bool bSend = false;
    NiInputSystem* pInputSystem = pInputService->GetInputSystem();

    if (IsKeyboardAction())
    {
        bSend = ProcessKeyboardAction(pInputSystem->GetKeyboard(), magnitude);
    }
    else if (IsMouseAction())
    {
        bSend = ProcessMouseAction(pInputSystem->GetMouse(), magnitude);
    }
    else if (IsGamePadAction())
    {
        NiInputGamePad* pGamePad;
        if (m_flags & InputService::DEVICE_SPECIFIC)
        {
            pGamePad = pInputSystem->GetGamePad(m_deviceID);
            bSend = ProcessGamePadAction(pGamePad, magnitude);
        }
        else
        {
            // Action is not device-specific, check all game pads
            for (efd::UInt32 i = 0; i < NiInputSystem::MAX_GAMEPADS; i++)
            {
                pGamePad = pInputSystem->GetGamePad(i);
                if (!pGamePad)
                    continue;

                bSend = ProcessGamePadAction(pGamePad, magnitude);

                // If action is triggered on one of gamepad, exit loop
                if (bSend)
                    break;
            }
        }
    }

    if (bSend)
        appData = m_appData;

    return bSend;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::ProcessKeyboardAction(
    NiInputKeyboard* pKeyboard,
    efd::Float32& magnitude) const
{
    if (!pKeyboard)
        return false;

    const NiInputKeyboard::KeyCode key = (NiInputKeyboard::KeyCode)m_control;

    // Check modifiers if required
    if (m_flags & InputService::USE_MODIFIERS)
    {
        // If modifiers do not match, exit
        if (pKeyboard->GetModifiers() != m_modifiers)
            return false;
    }

    // Only digital keyboard
    return CheckDigitalAction(
        pKeyboard->KeyWasPressed(key),
        pKeyboard->KeyWasReleased(key),
        pKeyboard->KeyIsDown(key),
        magnitude);
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::ProcessMouseAction(
    NiInputMouse* pMouse,
    efd::Float32& magnitude) const
{
    if (!pMouse)
        return false;

    const NiInputMouse::Button button = (NiInputMouse::Button)m_control;

    // Check modifiers if required. Note that the button is itself a modifier included in
    // the value returned by pMouse->GetModifiers.
    if (m_flags & InputService::USE_MODIFIERS)
    {
        if (pMouse->GetModifiers() != (m_modifiers | (1 << button)) &&
            pMouse->GetModifiers() != m_modifiers)
        {
            return false;
        }
    }

    if (IsAxis())
    {
        // Mouse returns axes deltas
        efd::SInt32 delta = 0;
        efd::SInt32 lastDelta = 0;

        const NiInputMouse::Axes axis = (NiInputMouse::Axes)m_control;

        pMouse->GetPositionDelta(axis, delta);
        pMouse->GetPositionLastDelta(axis, lastDelta);

        magnitude = (efd::Float32)delta;
        efd::Float32 lastMagnitude = (efd::Float32)lastDelta;

        return CheckAnalogAction(magnitude, lastMagnitude);
    }

    // No analog or ranged buttons for mouse
    return CheckDigitalAction(
        pMouse->ButtonWasPressed(button),
        pMouse->ButtonWasReleased(button),
        pMouse->ButtonIsDown(button),
        magnitude);
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::ProcessGamePadAction(
    NiInputGamePad* pGamePad,
    efd::Float32& magnitude) const
{
    if (!pGamePad)
        return false;

    // Check modifiers if required
    if (m_flags & InputService::USE_MODIFIERS)
    {
        // If modifiers do not match, exit.
        const efd::UInt32 currMods = pGamePad->GetModifiers();

        if (currMods != (m_modifiers | 1 << m_control) &&
            currMods != m_modifiers)
        {
            return false;
        }
    }

    efd::Float32 lastMagnitude = 0.0f;

    // Calculate button current and last magnitude
    if (IsAxis())
    {
        magnitude = (efd::Float32)pGamePad->GetStickAxisValue(m_control) / 255.0f;

        lastMagnitude = (efd::Float32)pGamePad->GetStickAxisLastValue(m_control) / 255.0f;

        return CheckAnalogAction(magnitude, lastMagnitude);
    }

    const NiInputGamePad::Button button = (NiInputGamePad::Button)m_control;
    magnitude = (efd::Float32)pGamePad->ButtonState(button) / 255.0f;
    lastMagnitude = (efd::Float32)pGamePad->ButtonLastState(button) / 255.0f;

    // Check if button is wanted to be digital or analog
    if (m_flags & InputService::ANALOG)
    {
        return CheckAnalogAction(magnitude, lastMagnitude);
    }

    // Action is digital. Ignore digital magnitude and return analog value.
    return CheckDigitalAction(
        pGamePad->ButtonWasPressed(button),
        pGamePad->ButtonWasReleased(button),
        pGamePad->ButtonIsDown(button),
        lastMagnitude);
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::CheckDigitalAction(
    efd::Bool wasPressed,
    efd::Bool wasReleased,
    efd::Bool isDown,
    efd::Float32& magnitude) const
{
    // No range checking for digital buttons
    if ((m_flags & InputService::ON_ACTIVATE) && wasPressed)
    {
        magnitude = 1.0f;
        return true;
    }

    if ((m_flags & InputService::ON_DEACTIVATE) && wasReleased)
    {
        magnitude = 0.0f;
        return true;
    }

    if ((m_flags & InputService::CONTINUOUS) && isDown)
    {
        magnitude = 1.0f;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceAction::CheckAnalogAction(
    efd::Float32 magnitude,
    efd::Float32 lastMagnitude) const
{
    if (m_flags & InputService::RANGED)
    {
        // For ranged analog buttons, send message on range enter or leave
        bool inRange = IsInRange(magnitude);
        bool lastInRange = IsInRange(lastMagnitude);

        if ((m_flags & InputService::ON_ACTIVATE) &&
            inRange && !lastInRange)
        {
            return true;
        }
        if ((m_flags & InputService::ON_DEACTIVATE) &&
            !inRange && lastInRange)
        {
            return true;
        }
        if ((m_flags & InputService::CONTINUOUS) && inRange)
        {
            return true;
        }
    }
    else
    {
        // For continuous non-ranged analog buttons always send a message
        if (m_flags & InputService::CONTINUOUS)
            return true;

        // Else send message if magnitude changed
        if (IsMouseAction())
        {
            // Report movement only if delta != 0
            if (efd::Abs(magnitude) > EE_NIS_FLOAT_PRECISION)
            {
                return true;
            }
        }
        else
        {
            // Report only if magnitude changed
            if (efd::Abs(magnitude - lastMagnitude) > EE_NIS_FLOAT_PRECISION)
            {
                return true;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputServiceAction::GetHash() const
{
    return InputServiceActionBase::GetHash() ^ m_semantic;
}

//--------------------------------------------------------------------------------------------------
void InputServiceAction::LoadTranslationMap()
{
    //From NiAction.h------------------------------------
    //ms_semanticMap["SEMANTIC_TYPE_MASK"] = NiAction::SEMANTIC_TYPE_MASK;
    //*** GAME PAD MAPPINGS ***//
    //ms_semanticMap["GAMEPAD_MASK"] = NiAction::GAMEPAD_MASK;
    //*** Defined Axis Mapping
    // Left analog stick
    ms_semanticMap["GP_AXIS_LEFT_H"] = NiAction::GP_AXIS_LEFT_H;
    ms_semanticMap["GP_AXIS_LEFT_V"] = NiAction::GP_AXIS_LEFT_V;
    // Right analog stick
    ms_semanticMap["GP_AXIS_RIGHT_H"] = NiAction::GP_AXIS_RIGHT_H;
    ms_semanticMap["GP_AXIS_RIGHT_V"] = NiAction::GP_AXIS_RIGHT_V;
    // Additional analog stick(s) for future support
    ms_semanticMap["GP_AXIS_X1_H"] = NiAction::GP_AXIS_X1_H;
    ms_semanticMap["GP_AXIS_X1_V"] = NiAction::GP_AXIS_X1_V;
    ms_semanticMap["GP_AXIS_X2_H"] = NiAction::GP_AXIS_X2_H;
    ms_semanticMap["GP_AXIS_X2_V"] = NiAction::GP_AXIS_X2_V;
    //*** Defined Button Mapping
    ms_semanticMap["GP_BUTTON_LUP"] = NiAction::GP_BUTTON_LUP;
    ms_semanticMap["GP_BUTTON_LDOWN"] = NiAction::GP_BUTTON_LDOWN;
    ms_semanticMap["GP_BUTTON_LLEFT"] = NiAction::GP_BUTTON_LLEFT;
    ms_semanticMap["GP_BUTTON_LRIGHT"] = NiAction::GP_BUTTON_LRIGHT;
    ms_semanticMap["GP_BUTTON_L1"] = NiAction::GP_BUTTON_L1;
    ms_semanticMap["GP_BUTTON_L2"] = NiAction::GP_BUTTON_L2;
    ms_semanticMap["GP_BUTTON_R1"] = NiAction::GP_BUTTON_R1;
    ms_semanticMap["GP_BUTTON_R2"] = NiAction::GP_BUTTON_R2;
    ms_semanticMap["GP_BUTTON_RUP"] = NiAction::GP_BUTTON_RUP;
    ms_semanticMap["GP_BUTTON_RDOWN"] = NiAction::GP_BUTTON_RDOWN;
    ms_semanticMap["GP_BUTTON_RLEFT"] = NiAction::GP_BUTTON_RLEFT;
    ms_semanticMap["GP_BUTTON_RRIGHT"] = NiAction::GP_BUTTON_RRIGHT;
    ms_semanticMap["GP_BUTTON_A"] = NiAction::GP_BUTTON_A;
    ms_semanticMap["GP_BUTTON_B"] = NiAction::GP_BUTTON_B;
    ms_semanticMap["GP_BUTTON_START"] = NiAction::GP_BUTTON_START;
    ms_semanticMap["GP_BUTTON_SELECT"] = NiAction::GP_BUTTON_SELECT;
    // Direction Pad (DPad)
    //ms_semanticMap["GP_DPAD"] = 0x10000200
    //*** Helper mapping - *_ANY_*
    // Any Axis
    ms_semanticMap["GP_AXIS_ANY_1"] = NiAction::GP_AXIS_ANY_1;
    ms_semanticMap["GP_AXIS_ANY_2"] = NiAction::GP_AXIS_ANY_2;
    ms_semanticMap["GP_AXIS_ANY_3"] = NiAction::GP_AXIS_ANY_3;
    ms_semanticMap["GP_AXIS_ANY_4"] = NiAction::GP_AXIS_ANY_4;
    ms_semanticMap["GP_AXIS_ANY_5"] = NiAction::GP_AXIS_ANY_5;
    ms_semanticMap["GP_AXIS_ANY_6"] = NiAction::GP_AXIS_ANY_6;
    ms_semanticMap["GP_AXIS_ANY_7"] = NiAction::GP_AXIS_ANY_7;
    ms_semanticMap["GP_AXIS_ANY_8"] = NiAction::GP_AXIS_ANY_8;
    // Any Button
    ms_semanticMap["GP_BUTTON_ANY_BASE"] = NiAction::GP_BUTTON_ANY_BASE;

    //*** KEYBOARD MAPPINGS ***//
    ms_semanticMap["KEY_MASK"] = NiAction::KEY_MASK;
    ms_semanticMap["KEY_ANY_BASE"] = NiAction::KEY_ANY_BASE;

    //*** MOUSE MAPPINGS ***//
    ms_semanticMap["MOUSE_MASK"] = NiAction::MOUSE_MASK;
    //*** Axis mappings
    ms_semanticMap["MOUSE_AXIS_X"] = NiAction::MOUSE_AXIS_X;
    ms_semanticMap["MOUSE_AXIS_Y"] = NiAction::MOUSE_AXIS_Y;
    ms_semanticMap["MOUSE_AXIS_Z"] = NiAction::MOUSE_AXIS_Z;
    //*** Button mappings
    ms_semanticMap["MOUSE_BUTTON_LEFT"] = NiAction::MOUSE_BUTTON_LEFT;
    ms_semanticMap["MOUSE_BUTTON_RIGHT"] = NiAction::MOUSE_BUTTON_RIGHT;
    ms_semanticMap["MOUSE_BUTTON_MIDDLE"] = NiAction::MOUSE_BUTTON_MIDDLE;
    ms_semanticMap["MOUSE_BUTTON_X1"] = NiAction::MOUSE_BUTTON_X1;
    ms_semanticMap["MOUSE_BUTTON_X2"] = NiAction::MOUSE_BUTTON_X2;
    ms_semanticMap["MOUSE_BUTTON_X3"] = NiAction::MOUSE_BUTTON_X3;
    ms_semanticMap["MOUSE_BUTTON_X4"] = NiAction::MOUSE_BUTTON_X4;
    ms_semanticMap["MOUSE_BUTTON_X5"] = NiAction::MOUSE_BUTTON_X5;
    //*** Any axis mapping
    ms_semanticMap["MOUSE_AXIS_ANY_1"] = NiAction::MOUSE_AXIS_ANY_1;
    ms_semanticMap["MOUSE_AXIS_ANY_2"] = NiAction::MOUSE_AXIS_ANY_2;
    ms_semanticMap["MOUSE_AXIS_ANY_3"] = NiAction::MOUSE_AXIS_ANY_3;
    //*** Any button mapping
    ms_semanticMap["MOUSE_BUTTON_ANY_BASE"] = NiAction::MOUSE_BUTTON_ANY_BASE;

//From NiKeyboardInput.h all key codes are pre-masked with NiAction::KEY_MASK
    //ms_semanticMap["KEY_NOKEY"] = NiInputKeyboard::KEY_NOKEY;
    ms_semanticMap["KEY_ESCAPE"] = NiInputKeyboard::KEY_ESCAPE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_1"] = NiInputKeyboard::KEY_1 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_2"] = NiInputKeyboard::KEY_2 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_3"] = NiInputKeyboard::KEY_3 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_4"] = NiInputKeyboard::KEY_4 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_5"] = NiInputKeyboard::KEY_5 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_6"] = NiInputKeyboard::KEY_6 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_7"] = NiInputKeyboard::KEY_7 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_8"] = NiInputKeyboard::KEY_8 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_9"] = NiInputKeyboard::KEY_9 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_0"] = NiInputKeyboard::KEY_0 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MINUS"] = NiInputKeyboard::KEY_MINUS | NiAction::KEY_MASK;
    ms_semanticMap["KEY_EQUALS"] = NiInputKeyboard::KEY_EQUALS | NiAction::KEY_MASK;
    ms_semanticMap["KEY_BACK"] = NiInputKeyboard::KEY_BACK | NiAction::KEY_MASK;
    ms_semanticMap["KEY_TAB"] = NiInputKeyboard::KEY_TAB | NiAction::KEY_MASK;
    ms_semanticMap["KEY_Q"] = NiInputKeyboard::KEY_Q | NiAction::KEY_MASK;
    ms_semanticMap["KEY_W"] = NiInputKeyboard::KEY_W | NiAction::KEY_MASK;
    ms_semanticMap["KEY_E"] = NiInputKeyboard::KEY_E | NiAction::KEY_MASK;
    ms_semanticMap["KEY_R"] = NiInputKeyboard::KEY_R | NiAction::KEY_MASK;
    ms_semanticMap["KEY_T"] = NiInputKeyboard::KEY_T | NiAction::KEY_MASK;
    ms_semanticMap["KEY_Y"] = NiInputKeyboard::KEY_Y | NiAction::KEY_MASK;
    ms_semanticMap["KEY_U"] = NiInputKeyboard::KEY_U | NiAction::KEY_MASK;
    ms_semanticMap["KEY_I"] = NiInputKeyboard::KEY_I | NiAction::KEY_MASK;
    ms_semanticMap["KEY_O"] = NiInputKeyboard::KEY_O | NiAction::KEY_MASK;
    ms_semanticMap["KEY_P"] = NiInputKeyboard::KEY_P | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LBRACKET"] = NiInputKeyboard::KEY_LBRACKET | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RBRACKET"] = NiInputKeyboard::KEY_RBRACKET | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RETURN"] = NiInputKeyboard::KEY_RETURN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LCONTROL"] = NiInputKeyboard::KEY_LCONTROL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_A"] = NiInputKeyboard::KEY_A | NiAction::KEY_MASK;
    ms_semanticMap["KEY_S"] = NiInputKeyboard::KEY_S | NiAction::KEY_MASK;
    ms_semanticMap["KEY_D"] = NiInputKeyboard::KEY_D | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F"] = NiInputKeyboard::KEY_F | NiAction::KEY_MASK;
    ms_semanticMap["KEY_G"] = NiInputKeyboard::KEY_G | NiAction::KEY_MASK;
    ms_semanticMap["KEY_H"] = NiInputKeyboard::KEY_H | NiAction::KEY_MASK;
    ms_semanticMap["KEY_J"] = NiInputKeyboard::KEY_J | NiAction::KEY_MASK;
    ms_semanticMap["KEY_K"] = NiInputKeyboard::KEY_K | NiAction::KEY_MASK;
    ms_semanticMap["KEY_L"] = NiInputKeyboard::KEY_L | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SEMICOLON"] = NiInputKeyboard::KEY_SEMICOLON | NiAction::KEY_MASK;
    ms_semanticMap["KEY_APOSTROPHE"] = NiInputKeyboard::KEY_APOSTROPHE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_GRAVE"] = NiInputKeyboard::KEY_GRAVE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LSHIFT"] = NiInputKeyboard::KEY_LSHIFT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_BACKSLASH"] = NiInputKeyboard::KEY_BACKSLASH | NiAction::KEY_MASK;
    ms_semanticMap["KEY_Z"] = NiInputKeyboard::KEY_Z | NiAction::KEY_MASK;
    ms_semanticMap["KEY_X"] = NiInputKeyboard::KEY_X | NiAction::KEY_MASK;
    ms_semanticMap["KEY_C"] = NiInputKeyboard::KEY_C | NiAction::KEY_MASK;
    ms_semanticMap["KEY_V"] = NiInputKeyboard::KEY_V | NiAction::KEY_MASK;
    ms_semanticMap["KEY_B"] = NiInputKeyboard::KEY_B | NiAction::KEY_MASK;
    ms_semanticMap["KEY_N"] = NiInputKeyboard::KEY_N | NiAction::KEY_MASK;
    ms_semanticMap["KEY_M"] = NiInputKeyboard::KEY_M | NiAction::KEY_MASK;
    ms_semanticMap["KEY_COMMA"] = NiInputKeyboard::KEY_COMMA | NiAction::KEY_MASK;
    ms_semanticMap["KEY_PERIOD"] = NiInputKeyboard::KEY_PERIOD | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SLASH"] = NiInputKeyboard::KEY_SLASH | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RSHIFT"] = NiInputKeyboard::KEY_RSHIFT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MULTIPLY"] = NiInputKeyboard::KEY_MULTIPLY | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LMENU"] = NiInputKeyboard::KEY_LMENU | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SPACE"] = NiInputKeyboard::KEY_SPACE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_CAPITAL"] = NiInputKeyboard::KEY_CAPITAL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F1"] = NiInputKeyboard::KEY_F1 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F2"] = NiInputKeyboard::KEY_F2 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F3"] = NiInputKeyboard::KEY_F3 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F4"] = NiInputKeyboard::KEY_F4 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F5"] = NiInputKeyboard::KEY_F5 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F6"] = NiInputKeyboard::KEY_F6 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F7"] = NiInputKeyboard::KEY_F7 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F8"] = NiInputKeyboard::KEY_F8 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F9"] = NiInputKeyboard::KEY_F9 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F10"] = NiInputKeyboard::KEY_F10 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMLOCK"] = NiInputKeyboard::KEY_NUMLOCK | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SCROLL"] = NiInputKeyboard::KEY_SCROLL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD7"] = NiInputKeyboard::KEY_NUMPAD7 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD8"] = NiInputKeyboard::KEY_NUMPAD8 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD9"] = NiInputKeyboard::KEY_NUMPAD9 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SUBTRACT"] = NiInputKeyboard::KEY_SUBTRACT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD4"] = NiInputKeyboard::KEY_NUMPAD4 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD5"] = NiInputKeyboard::KEY_NUMPAD5 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD6"] = NiInputKeyboard::KEY_NUMPAD6 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_ADD"] = NiInputKeyboard::KEY_ADD | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD1"] = NiInputKeyboard::KEY_NUMPAD1 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD2"] = NiInputKeyboard::KEY_NUMPAD2 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD3"] = NiInputKeyboard::KEY_NUMPAD3 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPAD0"] = NiInputKeyboard::KEY_NUMPAD0 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_DECIMAL"] = NiInputKeyboard::KEY_DECIMAL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_OEM_102"] = NiInputKeyboard::KEY_OEM_102 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F11"] = NiInputKeyboard::KEY_F11 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F12"] = NiInputKeyboard::KEY_F12 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F13"] = NiInputKeyboard::KEY_F13 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F14"] = NiInputKeyboard::KEY_F14 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_F15"] = NiInputKeyboard::KEY_F15 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_KANA"] = NiInputKeyboard::KEY_KANA | NiAction::KEY_MASK;
    ms_semanticMap["KEY_ABNT_C1"] = NiInputKeyboard::KEY_ABNT_C1 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_CONVERT"] = NiInputKeyboard::KEY_CONVERT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NOCONVERT"] = NiInputKeyboard::KEY_NOCONVERT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_YEN"] = NiInputKeyboard::KEY_YEN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_ABNT_C2"] = NiInputKeyboard::KEY_ABNT_C2 | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPADEQUALS"] = NiInputKeyboard::KEY_NUMPADEQUALS | NiAction::KEY_MASK;
    ms_semanticMap["KEY_PREVTRACK"] = NiInputKeyboard::KEY_PREVTRACK | NiAction::KEY_MASK;
    ms_semanticMap["KEY_AT"] = NiInputKeyboard::KEY_AT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_COLON"] = NiInputKeyboard::KEY_COLON | NiAction::KEY_MASK;
    ms_semanticMap["KEY_UNDERLINE"] = NiInputKeyboard::KEY_UNDERLINE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_KANJI"] = NiInputKeyboard::KEY_KANJI | NiAction::KEY_MASK;
    ms_semanticMap["KEY_STOP"] = NiInputKeyboard::KEY_STOP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_AX"] = NiInputKeyboard::KEY_AX | NiAction::KEY_MASK;
    ms_semanticMap["KEY_UNLABELED"] = NiInputKeyboard::KEY_UNLABELED | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NEXTTRACK"] = NiInputKeyboard::KEY_NEXTTRACK | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPADENTER"] = NiInputKeyboard::KEY_NUMPADENTER | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RCONTROL"] = NiInputKeyboard::KEY_RCONTROL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MUTE"] = NiInputKeyboard::KEY_MUTE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_CALCULATOR"] = NiInputKeyboard::KEY_CALCULATOR | NiAction::KEY_MASK;
    ms_semanticMap["KEY_PLAYPAUSE"] = NiInputKeyboard::KEY_PLAYPAUSE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MEDIASTOP"] = NiInputKeyboard::KEY_MEDIASTOP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_VOLUMEDOWN"] = NiInputKeyboard::KEY_VOLUMEDOWN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_VOLUMEUP"] = NiInputKeyboard::KEY_VOLUMEUP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBHOME"] = NiInputKeyboard::KEY_WEBHOME | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NUMPADCOMMA"] = NiInputKeyboard::KEY_NUMPADCOMMA | NiAction::KEY_MASK;
    ms_semanticMap["KEY_DIVIDE"] = NiInputKeyboard::KEY_DIVIDE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SYSRQ"] = NiInputKeyboard::KEY_SYSRQ | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RMENU"] = NiInputKeyboard::KEY_RMENU | NiAction::KEY_MASK;
    ms_semanticMap["KEY_PAUSE"] = NiInputKeyboard::KEY_PAUSE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_HOME"] = NiInputKeyboard::KEY_HOME | NiAction::KEY_MASK;
    ms_semanticMap["KEY_UP"] = NiInputKeyboard::KEY_UP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_PRIOR"] = NiInputKeyboard::KEY_PRIOR | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LEFT"] = NiInputKeyboard::KEY_LEFT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RIGHT"] = NiInputKeyboard::KEY_RIGHT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_END"] = NiInputKeyboard::KEY_END | NiAction::KEY_MASK;
    ms_semanticMap["KEY_DOWN"] = NiInputKeyboard::KEY_DOWN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_NEXT"] = NiInputKeyboard::KEY_NEXT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_INSERT"] = NiInputKeyboard::KEY_INSERT | NiAction::KEY_MASK;
    ms_semanticMap["KEY_DELETE"] = NiInputKeyboard::KEY_DELETE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_LWIN"] = NiInputKeyboard::KEY_LWIN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_RWIN"] = NiInputKeyboard::KEY_RWIN | NiAction::KEY_MASK;
    ms_semanticMap["KEY_APPS"] = NiInputKeyboard::KEY_APPS | NiAction::KEY_MASK;
    ms_semanticMap["KEY_POWER"] = NiInputKeyboard::KEY_POWER | NiAction::KEY_MASK;
    ms_semanticMap["KEY_SLEEP"] = NiInputKeyboard::KEY_SLEEP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WAKE"] = NiInputKeyboard::KEY_WAKE | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBSEARCH"] = NiInputKeyboard::KEY_WEBSEARCH | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBFAVORITES"] = NiInputKeyboard::KEY_WEBFAVORITES | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBREFRESH"] = NiInputKeyboard::KEY_WEBREFRESH | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBSTOP"] = NiInputKeyboard::KEY_WEBSTOP | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBFORWARD"] = NiInputKeyboard::KEY_WEBFORWARD | NiAction::KEY_MASK;
    ms_semanticMap["KEY_WEBBACK"] = NiInputKeyboard::KEY_WEBBACK | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MYCOMPUTER"] = NiInputKeyboard::KEY_MYCOMPUTER | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MAIL"] = NiInputKeyboard::KEY_MAIL | NiAction::KEY_MASK;
    ms_semanticMap["KEY_MEDIASELECT"] = NiInputKeyboard::KEY_MEDIASELECT | NiAction::KEY_MASK;
}

//--------------------------------------------------------------------------------------------------
void InputServiceAction::UnloadTranslationMap()
{
    ms_semanticMap.clear();
}

//--------------------------------------------------------------------------------------------------
