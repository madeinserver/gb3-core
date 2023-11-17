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

#include <eon/bapiOnline.h>
#include <egf/bapiEntity.h>
#include <egf/bapiInternal.h>
#include <egf/Entity.h>
#include <egf/Scheduler.h>
#include <egf/ScriptContext.h>
#include <egf/egfLogIDs.h>
#include <eon/ReplicationService.h>
#include <eon/GroupUpdatePolicy.h>

using namespace efd;
using namespace egf;
using namespace eon;


//------------------------------------------------------------------------------------------------
efd::UInt32 bapiOnline::GetVirtualProcessId()
{
    MessageService* pMessageService = g_bapiContext.GetSystemServiceAs<MessageService>();
    EE_ASSERT(pMessageService);
    return pMessageService->GetVirtualProcessID();
}

//------------------------------------------------------------------------------------------------
efd::UInt32 bapiOnline::GetNetID()
{
    INetService* pNetService = g_bapiContext.GetSystemServiceAs<INetService>();
    if (!pNetService)
    {
        EE_LOG(efd::kEntity, efd::ILogger::kERR1,
            ("GetNetID() failed to find NetService"));
        return 0;
    }
    return pNetService->GetNetID();
}

//------------------------------------------------------------------------------------------------
void bapiOnline::SubscribeReplicationChannel(efd::Category replicationChannel)
{
    ReplicationService* pReplicationService =
        g_bapiContext.GetSystemServiceAs<ReplicationService>();
    EE_ASSERT(pReplicationService);
    EE_LOG(efd::kBehaviorAPI, efd::ILogger::kLVL2,
        ("SubscribeReplicationChannel() bapiOnline joining replication channel %s",
        replicationChannel.ToString().c_str()));
    pReplicationService->SubscribeReplicationChannel(replicationChannel);
}


//------------------------------------------------------------------------------------------------
void bapiOnline::UnsubscribeReplicationChannel(efd::Category replicationChannel)
{
    ReplicationService* pReplicationService =
        g_bapiContext.GetSystemServiceAs<ReplicationService>();
    EE_ASSERT(pReplicationService);
    EE_LOG(efd::kBehaviorAPI, efd::ILogger::kLVL2,
        ("UnsubscribeReplicationChannel() bapiOnline leaving replication channel %s",
        replicationChannel.ToString().c_str()));
    pReplicationService->UnsubscribeReplicationChannel(replicationChannel);
}


//------------------------------------------------------------------------------------------------
efd::UInt32 FindAllReplicatedEntities(
    EntityManager::FilterFunction i_pfnFilter,
    void* pParam,
    efd::list<EntityID>& o_results)
{
    EntityManager* pEntityManager = g_bapiContext.GetSystemServiceAs<EntityManager>();
    EntityManager::FilteredIterator iter =
        pEntityManager->GetFilteredIterator(i_pfnFilter, pParam);
    Entity* pEntity;
    while (pEntityManager->GetNextEntity(iter, pEntity))
    {
        if (!pEntity->IsOwned())
        {
            o_results.push_back(pEntity->GetEntityID());
        }
    }

    return 1;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 bapiOnline::FindAllReplicatedEntities(efd::list<EntityID>* OutValue)
{
    egf::EntityID id = g_bapiContext.GetScriptEntity()->GetEntityID();
    return ::FindAllReplicatedEntities(bapiInternal::FF_ExcludeSingleID, &id, *OutValue);
}

//------------------------------------------------------------------------------------------------
bool FF_IncludeCatID(const Entity* pEntity, void* pParam)
{
    Category* pCatID = static_cast<Category*>(pParam);
    const ReplicatingEntity* pRepEntity = EE_DYNAMIC_CAST(ReplicatingEntity, pEntity);
    if (pRepEntity)
        return pRepEntity->UsesReplicationCategory(*pCatID);
    return false;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 bapiOnline::FindAllEntitiesByReplicationCategory(
    efd::Category catIDs,
    efd::list<egf::EntityID>* OutValue)
{
    OutValue->clear();
    return bapiInternal::FindAllEntities(FF_IncludeCatID, &catIDs, *OutValue);
}


//------------------------------------------------------------------------------------------------
bool bapiOnline::UsesReplicationCategory(egf::EntityID entityID, efd::Category catID)
{
    // Grab a pointer to the scheduler so we can call its methods
    Scheduler* pScheduler = g_bapiContext.GetSystemServiceAs<Scheduler>();
    if (pScheduler)
    {
        Entity* pEntity = pScheduler->LookupEntity(entityID);
        if (pEntity)
        {
            ReplicatingEntity* pRepEntity = EE_DYNAMIC_CAST(ReplicatingEntity, pEntity);
            if (pRepEntity)
            {
                return pRepEntity->UsesReplicationCategory(catID);
            }
        }
    }
    return false;
}


//------------------------------------------------------------------------------------------------
bool bapiOnline::SendViewEvent(
    efd::Category eventChannel,
    egf::EntityID entityID,
    const char* strBehavior,
    efd::ParameterList* pStream,
    double delay)
{
    EE_LOG(efd::kPython, efd::ILogger::kLVL2,
        ("SendViewEvent() API sending to channel %s for target %s, behavior=(%s)",
        eventChannel.ToString().c_str(), entityID.ToString().c_str(), strBehavior));

    ReplicatingEntity* pEntity = g_bapiContext.GetScriptEntityAs<ReplicatingEntity>();
    if (pEntity)
    {
        return pEntity->SendViewEvent(eventChannel, entityID, NULL, strBehavior,
                                      pStream, delay);
    }
    return false;
}

//------------------------------------------------------------------------------------------------
bool bapiOnline::SendLocalViewEvent(
    egf::EntityID entityID,
    const char* strBehavior,
    efd::ParameterList* pStream,
    double delay)
{
    EE_LOG(efd::kPython, efd::ILogger::kLVL2,
        ("SendLocalViewEvent() API sending to entity %s, behavior=(%s)",
        entityID.ToString().c_str(), strBehavior));

    ReplicationService* pReplicationService =
        g_bapiContext.GetSystemServiceAs<ReplicationService>();
    EE_ASSERT(pReplicationService);

    Entity* pEntity = pReplicationService->FindEntity(entityID);

    if (pEntity == NULL)
    {
        return false;
    }

    EventMessagePtr spEvent = pEntity->CreateEventMessage(
        entityID,
        NULL,
        strBehavior,
        pStream,
        NULL,
        delay,
        false);

    pReplicationService->QueueBehaviorForReplicatedEntity(entityID, spEvent);
    return true;
}


//------------------------------------------------------------------------------------------------
efd::Category bapiOnline::GetReplicationCategory(egf::EntityID id, efd::UInt32 group)
{
    ReplicationService* pRS = g_bapiContext.GetSystemServiceAs<ReplicationService>();
    if (!pRS)
    {
        return 0;
    }

    // Use FindEntity, only owned entities produce into their replication categories so only they
    // bother to set this data and it doesn't make sense to read the unset values.
    ReplicatingEntityPtr pEntity = pRS->FindEntity(id);
    if (!pEntity)
    {
        return 0;
    }

    return pEntity->GetReplicationCategory(group);
}


//------------------------------------------------------------------------------------------------
bool bapiOnline::SetReplicationCategory(
    egf::EntityID entityID,
    efd::UInt32 group,
    efd::Category replicationCategory)
{
    ReplicationService* pReplicationService =
        g_bapiContext.GetSystemServiceAs<ReplicationService>();
    if (!pReplicationService)
    {
        EE_LOG(efd::kEntity, efd::ILogger::kERR1,
            ("%s: unable to find ReplicationService", __FUNCTION__));
        return false;
    }

    // Use FindEntity, only owned entities produce into their replication categories so it doesn't
    // make sense to set this value for a replication.
    ReplicationProducerEntityPtr pEntity = pReplicationService->FindProducerEntity(entityID);
    if (!pEntity)
    {
        EE_LOG(efd::kEntity, efd::ILogger::kERR1,
            ("%s: unable to find Entity %s", __FUNCTION__, entityID.ToString().c_str()));
        return false;
    }
    return pEntity->SetReplicationCategory(group, replicationCategory);
}

//------------------------------------------------------------------------------------------------
void bapiOnline::SetGroupUpdateInterval(
    efd::UInt32 group,
    efd::TimeType newDelta)
{
    IReplicationGroupPolicy* pPolicy = IReplicationGroupPolicy::GetReplicationGroupPolicy(group);
    if (pPolicy)
    {
        pPolicy->SetMinUpdateDelta(newDelta);
    }
}
