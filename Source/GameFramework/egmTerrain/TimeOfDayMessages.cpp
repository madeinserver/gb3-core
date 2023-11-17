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

#include "egmTerrainPCH.h"
#include "TimeOfDayMessages.h"
#include <efd/DataStreamUtils.h>
#include <efd/Serialize.h>
#include <egf/egfBaseIDs.h>

using namespace egmTerrain;

const efd::Category ToDMessagesConstants::ms_timeOfDayMessageCategory = 
    efd::Category(efd::UniversalID::ECU_Any, efd::kNetID_Any, efd::kBASEID_FromExternalFramework);

//--------------------------------------------------------------------------------------------------
// ToDAnimationChangedMessage
//--------------------------------------------------------------------------------------------------

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToDAnimationChangedMessage);

//--------------------------------------------------------------------------------------------------
efd::IBasePtr ToDAnimationChangedMessage::FactoryMethod()
{
    return EE_NEW ToDAnimationChangedMessage();
}

//--------------------------------------------------------------------------------------------------
void ToDAnimationChangedMessage::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(m_animationRunning, ar);
    efd::Serializer::SerializeObject(m_speedMultiplier, ar);
}


//--------------------------------------------------------------------------------------------------
// ToDTimeChangedMessage
//--------------------------------------------------------------------------------------------------

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToDTimeChangedMessage);

//--------------------------------------------------------------------------------------------------
efd::IBasePtr ToDTimeChangedMessage::FactoryMethod()
{
    return EE_NEW ToDTimeChangedMessage();
}

//--------------------------------------------------------------------------------------------------
void ToDTimeChangedMessage::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(m_currentTime, ar);
}


//--------------------------------------------------------------------------------------------------
// ToDKeyframesChangedMessage
//--------------------------------------------------------------------------------------------------

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToDKeyframesChangedMessage);

//--------------------------------------------------------------------------------------------------
efd::IBasePtr ToDKeyframesChangedMessage::FactoryMethod()
{
    return EE_NEW ToDKeyframesChangedMessage();
}

//--------------------------------------------------------------------------------------------------
void ToDKeyframesChangedMessage::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(m_propertyName, ar);
    efd::Serializer::SerializeObject(m_numberOfKeyframes, ar);

    efd::SR_StdVector<efd::SR_Default>::Serialize(m_keyframesTimes, ar);
    efd::SR_StdVector<efd::SR_Default>::Serialize(m_keyframesValue, ar);
}

//--------------------------------------------------------------------------------------------------