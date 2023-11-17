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

#include <efd/ServiceManager.h>
#include <eon/ReplicationService.h>
#include <egf/FlatModelManager.h>
#include <efd/ILogger.h>
#include <efd/IConfigManager.h>
#include <efd/IDs.h>
#include <efd/Metrics.h>
#include <eon/OnlineEntityChangeMessage.h>
#include <eon/IReplicationGroupPolicy.h>
#include <efd/EnumManager.h>
#include <efd/AssetLoadResponse.h>
#include <egf/EntityFactoryResponse.h>
#include <eon/eonLogIDs.h>

//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace egf;
using namespace eon;


//------------------------------------------------------------------------------------------------
// ReplicationService service class method implementations
//
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ReplicationService);


EE_HANDLER(ReplicationService, HandleDiscoveryRequestMsg, eon::DiscoveryRequest);
EE_HANDLER(ReplicationService, HandleViewBehaviorMsg, eon::ViewEventMessage);

EE_HANDLER_WRAP(
    ReplicationService,
    HandleEntityDiscovery,
    eon::EntityMessage,
    efd::kMSGID_EntityDiscoveryMessage);

EE_HANDLER_WRAP(
    ReplicationService,
    HandleEntityUpdate,
    eon::EntityMessage,
    efd::kMSGID_EntityUpdateMessage);

EE_HANDLER_WRAP(
    ReplicationService,
    HandleEntityLoss,
    eon::EntityMessage,
    efd::kMSGID_EntityLossMessage);

EE_HANDLER_WRAP(
    ReplicationService,
    HandleOwnedEntityAdded,
    EntityChangeMessage,
    kMSGID_OwnedEntityAdded);

EE_HANDLER_WRAP(
    ReplicationService,
    HandleOwnedEntityRemoved,
    EntityChangeMessage,
    kMSGID_OwnedEntityRemoved);

EE_HANDLER(ReplicationService, HandleNetIDAssigned, AssignNetIDMessage);

EE_HANDLER(ReplicationService, HandleAssetLoadResponse, AssetLoadResponse);
EE_HANDLER_SUBCLASS(
    ReplicationService,
    HandleAssetLoadResponse,
    AssetLoadResponse,
    EntityFactoryResponse);

//------------------------------------------------------------------------------------------------
ReplicationService::ReplicationService()
    : m_myPrivateCategory(kCAT_INVALID)
    , m_pScheduler(NULL)
    , m_discoveryMsg(0)
    , m_updateMsg(0)
    , m_lossMsg(0)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 5400;
}


//------------------------------------------------------------------------------------------------
ReplicationService::~ReplicationService()
{
}


//------------------------------------------------------------------------------------------------
EntityPtr ReplicationService::EntityFactory(
    const FlatModel* i_pModel,
    egf::EntityID i_eid,
    bool i_master)
{
    if (i_master)
    {
        return EE_NEW ReplicationProducerEntity(i_pModel, i_eid);
    }
    return EE_NEW ReplicationConsumerEntity(i_pModel, i_eid);
}

//------------------------------------------------------------------------------------------------
efd::SyncResult ReplicationService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<egf::EntityManager>();
    pDependencyRegistrar->AddDependency<egf::Scheduler>(sdf_Optional);

    // first, register the messages we have an interest in with local message service...
    // Grab a pointer to the message service
    m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    if (!m_spMessageService)
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR1, ("Can't find MessageService!"));
        return efd::SyncResult_Failure;
    }

    m_pScheduler = m_pServiceManager->GetSystemServiceAs< Scheduler >();

    m_spEntityManager = m_pServiceManager->GetSystemServiceAs<EntityManager>();
    if (!m_spEntityManager)
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR2,
            ("Init: Failed to find local message service to register callback handlers!"));
        return efd::SyncResult_Failure;
    }

    FlatModelManager* pfmm = m_pServiceManager->GetSystemServiceAs<FlatModelManager>();
    if (pfmm)
    {
        pfmm->RegisterEntityFactory(&ReplicationService::EntityFactory, 1);
    }

    // Initialize replication group policies
    IConfigManager* pIConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    if (pIConfigManager)
    {
        EnumManager* pEnumManager = m_pServiceManager->GetSystemServiceAs<EnumManager>();
        // IReplicationGroupPolicy::Init gracefully handles NULL values for EnumManager
        IReplicationGroupPolicy::Init(pIConfigManager, pEnumManager);
    }

    // EntityChangeMessage messages are sent to local message:
    m_spMessageService->Subscribe(this, kCAT_LocalMessage);

    // For receiving the AssignNetIDMessage message:
    Category netServiceCat = m_spMessageService->GetServicePublicCategory(INetService::CLASS_ID);
    m_spMessageService->Subscribe(this, netServiceCat);

    return efd::SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ReplicationService::OnTick()
{
    if (m_discoveryMsg || m_updateMsg || m_lossMsg)
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL3,
            ("Completed OnTick() event.  Have %d entities in local cache. "
            "DiscMsg = %d, UpdateMsg=%d, LossMsg=%d",
            m_spEntityManager->GetCount(),
            m_discoveryMsg, m_updateMsg, m_lossMsg));
    }

    // process normal ReplicationService ticks here...
    m_discoveryMsg = 0;     // clear counts of messages received
    m_updateMsg = 0;
    m_lossMsg = 0;

    for (UInt32 i = 0; i < k_MAX_REPLICATION_GROUPS; ++i)
    {
        IReplicationGroupPolicy* pPolicy = IReplicationGroupPolicy::GetReplicationGroupPolicy(i);
        if (pPolicy)
        {
            pPolicy->BeginUpdate();
        }
    }

    for (DirtyReplicatorsMap::iterator iter = m_entityChanges.begin();
        iter != m_entityChanges.end();
        /* do nothing */)
    {
        ReplicationProducerEntity* pEntity = iter->first;
        ReplicationAction& ra = iter->second;
        bool lossSent = false;

        // We are about to send out some form of update for this entity (either discovery, loss,
        // or update) on one or more groups, so we want to bump the sequence number just once.
        pEntity->IncrementSequenceNumber();

        for (CategoryChangesMap::iterator itChanges = ra.m_catChanges.begin();
            itChanges != ra.m_catChanges.end();
            ++itChanges)
        {
            efd::UInt32 groupIndex = itChanges->first;
            CategoryChange& cc = itChanges->second;

            // send loss for old channel
            if (cc.m_old.IsValid())
            {
                IReplicationGroupPolicy* pPolicy =
                    IReplicationGroupPolicy::GetReplicationGroupPolicy(groupIndex);

                pPolicy->GenerateLoss(
                    m_pServiceManager,
                    pEntity,
                    groupIndex,
                    cc.m_old,
                    cc.m_new);
                EndCategoryProduction(cc.m_old);
                lossSent = true;
            }
        }

        if (lossSent)
        {
            // Extra bump for discoveries/update since we already used the current value to send
            // out the loss message and any new discovery needs to use a higher sequence.
            pEntity->IncrementSequenceNumber();
        }

        for (CategoryChangesMap::iterator itChanges = ra.m_catChanges.begin();
            itChanges != ra.m_catChanges.end();
            ++itChanges)
        {
            efd::UInt32 groupIndex = itChanges->first;
            CategoryChange& cc = itChanges->second;

            // send discovery for new channel
            if (cc.m_new.IsValid())
            {
                IReplicationGroupPolicy* pPolicy =
                    IReplicationGroupPolicy::GetReplicationGroupPolicy(groupIndex);

                QualityOfService qos = pPolicy->GetQualityOfService();
                EE_ASSERT(cc.m_new == pEntity->GetReplicationCategory(groupIndex));
                BeginCategoryProduction(cc.m_new, qos);
                pPolicy->GenerateDiscovery(m_pServiceManager, pEntity, groupIndex);
            }

            // clear dirty on discovered group since I just set either a discovery or a loss.
            ra.ClearDirtyGroup(groupIndex);
        }

        // No need to bump the sequence number again since any group we sent a discover to will not
        // be sending an update.

        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL3,
            ("Entity %s pre-update dirty groups = 0x%08X",
            pEntity->GetEntityID().ToString().c_str(),
            ra.m_dirty));

        if (ra.m_dirty)
        {
            // consider each dirty group's policy to see if it's time for an update. This will
            // make callbacks to clear dirtiness for any group that sends an update message.
            UpdateDirtyReplicationGroups(pEntity, ra.m_dirty);
        }

        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL3,
            ("Entity %s post-update dirty groups = 0x%08X",
            pEntity->GetEntityID().ToString().c_str(),
            ra.m_dirty));

        if (0 == ra.m_dirty)
        {
            // If we have resolved all dirtiness we can completely remove this entry:
            DirtyReplicatorsMap::iterator iterToDelete = iter;
            ++iter;
            m_entityChanges.erase(iterToDelete);
        }
        else
        {
            // We can still clear out the data related to categories, discover, and loss but we
            // need to keep the entry around so we can check the update policies:
            ra.ResetCategoryData();

            ++iter;
        }
    }

    for (UInt32 i = 0; i < k_MAX_REPLICATION_GROUPS; ++i)
    {
        IReplicationGroupPolicy* pPolicy = IReplicationGroupPolicy::GetReplicationGroupPolicy(i);
        if (pPolicy)
        {
            pPolicy->EndUpdate();
        }
    }

    return efd::AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ReplicationService::OnShutdown()
{
    if (m_spMessageService)
    {
        m_spMessageService->Unsubscribe(this, kCAT_LocalMessage);

        Category netSrvCat = m_spMessageService->GetServicePublicCategory(INetService::CLASS_ID);
        m_spMessageService->Unsubscribe(this, netSrvCat);

        if (m_myPrivateCategory.IsValid())
        {
            m_spMessageService->Unsubscribe(this, m_myPrivateCategory);
        }

        for (efd::map< efd::Category, efd::UInt32 >::iterator it = m_subscriptionMap.begin();
            it != m_subscriptionMap.end();
            ++it)
        {
            m_spMessageService->Unsubscribe(this, it->first);
        }
        m_subscriptionMap.clear();

        for (efd::map< efd::Category, efd::UInt32 >::iterator it = m_productionMap.begin();
            it != m_productionMap.end();
            ++it)
        {
            m_spMessageService->EndCategoryProduction(it->first, this);
        }
        m_productionMap.clear();

        m_spMessageService = NULL;
    }

    m_pScheduler = NULL;
    m_spEntityManager = NULL;

    m_pendingEntityCreates.clear();

    return efd::AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
bool ReplicationService::AddEntity(ReplicationConsumerEntity* pEntity)
{
    EE_ASSERT(m_spEntityManager);

    bool result = m_spEntityManager->AddEntity(pEntity);

    return result;
}

//------------------------------------------------------------------------------------------------
void ReplicationService::UpdateEntity(ReplicationConsumerEntity* pEntity, Category updateCategory)
{
    CategoryToEntityListMap::iterator foundIter = m_categoryToEntityListMap.find(updateCategory);

    // If we are subscribed to this category then keep track of the entity
    if (foundIter != m_categoryToEntityListMap.end())
    {
        foundIter->second->insert(pEntity);
    }
    else
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR3,
            ("Received update on unsubscribed category %s for entity %s (%s)",
            updateCategory.ToString().c_str(),
            pEntity->GetEntityID().ToString().c_str(),
            pEntity->GetModelName().c_str()));
    }
}

//------------------------------------------------------------------------------------------------
bool ReplicationService::RemoveEntity(ReplicationConsumerEntity* pEntity)
{
    // In theory only replicated entities get here
    EE_ASSERT(pEntity);
    EE_ASSERT(!pEntity->IsOwned());

    if (!pEntity->ReplicationGroupsEmpty())
    {
        // need to cleanup any remaining category subscriptions on this entity
        for (UInt32 i=0; i<32; ++i)
        {
            ChangeEntityReplicationCategory(pEntity, i, kCAT_INVALID);
        }
    }

    pEntity->SetReplicationService(NULL);

    // tick off another replica, plus by model ID. For owned entities, this happens in
    // Entity::Destroy.
    EE_LOG_METRIC_COUNT_FMT(kEntity, ("DESTROY.REPLICA.%s",
        pEntity->GetModelName().c_str()));

    // NOTE: The local cache might be the last object holding a reference to the entity, so this
    // call is very likely to destruct the entity.  It is not safe to use the entity pointer
    // after making this call.
    bool result = m_spEntityManager->DestroyEntity(pEntity);

    EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
        ("RemoveEntity: %s removed from ReplicationService.",
        pEntity->GetEntityID().ToString().c_str()));

    return result;
}

//------------------------------------------------------------------------------------------------
void ReplicationService::SubscribeReplicationChannel(efd::Category channelToAdd)
{
    EE_ASSERT(m_spMessageService);

    EE_LOG(efd::kReplicationService, ILogger::kLVL2,
        ("%s| ReplicationService subscribing to new replication category",
        channelToAdd.ToString().c_str()));

    efd::UInt32 refCount = ++(m_subscriptionMap[channelToAdd]);

    CategoryToEntityListMap::iterator foundIter = m_categoryToEntityListMap.find(channelToAdd);

    if (foundIter == m_categoryToEntityListMap.end())
    {
        // this list does not exist, we must add it
        m_categoryToEntityListMap[channelToAdd] = EE_NEW EntityList;
    }

    // The first time we subscribe to a replication channel we need to do the actual subscription
    // and then request initial discovery.
    if (1 == refCount)
    {
        // All EntityMessages route to one handler where a switch handles the differing types.
        // We might receive view behavior requests on any replication channel:
        m_spMessageService->Subscribe(this, channelToAdd);

        if (m_myPrivateCategory.IsValid())
        {
            // Request discovery messages for this channel to be sent directly to the client:
            DiscoveryRequestPtr spRequestMsg = EE_NEW DiscoveryRequest();
            spRequestMsg->SetSenderID(m_myPrivateCategory);
            // Send this request to everyone who produces onto the replication channel:
            EE_ASSERT(m_spMessageService);
            m_spMessageService->ProducerSend(spRequestMsg, channelToAdd);
        }
    }
}


//------------------------------------------------------------------------------------------------
void ReplicationService::UnsubscribeReplicationChannel(efd::Category channelToRemove)
{
    EE_ASSERT(m_spMessageService);

    if (!channelToRemove.IsValid())
    {
        return;
    }
    EE_LOG(efd::kReplicationService, ILogger::kLVL2,
        ("%s| ReplicationService unsubscribing to replication category",
        channelToRemove.ToString().c_str()));

    efd::map<efd::Category, efd::UInt32>::iterator iter = m_subscriptionMap.find(channelToRemove);
    if (iter != m_subscriptionMap.end())
    {
        UInt32 refCount = --(m_subscriptionMap[channelToRemove]);
        if (0 == refCount)
        {
            m_subscriptionMap.erase(channelToRemove);
            // This is the last Subscription to this Category so we must iterate over all entities
            // that we have received updates about from this Category and check to see if we are
            // still observing them:
            CategoryToEntityListMap::iterator eraseIter =
                m_categoryToEntityListMap.find(channelToRemove);

            if (eraseIter != m_categoryToEntityListMap.end())
            {
                EntityListPtr spEntityPointerList = eraseIter->second;
                for (EntityList::iterator it = spEntityPointerList->begin();
                    it != spEntityPointerList->end();
                    ++it)
                {
                    eon::ReplicationConsumerEntity* pEntity = *it;
                    pEntity->RemoveReplicationGroupCategory(channelToRemove);
                    if (pEntity->ReplicationGroupsEmpty())
                    {
                        RemoveEntity(pEntity);
                    }
                }
                m_categoryToEntityListMap.erase(eraseIter);
                spEntityPointerList = NULL;
            }
            else
            {
                EE_FAIL_MESSAGE((
                    "%s not found in subscriber list",
                    channelToRemove.ToString().c_str()));
            }

            // Only unsubscribe for real on the transition from 1 to 0 usages
            m_spMessageService->Unsubscribe(this, channelToRemove);
        }
    }
    else
    {
        EE_LOG(efd::kReplicationService, ILogger::kERR3,
            ("ReplicationService not subscribed to replication category %s, unsubscribe failed.",
            channelToRemove.ToString().c_str()));
    }
}


//------------------------------------------------------------------------------------------------
void ReplicationService::HandleEntityDiscovery(
    const EntityMessage* pMsg,
    efd::Category targetChannel)
{
    EE_MESSAGE_TRACE(
        pMsg,
        efd::kReplicationService,
        efd::ILogger::kLVL1,
        efd::ILogger::kLVL3,
        ("%s| %s type=0x%08X %s %s",
        pMsg->GetDescription().c_str(),
        __FUNCTION__,
        pMsg->GetClassID(),
        pMsg->GetEntityID().ToString().c_str(),
        targetChannel.ToString().c_str()));

    EE_LOG_METRIC_COUNT(kReplicationService, "RECEIVE.ENTITY_DISCOVER");

    // if here, we have a new message that contains a new entity...
    m_discoveryMsg++;

    // First we pull just the EntityID out of the stream.  This ID is always the very
    // first thing in the stream.
    pMsg->ResetForUnpacking();
    EntityID entityToDiscover = pMsg->GetEntityID();
    efd::utf8string modelName = pMsg->GetModelName();
    UInt32 groupIndex = pMsg->GetGroupIndex();

    Entity* pOwnedEntity = m_spEntityManager->LookupEntity(entityToDiscover);
    if (pOwnedEntity  && pOwnedEntity->IsOwned())
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR2,
            ("Discover: %s (%s) owned by our simulator, discover ignored.",
            modelName.c_str(),
            entityToDiscover.ToString().c_str()));
    }
    else
    {
        EntityPtr spNewEntity = NULL;
        ReplicationConsumerEntity* pExistingEntity = FindConsumerEntity(entityToDiscover);

        if (!pExistingEntity)
        {
            PendingCreateData data;
            data.m_spEntityMsg = pMsg;
            data.m_replicationGroupIndex = groupIndex;

            // create in progress...
            PendingEntityToDataMap::iterator it = m_pendingEntityCreates.find(entityToDiscover);
            if (it != m_pendingEntityCreates.end())
            {
                EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
                    ("Discover: creation in progress for %s (%s), adding group %d.",
                    modelName.c_str(),
                    entityToDiscover.ToString().c_str(),
                    groupIndex));

                it->second.push_back(data);
            }
            else
            {
                EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
                    ("Discover: begin creation for %s (%s) for group %d.",
                    modelName.c_str(),
                    entityToDiscover.ToString().c_str(),
                    groupIndex));

                m_pendingEntityCreates[entityToDiscover].push_back(data);

                m_spEntityManager->CreateEntity(
                    modelName,
                    m_myPrivateCategory,
                    0,                      // ParameterList pointer
                    0,                      // ParameterList pointer
                    entityToDiscover,
                    false);                 // is not the master entity
            }

            // Finalized in HandleAssetLoadResponse.
            return;
        }

        pExistingEntity->UpdatePropertiesFromStream(pMsg);
        UpdateEntity(pExistingEntity, pExistingEntity->GetReplicationCategory(groupIndex));

        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Discover: %s (%s) updated group %d.",
            modelName.c_str(),
            entityToDiscover.ToString().c_str(),
            groupIndex));

        // Treat this discover as an update
        // send notification that entity has changed on next tick
        m_spEntityManager->AddDirty(pExistingEntity);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleEntityUpdate(
    const EntityMessage* pMsg,
    efd::Category targetChannel)
{
    EE_MESSAGE_TRACE(
        pMsg,
        efd::kReplicationService,
        efd::ILogger::kLVL1,
        efd::ILogger::kLVL3,
        ("%s| %s type=0x%08X %s %s",
        pMsg->GetDescription().c_str(),
        __FUNCTION__,
        pMsg->GetClassID(),
        pMsg->GetEntityID().ToString().c_str(),
        targetChannel.ToString().c_str()));

    m_updateMsg++;

    UInt32 groupIndex = pMsg->GetGroupIndex();

    // if here, we have a message for an entity that we've already been told about...
    ReplicationConsumerEntity* pEntityReal = FindConsumerEntity(pMsg->GetEntityID());
    if (pEntityReal)
    {
        // if here, need to update spEntityReal with modified properties in spEntityMod
        // FromStream should overwrite dirty properties with new ones...
        pMsg->ResetForUnpacking();
        pEntityReal->UpdatePropertiesFromStream(pMsg);
        UpdateEntity(pEntityReal, pEntityReal->GetReplicationCategory(groupIndex));

        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Update: %s updated in local cache.",
            pEntityReal->GetEntityID().ToString().c_str()));

        EE_LOG_METRIC_COUNT(kReplicationService, "RECEIVE.ENTITY_UPDATE");

        // send notification that entity has changed on next tick
        m_spEntityManager->AddDirty(pEntityReal);
    }
    else
    {
        // create in progress...
        PendingEntityToDataMap::iterator it = m_pendingEntityCreates.find(pMsg->GetEntityID());
        if (it != m_pendingEntityCreates.end())
        {
            PendingCreateData data;
            data.m_spEntityMsg = pMsg;
            data.m_replicationGroupIndex = groupIndex;
            it->second.push_back(data);

            EE_LOG(efd::kReplicationService, efd::ILogger::kLVL3,
                ("Update queued for %s.",
                pMsg->GetEntityID().ToString().c_str()));

            EE_LOG_METRIC_COUNT(kReplicationService, "RECEIVE.ENTITY_UPDATE");
        }
        else
        {
            EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
                ("%s| Received update for unknown entity %s",
                pMsg->GetDescription().c_str(),
                pMsg->GetEntityID().ToString().c_str()));

            EE_LOG_METRIC_COUNT(kReplicationService, "ENTITY_UPDATE.ERROR");
        }
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleEntityLoss(
    const EntityMessage* pMsg,
    efd::Category targetChannel)
{
    EE_MESSAGE_TRACE(
        pMsg,
        efd::kReplicationService,
        efd::ILogger::kLVL1,
        efd::ILogger::kLVL3,
        ("%s| %s type=0x%08X %s %s",
        pMsg->GetDescription().c_str(),
        __FUNCTION__,
        pMsg->GetClassID(),
        pMsg->GetEntityID().ToString().c_str(),
        targetChannel.ToString().c_str()));

    m_lossMsg++;
    // if here, we have a message for an entity that we've already been told about...
    EE_LOG_METRIC_COUNT(kReplicationService, "RECEIVE.ENTITY_LOSS");

    EntityID entityID = pMsg->GetEntityID();
    UInt32 groupIndex = pMsg->GetGroupIndex();

    // Check to see if we are subscribed to the new Category
    ReplicationConsumerEntity* pEntityReal = FindConsumerEntity(entityID);
    if (!pEntityReal)
    {
        // create in progress...
        PendingEntityToDataMap::iterator it = m_pendingEntityCreates.find(entityID);
        if (it != m_pendingEntityCreates.end())
        {
            PendingCreateData data;
            data.m_spEntityMsg = pMsg;
            data.m_replicationGroupIndex = groupIndex;
            it->second.push_back(data);
        }
        else
        {
            EE_LOG(efd::kReplicationService, efd::ILogger::kLVL1,
                ("%s| Received loss for unknown entity %s",
                pMsg->GetDescription().c_str(), entityID.ToString().c_str()));
        }
    }
    else
    {
        // Losses should always arrive on the category that is being used.
        EE_ASSERT(pEntityReal->GetReplicationCategory(groupIndex) == targetChannel);

        EntityLossUpdateFromMessage(pEntityReal, pMsg);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::EntityLossUpdateFromMessage(
    ReplicationConsumerEntity* pEntityReal,
    const EntityMessage* pMsg)
{
    UInt32 groupIndex = pMsg->GetGroupIndex();
    SequenceNumber32 sequenceNumber = pMsg->GetSequenceNumber();
    Category newCategory = pMsg->GetCurrentCategory();

    if (!pEntityReal->IsCurrentSequence(groupIndex, sequenceNumber))
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Loss for %s groupIndex=%d newCategory=%s ignored: out of order",
            pEntityReal->GetEntityID().ToString().c_str(),
            groupIndex,
            newCategory.ToString().c_str()));
        return;
    }

    ChangeEntityReplicationCategory(pEntityReal, groupIndex, newCategory);

    if (pEntityReal->ReplicationGroupsEmpty())
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Loss: %s removed from local cache. groupIndex=%d newCategory=%s",
            pEntityReal->GetEntityID().ToString().c_str(),
            groupIndex,
            newCategory.ToString().c_str()));
        RemoveEntity(pEntityReal);
    }
    else
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Loss: %s replication group updated groupIndex=%d newCategory=%s",
            pEntityReal->GetEntityID().ToString().c_str(),
            groupIndex,
            newCategory.ToString().c_str()));
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleViewBehaviorMsg(
    const ViewEventMessage* pMsg,
    efd::Category targetChannel)
{
    EE_ASSERT(pMsg);
    EE_MESSAGE_TRACE(pMsg,
        efd::kReplicationService,
        efd::ILogger::kLVL1,
        efd::ILogger::kLVL3,
        ("%s| ReplicationService::HandleViewBehaviorMsg payload=%s %s",
        pMsg->GetDescription().c_str(),
        IMessage::ClassIDToString(pMsg->GetContentsClassID()).c_str(),
        targetChannel.ToString().c_str()));

    const EventMessage* pEvent = pMsg->GetContents(m_spMessageService);
    EE_ASSERT(pEvent);

    switch (pMsg->GetDeliveryType())
    {
    case ViewEventMessage::kDT_Entity:
        QueueBehaviorForReplicatedEntity(pMsg->GetTargetEntityID(), pEvent);
        break;

    case ViewEventMessage::kDT_All:
    case ViewEventMessage::kDT_Model:
        // DT32381 We could implement convenience methods that delegate the view behavior from
        // the source entity to various other entities.
        break;

    default:
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR1,
            ("%s| Message contains invalid delivery type: %d",
            pMsg->GetDescription().c_str(), pMsg->GetDeliveryType()));
        break;
    }
}


//------------------------------------------------------------------------------------------------
void ReplicationService::QueueBehaviorForReplicatedEntity(
    egf::EntityID entityID,
    const EventMessage* pEvent)
{
    EE_ASSERT(m_pScheduler);
    if (!m_pScheduler)
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR2,
            ("%s| Entity %s cannot run event because scheduler is not found",
            pEvent->GetDescription().c_str(), entityID.ToString().c_str()));
        return;
    }

    Entity* pEntity = m_spEntityManager->LookupEntity(entityID);
    if (pEntity)
    {
        m_pScheduler->ProcessEventMessage(pEntity, pEvent, true);
    }
    else
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("%s| Entity %s not in local cache, cannot run view event",
            pEvent->GetDescription().c_str(), entityID.ToString().c_str()));
    }
}


//------------------------------------------------------------------------------------------------
void ReplicationService::HandleOwnedEntityAdded(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);
    EE_ASSERT(pEntity->IsOwned());
    ReplicationProducerEntity* pRepEnt = EE_DYNAMIC_CAST(ReplicationProducerEntity, pEntity);
    if (pRepEnt)
    {
        // In response to calling SetReplicationService we will get ChangeReplicationGroupCategory
        // calls for any categories that entity was using.
        pRepEnt->SetReplicationService(this);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleOwnedEntityRemoved(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);
    EE_ASSERT(pEntity->IsOwned());

    ReplicationProducerEntity* pRepEnt = EE_DYNAMIC_CAST(ReplicationProducerEntity, pEntity);
    if (pRepEnt)
    {
        // In response to calling SetReplicationService we will get ChangeReplicationGroupCategory
        // calls for any categories that entity was using.
        pRepEnt->SetReplicationService(NULL);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::SetDirty(ReplicationProducerEntity* pEntity, efd::UInt32 groups)
{
    m_entityChanges[pEntity].OnDirty(groups);
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ClearDirty(ReplicationProducerEntity* pEntity, efd::UInt32 groupIndex)
{
    DirtyReplicatorsMap::iterator iter = m_entityChanges.find(pEntity);
    if (iter != m_entityChanges.end())
    {
        iter->second.ClearDirtyGroup(groupIndex);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleNetIDAssigned(const efd::AssignNetIDMessage*, efd::Category)
{
    // Create private category for this instance of ReplicationService
    if (m_myPrivateCategory.IsValid())
    {
        m_spMessageService->Unsubscribe(this, m_myPrivateCategory);
        m_subscriptionMap.erase(m_myPrivateCategory);
        m_categoryToEntityListMap.erase(m_myPrivateCategory);
    }
    m_myPrivateCategory =
        m_spMessageService->GetServicePrivateCategory(ReplicationService::CLASS_ID);
    m_spMessageService->Subscribe(this, m_myPrivateCategory);

    EE_LOG(efd::kReplicationService, efd::ILogger::kLVL1,
        ("ReplicationService::HandleNetIDAssigned: Using Private Category %s",
        m_myPrivateCategory.ToString().c_str()));

    // REVIEW: Why do we add ourself to the subscriptions list?
    SubscribeReplicationChannel(m_myPrivateCategory);

    // I just joined a channel manager, request discoveries from all producers since there may now
    // be new remote producers of the same categories.
    efd::map< efd::Category, efd::UInt32 >::iterator it = m_subscriptionMap.begin();
    for (; it != m_subscriptionMap.end(); ++it)
    {
        if (it->first != m_myPrivateCategory)
        {
            // Request discovery messages for this channel to be sent directly to the client:
            DiscoveryRequestPtr spRequestMsg = EE_NEW DiscoveryRequest();
            spRequestMsg->SetSenderID(m_myPrivateCategory);
            // Send this request to everyone who produces onto the replication channel:
            EE_ASSERT(m_spMessageService);
            m_spMessageService->ProducerSend(spRequestMsg, it->first);
        }
    }
}

//------------------------------------------------------------------------------------------------
bool ReplicationService::SendDiscoveryMessage(
    ReplicationProducerEntity* pEntity,
    efd::Category targetCategory,
    efd::Category updateGroupCategory)
{
    EE_ASSERT(pEntity);

    bool sent = false;

    for (UInt32 replicationGroup = 0;
        replicationGroup <= PropertyDescriptor::MAX_REPLICATION_GROUP_INDEX;
        ++replicationGroup)
    {
        Category result = pEntity->GetReplicationCategory(replicationGroup);
        if (result == updateGroupCategory)
        {
            IReplicationGroupPolicy* pUpdatePolicy =
                IReplicationGroupPolicy::GetReplicationGroupPolicy(replicationGroup);
            pUpdatePolicy->GenerateP2PDiscovery(
                m_pServiceManager,
                pEntity,
                replicationGroup,
                targetCategory,
                QOS_RELIABLE);

            sent = true;
        }
    }
    return sent;
}

//------------------------------------------------------------------------------------------------
void ReplicationService::UpdateDirtyReplicationGroups(
    ReplicationProducerEntity* pEntity,
    UInt32 dirtyGroups)
{
    for (UInt32 replicationGroup = 0;
        replicationGroup <= PropertyDescriptor::MAX_REPLICATION_GROUP_INDEX;
        ++replicationGroup)
    {
        if (BitUtils::TestBitByIndex(dirtyGroups, replicationGroup))
        {
            IReplicationGroupPolicy* pPolicy =
                IReplicationGroupPolicy::GetReplicationGroupPolicy(replicationGroup);
            pPolicy->GenerateUpdate(m_pServiceManager, pEntity, replicationGroup);

            dirtyGroups = BitUtils::ClearBitByIndex(dirtyGroups, replicationGroup);
            if (0 == dirtyGroups)
            {
                // When all the groups that were dirty are handled then we can early out
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleDiscoveryRequestMsg(
    const eon::DiscoveryRequest* pMsg,
    efd::Category targetChannel)
{
    EE_LOG_METRIC_COUNT(kReplicationService, "RECEIVE.DISCOVER_REQUEST");

    const efd::Category& senderID = pMsg->GetSenderID();
    if (senderID == m_myPrivateCategory)
    {
        // no need to send discovery requests to self
        return;
    }

    // if here, we are responding to a broadcast request from a ReplicationService on another
    // process. The other service just joined a replication channel and needs to discover any
    // entities using that category. Send him discovery msgs for everyone we own.
    int entityCnt = 0;
    int msgcnt = 0;

    for (UInt32 i = 0; i < k_MAX_REPLICATION_GROUPS; ++i)
    {
        IReplicationGroupPolicy* pPolicy = IReplicationGroupPolicy::GetReplicationGroupPolicy(i);
        if (pPolicy)
        {
            pPolicy->BeginP2PUpdate();
        }
    }

    egf::Entity* pEntity;
    EntityManager::EntityMap::const_iterator eePos = m_spEntityManager->GetFirstEntityPos();
    while (m_spEntityManager->GetNextEntity(eePos, pEntity))
    {
        ReplicationProducerEntity* pReplicationProducerEntity =
            EE_DYNAMIC_CAST(ReplicationProducerEntity, pEntity);

        // Make sure we have the correct kind of entity (ReplicationProducerEntity)
        // and that Entity is done being constructed (which we can tell by it having
        // m_pReplicationService set).
        if (pReplicationProducerEntity && pReplicationProducerEntity->m_pReplicationService)
        {
            // All ReplicationProducerEntity instances should be owned, but not all owned Entities
            // are necessarily ReplicationProducerEntity.
            EE_ASSERT(pEntity->IsOwned());
            ++entityCnt;
            // Send a point to point discovery request that does not bump the sequence number
            if (SendDiscoveryMessage(pReplicationProducerEntity, senderID, targetChannel))
            {
                msgcnt++;       // inc. number of messages we sent
            }
        }
    }

    for (UInt32 i = 0; i < k_MAX_REPLICATION_GROUPS; ++i)
    {
        IReplicationGroupPolicy* pPolicy = IReplicationGroupPolicy::GetReplicationGroupPolicy(i);
        if (pPolicy)
        {
            pPolicy->EndP2PUpdate();
        }
    }

    EE_LOG(efd::kReplicationService, efd::ILogger::kLVL1,
        ("Request for entity discover msgs received from %s. Replied with %d of %d "
        "ReplicationProducerEntity instances.",
        senderID.ToString().c_str(),
        msgcnt,
        entityCnt));
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ChangeReplicationGroupCategory(
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex,
    efd::Category oldCat,
    efd::Category newCat)
{
    m_entityChanges[pEntity].OnChangeCategory(groupIndex, oldCat, newCat);
}

//------------------------------------------------------------------------------------------------
bool ReplicationService::GetNextEntity(
    EntityManager::EntityMap::const_iterator& io_iter,
    ReplicationConsumerEntity*& o_pEntity) const
{
    EE_ASSERT(m_spEntityManager);
    o_pEntity = NULL;
    egf::Entity* pEntity;
    while (m_spEntityManager->GetNextEntity(io_iter, pEntity))
    {
        o_pEntity = EE_DYNAMIC_CAST(ReplicationConsumerEntity, pEntity);
        if (o_pEntity)
            return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
ReplicationService::CategoryChange::CategoryChange(efd::Category oldCat, efd::Category newCat)
: m_old(oldCat)
, m_new(newCat)
{
}

//------------------------------------------------------------------------------------------------
ReplicationService::ReplicationAction::ReplicationAction()
    : m_dirty(0)
{
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ReplicationAction::OnDirty(efd::UInt32 groups)
{
    m_dirty |= groups;
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ReplicationAction::OnChangeCategory(
    efd::UInt32 group,
    efd::Category oldCat,
    efd::Category newCat)
{
    CategoryChangesMap::iterator iter = m_catChanges.find(group);
    if (iter != m_catChanges.end())
    {
        // If I have an existing update that means this group changed multiple times
        // during the same tick. I need to keep the original "old category" and the
        // final "new category" but I can ignore any other categories since I never
        // sent any discover or loss on those anyway.
        iter->second.m_new = newCat;
    }
    else
    {
        // new update, add an entry with the given old and new values:
        m_catChanges.insert(
            CategoryChangesMap::value_type(group, CategoryChange(oldCat, newCat)));
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ReplicationAction::ClearDirtyGroup(efd::UInt32 groupIndex)
{
    m_dirty = BitUtils::ClearBitByIndex(m_dirty, groupIndex);
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ReplicationAction::ClearAllDirtyGroups()
{
    m_dirty = 0;
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ReplicationAction::ResetCategoryData()
{
    m_catChanges.clear();
}

//------------------------------------------------------------------------------------------------
void ReplicationService::HandleAssetLoadResponse(
    const efd::AssetLoadResponse* pResponse,
    efd::Category targetChannel)
{
    if (targetChannel != m_myPrivateCategory)
    {
        // We send our entity create message out on our private category. If we get a response
        // on another category it's not something we care about.
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR1,
            ("Discarding HandleAssetLoadResponse, why do we do this?"));
        return;
    }

    if (pResponse->GetResult() != AssetLoadResponse::ALR_Success)
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kERR1,
            ("Discover: %s failed to create entity (bad model name? '%s'), "
            "discovery ignored.",
            pResponse->GetURN().c_str(),
            pResponse->GetAssetPath().c_str()));

        return;
    }

    const EntityFactoryResponse* pMsg = EE_DYNAMIC_CAST(EntityFactoryResponse, pResponse);
    EE_ASSERT(pMsg);

    EntityPtr spNewEntity = pMsg->GetEntity();
    EE_ASSERT(spNewEntity);

    ReplicationConsumerEntity* pReplicant =
        EE_DYNAMIC_CAST(ReplicationConsumerEntity, spNewEntity);
    EE_ASSERT(pReplicant);

    ApplyReplicationMessages(pReplicant);
}

//------------------------------------------------------------------------------------------------
bool ReplicationService::ApplyReplicationMessages(ReplicationConsumerEntity* pReplicant)
{
    EE_ASSERT(pReplicant);

    PendingEntityToDataMap::iterator it = m_pendingEntityCreates.find(pReplicant->GetEntityID());

    // Should not get a factory response for something we didn't ask for.
    if (it != m_pendingEntityCreates.end())
    {
        efd::list<PendingCreateData>& createDataList = it->second;
        for (efd::list<PendingCreateData>::iterator dataIt = createDataList.begin();
            dataIt != createDataList.end();
            ++dataIt)
        {
            ClassID msgClassId = dataIt->m_spEntityMsg->GetClassID();

            // if this is a discovery or update message, update properties from stream.
            if (msgClassId == kMSGID_EntityDiscoveryMessage ||
                msgClassId == kMSGID_EntityUpdateMessage)
            {
                pReplicant->UpdatePropertiesFromStream(dataIt->m_spEntityMsg);

                UpdateEntity(
                    pReplicant,
                    pReplicant->GetReplicationCategory(dataIt->m_replicationGroupIndex));
            }
            // Loss message, EntityLossUpdateFromMessage will call RemoveEntity if necessary
            else if (msgClassId == kMSGID_EntityLossMessage)
            {
                EntityLossUpdateFromMessage(pReplicant, dataIt->m_spEntityMsg);
            }
            else
            {
                // unexpected message
                EE_ASSERT(false);
            }
        }

        m_pendingEntityCreates.erase(it);
    }

    // If we got both discover and loss messages the end result might be that we are totally lost
    // in which case we don't want to add this entity to the EntityManager. If we have in-use
    // replication groups then we have not been lost.
    if (!pReplicant->ReplicationGroupsEmpty())
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Discover: %s of type '%s' added to local cache.",
            pReplicant->GetEntityID().ToString().c_str(),
            pReplicant->GetModelName().c_str()));
        return true;
    }
    else
    {
        EE_LOG(efd::kReplicationService, efd::ILogger::kLVL2,
            ("Entity %s of type '%s' lost during async creation.",
            pReplicant->GetEntityID().ToString().c_str(),
            pReplicant->GetModelName().c_str()));
        return false;
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::ChangeEntityReplicationCategory(
    ReplicationConsumerEntity* pEntityReal,
    UInt32 groupIndex,
    Category newCategory)
{
    // Remove the entity from the EnitityList for old category.
    CategoryToEntityListMap::iterator foundIter =
        m_categoryToEntityListMap.find(pEntityReal->GetReplicationCategory(groupIndex));
    if (foundIter != m_categoryToEntityListMap.end())
    {
        foundIter->second->erase(pEntityReal);
    }

    // If the new category is valid but I'm not subscribed to that channel then for our purposes
    // this still counts as a loss. So only update to replication groups I know about.
    if (m_subscriptionMap.find(newCategory) != m_subscriptionMap.end())
    {
        pEntityReal->SetReplicationCategory(groupIndex, newCategory);
    }
    else
    {
        // we are not listening to the new Category, so the Entity is no longer replicated to
        // this group for us
        pEntityReal->SetReplicationCategory(groupIndex, kCAT_INVALID);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::BeginCategoryProduction(efd::Category newCat, efd::QualityOfService qos)
{
    if (1 == ++(m_productionMap[newCat]))
    {
        m_spMessageService->BeginCategoryProduction(newCat, qos, this);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationService::EndCategoryProduction(efd::Category oldCat)
{
    efd::map<efd::Category, efd::UInt32>::iterator iter = m_productionMap.find(oldCat);
    if (iter != m_productionMap.end())
    {
        if (0 == --(iter->second))
        {
            m_spMessageService->EndCategoryProduction(oldCat, this);
            m_productionMap.erase(iter);
        }
    }
}

