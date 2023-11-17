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

#include "egmToolServicesPCH.h"
#include "ToolServicesMessages.h"
#include <egf/egfBaseIDs.h>
#include <efd/SerializeRoutines.h>

using namespace egmToolServices;

//------------------------------------------------------------------------------------------------
// Message categories
//------------------------------------------------------------------------------------------------
const efd::Category ToolMessagesConstants::ms_fromFrameworkCategory =
    efd::Category(efd::UniversalID::ECU_Any, efd::kNetID_Any, efd::kBASEID_FromFramework);
const efd::Category ToolMessagesConstants::ms_fromExternalFrameworkCategory =
    efd::Category(efd::UniversalID::ECU_Any, efd::kNetID_Any, efd::kBASEID_FromExternalFramework);

//------------------------------------------------------------------------------------------------
// AddEntityMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(AddEntityMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr AddEntityMessage::FactoryMethod()
{
    return EE_NEW AddEntityMessage();
}

//------------------------------------------------------------------------------------------------
void AddEntityMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_entityId, ar);
    efd::Serializer::SerializeObject(m_modelName, ar);

    efd::SR_StdList<efd::SR_Allocate<efd::SR_ExternalAllocator> >::Serialize(m_overrides, ar);
}

//------------------------------------------------------------------------------------------------
void AddEntityMessage::OverrideEntry::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(PropertyName, ar);
    efd::Serializer::SerializeObject(PropertyValue, ar);
}


//------------------------------------------------------------------------------------------------
// RemoveEntityMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(RemoveEntityMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr RemoveEntityMessage::FactoryMethod()
{
    return EE_NEW RemoveEntityMessage();
}

//------------------------------------------------------------------------------------------------
void RemoveEntityMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_entityId, ar);
}


//------------------------------------------------------------------------------------------------
// SetEntityPropertyMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SetEntityPropertyMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SetEntityPropertyMessage::FactoryMethod()
{
    return EE_NEW SetEntityPropertyMessage();
}

//------------------------------------------------------------------------------------------------
void SetEntityPropertyMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_entityId, ar);
    efd::Serializer::SerializeObject(m_propertyName, ar);
    efd::Serializer::SerializeObject(m_propertyValue, ar);
}



//------------------------------------------------------------------------------------------------
// SelectEntitiesMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SelectEntitiesMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SelectEntitiesMessage::FactoryMethod()
{
    return EE_NEW SelectEntitiesMessage();
}



//------------------------------------------------------------------------------------------------
// SetEntitiesPropertiesMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SetEntitiesPropertiesMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SetEntitiesPropertiesMessage::FactoryMethod()
{
    return EE_NEW SetEntitiesPropertiesMessage();
}

//------------------------------------------------------------------------------------------------
void SetEntitiesPropertiesMessage::Serialize(efd::Archive& ar)
{
    EE_UNUSED_ARG(ar);
    EE_ASSERT(false);

    efd::IMessage::Serialize(ar);
    //efd::SR_StdList<>::Serialize(m_overrides, ar);
}

//------------------------------------------------------------------------------------------------
void SetEntitiesPropertiesMessage::OverrideEntry::Serialize(efd::Archive& ar)
{
    EE_UNUSED_ARG(ar);
    EE_ASSERT(false);
    //efd::Serializer::SerializeObject(Entityid, ar);
    //efd::Serializer::SerializeObject(PropertyName, ar);
    //efd::Serializer::SerializeObject(PropertyValue, ar);
    //efd::Serializer::SerializeObject(Key, ar);
}


//------------------------------------------------------------------------------------------------
// SetGizmoBasisMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SetGizmoBasisMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SetGizmoBasisMessage::FactoryMethod()
{
    return EE_NEW SetGizmoBasisMessage();
}

//------------------------------------------------------------------------------------------------
void SetGizmoBasisMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_relativeSpace, ar);
}


//------------------------------------------------------------------------------------------------
// BeginCloneEntityMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(BeginCloneEntityMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr BeginCloneEntityMessage::FactoryMethod()
{
    return EE_NEW BeginCloneEntityMessage();
}


//------------------------------------------------------------------------------------------------
// SettingsUpdateMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SettingsUpdateMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SettingsUpdateMessage::FactoryMethod()
{
    return EE_NEW SettingsUpdateMessage();
}

//------------------------------------------------------------------------------------------------
void SettingsUpdateMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_settingName, ar);
    efd::Serializer::SerializeObject(m_settingValue, ar);
}


//------------------------------------------------------------------------------------------------
// SetActiveCameraMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(egmToolServices::SetActiveCameraMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr SetActiveCameraMessage::FactoryMethod()
{
    return EE_NEW egmToolServices::SetActiveCameraMessage();
}

//------------------------------------------------------------------------------------------------
void SetActiveCameraMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_cameraName, ar);
    efd::Serializer::SerializeObject(m_window, ar);
}


//------------------------------------------------------------------------------------------------
// CameraUpdateMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(CameraUpdateMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr CameraUpdateMessage::FactoryMethod()
{
    return EE_NEW CameraUpdateMessage();
}

//------------------------------------------------------------------------------------------------
void CameraUpdateMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::SR_StdMap<>::Serialize(m_changes, ar);
}


//------------------------------------------------------------------------------------------------
// CreateEntityMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(CreateEntityMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr CreateEntityMessage::FactoryMethod()
{
    return EE_NEW CreateEntityMessage();
}

//------------------------------------------------------------------------------------------------
void CreateEntityMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_modelName, ar);
    efd::Serializer::SerializeObject(m_position, ar);
    efd::Serializer::SerializeObject(m_rotation, ar);
}


//------------------------------------------------------------------------------------------------
// InputActionMessage
//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(InputActionMessage);

//------------------------------------------------------------------------------------------------
efd::IMessagePtr InputActionMessage::FactoryMethod()
{
    return EE_NEW InputActionMessage();
}

//------------------------------------------------------------------------------------------------
void InputActionMessage::Serialize(efd::Archive& ar)
{
    efd::IMessage::Serialize(ar);
    efd::Serializer::SerializeObject(m_name, ar);
    efd::Serializer::SerializeObject(m_active, ar);
}



//--------------------------------------------------------------------------------------------------
// SourcePaintDataMessage
//--------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(SourcePaintDataMessage);

//------------------------------------------------------------------------------------------------
inline SourcePaintDataMessage::SourcePaintDataMessage(efd::UInt32 x, efd::UInt32 y, 
    efd::UInt32 width, efd::UInt32 height, efd::Point2 center, efd::Point2 dimensions)
{
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_center = center;
    m_dimensions = dimensions;
}
//------------------------------------------------------------------------------------------------
inline SourcePaintDataMessage::~SourcePaintDataMessage()
{

}
//------------------------------------------------------------------------------------------------
efd::IBasePtr SourcePaintDataMessage::FactoryMethod()
{
    return EE_NEW SourcePaintDataMessage();
}

//--------------------------------------------------------------------------------------------------
void SourcePaintDataMessage::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(m_x, ar);
    efd::Serializer::SerializeObject(m_y, ar);
    efd::Serializer::SerializeObject(m_width, ar);
    efd::Serializer::SerializeObject(m_height, ar);
    efd::Serializer::SerializeObject(m_center, ar);
    efd::Serializer::SerializeObject(m_dimensions, ar);
}

//--------------------------------------------------------------------------------------------------
// TerrainAssetMigrationRequestMessage
//--------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(TerrainAssetMigrationRequestMessage);

//--------------------------------------------------------------------------------------------------
efd::IMessagePtr TerrainAssetMigrationRequestMessage::FactoryMethod()
{
    return EE_NEW TerrainAssetMigrationRequestMessage();
}

//--------------------------------------------------------------------------------------------------
TerrainAssetMigrationRequestMessage::TerrainAssetMigrationRequestMessage(const efd::ID128& entityId, 
    const efd::utf8string& assetId, const efd::utf8string& assetPath):
    m_entityId(entityId),
    m_assetId(assetId),
    m_assetPath(assetPath)
{
}

//--------------------------------------------------------------------------------------------------
efd::ClassID TerrainAssetMigrationRequestMessage::GetClassID() const
{
    return TerrainAssetMigrationRequestMessage::CLASS_ID;
}

//--------------------------------------------------------------------------------------------------
void TerrainAssetMigrationRequestMessage::Serialize(efd::Archive& ar)
{
    efd::Serializer::SerializeObject(m_entityId, ar);
    efd::Serializer::SerializeObject(m_assetId, ar);
    efd::Serializer::SerializeObject(m_assetPath, ar);
}

//--------------------------------------------------------------------------------------------------
