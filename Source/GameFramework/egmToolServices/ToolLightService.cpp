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

#include <ecr/LightService.h>
#include "ToolLightService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolLightService);


//------------------------------------------------------------------------------------------------
ToolLightService::ToolLightService(bool enableAutomaticRelighting) :
    LightService(enableAutomaticRelighting)
{
    /* */
}

//------------------------------------------------------------------------------------------------
ToolLightService::~ToolLightService()
{
    /* */
}

//------------------------------------------------------------------------------------------------
const char* ToolLightService::GetDisplayName() const
{
    return "ToolLightService";
}

//------------------------------------------------------------------------------------------------
void ToolLightService::OnServiceRegistered(efd::IAliasRegistrar* pAliasRegistrar)
{
    pAliasRegistrar->AddIdentity<LightService>();
    LightService::OnServiceRegistered(pAliasRegistrar);
}

//------------------------------------------------------------------------------------------------
void ToolLightService::LightEntityChanged(LightService::LightData* pLightData, egf::Entity* pEntity)
{
    // Found the light
    NiLight* pkLight = pLightData->m_spLight;

    // Store properties on NiLight and add Entity to changed light list.
    egf::EntityID kEntityID = pEntity->GetEntityID();

    UpdateLightProperties(pkLight, pEntity);
    UpdateLightData(pEntity, kEntityID);
    UpdateShadowCaster(pkLight, pEntity);

    // Normally we would only push the light to the changed light list if
    // the light if the "UpdateLightingOnMove" property is true. However, in
    // tool mode we always act as if this property is true.
    m_changedLightList.push_back(pLightData);
}

//------------------------------------------------------------------------------------------------
