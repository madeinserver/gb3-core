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

#include "InputStickMessage.h"

using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(InputStickMessage);

//------------------------------------------------------------------------------------------------
// On demand class creation
//------------------------------------------------------------------------------------------------
efd::IBasePtr InputStickMessage::FactoryMethod()
{
    return EE_NEW InputStickMessage;
}

//------------------------------------------------------------------------------------------------
// Message data saving to stream and getting it back
//------------------------------------------------------------------------------------------------
void InputStickMessage::Serialize(efd::Archive& ar)
{
    InputActionMessage::Serialize(ar);

    efd::Serializer::SerializeObject(m_x, ar);
    efd::Serializer::SerializeObject(m_y, ar);
    efd::Serializer::SerializeObject(m_z, ar);
}

//------------------------------------------------------------------------------------------------
