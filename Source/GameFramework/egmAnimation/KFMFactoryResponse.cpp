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

#include <egmAnimation/KFMFactoryResponse.h>

using namespace efd;
using namespace egmAnimation;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(KFMFactoryResponse);

//------------------------------------------------------------------------------------------------
KFMFactoryResponse::KFMFactoryResponse(
    const efd::utf8string& urn,
    const efd::Category& responseCategory,
    AssetLoadResponse::AssetLoadResult result,
    const efd::utf8string& assetPath,
    bool isReload)
    : AssetLoadResponse(urn, responseCategory, result, assetPath, isReload)
{
}

//------------------------------------------------------------------------------------------------
KFMFactoryResponse::KFMFactoryResponse()
    : AssetLoadResponse()
{
}

//------------------------------------------------------------------------------------------------
NiActorManager* KFMFactoryResponse::GetActor() const
{
    return m_spActor;
}

//------------------------------------------------------------------------------------------------
void KFMFactoryResponse::SetActor(NiActorManager* pActor)
{
    m_spActor = pActor;
}
