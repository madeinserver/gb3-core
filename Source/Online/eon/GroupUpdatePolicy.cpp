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

#include <efd/Metrics.h>
#include <eon/GroupUpdatePolicy.h>
#include <efd/MessageService.h>
#include <efd/ServiceManager.h>
#include <eon/ReplicationProducerEntity.h>

using namespace eon;
using namespace efd;
using namespace egf;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(GroupUpdatePolicy);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(GroupUpdatePolicy::GroupUpdatePolicyData);

//------------------------------------------------------------------------------------------------
GroupUpdatePolicy::GroupUpdatePolicy()
{
}

//------------------------------------------------------------------------------------------------
GroupUpdatePolicy::~GroupUpdatePolicy()
{
}

//------------------------------------------------------------------------------------------------
void GroupUpdatePolicy::GenerateDiscovery(
    const efd::ServiceManager* pServiceManager,
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex)
{
    // Indicate when initial discovery occurred so that we won't send out out first update until
    // enough time has passed.
    GroupUpdatePolicyData& data = GetPolicyData(pEntity, groupIndex);
    data.m_lastUpdate = pServiceManager->GetServiceManagerTime();

    EntityMessagePtr spEntityMessage =
        EE_NEW efd::MessageWrapper<EntityMessage, kMSGID_EntityDiscoveryMessage>();
    EE_ASSERT(spEntityMessage);

    Category destinationCategory = pEntity->GetReplicationCategory(groupIndex);
    spEntityMessage->SetCurrentCategory(destinationCategory);

    pEntity->WriteHeaderData(spEntityMessage, groupIndex);
    pEntity->StreamChangedProperties(spEntityMessage->GetArchive(), groupIndex);

    MessageService* pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    pMessageService->SendRemote(
        spEntityMessage,
        destinationCategory,
        GetQualityOfService());

    EE_LOG_METRIC_COUNT(kReplicationService, "SEND.ENTITY_DISCOVER");
}

//------------------------------------------------------------------------------------------------
void GroupUpdatePolicy::GenerateUpdate(
    const efd::ServiceManager* pServiceManager,
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex)
{
    TimeType currentTime = pServiceManager->GetServiceManagerTime();
    GroupUpdatePolicyData& data = GetPolicyData(pEntity, groupIndex);
    if ((currentTime - data.m_lastUpdate) < m_minUpdateDelta)
    {
        return;
    }
    data.m_lastUpdate = currentTime;

    EntityMessagePtr spUpdateMsg;
    METRICS_ONLY(efd::utf8string probeName;)
    if (m_treatUpdatesAsDiscovers)
    {
        spUpdateMsg = EE_NEW efd::MessageWrapper<EntityMessage, kMSGID_EntityDiscoveryMessage>();
        METRICS_ONLY(probeName = "SEND.ENTITY_DISCOVER";)
    }
    else
    {
        spUpdateMsg = EE_NEW efd::MessageWrapper<EntityMessage, kMSGID_EntityUpdateMessage>();
        METRICS_ONLY(probeName = "SEND.ENTITY_UPDATE";)
    }
    EE_ASSERT(spUpdateMsg);

    Category destinationCategory = pEntity->GetReplicationCategory(groupIndex);
    spUpdateMsg->SetCurrentCategory(destinationCategory);

    pEntity->WriteHeaderData(spUpdateMsg, groupIndex);
    if (GetUpdateAll())
    {
        // For small replication groups sent on unreliable channels we can send all properties
        // rather than just the dirty properties.
        pEntity->StreamChangedProperties(spUpdateMsg->GetArchive(), groupIndex);
    }
    else
    {
        pEntity->StreamDirtyProperties(spUpdateMsg->GetArchive(), groupIndex);
    }

    MessageService* pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    pMessageService->SendRemote(
        spUpdateMsg,
        destinationCategory,
        GetQualityOfService());

    EE_LOG_METRIC_COUNT(kReplicationService, probeName.c_str());
}

//------------------------------------------------------------------------------------------------
void GroupUpdatePolicy::GenerateLoss(
    const efd::ServiceManager* pServiceManager,
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex,
    efd::Category oldCategory,
    efd::Category newCategory)
{
    EntityMessagePtr spEntityMessage =
        EE_NEW efd::MessageWrapper<EntityMessage, kMSGID_EntityLossMessage>();
    EE_ASSERT(spEntityMessage);

    // For a loss message we send the new current category that the entity is switching into:
    spEntityMessage->SetCurrentCategory(newCategory);
    pEntity->WriteHeaderData(spEntityMessage, groupIndex);

    MessageService* pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    pMessageService->SendRemote(
        spEntityMessage,
        oldCategory,
        GetQualityOfService());

    EE_LOG_METRIC_COUNT(kReplicationService, "SEND.ENTITY_LOSS");
}

//------------------------------------------------------------------------------------------------
void GroupUpdatePolicy::GenerateP2PDiscovery(
    const efd::ServiceManager* pServiceManager,
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex,
    efd::Category callbackChannel,
    efd::QualityOfService qos)
{
    EntityMessagePtr spEntityMessage =
        EE_NEW efd::MessageWrapper<EntityMessage, kMSGID_EntityDiscoveryMessage>();
    EE_ASSERT(spEntityMessage);

    Category destinationCategory = pEntity->GetReplicationCategory(groupIndex);
    spEntityMessage->SetCurrentCategory(destinationCategory);

    pEntity->WriteHeaderData(spEntityMessage, groupIndex);
    pEntity->StreamChangedProperties(spEntityMessage->GetArchive(), groupIndex, false);

    MessageService* pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    pMessageService->SendRemote(spEntityMessage, callbackChannel, qos);

    EE_LOG_METRIC_COUNT(kReplicationService, "SEND.ENTITY_DISCOVER");
}

//------------------------------------------------------------------------------------------------
GroupUpdatePolicy::GroupUpdatePolicyData::GroupUpdatePolicyData()
: m_lastUpdate(0.0)
{
}

//------------------------------------------------------------------------------------------------
GroupUpdatePolicy::GroupUpdatePolicyData& GroupUpdatePolicy::GetPolicyData(
    ReplicationProducerEntity* pEntity,
    efd::UInt32 groupIndex)
{
    GroupUpdatePolicyData* pResult;
    IReplicationGroupPolicyData* pData = pEntity->GetReplicationGroupPolicyData(groupIndex);
    if (!pData)
    {
        pResult = EE_NEW GroupUpdatePolicyData();
        pEntity->SetReplicationGroupPolicyData(groupIndex, pResult);
    }
    else
    {
        pResult = EE_DYNAMIC_CAST(GroupUpdatePolicyData, pData);
        EE_ASSERT(pResult);
    }
    return *pResult;
}

//------------------------------------------------------------------------------------------------
