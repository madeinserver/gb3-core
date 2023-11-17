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

#include "egmToolServicesPCH.h"

#include "NoTexturesRenderClick.h"

#include <NiTexturingProperty.h>

using namespace ecr;
using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
NoTexturesRenderClick::NoTexturesRenderClick()
{
    m_pkTexturingProperty = NiTexturingProperty::GetDefault();

    m_pkTexturePropList = NiNew NiPropertyList();
    m_pkTexturePropList->AddTail(m_pkTexturingProperty);

    m_pkPropertySwapProcessor = NiNew PropertySwapProcessor(m_pkTexturePropList);

    SetProcessor(m_pkPropertySwapProcessor);
}
//--------------------------------------------------------------------------------------------------
NoTexturesRenderClick::~NoTexturesRenderClick()
{
    NiDelete m_pkTexturePropList;
}
//--------------------------------------------------------------------------------------------------
