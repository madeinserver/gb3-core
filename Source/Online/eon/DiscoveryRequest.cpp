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

#include <efd/ILogger.h>
#include <efd/Metrics.h>
#include <eon/DiscoveryRequest.h>
#include <efdNetwork/NetCategory.h>
#include <efd/ServiceManager.h>


//------------------------------------------------------------------------------------------------
using namespace efd;
using namespace eon;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(DiscoveryRequest);

//------------------------------------------------------------------------------------------------
DiscoveryRequest::DiscoveryRequest()
    : m_senderID(0)
{
}

//------------------------------------------------------------------------------------------------
/*virtual*/ DiscoveryRequest::~DiscoveryRequest()
{
}

//------------------------------------------------------------------------------------------------
/*virtual*/ void DiscoveryRequest::Serialize(efd::Archive& ar)
{
    IMessage::Serialize(ar);
    Serializer::SerializeObject(m_senderID, ar); // who we are
}

//------------------------------------------------------------------------------------------------
/*virtual*/ Category DiscoveryRequest::GetSenderID() const
{
    return m_senderID;
}

//------------------------------------------------------------------------------------------------
/*virtual*/ void DiscoveryRequest::SetSenderID(efd::Category senderID)
{
    m_senderID = senderID;
}

//------------------------------------------------------------------------------------------------
