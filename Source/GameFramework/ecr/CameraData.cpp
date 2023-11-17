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

#include "ecrPCH.h"

#include "CameraData.h"

#include <egf/EntityManager.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include "RenderService.h"

using namespace egf;
using namespace efd;
using namespace ecr;

//--------------------------------------------------------------------------------------------------
CameraData::CameraData(NiCamera* pCamera, egf::EntityManager* pEntityManager)
    : m_spEntityManager(pEntityManager)
    , m_zoom(1)
{
    m_spCamera = pCamera;
}

//--------------------------------------------------------------------------------------------------
NiCamera* CameraData::GetCamera() const
{
    return m_spCamera;
}

//--------------------------------------------------------------------------------------------------
egf::EntityID CameraData::GetId() const
{
    return m_id;
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetId(egf::EntityID id)
{
    m_id = id;
}

//--------------------------------------------------------------------------------------------------
efd::Point3 CameraData::GetTranslate()
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    efd::Point3 position;
    pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Position, position);

    return position;
}

//--------------------------------------------------------------------------------------------------
efd::Point3 CameraData::GetRotate()
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    efd::Point3 rotation;
    pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Rotation, rotation);

    return rotation;
}

//--------------------------------------------------------------------------------------------------
efd::Float32 CameraData::GetScale()
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    efd::Float32 scale;
    pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Scale, scale);

    return scale;
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetTranslate(efd::Point3 translation)
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    pEntity->SetPropertyValue(kPropertyID_StandardModelLibrary_Position, translation);
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetRotate(efd::Point3 rotation)
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    pEntity->SetPropertyValue(kPropertyID_StandardModelLibrary_Rotation, rotation);
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetScale(efd::Float32 scale)
{
    EE_ASSERT(m_spEntityManager);

    Entity* pEntity = m_spEntityManager->LookupEntity(m_id);
    EE_ASSERT(pEntity);

    pEntity->SetPropertyValue(kPropertyID_StandardModelLibrary_Scale, scale);
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetRotate(const NiMatrix3& rotation)
{
    efd::Point3 finalRotation;
    rotation.ToEulerAnglesXYZ(finalRotation.x, finalRotation.y, finalRotation.z);

    finalRotation.x = finalRotation.x * -EE_RADIANS_TO_DEGREES;
    finalRotation.y = finalRotation.y * -EE_RADIANS_TO_DEGREES;
    finalRotation.z = finalRotation.z * -EE_RADIANS_TO_DEGREES;

    SetRotate(finalRotation);
}

//--------------------------------------------------------------------------------------------------
efd::Float32 CameraData::GetZoomFactor()
{
    return m_zoom;
}

//--------------------------------------------------------------------------------------------------
void CameraData::SetZoomFactor(efd::Float32 zoom)
{
    m_zoom = zoom;
}

//--------------------------------------------------------------------------------------------------