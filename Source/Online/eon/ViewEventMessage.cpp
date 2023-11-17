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

#include <eon/ViewEventMessage.h>
#include <efd/ILogger.h>
#include <efd/efdLogIDs.h>


using namespace eon;

//------------------------------------------------------------------------------------------------
ViewEventMessage::ViewEventMessage()
{
}

//------------------------------------------------------------------------------------------------
ViewEventMessage::ViewEventMessage(egf::EventMessage* pMessage, const efd::Category& cat)
    : efd::EnvelopeMessage(pMessage, cat)
{
}

//------------------------------------------------------------------------------------------------
void ViewEventMessage::SetTargetEntity(egf::EntityID i_target)
{
    m_deliveryType = kDT_Entity;
    m_targetEntity = i_target;
}

//------------------------------------------------------------------------------------------------
void ViewEventMessage::SetTargetModel(const efd::utf8string& i_target)
{
    m_deliveryType = kDT_Model;
    m_targetModel = i_target;
}

//------------------------------------------------------------------------------------------------
void ViewEventMessage::SetTargetAll()
{
    m_deliveryType = kDT_All;
}

//------------------------------------------------------------------------------------------------
ViewEventMessage::DeliveryType ViewEventMessage::GetDeliveryType() const
{
    return (DeliveryType)m_deliveryType;
}

//------------------------------------------------------------------------------------------------
egf::EntityID ViewEventMessage::GetTargetEntityID() const
{
    return m_targetEntity;
}

//------------------------------------------------------------------------------------------------
const egf::EventMessage* ViewEventMessage::GetContents(efd::MessageFactory* pMessageFactory) const
{
    return (const egf::EventMessage*)efd::EnvelopeMessage::GetContents(pMessageFactory);
}

//------------------------------------------------------------------------------------------------
void ViewEventMessage::SetChild(egf::EventMessage *pMessage)
{
    efd::EnvelopeMessage::SetChild(pMessage);
}

//------------------------------------------------------------------------------------------------
void ViewEventMessage::Serialize(efd::Archive& ar)
{
    efd::EnvelopeMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_deliveryType, ar);
    switch (m_deliveryType)
    {
    case kDT_Entity:
        efd::Serializer::SerializeObject(m_targetEntity, ar);
        break;

    case kDT_Model:
        efd::Serializer::SerializeObject(m_targetModel, ar);
        break;

    case kDT_All:
        break;

    default:
        EE_LOG(efd::kMessage, efd::ILogger::kERR1,
            ("%s failed, invalid type %d", __FUNCTION__, m_deliveryType));
        ar.RaiseError();
        break;
    }
}

//------------------------------------------------------------------------------------------------
