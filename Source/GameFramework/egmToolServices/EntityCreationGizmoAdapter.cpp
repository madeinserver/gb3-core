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
#include "egmToolServicesLibType.h"

#include <egf/StandardModelLibraryPropertyIDs.h>
#include "EntityCreationGizmoAdapter.h"

#include "SelectionService.h"
#include "EntitySelectionAdapter.h"
#include "CreationGizmoPolicy.h"

#include "ToolSnapService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(EntityCreationGizmoAdapter);

//-----------------------------------------------------------------------------------------------
EntityCreationGizmoAdapter::EntityCreationGizmoAdapter(efd::ServiceManager* pServiceManager)
    : CreationGizmoAdapter(pServiceManager)
    , m_positionName("Position")
    , m_rotationName("Rotation")
    , m_scaleName("Scale")
    , m_startingRotation(0, 0, 0)
    , m_spTempEntity(NULL)
    , m_bEntityAdded(false)
    , m_modelName("")
{
    m_pEntityManager = pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);

    m_pSceneGraphService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        pServiceManager->GetSystemServiceAs<SceneGraphService>());
    EE_ASSERT(m_pSceneGraphService);

    m_pGizmoService = pServiceManager->GetSystemServiceAs<GizmoService>();
    EE_ASSERT(m_pGizmoService);

    m_pSelectionService = pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(m_pSelectionService);

    m_pSnapService = pServiceManager->GetSystemServiceAs<ToolSnapService>();
    EE_ASSERT(m_pSnapService);

    m_pFlatModelManager = pServiceManager->GetSystemServiceAs<egf::FlatModelManager>();
    EE_ASSERT(m_pFlatModelManager);

    m_pRenderService = pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);
}

//-----------------------------------------------------------------------------------------------
EntityCreationGizmoAdapter::~EntityCreationGizmoAdapter()
{
    m_spTempEntity = NULL;
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::OnTick()
{
    if (!m_transforming)
    {
        const efd::utf8string& modelName = 
            m_pGizmoService->GetPolicy<CreationGizmoPolicy>()->GetCreationModeModel();

        if (!modelName.empty())
        {
            if (modelName != m_modelName)
            {
                if (m_spTempEntity != NULL)
                {
                    m_pEntityManager->DestroyEntity(m_spTempEntity);
                    m_spTempEntity = NULL;
                    m_bEntityAdded = false;
                }
            }

            if (m_spTempEntity == NULL)
            {
                m_spTempEntity = m_pFlatModelManager->FactoryEntity(modelName);

                m_modelName = modelName;
            }
        }
    }

    if (m_spTempEntity != NULL)
    {
        if (!m_bEntityAdded)
        {
            m_bEntityAdded = m_pEntityManager->AddEntity(m_spTempEntity);
            m_spTempEntity->GetDefaultPropertyValue(kPropertyID_StandardModelLibrary_Rotation,
                m_startingRotation);

            m_pRenderService->InvalidateRenderContexts();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::OnRemoved()
{
    CreationGizmoAdapter::OnRemoved();

    if (m_spTempEntity != NULL)
    {
        if (m_bEntityAdded)
        {
            m_spTempEntity->GetDefaultPropertyValue(kPropertyID_StandardModelLibrary_Rotation,
                m_startingRotation);
        }

        m_pEntityManager->DestroyEntity(m_spTempEntity);
        m_spTempEntity = NULL;
        m_bEntityAdded = false;
    }
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::IsVisible() const
{
    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::IsSelected(NiAVObject* pObject) const
{
    if (m_spTempEntity == NULL)
        return false;

    Entity* pEntity = m_pSceneGraphService->GetEntityFromSceneGraph(pObject);
    return m_spTempEntity == pEntity;
}

//-----------------------------------------------------------------------------------------------
efd::Point3 EntityCreationGizmoAdapter::GetOrigin() const
{
    if (m_spTempEntity == NULL)
        return Point3::ZERO;

    efd::Point3 rotation;
    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

    return rotation;
}

//-----------------------------------------------------------------------------------------------
efd::Matrix3 EntityCreationGizmoAdapter::GetRotation() const
{
    return efd::Matrix3::IDENTITY;
}

//-----------------------------------------------------------------------------------------------
efd::UInt32 EntityCreationGizmoAdapter::GetTargets() const
{
    return 1;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::SnapToPoints(
    efd::UInt32 targetIndex, 
    efd::Point3 translationDelta)
{
    if (m_spTempEntity == NULL)
        return false;

    efd::Point3 position;
    GetTranslation(targetIndex, position);

    if (m_pSnapService->SnapToPoints(m_spTempEntity, position, translationDelta))
    {
        SetTranslation(targetIndex, position);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::CanTranslate(efd::UInt32 targetIndex)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return false;

    efd::Point3 position;
    return m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, 
        position) == PropertyResult_OK;
}

//-----------------------------------------------------------------------------------------------
efd::Point3 EntityCreationGizmoAdapter::GetTranslationStart(efd::UInt32 targetIndex) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return Point3::ZERO;

    efd::Point3 position;
    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);

    return position;
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::GetTranslation(efd::UInt32 targetIndex, efd::Point3& position) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);   
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::SetTranslation(efd::UInt32 targetIndex, const efd::Point3& position)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    m_spTempEntity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::CanRotate(efd::UInt32 targetIndex)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return false;

    efd::Point3 rotation;
    return m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, 
        rotation) == PropertyResult_OK;
}

//-----------------------------------------------------------------------------------------------
efd::Matrix3 EntityCreationGizmoAdapter::GetRotationStart(efd::UInt32 targetIndex) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return Matrix3::IDENTITY;

    efd::Point3 rotation;
    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

    efd::Matrix3 kWorldRotation, kXRot, kYRot, kZRot;

    kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS);
    kWorldRotation = kXRot * kYRot * kZRot;
    kWorldRotation.Reorthogonalize();

    return kWorldRotation;
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::GetRotation(efd::UInt32 targetIndex, efd::Matrix3& rotation) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    efd::Point3 eulerRotation;
    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, eulerRotation);

    efd::Matrix3 kXRot, kYRot, kZRot;

    kXRot.MakeXRotation(eulerRotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(eulerRotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(eulerRotation.z * -EE_DEGREES_TO_RADIANS);
    rotation = kXRot * kYRot * kZRot;
    rotation.Reorthogonalize();
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::SetRotation(efd::UInt32 targetIndex, const efd::Matrix3& rotation)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    efd::Point3 eulerRotation;
    rotation.ToEulerAnglesXYZ(eulerRotation.x, eulerRotation.y, eulerRotation.z);

    eulerRotation.x = eulerRotation.x * -EE_RADIANS_TO_DEGREES;
    eulerRotation.y = eulerRotation.y * -EE_RADIANS_TO_DEGREES;
    eulerRotation.z = eulerRotation.z * -EE_RADIANS_TO_DEGREES;

    m_spTempEntity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, eulerRotation);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityCreationGizmoAdapter::CanScale(efd::UInt32 targetIndex)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return false;

    efd::Float32 scale;
    return m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, 
        scale) == PropertyResult_OK;
}

//-----------------------------------------------------------------------------------------------
efd::Float32 EntityCreationGizmoAdapter::GetScaleStart(efd::UInt32 targetIndex) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return 0;

    efd::Float32 scale;
    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, scale);

    return scale;
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::GetScale(efd::UInt32 targetIndex, efd::Float32& scale) const
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    m_spTempEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, scale);
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::SetScale(efd::UInt32 targetIndex, const efd::Float32& scale)
{
    EE_UNUSED_ARG(targetIndex);

    if (m_spTempEntity == NULL)
        return;

    m_spTempEntity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, scale);
}

//-----------------------------------------------------------------------------------------------
void EntityCreationGizmoAdapter::Create()
{
    if (m_spTempEntity == NULL)
        return;

    m_pGizmoService->GetPolicy<CreationGizmoPolicy>()->CreateEntity(m_spTempEntity);
}

//-----------------------------------------------------------------------------------------------