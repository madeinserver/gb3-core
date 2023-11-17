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

#include "InputServiceActionBase.h"
#include "InputService.h"
#include <efd/ecrLogIDs.h>

using namespace ecrInput;
FlagMap InputServiceActionBase::ms_actionFlagsMap;
FlagMap InputServiceActionBase::ms_modifierMap;

//--------------------------------------------------------------------------------------------------
InputServiceActionBase::~InputServiceActionBase()
{
}

//--------------------------------------------------------------------------------------------------
void ecrInput::WriteEnumToAttribute(
    efd::TiXmlElement* pElement,
    const char* attrib,
    const FlagMap& map,
    efd::UInt32 value)
{
    for (FlagMap::const_iterator iter = map.begin();
        iter != map.end();
        ++iter)
    {
        if (value == iter->second)
        {
            pElement->SetAttribute(attrib, iter->first.c_str());
            return;
        }
    }

    // could not convert the entire value into flags, just write the integer value:
    pElement->SetAttribute(attrib, (int)value);
}

//--------------------------------------------------------------------------------------------------
efd::Bool ecrInput::ReadEnumFromAttribute(
    efd::TiXmlElement* i_pElement,
    const char* i_attrib,
    const FlagMap& i_map,
    efd::UInt32& o_value)
{
    o_value = 0;

    if (i_pElement->QueryIntAttribute(i_attrib, (int*)&o_value) != efd::TIXML_SUCCESS)
    {
        efd::utf8string strValue = i_pElement->Attribute(i_attrib);
        if (!strValue.empty())
        {
            FlagMap::const_iterator iter = i_map.find(strValue);
            if (iter != i_map.end())
            {
                o_value = iter->second;
            }
            else
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                    ("Invalid '%s' value in XML file (%s).",
                    i_attrib,
                    strValue.c_str()));
                return false;
            }
        }
    }

    return true;
}


//--------------------------------------------------------------------------------------------------
void ecrInput::WriteFlagsToAttribute(
    efd::TiXmlElement* pElement,
    const char* attrib,
    const FlagMap& map,
    efd::UInt32 flags)
{
    efd::UInt32 originalFlags = flags;
    efd::utf8string result;

    for (FlagMap::const_iterator iter = map.begin();
        flags && iter != map.end();
        ++iter)
    {
        efd::UInt32 flag = iter->second;
        if (efd::BitUtils::AllBitsAreSet(flags, flag))
        {
            if (!result.empty())
                result.append("|");
            result.append(iter->first);
            flags = efd::BitUtils::ClearBits(flags, flag);
        }
    }

    if (0 == flags)
    {
        // Converted entire value into string form (or there are no flags)
        pElement->SetAttribute(attrib, result.c_str());
    }
    else
    {
        // could not convert the entire value into flags, just write the integer value:
        pElement->SetAttribute(attrib, originalFlags);
    }
}

//--------------------------------------------------------------------------------------------------
efd::Bool ecrInput::ReadFlagsFromAttribute(
    efd::TiXmlElement* i_pElement,
    const char* i_attrib,
    const FlagMap& i_map,
    efd::UInt32& o_flags)
{
    o_flags = 0;

    if (i_pElement->QueryIntAttribute(i_attrib, (int*)&o_flags) != efd::TIXML_SUCCESS)
    {
        efd::utf8string actionFlags = i_pElement->Attribute(i_attrib);
        actionFlags.toupper();
        efd::vector<efd::utf8string> flags;
        actionFlags.split("|", flags);
        for (efd::vector<efd::utf8string>::iterator flagsIter = flags.begin();
            flagsIter != flags.end();
            ++flagsIter)
        {
            efd::utf8string flag = *flagsIter;
            flag.trim(efd::TrimAll);
            FlagMap::const_iterator iter = i_map.find(flag);
            if (iter != i_map.end())
            {
                o_flags |= iter->second;
            }
            else
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                    ("Invalid '%s' value in XML file ('%s').",
                    i_attrib,
                    flag.c_str()));
                return false;
            }
        }
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool InputServiceActionBase::LoadXml(efd::TiXmlElement* pElement)
{
    if (!ReadFlagsFromAttribute(pElement, "Flags", ms_actionFlagsMap, m_flags))
    {
        return false;
    }

    if (pElement->QueryIntAttribute("AppData", (int*)&m_appData) != efd::TIXML_SUCCESS)
    {
        m_appData = 0;
    }
    if (pElement->QueryIntAttribute("DeviceID", (int*)&m_deviceID) != efd::TIXML_SUCCESS)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Couldn't retrieve attribute 'DeviceID' in XML file."));
        return false;
    }

    if (!ReadFlagsFromAttribute(pElement, "Modifiers", ms_modifierMap, m_modifiers))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
efd::Bool ecrInput::InputServiceActionBase::SaveXml(efd::TiXmlElement* pElement)
{
    WriteFlagsToAttribute(pElement, "Flags", ms_actionFlagsMap, m_flags);
    if (m_appData)
    {
        pElement->SetAttribute("AppData", m_appData);
    }
    pElement->SetAttribute("DeviceID", m_deviceID);
    WriteFlagsToAttribute(pElement, "Modifiers", ms_modifierMap, m_modifiers);

    return true;
}

//--------------------------------------------------------------------------------------------------
void InputServiceActionBase::LoadTranslationMap()
{
    ms_actionFlagsMap["ON_ACTIVATE"] = InputService::ON_ACTIVATE;
    ms_actionFlagsMap["ON_DEACTIVATE"] = InputService::ON_DEACTIVATE;
    ms_actionFlagsMap["CONTINUOUS"] = InputService::CONTINUOUS;
    ms_actionFlagsMap["ANALOG"] = InputService::ANALOG;
    ms_actionFlagsMap["RANGED"] = InputService::RANGED;
    ms_actionFlagsMap["SPHERIC_COORDS"] = InputService::SPHERIC_COORDS;
    ms_actionFlagsMap["DEVICE_SPECIFIC"] = InputService::DEVICE_SPECIFIC;
    ms_actionFlagsMap["USE_MODIFIERS"] = InputService::USE_MODIFIERS;
    ms_actionFlagsMap["REVERSE_X_AXIS"] = InputService::REVERSE_X_AXIS;
    ms_actionFlagsMap["REVERSE_Y_AXIS"] = InputService::REVERSE_Y_AXIS;
    ms_actionFlagsMap["REVERSE_Z_AXIS"] = InputService::REVERSE_Z_AXIS;

    ms_modifierMap["KMOD_NONE"] = NiInputKeyboard::KMOD_NONE;
    ms_modifierMap["KMOD_LCONTROL"] = NiInputKeyboard::KMOD_LCONTROL;
    ms_modifierMap["KMOD_RCONTROL"] = NiInputKeyboard::KMOD_RCONTROL;
    ms_modifierMap["KMOD_LMENU"] = NiInputKeyboard::KMOD_LMENU;
    ms_modifierMap["KMOD_RMENU"] = NiInputKeyboard::KMOD_RMENU;
    ms_modifierMap["KMOD_LWIN"] = NiInputKeyboard::KMOD_LWIN;
    ms_modifierMap["KMOD_RWIN"] = NiInputKeyboard::KMOD_RWIN;
    ms_modifierMap["KMOD_LSHIFT"] = NiInputKeyboard::KMOD_LSHIFT;
    ms_modifierMap["KMOD_RSHIFT"] = NiInputKeyboard::KMOD_RSHIFT;
    ms_modifierMap["KMOD_CAPS_LOCK"] = NiInputKeyboard::KMOD_CAPS_LOCK;

    // Mouse modifiers - note not equivalent to keyboard
    ms_modifierMap["MMOD_NONE"] = NiInputMouse::MMOD_NONE;
    // Mouse buttons
    ms_modifierMap["MMOD_LEFT"] = NiInputMouse::MMOD_LEFT;
    ms_modifierMap["MMOD_RIGHT"] = NiInputMouse::MMOD_RIGHT;
    ms_modifierMap["MMOD_MIDDLE"] = NiInputMouse::MMOD_MIDDLE;
    ms_modifierMap["MMOD_X1"] = NiInputMouse::MMOD_X1;
    ms_modifierMap["MMOD_X2"] = NiInputMouse::MMOD_X2;
    ms_modifierMap["MMOD_X3"] = NiInputMouse::MMOD_X3;
    ms_modifierMap["MMOD_X4"] = NiInputMouse::MMOD_X4;
    ms_modifierMap["MMOD_X5"] = NiInputMouse::MMOD_X5;
    // Keyboard modifiers in mouse space
    ms_modifierMap["MMOD_LCONTROL"] = NiInputMouse::MMOD_LCONTROL;
    ms_modifierMap["MMOD_RCONTROL"] = NiInputMouse::MMOD_RCONTROL;
    ms_modifierMap["MMOD_LMENU"] = NiInputMouse::MMOD_LMENU;
    ms_modifierMap["MMOD_RMENU"] = NiInputMouse::MMOD_RMENU;
    ms_modifierMap["MMOD_LWIN"] = NiInputMouse::MMOD_LWIN;
    ms_modifierMap["MMOD_RWIN"] = NiInputMouse::MMOD_RWIN;
    ms_modifierMap["MMOD_LSHIFT"] = NiInputMouse::MMOD_LSHIFT;
    ms_modifierMap["MMOD_RSHIFT"] = NiInputMouse::MMOD_RSHIFT;
    ms_modifierMap["MMOD_CAPS_LOCK"] = NiInputMouse::MMOD_CAPS_LOCK;
}

//--------------------------------------------------------------------------------------------------
void InputServiceActionBase::UnloadTranslationMap()
{
    ms_actionFlagsMap.clear();
    ms_modifierMap.clear();
}

//--------------------------------------------------------------------------------------------------
