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

#include "egmAnimationPCH.h"
#include <efd/utf8string.h>
#include <egmAnimation/KFMFactoryRequest.h>

using namespace efd;
using namespace egmAnimation;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(KFMFactoryRequest);

//------------------------------------------------------------------------------------------------
KFMFactoryRequest::KFMFactoryRequest()
    : AssetLoadRequest()
    , m_cumulative(false)
{
}

//------------------------------------------------------------------------------------------------
KFMFactoryRequest::KFMFactoryRequest(
    const utf8string& urn,
    const Category& responseCategory,
    const efd::utf8string& assetPath,
    const bool isBackground,
    const bool isPreemptive,
    const bool cumulative)
    : AssetLoadRequest(urn, responseCategory, assetPath, isBackground, isPreemptive)
    , m_cumulative(cumulative)
{
}

//------------------------------------------------------------------------------------------------
void KFMFactoryRequest::SetAccumulation(bool cumulative)
{
    m_cumulative = cumulative;
}

//------------------------------------------------------------------------------------------------
bool KFMFactoryRequest::GetAccumulation() const
{
    return m_cumulative;
}
