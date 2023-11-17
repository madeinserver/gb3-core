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
#include "EntityGizmoAdapter.h"

#include "SelectionService.h"
#include "EntitySelectionAdapter.h"

#include "ToolSnapService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(EntityTransformGizmoAdapter);

//-----------------------------------------------------------------------------------------------
EntityTransformGizmoAdapter::EntityTransformGizmoAdapter(efd::ServiceManager* pServiceManager)
    : TransformGizmoAdapter(pServiceManager)
    , m_positionName("Position")
    , m_rotationName("Rotation")
    , m_scaleName("Scale")
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
}

//-----------------------------------------------------------------------------------------------
EntityTransformGizmoAdapter::~EntityTransformGizmoAdapter()
{
    ClearMementos();
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::IsVisible() const
{
    if (!m_pSelectionService->GetAdapter<EntitySelectionAdapter>()->HasSelection())
        return false;

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::IsSelected(NiAVObject* pObject) const
{
    Entity* pEntity = m_pSceneGraphService->GetEntityFromSceneGraph(pObject);

    NiAVObjectPtr spSelectedObject;

    EntitySelectionAdapter* pAdapter = m_pSelectionService->GetAdapter<EntitySelectionAdapter>();
    return pAdapter->GetSelectedEntities().find(pEntity->GetEntityID(), spSelectedObject);
}

//-----------------------------------------------------------------------------------------------
efd::Point3 EntityTransformGizmoAdapter::GetOrigin() const
{
    EntitySelectionAdapter* pAdapter = m_pSelectionService->GetAdapter<EntitySelectionAdapter>();
    return pAdapter->GetCenter();
}

//-----------------------------------------------------------------------------------------------
efd::Matrix3 EntityTransformGizmoAdapter::GetRotation() const
{
    EntitySelectionAdapter* pAdapter = m_pSelectionService->GetAdapter<EntitySelectionAdapter>();

    if (pAdapter->GetSelectedEntities().size() == 1 &&
        m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
    {
        egf::EntityID kEntityId = pAdapter->GetSelectedEntities().begin()->first;

        Entity* pEntity = m_pEntityManager->LookupEntity(kEntityId);
        EE_ASSERT(pEntity);

        efd::Point3 rotation;
        if (pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation) ==
            PropertyResult_OK)
        {
            efd::Matrix3 kWorldRotation, kXRot, kYRot, kZRot;

            kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS);
            kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS);
            kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS);
            kWorldRotation = kXRot * kYRot * kZRot;
            kWorldRotation.Reorthogonalize();

            return kWorldRotation;
        }
    }

    return efd::Matrix3::IDENTITY;
}

//-----------------------------------------------------------------------------------------------
efd::UInt32 EntityTransformGizmoAdapter::GetTargets() const
{
    EntitySelectionAdapter* pAdapter = m_pSelectionService->GetAdapter<EntitySelectionAdapter>();
    return pAdapter->GetSelectedEntities().size();
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::BeginTransform()
{
   TransformGizmoAdapter::BeginTransform();

    EntitySelectionAdapter* pAdapter = m_pSelectionService->GetAdapter<EntitySelectionAdapter>();

    for (efd::map<egf::EntityID, NiAVObjectPtr>::const_iterator iter =
         pAdapter->GetSelectedEntities().begin();
         iter != pAdapter->GetSelectedEntities().end();
         ++iter)
    {
        const egf::EntityID entityId = iter->first;

        Entity* pEntity = m_pEntityManager->LookupEntity(entityId);
        EE_ASSERT(pEntity);

        EntityMemento* pMemento = EE_EXTERNAL_NEW EntityMemento();
        pMemento->Entity = pEntity;

        // Save Position
        pMemento->HasPosition = pEntity->GetPropertyValue(
            egf::kPropertyID_StandardModelLibrary_Position,
            pMemento->StartPosition) == PropertyResult_OK;

        // Save Rotation
        Point3 kStartRotation;
        pMemento->HasRotation = pEntity->GetPropertyValue(
            egf::kPropertyID_StandardModelLibrary_Rotation,
            kStartRotation) == PropertyResult_OK;

        if (pMemento->HasRotation)
        {
            NiMatrix3 kXRot, kYRot, kZRot;
            kXRot.MakeXRotation(kStartRotation.x * -EE_DEGREES_TO_RADIANS);
            kYRot.MakeYRotation(kStartRotation.y * -EE_DEGREES_TO_RADIANS);
            kZRot.MakeZRotation(kStartRotation.z * -EE_DEGREES_TO_RADIANS);
            pMemento->StartRotation = kXRot * kYRot * kZRot;
            pMemento->StartRotation.Reorthogonalize();
        }

        // Save Scale
        pMemento->HasScale = pEntity->GetPropertyValue(
            egf::kPropertyID_StandardModelLibrary_Scale,
            pMemento->StartScale) == PropertyResult_OK;

        // Store memento
        m_entityStartStates.push_back(pMemento);
    }

    if (GetTargets() == 1)
    {
        EntityMemento* pMemento = m_entityStartStates[0];
        m_pSnapService->BeginSnap(pMemento->Entity);
    }
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::EndTransform(bool cancel)
{
    TransformGizmoAdapter::EndTransform(cancel);

    if (GetTargets() == 1)
    {
        m_pSnapService->EndSnap();
    }

    if (!cancel)
    {
        SetEntitiesPropertiesMessagePtr spSetProps = EE_NEW SetEntitiesPropertiesMessage();

        efd::Point3 positionValue;
        efd::Point3 rotationValue;
        efd::Float32 scaleValue;
        efd::utf8string positionStringValue;
        efd::utf8string rotationStringValue;
        efd::utf8string scaleStringValue;

        for (EntityMementos::const_iterator it = m_entityStartStates.begin();
             it != m_entityStartStates.end();
             ++it)
        {
            EntityMemento* pMemento = *it;

            if (m_translateDirty && pMemento->HasPosition)
            {
                pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position,
                    positionValue);
                ParseHelper<Point3>::ToString(positionValue, positionStringValue);
                spSetProps->AddEntry(pMemento->Entity->GetDataFileID(), m_positionName,
                    positionStringValue, NULL);
            }

            if (m_rotateDirty && pMemento->HasRotation)
            {
                pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation,
                    rotationValue);
                ParseHelper<Point3>::ToString(rotationValue, rotationStringValue);
                spSetProps->AddEntry(pMemento->Entity->GetDataFileID(), m_rotationName,
                    rotationStringValue, NULL);
            }

            if (m_scaleDirty && pMemento->HasScale)
            {
                pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale,
                    scaleValue);
                ParseHelper<efd::Float32>::ToString(scaleValue, scaleStringValue);
                spSetProps->AddEntry(pMemento->Entity->GetDataFileID(), m_scaleName,
                    scaleStringValue, NULL);
            }
        }

        m_pMessageService->SendImmediate(spSetProps,
            ToolMessagesConstants::ms_fromFrameworkCategory);
    }

    m_translateDirty = false;
    m_rotateDirty = false;
    m_scaleDirty = false;

    ClearMementos();
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::ClearMementos()
{
    for (EntityMementos::const_iterator it = m_entityStartStates.begin();
         it != m_entityStartStates.end();
         ++it)
    {
        EE_EXTERNAL_DELETE *it;
    }

    m_entityStartStates.clear();
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::SnapToPoints(
    efd::UInt32 targetIndex,
    efd::Point3 translationDelta)
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());

    EntityMemento* pMemento = m_entityStartStates[targetIndex];

    return m_pSnapService->SnapToPoints(pMemento->Entity, pMemento->StartPosition, 
        translationDelta);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::CanTranslate(efd::UInt32 targetIndex)
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());

    return m_entityStartStates[targetIndex]->HasPosition;
}

//-----------------------------------------------------------------------------------------------
efd::Point3 EntityTransformGizmoAdapter::GetTranslationStart(efd::UInt32 targetIndex) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    return m_entityStartStates[targetIndex]->StartPosition;
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::GetTranslation(
    efd::UInt32 targetIndex,
    efd::Point3& position) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::SetTranslation(
    efd::UInt32 targetIndex,
    const efd::Point3& position)
{
    m_translateDirty = true;

    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    pMemento->Entity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::CanRotate(efd::UInt32 targetIndex)
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());

    return m_entityStartStates[targetIndex]->HasRotation;
}

//-----------------------------------------------------------------------------------------------
efd::Matrix3 EntityTransformGizmoAdapter::GetRotationStart(efd::UInt32 targetIndex) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    return m_entityStartStates[targetIndex]->StartRotation;
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::GetRotation(efd::UInt32 targetIndex, efd::Matrix3& rotation) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    efd::Point3 eulerRotation;
    pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation,
        eulerRotation);

    efd::Matrix3 kXRot, kYRot, kZRot;

    kXRot.MakeXRotation(eulerRotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(eulerRotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(eulerRotation.z * -EE_DEGREES_TO_RADIANS);
    rotation = kXRot * kYRot * kZRot;
    rotation.Reorthogonalize();
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::SetRotation(efd::UInt32 targetIndex, const efd::Matrix3& rotation)
{
    m_rotateDirty = true;

    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    efd::Point3 eulerRotation;
    rotation.ToEulerAnglesXYZ(eulerRotation.x, eulerRotation.y, eulerRotation.z);

    eulerRotation.x = eulerRotation.x * -EE_RADIANS_TO_DEGREES;
    eulerRotation.y = eulerRotation.y * -EE_RADIANS_TO_DEGREES;
    eulerRotation.z = eulerRotation.z * -EE_RADIANS_TO_DEGREES;

    pMemento->Entity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation,
        eulerRotation);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityTransformGizmoAdapter::CanScale(efd::UInt32 targetIndex)
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());

    return m_entityStartStates[targetIndex]->HasScale;
}

//-----------------------------------------------------------------------------------------------
efd::Float32 EntityTransformGizmoAdapter::GetScaleStart(efd::UInt32 targetIndex) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    return m_entityStartStates[targetIndex]->StartScale;
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::GetScale(efd::UInt32 targetIndex, efd::Float32& scale) const
{
    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    pMemento->Entity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, scale);
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::SetScale(efd::UInt32 targetIndex, const efd::Float32& scale)
{
    m_scaleDirty = true;

    EE_ASSERT(targetIndex < m_entityStartStates.size());
    EntityTransformGizmoAdapter::EntityMemento* pMemento = m_entityStartStates[targetIndex];

    pMemento->Entity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Scale, scale);
}

//-----------------------------------------------------------------------------------------------
void EntityTransformGizmoAdapter::OnTransformClone()
{
    EndTransform(false);

    BeginCloneEntityMessagePtr spCloneEntitiesMsg = EE_NEW BeginCloneEntityMessage();
    m_pMessageService->SendImmediate(spCloneEntitiesMsg);
}
//-----------------------------------------------------------------------------------------------
