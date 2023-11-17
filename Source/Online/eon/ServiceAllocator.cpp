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
#include "eonPCH.h"

// Needed by EntityManager.h
#include <egf/EntityFactoryResponse.h>
#include <egf/FlatModelFactory.h>
#include <egf/EntityLoaderMessages.h>

#include <egf/EntityManager.h>
#include <eon/OnlineEntityManager.h>
#include <eon/ReplicationService.h>
#include <eon/ServiceAllocator.h>

using namespace efd;
using namespace eon;

//------------------------------------------------------------------------------------------------
Bool eon::CreateOnlineServices(ServiceManager* pServiceManager, UInt32 flags)
{
    // The OnlineEntityManager has some specialized message handling for replicated entities. We
    // register it using the regular EntityManager class ID because that is how the scheduler looks
    // up the entity manager.
    eon::OnlineEntityManagerPtr spOnlineEntityManager = EE_NEW eon::OnlineEntityManager();
    pServiceManager->RegisterSystemService(spOnlineEntityManager);

    // The ReplicationService receives messages related to server owned entity property changes.
    // It keeps local replicated entities up-to-date with their server-side master entities.
    eon::ReplicationServicePtr spReplicationService = EE_NEW eon::ReplicationService();
    pServiceManager->RegisterSystemService(spReplicationService);

    EE_UNUSED_ARG(flags);
    return true;
}
