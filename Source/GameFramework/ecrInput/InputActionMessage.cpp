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

#include "InputActionMessage.h"

using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(InputActionMessage);

//------------------------------------------------------------------------------------------------
efd::IBasePtr InputActionMessage::FactoryMethod()
{
    return EE_NEW InputActionMessage;
}

//------------------------------------------------------------------------------------------------
void InputActionMessage::Serialize(efd::Archive&)
{
    EE_FAIL("This message can only be sent locally");
}

//------------------------------------------------------------------------------------------------
