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

#include <eon/EntityMessage.h>
#include <eon/ViewEventMessage.h>
#include <efd/Metrics.h>


//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace egf;
using namespace eon;


//------------------------------------------------------------------------------------------------
// Server message class method implementations
//
EE_IMPLEMENT_CONCRETE_CLASS_INFO(EntityMessage);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ViewEventMessage);

//------------------------------------------------------------------------------------------------
EntityMessage::EntityMessage()
    : m_entityID(0)
    , m_modelName()
    , m_groupIndex(0)
    , m_sequenceNumber(0)
    , m_cat()
    , m_owningProcess(kNetID_Unassigned)
{
}

//------------------------------------------------------------------------------------------------
EntityMessage::~EntityMessage()
{
}

//------------------------------------------------------------------------------------------------
void EntityMessage::Serialize(efd::Archive& ar)
{
    StreamMessage::Serialize(ar);
    Serializer::SerializeObject(m_entityID, ar);
    Serializer::SerializeObject(m_modelName, ar);
    Serializer::SerializeObject(m_groupIndex, ar);
    Serializer::SerializeObject(m_sequenceNumber, ar);
    Serializer::SerializeObject(m_cat, ar);
    Serializer::SerializeObject(m_owningProcess, ar);
}

//------------------------------------------------------------------------------------------------
egf::EntityID EntityMessage::GetEntityID() const
{
    return m_entityID;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetEntityID(egf::EntityID eid)
{
    m_entityID = eid;
}

//------------------------------------------------------------------------------------------------
const efd::utf8string& EntityMessage::GetModelName() const
{
    return m_modelName;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetModelName(const efd::utf8string& modelName)
{
    m_modelName = modelName;
}

//------------------------------------------------------------------------------------------------
efd::SequenceNumber32 EntityMessage::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetSequenceNumber(efd::SequenceNumber32 sequenceNumber)
{
    m_sequenceNumber = sequenceNumber;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 EntityMessage::GetGroupIndex() const
{
    return m_groupIndex;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetGroupIndex(efd::UInt32 groupIndex)
{
    m_groupIndex = groupIndex;
}

//------------------------------------------------------------------------------------------------
efd::Category EntityMessage::GetCurrentCategory() const
{
    return m_cat;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetCurrentCategory(efd::Category cat)
{
    m_cat = cat;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 EntityMessage::GetOwningProcessID() const
{
    return m_owningProcess;
}

//------------------------------------------------------------------------------------------------
void EntityMessage::SetOwningProcessID(efd::UInt32 id)
{
    m_owningProcess = id;
}
