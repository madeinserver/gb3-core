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
#include "CreationGizmoPolicy.h"

#include "SelectionService.h"
#include "EntitySelectionAdapter.h"

#include "SelectionGizmo.h"
#include "TranslationGizmo.h"
#include "RotationGizmo.h"
#include "ScaleGizmo.h"

#include "EntityCreationGizmoAdapter.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(CreationGizmoPolicy);

//-----------------------------------------------------------------------------------------------
CreationGizmoPolicy::CreationGizmoPolicy(efd::ServiceManager* pServiceManager)
    : m_pServiceManager(pServiceManager)
    , m_isActive(false)
    , m_ksPosition("Position")
    , m_ksRotation("Rotation")
{
    m_pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pRenderService = pServiceManager->GetSystemServiceAs<ecr::RenderService>();
    EE_ASSERT(m_pRenderService);

    m_pEntityManager = pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);

    m_pSceneGraphService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        pServiceManager->GetSystemServiceAs<SceneGraphService>());
    EE_ASSERT(m_pSceneGraphService);

    m_pGizmoService = pServiceManager->GetSystemServiceAs<GizmoService>();
    EE_ASSERT(m_pGizmoService);

    m_pSelectionService = pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(m_pSelectionService);
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::OnAdded(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    m_spCreationGizmo = EE_NEW CreationGizmo(m_pServiceManager, 
        EE_NEW EntityCreationGizmoAdapter(m_pServiceManager));
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::OnTick(
    GizmoService* pGizmoService,
    efd::TimeType timeElapsed,
    ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pGizmoService);
    EE_UNUSED_ARG(timeElapsed);
    EE_UNUSED_ARG(pSurface);
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::OnRemoved(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);

    m_spCreationGizmo = NULL;
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::Activate(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    if (m_isActive)
        pGizmoService->AddGizmo(m_spCreationGizmo);
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::Deactivate(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    pGizmoService->RemoveGizmo(m_spCreationGizmo);
}

//-----------------------------------------------------------------------------------------------
efd::Bool CreationGizmoPolicy::IsSubjectCovered(
    GizmoService* pGizmoService,
    const efd::utf8string& subjectName)
{
    EE_UNUSED_ARG(pGizmoService);

    if (subjectName == "Entity")
        return true;

    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool CreationGizmoPolicy::SetActiveGizmo(
    GizmoService* pGizmoService,
    const efd::utf8string& gizmoName)
{
    EE_UNUSED_ARG(pGizmoService);
    EE_UNUSED_ARG(gizmoName);

    if (gizmoName == "Create")
    {
        pGizmoService->AddGizmo(m_spCreationGizmo);
        m_isActive = true;
        return true;
    }
    else
    {
        pGizmoService->RemoveGizmo(m_spCreationGizmo);
        m_isActive = false;
        return false;
    }
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::CreateEntity(egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pEntity);

    AddEntityMessagePtr spCreateEntityMessage = EE_NEW AddEntityMessage();
    spCreateEntityMessage->SetModelName(pEntity->GetModelName());

    efd::Point3 position;
    if (pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position,
        position) == egf::PropertyResult_OK)
    {
        utf8string strPosition;
        ParseHelper<Point3>::ToString(position, strPosition);
        spCreateEntityMessage->AddOverride(m_ksPosition, strPosition, NULL);
    }

    efd::Point3 rotation;
    if (pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation,
        rotation) == egf::PropertyResult_OK)
    {
        utf8string strRotation;
        ParseHelper<Point3>::ToString(rotation, strRotation);
        spCreateEntityMessage->AddOverride(m_ksRotation, strRotation, NULL);
    }

    m_pMessageService->SendImmediate(spCreateEntityMessage, 
        ToolMessagesConstants::ms_fromFrameworkCategory);
}

//-----------------------------------------------------------------------------------------------
const efd::utf8string& CreationGizmoPolicy::GetCreationModeModel()
{
    return m_creationModeModel;
}

//-----------------------------------------------------------------------------------------------
void CreationGizmoPolicy::SetCreationModeModel(efd::utf8string& modelName)
{
    m_creationModeModel = modelName;
}

//-----------------------------------------------------------------------------------------------