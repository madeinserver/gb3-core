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

#include "MouseMessages.h"

using namespace ecrInput;


EE_IMPLEMENT_CONCRETE_CLASS_INFO(MouseMessage);

//------------------------------------------------------------------------------------------------
void MouseMessage::Serialize(efd::Archive& ar)
{
    IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_x, ar);
    efd::Serializer::SerializeObject(m_y, ar);
}

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(MouseMoveMessage);

//------------------------------------------------------------------------------------------------
MouseMoveMessage::MouseMoveMessage()
    : m_xDelta(0)
    , m_yDelta(0)
    , m_zDelta(0)
{
}

//------------------------------------------------------------------------------------------------
MouseMoveMessage::MouseMoveMessage(efd::SInt32 xDelta, efd::SInt32 yDelta, efd::SInt32 zDelta)
    : m_xDelta(xDelta)
    , m_yDelta(yDelta)
    , m_zDelta(zDelta)
{
}

//------------------------------------------------------------------------------------------------
MouseMoveMessage::~MouseMoveMessage()
{
}

//------------------------------------------------------------------------------------------------
efd::IMessagePtr MouseMoveMessage::FactoryMethod()
{
    return EE_NEW MouseMoveMessage;
}

//------------------------------------------------------------------------------------------------
void MouseMoveMessage::Serialize(efd::Archive& ar)
{
    MouseMessage::Serialize(ar);

    efd::Serializer::SerializeObject(m_xDelta, ar);
    efd::Serializer::SerializeObject(m_yDelta, ar);
    efd::Serializer::SerializeObject(m_zDelta, ar);
}


//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(MouseDownMessage);

//------------------------------------------------------------------------------------------------
MouseDownMessage::MouseDownMessage() :
    m_button(MBUTTON_LEFT)
{
}

//------------------------------------------------------------------------------------------------
MouseDownMessage::MouseDownMessage(MouseButton button) :
    m_button(button)
{
}

//------------------------------------------------------------------------------------------------
MouseDownMessage::~MouseDownMessage()
{
}

//------------------------------------------------------------------------------------------------
efd::IMessagePtr MouseDownMessage::FactoryMethod()
{
    return EE_NEW MouseDownMessage;
}

//------------------------------------------------------------------------------------------------
void MouseDownMessage::Serialize(efd::Archive& ar)
{
    MouseMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_button, ar);
}


//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(MouseUpMessage);

//------------------------------------------------------------------------------------------------
MouseUpMessage::MouseUpMessage() :
    m_button(MBUTTON_LEFT)
{
}

//------------------------------------------------------------------------------------------------
MouseUpMessage::MouseUpMessage(MouseButton button) :
    m_button(button)
{
}

//------------------------------------------------------------------------------------------------
MouseUpMessage::~MouseUpMessage()
{
}

//------------------------------------------------------------------------------------------------
efd::IMessagePtr MouseUpMessage::FactoryMethod()
{
    return EE_NEW MouseUpMessage;
}

//------------------------------------------------------------------------------------------------
void MouseUpMessage::Serialize(efd::Archive& ar)
{
    MouseMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_button, ar);
}

//------------------------------------------------------------------------------------------------
