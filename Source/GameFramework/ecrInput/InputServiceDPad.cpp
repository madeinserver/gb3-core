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

#include "InputServiceDPad.h"

#include <efd/TinyXML.h>

using namespace ecrInput;

efd::map<efd::utf8string, efd::UInt32> InputServiceDPad::ms_padTypePad;

//--------------------------------------------------------------------------------------------------
InputServiceDPad::~InputServiceDPad()
{
}

//--------------------------------------------------------------------------------------------------
// Input processing
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceDPad::ProcessInput(
    InputService* pInputService,
    efd::UInt32& appData,
    efd::Float32& magnitude,
    efd::Float32& x,
    efd::Float32& y,
    efd::Float32& z)
{
    NiInputSystem* pInputSystem = pInputService->GetInputSystem();

    efd::Bool up = false;
    efd::Bool down = false;
    efd::Bool left = false;
    efd::Bool right = false;

    if (IsKeyboardDPad())
    {
        NiInputKeyboard* pkKeyboard = pInputSystem->GetKeyboard();
        if (pkKeyboard == NULL)
            return false;

        if ((m_flags & InputService::USE_MODIFIERS) &&
            !CheckKeyboardModifiers(pInputSystem))
        {
            return false;
        }

        up = pkKeyboard->KeyIsDown((NiInputKeyboard::KeyCode)m_upControl);
        down = pkKeyboard->KeyIsDown((NiInputKeyboard::KeyCode)m_downControl);
        left = pkKeyboard->KeyIsDown((NiInputKeyboard::KeyCode)m_leftControl);
        right = pkKeyboard->KeyIsDown((NiInputKeyboard::KeyCode)m_rightControl);
    }
    else
    {
        NiInputGamePad* pkGamePad = pInputSystem->GetGamePad(m_deviceID);
        if (pkGamePad == NULL)
            return false;

        if ((m_flags & InputService::USE_MODIFIERS) &&
            !CheckGamePadModifiers(pInputSystem, m_deviceID))
        {
            return false;
        }

        up = pkGamePad->ButtonIsDown((NiInputGamePad::Button)m_upControl);
        down = pkGamePad->ButtonIsDown((NiInputGamePad::Button)m_downControl);
        left = pkGamePad->ButtonIsDown((NiInputGamePad::Button)m_leftControl);
        right = pkGamePad->ButtonIsDown((NiInputGamePad::Button)m_rightControl);
    }

    if (up ^ down)
    {
        if (up)
            y = 1.0f;
        else
            y = -1.0f;
    }
    else
    {
        y = 0.0f;
    }

    if (left ^ right)
    {
        if (right)
            x = 1.0f;
        else
            x = -1.0f;
    }
    else
    {
        x = 0.0f;
    }

    z = 0.0f;
    magnitude = 1.0f;

    if (x != 0.0f && y != 0.0f)
    {
        efd::Float32 fClamp = efd::Sqrt(2.0f) / 2.0f;

        x *= fClamp;
        y *= fClamp;
    }

    efd::Bool send = (m_flags & InputService::CONTINUOUS) != 0;
    if (!send)
    {
        up = (x != 0.0f || y != 0.0f);
        down = (m_lastX != 0.0f || m_lastY != 0.0f);

        if ((m_flags & InputService::ON_ACTIVATE) && (x != m_lastX || y != m_lastY))
            send = true;
        else if ((m_flags & InputService::ON_ACTIVATE) && (up ^ down))
            send = true;
    }

    m_lastX = x;
    m_lastY = y;

    // Reverse axes if requested
    if (send)
    {
        appData = m_appData;

        if (m_flags & InputService::REVERSE_X_AXIS)
            x = -x;

        if (m_flags & InputService::REVERSE_Y_AXIS)
            y = -y;
    }

    return send;
}

//--------------------------------------------------------------------------------------------------
// Data accessors - setting
//--------------------------------------------------------------------------------------------------
void InputServiceDPad::SetType(InputService::DPadType pad)
{
    m_padType = pad;
    switch (pad)
    {
    case InputService::GP_DPAD_L:
        m_upControl = NiAction::SemanticToDeviceControl(
            m_upSemantic = NiAction::GP_BUTTON_LUP);
        m_downControl = NiAction::SemanticToDeviceControl(
            m_downSemantic = NiAction::GP_BUTTON_LDOWN);
        m_leftControl = NiAction::SemanticToDeviceControl(
            m_leftSemantic = NiAction::GP_BUTTON_LLEFT);
        m_rightControl = NiAction::SemanticToDeviceControl(
            m_rightSemantic = NiAction::GP_BUTTON_LRIGHT);
        break;

    case InputService::GP_DPAD_R:
        m_upControl = NiAction::SemanticToDeviceControl(
            m_upSemantic = NiAction::GP_BUTTON_RUP);
        m_downControl = NiAction::SemanticToDeviceControl(
            m_downSemantic = NiAction::GP_BUTTON_RDOWN);
        m_leftControl = NiAction::SemanticToDeviceControl(
            m_leftSemantic = NiAction::GP_BUTTON_RLEFT);
        m_rightControl = NiAction::SemanticToDeviceControl(
            m_rightSemantic = NiAction::GP_BUTTON_RRIGHT);
        break;

    case InputService::KEY_DPAD_ARROWS:
        m_upControl = NiAction::SemanticToDeviceControl(
            m_upSemantic = KEY_MAP(NiInputKeyboard::KEY_UP));
        m_downControl = NiAction::SemanticToDeviceControl(
            m_downSemantic = KEY_MAP(NiInputKeyboard::KEY_DOWN));
        m_leftControl = NiAction::SemanticToDeviceControl(
            m_leftSemantic = KEY_MAP(NiInputKeyboard::KEY_LEFT));
        m_rightControl = NiAction::SemanticToDeviceControl(
            m_rightSemantic = KEY_MAP(NiInputKeyboard::KEY_RIGHT));
        break;

    case InputService::KEY_DPAD_NUMPAD:
        m_upControl = NiAction::SemanticToDeviceControl(
            m_upSemantic = KEY_MAP(
            NiInputKeyboard::KEY_NUMPAD8));
        m_downControl = NiAction::SemanticToDeviceControl(
            m_downSemantic = KEY_MAP(
            NiInputKeyboard::KEY_NUMPAD2));
        m_leftControl = NiAction::SemanticToDeviceControl(
            m_leftSemantic = KEY_MAP(
            NiInputKeyboard::KEY_NUMPAD4));
        m_rightControl = NiAction::SemanticToDeviceControl(
            m_rightSemantic = KEY_MAP(
            NiInputKeyboard::KEY_NUMPAD6));
        break;

    case InputService::KEY_DPAD_WSAD:
        m_upControl = NiAction::SemanticToDeviceControl(
            m_upSemantic = KEY_MAP(NiInputKeyboard::KEY_W));
        m_downControl = NiAction::SemanticToDeviceControl(
            m_downSemantic = KEY_MAP(NiInputKeyboard::KEY_S));
        m_leftControl = NiAction::SemanticToDeviceControl(
            m_leftSemantic = KEY_MAP(NiInputKeyboard::KEY_A));
        m_rightControl = NiAction::SemanticToDeviceControl(
            m_rightSemantic = KEY_MAP(NiInputKeyboard::KEY_D));
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------
// XML serialization methods
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceDPad::SaveXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::SaveXml(pElement);

    pElement->SetAttribute("ActionClsID", "DPAD");
    WriteEnumToAttribute(pElement, "DPadType", ms_padTypePad, m_padType);
    pElement->SetAttribute("UpSemantic", m_upSemantic);
    pElement->SetAttribute("DownSemantic", m_downSemantic);
    pElement->SetAttribute("LeftSemantic", m_leftSemantic);
    pElement->SetAttribute("RightSemantic", m_rightSemantic);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceDPad::LoadXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::LoadXml(pElement);

    efd::UInt32 valueOfPadType;
    if (ReadEnumFromAttribute(pElement, "DPadType", ms_padTypePad, valueOfPadType))
    {
        m_padType = static_cast<InputService::DPadType>(valueOfPadType);
    }

    pElement->QueryIntAttribute("UpSemantic", (int*)&m_upSemantic);
    pElement->QueryIntAttribute("DownSemantic", (int*)&m_downSemantic);
    pElement->QueryIntAttribute("LeftSemantic", (int*)&m_leftSemantic);
    pElement->QueryIntAttribute("RightSemantic", (int*)&m_rightSemantic);

    // Update device control from semantic
    SetCustomSemantic(
        m_upSemantic,
        m_downSemantic,
        m_leftSemantic,
        m_rightSemantic);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputServiceDPad::GetHash() const
{
    efd::UInt32 hash = InputServiceActionBase::GetHash();

    if (m_padType == InputService::CUSTOM_DPAD)
    {
        // Custom stick flag
        hash |= 0xc000;

        // Device type bits
        hash |= m_upSemantic & 0xf0000000;
        hash |= m_downSemantic & 0xf0000000;
        hash |= m_leftSemantic & 0xf0000000;
        hash |= m_rightSemantic & 0xf0000000;

        // Semantic bits xor'ed
        hash ^= m_upSemantic & 0x3fff;
        hash ^= m_downSemantic & 0x3fff;
        hash ^= m_leftSemantic & 0x3fff;
        hash ^= m_rightSemantic & 0x3fff;
    }
    else
    {
        // Stick type in lower hash bits
        hash |= ((efd::UInt32)m_padType) & 0x3fff;
    }

    return hash;
}

//--------------------------------------------------------------------------------------------------
void InputServiceDPad::LoadTranslationMap()
{
    ms_padTypePad["GP_DPAD_L"] = InputService::GP_DPAD_L;
    ms_padTypePad["GP_DPAD_R"] = InputService::GP_DPAD_R;
    ms_padTypePad["KEY_DPAD_ARROWS"] = InputService::KEY_DPAD_ARROWS;
    ms_padTypePad["KEY_DPAD_NUMPAD"] = InputService::KEY_DPAD_NUMPAD;
    ms_padTypePad["KEY_DPAD_WSAD"] = InputService::KEY_DPAD_WSAD;
    ms_padTypePad["CUSTOM_DPAD"] = InputService::CUSTOM_DPAD;
}

//--------------------------------------------------------------------------------------------------
void InputServiceDPad::UnloadTranslationMap()
{
    ms_padTypePad.clear();
}

//--------------------------------------------------------------------------------------------------
