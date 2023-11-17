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

#include "WireframeRenderClick.h"

#include <NiWireframeProperty.h>

using namespace ecr;
using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
WireframeRenderClick::WireframeRenderClick()
{
    m_pkWireframeProperty = NiNew NiWireframeProperty();
    m_pkWireframeProperty->SetWireframe(true);

    m_pkWirePropList = NiNew NiPropertyList();
    m_pkWirePropList->AddTail(m_pkWireframeProperty);

    m_pkPropertySwapProcessor = NiNew PropertySwapProcessor(m_pkWirePropList);

    SetProcessor(m_pkPropertySwapProcessor);
}
//--------------------------------------------------------------------------------------------------
WireframeRenderClick::~WireframeRenderClick()
{
    NiDelete m_pkWirePropList;
}
//--------------------------------------------------------------------------------------------------
