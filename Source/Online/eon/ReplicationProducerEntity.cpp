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
#include <eon/IReplicationGroupPolicy.h>
#include <eon/eonLogIDs.h>

//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace egf;
using namespace eon;


//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ReplicationProducerEntity);

//------------------------------------------------------------------------------------------------
ReplicationProducerEntity::ReplicationProducerEntity(
    const egf::FlatModel* i_pTemplate,
    egf::EntityID i_eid)
    : ReplicatingEntity(i_pTemplate, i_eid, true)
    , m_lastUpdate()
{
}

//------------------------------------------------------------------------------------------------
ReplicationProducerEntity::~ReplicationProducerEntity()
{
    for (GroupUpdatePolicyData::iterator iter = m_groupPolicySettings.begin();
        iter != m_groupPolicySettings.end();
        ++iter)
    {
        EE_DELETE iter->second;
    }
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::SetReplicationService(eon::ReplicationService* pRS)
{
    // We only expect this to be set or unset, not changed between different instances:
    EE_ASSERT(!m_pReplicationService || !pRS || m_pReplicationService == pRS);

    if (m_pReplicationService != pRS)
    {
        if (m_pReplicationService)
        {
            // Leaving the replication service, fake a leave on all in use categories. In response
            // the ReplicationService will call EndCategoryProduction.
            for (ReplicationCategoryData::const_iterator iter = m_replicationSettings.begin();
                iter != m_replicationSettings.end();
                ++iter)
            {
                m_pReplicationService->ChangeReplicationGroupCategory(
                    this,
                    iter->first,
                    iter->second,
                    kCAT_INVALID);
            }
        }

        // If we are unsetting the replication service, clear the dirty flag for all groups.
        if (!pRS)
        {
            for (UInt32 i = 0; i < 32; ++i)
            {
                m_pReplicationService->ClearDirty(this, i);
                ClearReplicationDirty(i);
            }
        }

        ReplicatingEntity::SetReplicationService(pRS);
        if (m_pReplicationService)
        {
            // Joined the replication service, if any of our replication channels have already been
            // set we need to send out initial discovery messages. Even more important, this will
            // cause the ReplicationService to call BeginCategoryProduction which will allow us to
            // receive discovery requests.
            for (ReplicationCategoryData::const_iterator iter = m_replicationSettings.begin();
                iter != m_replicationSettings.end();
                ++iter)
            {
                m_pReplicationService->ChangeReplicationGroupCategory(
                    this,
                    iter->first,
                    kCAT_INVALID,
                    iter->second);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
bool ReplicationProducerEntity::SetReplicationCategory(
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
        Category oldCat;
        m_replicationSettings.find(groupIndex, oldCat);

        if (oldCat != cat)
        {
            if (cat.IsValid())
            {
                m_replicationSettings[groupIndex] = cat;
            }
            else
            {
                // An invalid category means we are no longer in this replication group
                m_replicationSettings.erase(groupIndex);
            }

            if (m_pReplicationService)
            {
                m_pReplicationService->ChangeReplicationGroupCategory(
                    this,
                    groupIndex,
                    oldCat,
                    cat);
            }
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::WriteHeaderData(EntityMessage* pEntityMessage, UInt32 groupIndex)
{
    // First, output all of our entity specific data
    pEntityMessage->SetEntityID(GetEntityID());
    pEntityMessage->SetModelName(GetModelName());

    // Now we need some timestamp information. We send the groupIndex that is being updated and the
    // entities sequence number.
    pEntityMessage->SetGroupIndex(groupIndex);
    pEntityMessage->SetSequenceNumber(GetSequenceNumber());

    // If we are generating the header for a loss message then we will not have a service manager
    // but that's fine because we don't need to specify the owning process in that case anyway.
    const ServiceManager* pServiceManager = GetServiceManager();
    if (pServiceManager)
    {
        MessageService* pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
        EE_ASSERT(pMessageService);
        pEntityMessage->SetOwningProcessID(pMessageService->GetNetID());
    }
}

//------------------------------------------------------------------------------------------------
size_t ReplicationProducerEntity::StreamChangedProperties(
    efd::Archive& ar,
    efd::UInt32 groupIndex,
    bool clearReplicationDirty)
{
    EE_ASSERT(ar.IsPacking());

    size_t initialOffset = ar.GetCurrentPosition();
    UInt32 cPropertiesWritten = 0;
    // Make a window reserving space for writing the count of properties. We will serialize into
    // this window at the end once we know how many properties are written.
    Archive arSizeWindow = ar.MakeWindow(sizeof(cPropertiesWritten));

    egf::Entity::PropertyMap::const_iterator head = m_propertyMap.begin();
    for (; head != m_propertyMap.end(); ++head)
    {
        PropertyID propID = head->first;
        const IProperty* pProp = head->second;
        EE_ASSERT(pProp);

        const PropertyDescriptor* pPropDesc = GetModel()->GetPropertyDescriptor(propID);
        EE_ASSERT(pPropDesc);

        // Is this Update (Replication) Group used by this Property?
        if (pPropDesc->GetUpdateGroup(groupIndex))
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL1,
                ("%s (%s) streaming property '%s' (%d) in group %d cleardirty=%d",
                GetEntityID().ToString().c_str(),
                GetModelName().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex,
                clearReplicationDirty));

            Serializer::SerializeObject(propID, ar);
            pProp->SerializeProperty(propID, ar);
            ++cPropertiesWritten;
        }
    }

    // Now go back and write the count of properties written into the window we reserved:
    Serializer::SerializeObject(cPropertiesWritten, arSizeWindow);

    // Once we generate a replication (discover, update, or even loss) then we are no longer
    // dirty on that particular category:
    EE_ASSERT(m_pReplicationService);
    if (clearReplicationDirty)
    {
        m_pReplicationService->ClearDirty(this, groupIndex);
        ClearReplicationDirty(groupIndex);
    }

    return ar.GetCurrentPosition() - initialOffset;
}

//------------------------------------------------------------------------------------------------
size_t ReplicationProducerEntity::StreamDirtyProperties(
    efd::Archive& ar,
    efd::UInt32 groupIndex,
    bool clearReplicationDirty)
{
    EE_ASSERT(ar.IsPacking());

    size_t initialOffset = ar.GetCurrentPosition();
    UInt32 cPropertiesWritten = 0;
    // Make a window reserving space for writing the count of properties. We will serialize into
    // this window at the end once we know how many properties are written.
    Archive arSizeWindow = ar.MakeWindow(sizeof(cPropertiesWritten));

    for (DirtyReplicationMap::iterator head = m_dirtyPropertyMap.begin();
        head != m_dirtyPropertyMap.end();
        /* do nothing */)
    {
        PropertyID propID = head->first;
        DirtyPropertyData& dpd = head->second;
        const IProperty* pProp = dpd.m_pProp;
        EE_ASSERT(pProp);

        const PropertyDescriptor* pPropDesc = GetModel()->GetPropertyDescriptor(propID);
        EE_UNUSED_ARG(pPropDesc);
        EE_ASSERT(pPropDesc);

        if (BitUtils::TestBitByIndex(dpd.m_dirtyMask, groupIndex))
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL1,
                ("%s (%s) updating property '%s' (0x%08X) to group %d",
                GetEntityID().ToString().c_str(),
                GetModelName().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex));

            Serializer::SerializeObject(propID, ar);
            pProp->SerializeProperty(propID, ar);
            ++cPropertiesWritten;

            if (clearReplicationDirty)
            {
                dpd.m_dirtyMask = BitUtils::ClearBitByIndex(dpd.m_dirtyMask, groupIndex);
            }
        }
        else
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL3,
                ("%s (%s) update skipping property '%s' (0x%08X) in group %d (not dirty in group)",
                GetEntityID().ToString().c_str(),
                GetModelName().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex));
        }

        if (0 == dpd.m_dirtyMask)
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL2,
                ("%s (%s) updating property '%s' (0x%08X) in group %d is now clean",
                GetEntityID().ToString().c_str(),
                GetModelName().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex));

            // property no longer dirty, remove it from dirty map
            DirtyReplicationMap::iterator iterToDelete = head;
            ++head;
            m_dirtyPropertyMap.erase(iterToDelete);
        }
        else
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL3,
                ("%s (%s) updating property '%s' (0x%08X) in group %d is still dirty in 0x%08X",
                GetEntityID().ToString().c_str(),
                GetModelName().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex,
                dpd.m_dirtyMask));

            ++head;
        }
    }

    // Now go back and write the count of properties written into the window we reserved:
    Serializer::SerializeObject(cPropertiesWritten, arSizeWindow);

    // Once we generate a replication (discover, update, or even loss) then we are no longer
    // dirty on that particular category:
    if (clearReplicationDirty)
    {
        EE_ASSERT(m_pReplicationService);
        m_pReplicationService->ClearDirty(this, groupIndex);
    }

    return ar.GetCurrentPosition() - initialOffset;
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::ClearReplicationDirty(efd::UInt32 groupIndex)
{
    for (DirtyReplicationMap::iterator head = m_dirtyPropertyMap.begin();
        head != m_dirtyPropertyMap.end();
        /* do nothing */)
    {
        PropertyID propID = head->first;
        DirtyPropertyData& dpd = head->second;

        const PropertyDescriptor* pPropDesc = GetModel()->GetPropertyDescriptor(propID);
        EE_UNUSED_ARG(pPropDesc);
        EE_ASSERT(pPropDesc);

        dpd.m_dirtyMask = BitUtils::ClearBitByIndex(dpd.m_dirtyMask, groupIndex);

        if (0 == dpd.m_dirtyMask)
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL2,
                ("%s: %s property '%s' (0x%08X) in group %d is now clean",
                __FUNCTION__,
                GetEntityID().ToString().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex));

            // property no longer dirty, remove it from dirty map
            DirtyReplicationMap::iterator iterToDelete = head;
            ++head;
            m_dirtyPropertyMap.erase(iterToDelete);
        }
        else
        {
            EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL3,
                ("%s: %s property '%s' (0x%08X) in group %d is still dirty in 0x%08X",
                __FUNCTION__,
                GetEntityID().ToString().c_str(),
                pPropDesc->GetName().c_str(),
                propID,
                groupIndex,
                dpd.m_dirtyMask));

            ++head;
        }
    }
}

//------------------------------------------------------------------------------------------------
efd::SequenceNumber32 ReplicationProducerEntity::GetSequenceNumber() const
{
    return m_lastUpdate;
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::IncrementSequenceNumber()
{
    ++m_lastUpdate;
}

//------------------------------------------------------------------------------------------------
IReplicationGroupPolicyData* ReplicationProducerEntity::GetReplicationGroupPolicyData(
    efd::UInt32 groupIndex)
{
    IReplicationGroupPolicyData* pPolicyData = NULL;
    m_groupPolicySettings.find(groupIndex, pPolicyData);
    return pPolicyData;
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::SetReplicationGroupPolicyData(
    efd::UInt32 groupIndex,
    IReplicationGroupPolicyData* pGroupUpdatePolicy)
{
    IReplicationGroupPolicyData* pPolicyData = NULL;
    m_groupPolicySettings.find(groupIndex, pPolicyData);
    EE_DELETE pPolicyData;

    m_groupPolicySettings[groupIndex] = pGroupUpdatePolicy;
}

//------------------------------------------------------------------------------------------------
void ReplicationProducerEntity::SetDirty(PropertyID propID, const IProperty* prop)
{
    Entity::SetDirty(propID, prop);

    const PropertyDescriptor* pDesc = GetModel()->GetPropertyDescriptor(propID);
    EE_ASSERT(pDesc);
    efd::UInt32 groups = pDesc->GetUpdateGroups();
    if (groups)
    {
        EE_LOG(efd::kReplicationProducerEntity, efd::ILogger::kLVL2,
            ("%s (%s) dirting property '%s' (0x%08X) in groups 0x%08X, prop=%p, RS=%p",
            GetEntityID().ToString().c_str(),
            GetModelName().c_str(),
            pDesc->GetName().c_str(),
            propID,
            groups,
            prop,
            m_pReplicationService));

        // In rapid iteration cases where the property was removed prop can be NULL. In this case we
        // don't need to generate a replication update for that property (rapid iteration changes
        // are applied to all processes).
        if (prop)
        {
            // propID might already be in the dirty map, but that's ok, we always want to clobber
            // any previous value with the new value.
            DirtyReplicationMap::iterator i = m_dirtyPropertyMap.find(propID);
            if (i != m_dirtyPropertyMap.end())
                m_dirtyPropertyMap.erase(i);

            m_dirtyPropertyMap.insert(
                DirtyReplicationMap::value_type(propID, DirtyPropertyData(prop, groups)));

            if (m_pReplicationService)
            {
                m_pReplicationService->SetDirty(this, groups);
            }
        }
    }
}
