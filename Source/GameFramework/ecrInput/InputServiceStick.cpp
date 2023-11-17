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

#include "InputServiceStick.h"
#include <efd/TinyXML.h>

using namespace ecrInput;

efd::map<efd::utf8string, efd::UInt32> InputServiceStick::ms_stickTypeMap;

//--------------------------------------------------------------------------------------------------
InputServiceStick::~InputServiceStick()
{
}

//--------------------------------------------------------------------------------------------------
// Input processing
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceStick::ProcessInput(
    InputService* pInputService,
    efd::UInt32& appData,
    efd::Float32& magnitude,
    efd::Float32& x,
    efd::Float32& y,
    efd::Float32& z)
{
    if (m_stickType == InputService::INVALID_STICK)
        return false;

    NiInputGamePad* pGamePad = pInputService->GetInputSystem()->GetGamePad(m_deviceID);

    if (!pGamePad)
        return false;

    efd::Float32 lastX = 0.0f;
    efd::Float32 lastY = 0.0f;
    efd::Float32 lastZ = 0.0f;
    if (m_stickType == InputService::CUSTOM_STICK)
    {
        if (m_axisSemanticX)
        {
            x = ConvertAxisValue(pGamePad->GetAxisValue(m_axisControlX));
            lastX = ConvertAxisValue(pGamePad->GetAxisLastValue(m_axisControlX));
        }
        if (m_axisSemanticY)
        {
            y = ConvertAxisValue(pGamePad->GetAxisValue(m_axisControlY));
            lastY = ConvertAxisValue(pGamePad->GetAxisLastValue(m_axisControlY));
        }
        if (m_axisSemanticZ)
        {
            z = ConvertAxisValue(pGamePad->GetAxisValue(m_axisControlZ));
            lastZ = ConvertAxisValue(pGamePad->GetAxisLastValue(m_axisControlZ));
        }
    }
    else
    {
        efd::UInt32 uiStick = (efd::UInt32)m_stickType;
        efd::SInt32 axisX, axisY;

        pGamePad->GetStickValue(uiStick, axisX, axisY);
        x = ConvertAxisValue(axisX);
        y = ConvertAxisValue(axisY);

        pGamePad->GetStickLastValue(uiStick, axisX, axisY);
        lastX = ConvertAxisValue(axisX);
        lastY = ConvertAxisValue(axisY);
    }

    if (m_pFilter)
        m_pFilter(m_appData, x, y, z);

    efd::Bool changed = (lastX != x) || (lastY != y) || (lastZ != z);
    efd::Bool send = (m_flags & InputService::CONTINUOUS) != 0;
    magnitude = efd::Sqrt(x * x + y * y + z * z);

    if (!send && !(m_flags & InputService::RANGED))
    {
        efd::Bool moved = (x != 0.0f || y != 0.0f || z != 0.0);
        efd::Bool lastMoved = (lastX != 0.0f || lastY != 0.0f || lastZ != 0.0f);

        if ((m_flags & InputService::ON_ACTIVATE) && (moved && !lastMoved || changed))
            send = true;
        else if (m_flags & InputService::ON_DEACTIVATE && !moved && lastMoved)
            send = true;
    }
    if (!send && (m_flags & InputService::RANGED))
    {
        efd::Bool bInRange = IsInRange(magnitude, x, y, z);
        efd::Bool bLastInRange = IsInRange(magnitude, lastX, lastY, lastZ);

        if ((m_flags & InputService::ON_ACTIVATE) && bInRange && !bLastInRange)
            send = true;
        else if (m_flags & InputService::ON_DEACTIVATE && !bInRange && bLastInRange)
            send = true;
    }

    // Reverse axes if requested
    if (send)
    {
        appData = m_appData;

        // Make X, Y and Z laying on unit sphere
        if ((m_flags & InputService::SPHERIC_COORDS) != 0 && magnitude != 0.0f)
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }

        if (m_flags & InputService::REVERSE_X_AXIS)
            x = -x;

        if (m_flags & InputService::REVERSE_Y_AXIS)
            y = -y;

        if (m_flags & InputService::REVERSE_Z_AXIS)
            z = -z;
    }

    return send;
}

//--------------------------------------------------------------------------------------------------
// XML serialization methods
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceStick::SaveXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::SaveXml(pElement);

    pElement->SetAttribute("ActionClsID", "STICK");
    WriteEnumToAttribute(pElement, "StickType", ms_stickTypeMap, m_stickType);
    pElement->SetAttribute("XAxisSemantic", m_axisSemanticX);
    pElement->SetAttribute("YAxisSemantic", m_axisSemanticY);
    pElement->SetAttribute("ZAxisSemantic", m_axisSemanticZ);
    pElement->SetDoubleAttribute("XMinRange", m_minRangeX);
    pElement->SetDoubleAttribute("XMaxRange", m_maxRangeX);
    pElement->SetDoubleAttribute("YMinRange", m_minRangeY);
    pElement->SetDoubleAttribute("YMaxRange", m_maxRangeY);
    pElement->SetDoubleAttribute("ZMinRange", m_minRangeZ);
    pElement->SetDoubleAttribute("ZMaxRange", m_maxRangeZ);

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceStick::LoadXml(efd::TiXmlElement* pElement)
{
    InputServiceActionBase::LoadXml(pElement);

    efd::UInt32 stickInt;
    if (ReadEnumFromAttribute(pElement, "StickType", ms_stickTypeMap, stickInt))
        m_stickType = static_cast<InputService::StickType>(stickInt);

    pElement->QueryIntAttribute("XAxisSemantic", (int*)&m_axisSemanticX);
    pElement->QueryIntAttribute("YAxisSemantic", (int*)&m_axisSemanticY);
    pElement->QueryIntAttribute("ZAxisSemantic", (int*)&m_axisSemanticZ);
    pElement->QueryFloatAttribute("XMinRange", &m_minRangeX);
    pElement->QueryFloatAttribute("XMaxRange", &m_maxRangeX);
    pElement->QueryFloatAttribute("YMinRange", &m_minRangeY);
    pElement->QueryFloatAttribute("YMaxRange", &m_maxRangeY);
    pElement->QueryFloatAttribute("ZMinRange", &m_minRangeZ);
    pElement->QueryFloatAttribute("ZMaxRange", &m_maxRangeZ);

    // Update device control from semantic
    SetCustomSemantic(m_axisSemanticX, m_axisSemanticY, m_axisSemanticZ);

    return true;
}

//--------------------------------------------------------------------------------------------------
// Check if provided value is in range of this action
//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceStick::IsInRange(
    efd::Float32 magnitude,
    efd::Float32 x,
    efd::Float32 y,
    efd::Float32 z) const
{
    if (m_flags & InputService::SPHERIC_COORDS)
    {
        // Calculate spheric coordinates angles
        efd::Float32 fPhi = efd::ATan2(y, x);
        efd::Float32 fTheta = efd::ACos(z / magnitude);
        // If min == max, skip range check
        if ((m_minRangeX <= magnitude && m_maxRangeX >= magnitude) ||
            (m_minRangeX == m_maxRangeX))
        {
            if ((m_minRangeY <= fPhi && m_maxRangeY >= fPhi) ||
                (m_minRangeY == m_maxRangeY))
            {
                if ((m_minRangeZ <= fTheta && m_maxRangeZ >= fTheta) ||
                    (m_minRangeZ == m_maxRangeZ))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if ((m_minRangeX <= x && m_maxRangeX >= x) ||
            (m_minRangeX == m_maxRangeX))
        {
            if ((m_minRangeY <= y && m_maxRangeY >= y) ||
                (m_minRangeY == m_maxRangeY))
            {
                if ((m_minRangeZ <= z && m_maxRangeZ >= z) ||
                    (m_minRangeZ == m_maxRangeZ))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 InputServiceStick::GetHash() const
{
    efd::UInt32 hash = InputServiceActionBase::GetHash();

    if (m_stickType == InputService::CUSTOM_STICK)
    {
        // Custom stick flag
        hash |= 0x8000;

        // Device type bits
        hash |= m_axisSemanticX & 0xf0000000;
        hash |= m_axisSemanticY & 0xf0000000;
        hash |= m_axisSemanticZ & 0xf0000000;

        // Semantic bits xor'ed
        hash ^= m_axisSemanticX & 0x3fff;
        hash ^= m_axisSemanticY & 0x3fff;
        hash ^= m_axisSemanticZ & 0x3fff;
    }
    else
    {
        // Stick type in lower hash bits
        hash |= ((efd::UInt32)m_stickType) & 0x3fff;
    }

    return hash;
}

//--------------------------------------------------------------------------------------------------
void InputServiceStick::LoadTranslationMap()
{
    ms_stickTypeMap["GP_STICK_LEFT"] = InputService::GP_STICK_LEFT;
    ms_stickTypeMap["GP_STICK_RIGHT"] = InputService::GP_STICK_RIGHT;
    ms_stickTypeMap["GP_STICK_X1"] = InputService::GP_STICK_X1;
    ms_stickTypeMap["GP_STICK_X2"] = InputService::GP_STICK_X2;
    ms_stickTypeMap["CUSTOM_STICK"] = InputService::CUSTOM_STICK;
}

//--------------------------------------------------------------------------------------------------
void InputServiceStick::UnloadTranslationMap()
{
    ms_stickTypeMap.clear();
}

//--------------------------------------------------------------------------------------------------