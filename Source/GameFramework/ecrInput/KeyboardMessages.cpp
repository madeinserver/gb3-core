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

#include "KeyboardMessages.h"

using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(KeyDownMessage);

//------------------------------------------------------------------------------------------------
KeyDownMessage::KeyDownMessage() :
    m_key(NiInputKeyboard::KEY_NOKEY)
{
}

//------------------------------------------------------------------------------------------------
KeyDownMessage::KeyDownMessage(NiInputKeyboard::KeyCode key) :
    m_key(key)
{
}

//------------------------------------------------------------------------------------------------
KeyDownMessage::~KeyDownMessage()
{
}

//------------------------------------------------------------------------------------------------
efd::IMessagePtr KeyDownMessage::FactoryMethod()
{
    return EE_NEW KeyDownMessage;
}

//------------------------------------------------------------------------------------------------
void KeyDownMessage::Serialize(efd::Archive& ar)
{
    IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_key, ar);
}

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(KeyUpMessage);

//------------------------------------------------------------------------------------------------
KeyUpMessage::KeyUpMessage() :
    m_key(NiInputKeyboard::KEY_NOKEY)
{
}

//------------------------------------------------------------------------------------------------
KeyUpMessage::KeyUpMessage(NiInputKeyboard::KeyCode key)
    : m_key(key)
{
}

//------------------------------------------------------------------------------------------------
KeyUpMessage::~KeyUpMessage()
{
}

//------------------------------------------------------------------------------------------------
efd::IMessagePtr KeyUpMessage::FactoryMethod()
{
    return EE_NEW KeyUpMessage;
}

//------------------------------------------------------------------------------------------------
void KeyUpMessage::Serialize(efd::Archive& ar)
{
    IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_key, ar);
}

//------------------------------------------------------------------------------------------------
