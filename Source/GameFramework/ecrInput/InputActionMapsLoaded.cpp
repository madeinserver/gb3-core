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

#include "InputActionMapsLoaded.h"

using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(InputActionMapsLoaded);

//------------------------------------------------------------------------------------------------
InputActionMapsLoaded::InputActionMapsLoaded()
{
}
//------------------------------------------------------------------------------------------------
InputActionMapsLoaded::InputActionMapsLoaded(const efd::utf8string& urn, const bool success)
    : m_urn(urn)
    , m_success(success)
{
}
//------------------------------------------------------------------------------------------------
efd::utf8string InputActionMapsLoaded::GetRequestedURN() const
{
    return m_urn;
}
//------------------------------------------------------------------------------------------------
void InputActionMapsLoaded::SetRequestedURN(const efd::utf8string& urn)
{
    m_urn = urn;
}
//------------------------------------------------------------------------------------------------
bool InputActionMapsLoaded::GetSuccess() const
{
    return m_success;
}
//------------------------------------------------------------------------------------------------
void InputActionMapsLoaded::SetSuccess(const bool success)
{
    m_success = success;
}
//------------------------------------------------------------------------------------------------