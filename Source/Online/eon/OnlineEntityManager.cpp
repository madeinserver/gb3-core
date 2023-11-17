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

#include "eonPCH.h"

#include <eon/eonLogIDs.h>
#include <eon/OnlineEntityManager.h>
#include <eon/OnlineEntityChangeMessage.h>
#include <eon/OnlineEntity.h>
#include <egf/EntityFactoryResponse.h>
#include <eon/ReplicationService.h>
#include <efd/ServiceManager.h>
#include <efd/IServiceDetailRegister.h>

using namespace efd;
using namespace egf;
using namespace eon;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(OnlineEntityManager);


//------------------------------------------------------------------------------------------------
OnlineEntityManager::OnlineEntityManager()
: EntityManager()
, m_spReplicatedEntityAddedMsg(EE_NEW ReplicatedEntityAddedMessage)
, m_spReplicatedEntityUpdatedMsg(EE_NEW ReplicatedEntityUpdatedMessage)
, m_spReplicatedEntityRemovedMsg(EE_NEW ReplicatedEntityRemovedMessage)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2500;
}

//------------------------------------------------------------------------------------------------
void OnlineEntityManager::OnServiceRegistered(efd::IAliasRegistrar* pAliasRegistrar)
{
    pAliasRegistrar->AddIdentity<EntityManager>();
    EntityManager::OnServiceRegistered(pAliasRegistrar);
}

//------------------------------------------------------------------------------------------------
void OnlineEntityManager::OnEntityEndLifecycle(Entity* pEntity, efd::UInt32 lifecycle)
{
    switch (lifecycle)
    {
    case ReplicationConsumerEntity::lifecycle_OnDiscovery:
        // This is a little bit confusing but the OnDiscovery lifecycle for replicated entities
        // needs the exact same finishing code as OnCreate for owned entities so we simply call
        // the base implementation as if this was an OnCreate lifecycle completing.
        EntityManager::OnEntityEndLifecycle(pEntity, Entity::lifecycle_OnCreate);
        break;

    case ReplicationConsumerEntity::lifecycle_OnReplicaAssetsLoaded:
        // This is also a little bit confusing but the OnReplicaAssetsLoaded lifecycle for
        // consumer entities needs the exact same finishing code as OnAssetsLoaded for producer
        // entities so we simply call the base implementation as if this was an OnAssetsLoaded
        // lifecycle completing.
        EntityManager::OnEntityEndLifecycle(pEntity, Entity::lifecycle_OnAssetsLoaded);
        break;

    default:
        // All other lifecycle events just get the normal default behavior:
        EntityManager::OnEntityEndLifecycle(pEntity, lifecycle);
        break;
    }
}

//------------------------------------------------------------------------------------------------
void OnlineEntityManager::SendEntityCreationNotification(Entity* pEntity)
{
    if (pEntity->IsOwned())
    {
        EntityManager::SendEntityCreationNotification(pEntity);
    }
    else
    {
        if (m_spMessageService)
        {
            m_spReplicatedEntityAddedMsg->SetEntity(pEntity);
            m_spMessageService->SendImmediate(m_spReplicatedEntityAddedMsg);
            m_spReplicatedEntityAddedMsg->SetEntity(NULL);
        }
    }
}

//------------------------------------------------------------------------------------------------
void OnlineEntityManager::SendEntityUpdateNotification(Entity* pEntity)
{
    if (pEntity->IsOwned())
    {
        EntityManager::SendEntityUpdateNotification(pEntity);
    }
    else
    {
        if (m_spMessageService)
        {
            m_spReplicatedEntityUpdatedMsg->SetEntity(pEntity);
            m_spMessageService->SendImmediate(m_spReplicatedEntityUpdatedMsg);
            m_spReplicatedEntityUpdatedMsg->SetEntity(NULL);
        }
    }
}

//------------------------------------------------------------------------------------------------
void OnlineEntityManager::SendEntityRemovalNotification(Entity* pEntity)
{
    if (pEntity->IsOwned())
    {
        EntityManager::SendEntityRemovalNotification(pEntity);
    }
    else
    {
        if (m_spMessageService)
        {
            m_spReplicatedEntityRemovedMsg->SetEntity(pEntity);
            m_spMessageService->SendImmediate(m_spReplicatedEntityRemovedMsg);
            m_spReplicatedEntityRemovedMsg->SetEntity(NULL);
        }
    }
}

//------------------------------------------------------------------------------------------------
bool OnlineEntityManager::AddEntity(Entity* pEntity, ParameterList* pDataStream)
{
    ReplicationConsumerEntity* pReplicationConsumerEntity =
        EE_DYNAMIC_CAST(ReplicationConsumerEntity, pEntity);
    if (pReplicationConsumerEntity)
    {
        // Ask ReplicationService to apply all pending discovers, updates and losses
        ReplicationService* pReplicationService =
            m_pServiceManager->GetSystemServiceAs<ReplicationService>();
        if (!pReplicationService->ApplyReplicationMessages(pReplicationConsumerEntity))
        {
            EE_LOG(efd::kReplicationService, efd::ILogger::kLVL1,
                ("OnlineEntityManager: %s was lost before async entity creation completed.",
                pReplicationConsumerEntity->GetEntityID().ToString().c_str()));
            return false;
        }
    }
    return EntityManager::AddEntity(pEntity, pDataStream);
}

//------------------------------------------------------------------------------------------------
bool OnlineEntityManager::DestroyEntity(Entity* pEntity)
{
    ReplicationConsumerEntity* pReplicationConsumerEntity =
        EE_DYNAMIC_CAST(ReplicationConsumerEntity, pEntity);
    if (pReplicationConsumerEntity)
    {
        return RemoveEntity(pEntity);
    }
    else
    {
        return EntityManager::DestroyEntity(pEntity);
    }
}

//------------------------------------------------------------------------------------------------
const char* OnlineEntityManager::GetDisplayName() const
{
    return "OnlineEntityManager";
}
