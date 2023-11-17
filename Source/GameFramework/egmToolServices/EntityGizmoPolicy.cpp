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

#include "EntityGizmoPolicy.h"

#include "SelectionService.h"
#include "EntitySelectionAdapter.h"

#include "SelectionGizmo.h"
#include "TranslationGizmo.h"
#include "RotationGizmo.h"
#include "ScaleGizmo.h"
#include "TerrainEditGizmo.h"

#include "EntityGizmoAdapter.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(EntityGizmoPolicy);

//-----------------------------------------------------------------------------------------------
EntityGizmoPolicy::EntityGizmoPolicy(efd::ServiceManager* pServiceManager)
    : m_pServiceManager(pServiceManager)
    , m_pActiveGizmo(NULL)
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
void EntityGizmoPolicy::OnAdded(GizmoService* pGizmoService)
{
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    EntityTransformGizmoAdapterPtr spEntityAdapter = 
        EE_NEW EntityTransformGizmoAdapter(m_pServiceManager);

    // Standard Entity Gizmos
    AddGizmo("Selection", EE_NEW SelectionGizmo(m_pServiceManager, spEntityAdapter));
    AddGizmo("Translation", EE_NEW TranslationGizmo(m_pServiceManager, spEntityAdapter));
    AddGizmo("Rotation", EE_NEW RotationGizmo(m_pServiceManager, spEntityAdapter));
    AddGizmo("UniformScale", EE_NEW ScaleGizmo(m_pServiceManager, spEntityAdapter));

    SetActiveGizmo(pGizmoService, "Translation");
}

//-----------------------------------------------------------------------------------------------
void EntityGizmoPolicy::OnTick(
    GizmoService* pGizmoService,
    efd::TimeType timeElapsed,
    ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pGizmoService);
    EE_UNUSED_ARG(timeElapsed);
    EE_UNUSED_ARG(pSurface);
}

//-----------------------------------------------------------------------------------------------
void EntityGizmoPolicy::OnRemoved(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);

    m_gizmoMap.clear();
    m_pActiveGizmo = NULL;
}

//-----------------------------------------------------------------------------------------------
void EntityGizmoPolicy::Activate(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    if (m_pActiveGizmo != NULL)
        m_pGizmoService->AddGizmo(m_pActiveGizmo);
}

//-----------------------------------------------------------------------------------------------
void EntityGizmoPolicy::Deactivate(GizmoService* pGizmoService)
{
    EE_UNUSED_ARG(pGizmoService);

    if (m_pActiveGizmo != NULL)
        pGizmoService->RemoveGizmo(m_pActiveGizmo);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityGizmoPolicy::IsSubjectCovered(
    GizmoService* pGizmoService,
    const efd::utf8string& subjectName)
{
    EE_UNUSED_ARG(pGizmoService);

    if (subjectName == "Entity")
        return true;

    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntityGizmoPolicy::SetActiveGizmo(
    GizmoService* pGizmoService,
    const efd::utf8string& gizmoName)
{
    EE_UNUSED_ARG(pGizmoService);

    if (m_pActiveGizmo != NULL)
    {
        m_pGizmoService->RemoveGizmo(m_pActiveGizmo);
        m_pActiveGizmo = NULL;
    }

    IGizmoPtr spActiveGizmo;
    m_gizmoMap.find(gizmoName, spActiveGizmo);

    if (spActiveGizmo == NULL)
        return false;

    m_pActiveGizmo = spActiveGizmo;

    m_pGizmoService->AddGizmo(m_pActiveGizmo);

    m_pRenderService->InvalidateRenderContexts();

    return true;
}

//-----------------------------------------------------------------------------------------------
