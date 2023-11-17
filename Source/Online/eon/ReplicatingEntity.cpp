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
#include <eon/OnlineEntity.h>
#include <eon/ReplicationService.h>
#include <eon/ViewEventMessage.h>


//------------------------------------------------------------------------------------------------
#include <eon/eonSDM.h>

// Note: This inserts eon into the static data manager chain.  It must be placed in a
// compilation unit that is always used when the library is used (if it is only placed in
// eonSDM.cpp, it will be removed by the linker)
static eonSDM eonSDMObject;


//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace egf;
using namespace eon;


//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ReplicatingEntity);

//------------------------------------------------------------------------------------------------
ReplicatingEntity::ReplicatingEntity(
    const egf::FlatModel* i_pTemplate,
    egf::EntityID i_eid,
    bool i_createAsMaster)
    : Entity(i_pTemplate, i_eid, i_createAsMaster)
    , m_pReplicationService(NULL)
{
}

//------------------------------------------------------------------------------------------------
void ReplicatingEntity::SetReplicationService(eon::ReplicationService* pRS)
{
    m_pReplicationService = pRS;
}

//------------------------------------------------------------------------------------------------
efd::Category ReplicatingEntity::GetReplicationCategory(efd::UInt32 groupIndex) const
{
    Category result = kCAT_INVALID;
    m_replicationSettings.find(groupIndex, result);
    return result;
}

//------------------------------------------------------------------------------------------------
efd::Bool ReplicatingEntity::UsesReplicationCategory(efd::Category cat) const
{
    for (ReplicationCategoryData::const_iterator iter = m_replicationSettings.begin();
          iter != m_replicationSettings.end();
          ++iter)
    {
        if (iter->second == cat)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------
bool ReplicatingEntity::SendViewEvent(
    efd::Category eventChannel,
    egf::EntityID entityID,
    egf::FlatModelID mixinModelID,
    egf::FlatModelID invokeModelID,
    egf::BehaviorID behaviorID,
    efd::ParameterList* pParams,
    efd::TimeType delay)
{
    EventMessagePtr spEvent = EventMessage::CreateEvent(
        entityID,
        mixinModelID,
        invokeModelID,
        behaviorID,
        delay,
        pParams,
        false); // If we have a callback then we need a reply

    ViewEventMessagePtr spViewEvent = EE_NEW ViewEventMessage(spEvent, eventChannel);
    spViewEvent->SetTargetEntity(entityID);

    if (spEvent && spViewEvent)
    {
        // Grab a pointer to the net service so we can send the event
        MessageServicePtr spMessageService =
            GetServiceManager()->GetSystemServiceAs<MessageService>();
        EE_ASSERT(spMessageService);

        // Send the event
        // QOS_INVALID is passed in here to use the QualityOfService already associated with
        // eventChannel
        spMessageService->Send(spViewEvent, eventChannel, QOS_INVALID);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ReplicatingEntity::SendViewEvent(
    efd::Category eventChannel,
    egf::EntityID entityID,
    const char* strMixinModel,
    const char* strBehavior,
    efd::ParameterList* pParams,
    efd::TimeType delay)
{
    EventMessagePtr spEvent =
        CreateEventMessage(entityID, strMixinModel, strBehavior, pParams, NULL, delay, false);

    ViewEventMessagePtr spViewEvent = EE_NEW ViewEventMessage(spEvent, eventChannel);
    spViewEvent->SetTargetEntity(entityID);

    if (spEvent && spViewEvent)
    {
        // Grab a pointer to the net service so we can send the event
        MessageServicePtr spMessageService =
            GetServiceManager()->GetSystemServiceAs<MessageService>();
        EE_ASSERT(spMessageService);

        // Send the event
        // QOS_INVALID is passed in here to use the QualityOfService already associated with
        // eventChannel
        spMessageService->Send(spViewEvent, eventChannel, QOS_INVALID);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
const char* ReplicatingEntity::GetLifecycleName(efd::UInt32 lifecycle)
{
    switch (lifecycle)
    {
    case lifecycle_OnBeginMigration:
        return "OnBeginMigration";

    case lifecycle_OnEndMigration:
        return "OnEndMigration";
    }

    return Entity::GetLifecycleName(lifecycle);
}

