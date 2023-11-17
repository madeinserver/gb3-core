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

#include <eon/OnlineEntity.h>
#include <eon/eonLogIDs.h>
#include <efd/Metrics.h>

//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace egf;
using namespace eon;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ReplicationConsumerEntity);

//------------------------------------------------------------------------------------------------
ReplicationConsumerEntity::ReplicationConsumerEntity(
    const egf::FlatModel* i_pTemplate,
    egf::EntityID i_eid)
    : ReplicatingEntity(i_pTemplate, i_eid, false)
    , m_activeReplicationGroups(0)
    , m_owningProcess(kNetID_Unassigned)
{
}

//------------------------------------------------------------------------------------------------
void ReplicationConsumerEntity::OnAdded(ParameterList* pDataStream)
{
    // We need to call the parent function since it does important setup work
    ReplicatingEntity::OnAdded(pDataStream);
    _OnLifecycleEvent(lifecycle_OnDiscovery);
}

//------------------------------------------------------------------------------------------------
void ReplicationConsumerEntity::Destroy()
{
    // Don't call the parent method, all it does is launch the wrong behavior

    // tick off replica entity destroyed by model ID
    EE_LOG_METRIC_COUNT_FMT(kEntity, ("DESTROY.REPLICA.%s", GetModelName().c_str()));

    _OnLifecycleEvent(lifecycle_OnFinalLoss);
}

//------------------------------------------------------------------------------------------------
void ReplicationConsumerEntity::OnAssetsLoaded()
{
    // Don't call the parent method, all it does is launch the wrong behavior
    _OnLifecycleEvent(lifecycle_OnReplicaAssetsLoaded);
}

//------------------------------------------------------------------------------------------------
bool ReplicationConsumerEntity::UpdatePropertiesFromStream(const EntityMessage* pEntityMessage)
{
    // First, input all of our entity specific data
    EntityID eid = pEntityMessage->GetEntityID();
    EE_ASSERT(eid == GetEntityID());

    utf8string modelName = pEntityMessage->GetModelName();
    EE_ASSERT(modelName == GetModelName());

    UInt32 groupIndex = pEntityMessage->GetGroupIndex();

    efd::SequenceNumber32 sequenceNumber = pEntityMessage->GetSequenceNumber();

    efd::SequenceNumber32 previousUpdateSequence = GetSequenceNumberFromIndex(groupIndex);

    EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kLVL2,
        ("UpdatePropertiesFromStream: %s (%s) group='%d' prevSequence='%d' sequence='%d' cat='%s'",
        eid.ToString().c_str(),
        modelName.c_str(),
        groupIndex,
        previousUpdateSequence.GetValue(),
        sequenceNumber.GetValue(),
        pEntityMessage->GetCurrentCategory().ToString().c_str()));

    if (previousUpdateSequence >= sequenceNumber)
    {
        // We have already received a more recent update for this exact same group!  That means
        // we can ignore this entire update.  In theory this can happen if the updates are sent
        // over a reliable but unordered quality of service.

        // Of course we should never receive exactly the same update twice, so it should never
        // truly be an equal sequence number.  Review: this would not be true if someone chooses
        // to use raw UDP with send-multiple-time "reliability" for a replication channel.

        // Since this is an expected case its not really an error, so return true.
        EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kLVL3,
            ("UpdatePropertiesFromStream: skipping, old sequence."));
        return true;
    }

    // Update the NetID if it has been set.
    UInt32 netID = pEntityMessage->GetOwningProcessID();
    if (netID != kNetID_Unassigned)
    {
        m_owningProcess = netID;
    }

    Archive& ar = pEntityMessage->GetArchive();

    // get number of properties in the stream
    UInt32 propCount;
    Serializer::SerializeObject(propCount, ar);

    // now iterate through the streamed properties and stream them in
    for (UInt32 ii = propCount; ii > 0; ii--)
    {
        PropertyID propID;
        Serializer::SerializeObject(propID, ar);

        const PropertyDescriptor* ppd = m_pFlatModel->GetPropertyDescriptor(propID);
        if (!ppd)
        {
            // Error! Unable to clone specified property from our specified model
            EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kERR1,
                ("Error: Invalid property (%d) in model %s when streaming in entity",
                propID, modelName.c_str()));

            return false;
        }

        // A property might be updated by multiple groups.  So for each property we need to
        // consider every group that property is in to find the group with the highest sequence
        // number.  This is the sequence number for that property.
        if (GetSequenceNumberFromMask(ppd->GetReplicationGroups()) < sequenceNumber)
        {
            // Update the property!  We use GetPropertyForWriting which will fetch either an
            // IBuiltinModel or an IProperty as needed.  It will create these if they do not
            // already exist.
            IProperty* pProp = GetPropertyForWriting(ppd);

            pProp->SerializeProperty(propID, ar);
            if (!ar.GetError())
            {
                EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kLVL1,
                    ("FromStream read %s (%s) propID=0x%08X name %s",
                    eid.ToString().c_str(),
                    modelName.c_str(),
                    propID,
                    ppd->GetName().c_str()));

                // Set the property as being locally dirty.  This will send out a local "entity
                // updated" message just as if this were any other type of entity property change.
                SetDirty(propID, pProp);
            }
            else
            {
                // NOTE: A side effect of calling GetPropertyForWriting might have been the
                // creation of a new IProperty instance which has already been added to our
                // property map, but we then failed to update that IProperty!  This is OK, it
                // just means we have something in our property map that is still set to the
                // default value, but its still correct data.
                EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kERR1,
                    ("Error: stream property (%d) in model %s failed",
                    propID, modelName.c_str()));
                return false;
            }
        }
        else
        {
            // Skip updating this property and continue checking the result.
            const IProperty* pProp = GetPropertyForReading(ppd);
            pProp->AdvanceStream(propID, ar);
            if (!ar.GetError())
            {
                EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kLVL3,
                    ("Property update skipping propID=0x%08X name='%s' in %s",
                    propID,
                    ppd->GetName().c_str(),
                    GetEntityID().ToString().c_str()));
            }
            else
            {
                EE_LOG(efd::kReplicationConsumerEntity, efd::ILogger::kERR1,
                    ("Error: skip property (%d) in model %s failed",
                    propID, modelName.c_str()));
                return false;
            }
        }
    }

    Category updateCategory = pEntityMessage->GetCurrentCategory();

    // We have gotten an update/discovery about this group and it has a valid sequence therefore
    // that group is now in active use.
    if (k_INVALID_GROUP_ID != groupIndex)
    {
        m_activeReplicationGroups = BitUtils::SetBitByIndex(m_activeReplicationGroups, groupIndex);

        // we also store the channel that this replication came across.  Note that we set the value
        // directly rather than calling SetReplicationCategory because in this case we don't want
        // to filter based on the flat model.  The fact that we have received this update proves
        // that it's valid from the sender's perspective and if we're on the client our flat
        // model won't necessarily preserve the server-side replication group settings.
        m_replicationSettings[groupIndex] = updateCategory;
    }

    // Now we update the sequence numbers:
    SetSequenceNumber(groupIndex, sequenceNumber);

    return true;
}

//------------------------------------------------------------------------------------------------
bool ReplicationConsumerEntity::ReplicationGroupsEmpty()
{
    return (m_activeReplicationGroups == 0);
}

//------------------------------------------------------------------------------------------------
void ReplicationConsumerEntity::RemoveReplicationGroupCategory(efd::Category oldCategory)
{
    for (ReplicationCategoryData::iterator iter = m_replicationSettings.begin();
        iter != m_replicationSettings.end();)
    {
        if (iter->second == oldCategory)
        {
            m_activeReplicationGroups =
                BitUtils::ClearBitByIndex(m_activeReplicationGroups, iter->first);
            ReplicationCategoryData::iterator eraseIter = iter;
            ++iter;
            m_replicationSettings.erase(eraseIter);
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------------------------
bool ReplicationConsumerEntity::SetReplicationCategory(
    efd::UInt32 groupIndex,
    efd::Category cat)
{
    if (groupIndex < k_MAX_REPLICATION_GROUPS && GetModel()->GetReplicationGroup(groupIndex))
    {
        EE_LOG(efd::kReplicationGroups, ILogger::kLVL1,
            ("<Group=%i>: Set %s replication channel to <Channel=%s>",
            groupIndex,
            GetEntityID().ToString().c_str(),
            cat.ToString().c_str()));

        if (cat.IsValid())
        {
            m_activeReplicationGroups =
                BitUtils::SetBitByIndex(m_activeReplicationGroups, groupIndex);

            m_replicationSettings[groupIndex] = cat;
        }
        else
        {
            m_activeReplicationGroups =
                BitUtils::ClearBitByIndex(m_activeReplicationGroups, groupIndex);

            // An invalid category means we are no longer in this replication group
            m_replicationSettings.erase(groupIndex);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
// DT32403 This gets called many times per update.  As such its a likely good candidate for some
// optimization. We could cache the sequence number in a mask->sequence map and then invalidate
// the cache whenever the sequence number for any related group is updated (or just whenever
// m_sequences changes to keep things simple). Need to consider the memory-vs-cpu tradeoff there.
efd::SequenceNumber32 ReplicationConsumerEntity::GetSequenceNumberFromMask(efd::UInt32 groupMask)
{
    efd::SequenceNumber32 lowest;
    for (UInt32 i=0; i< k_MAX_REPLICATION_GROUPS; ++i)
    {
        if (BitUtils::TestBitByIndex(groupMask, i))
        {
            if (m_sequences[i] > lowest)
            {
                lowest = m_sequences[i];
            }
        }
    }
    return lowest;
}

//------------------------------------------------------------------------------------------------
efd::SequenceNumber32 ReplicationConsumerEntity::GetSequenceNumberFromIndex(efd::UInt32 groupIndex)
{
    if (groupIndex < k_MAX_REPLICATION_GROUPS)
    {
        return m_sequences[ groupIndex ];
    }

    // the value k_INVALID_GROUP_ID should be the only value we see outside of the allowed range
    EE_ASSERT(k_INVALID_GROUP_ID == groupIndex);

    // Return a default value
    return efd::SequenceNumber32();
}

//------------------------------------------------------------------------------------------------
efd::Bool ReplicationConsumerEntity::SetSequenceNumber(
    efd::UInt32 groupIndex,
    efd::SequenceNumber32 sequenceNumber)
{
    // NOTE: We treat groupIndex k_INVALID_GROUP_ID as a special value which means "all groups":
    if (k_INVALID_GROUP_ID == groupIndex)
    {
        // REVIEW: we could check the model and only set those sequences that are in use.  If we
        // switch to non-vector storage that would reduce memory and cut down on allocations.
        for (UInt32 i=0; i < k_MAX_REPLICATION_GROUPS; ++i)
        {
            m_sequences[ i ] = sequenceNumber;
        }
    }
    else if (groupIndex < k_MAX_REPLICATION_GROUPS)
    {
        m_sequences[ groupIndex ] = sequenceNumber;
    }
    else
    {
        EE_FAIL("Invalid argument passed to Entity::SetSequenceNumber");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
const char* ReplicationConsumerEntity::GetLifecycleName(efd::UInt32 lifecycle)
{
    switch (lifecycle)
    {
    case lifecycle_OnDiscovery:
        return "OnDiscovery";

    case lifecycle_OnReplicaAssetsLoaded:
        return "OnReplicaAssetsLoaded";

    case lifecycle_OnFinalLoss:
        return "OnFinalLoss";
    }

    return ReplicatingEntity::GetLifecycleName(lifecycle);
}

//------------------------------------------------------------------------------------------------
Entity::LifeCycles ReplicationConsumerEntity::ProcessEndLifecycle(efd::UInt32 lifecycle)
{
    // translate calls from replica only lifecycles to equivalent Entity lifecycles
    switch (lifecycle)
    {
    case lifecycle_OnDiscovery:
        return Entity::ProcessEndLifecycle(lifecycle_OnCreate);
    case lifecycle_OnReplicaAssetsLoaded:
        return Entity::ProcessEndLifecycle(lifecycle_OnAssetsLoaded);
    case lifecycle_OnFinalLoss:
        return Entity::ProcessEndLifecycle(lifecycle_OnDestroy);
    default:
        return Entity::ProcessEndLifecycle(lifecycle);
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationConsumerEntity::HandleMessage(const EventMessage *pMsg, efd::Category targetChannel)
{
    EE_UNUSED_ARG(pMsg);
    EE_UNUSED_ARG(targetChannel);
}

//------------------------------------------------------------------------------------------------
efd::UInt32 ReplicationConsumerEntity::GetOwningProcessID() const
{
    return m_owningProcess;
}
