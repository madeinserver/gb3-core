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

#include "TransformGizmo.h"

#include <NiMath.h>

#include <efd/ServiceManager.h>

#include <egf/EntityManager.h>
#include <egf/FlatModelManager.h>
#include <efd/ParseHelper.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CameraService.h>

#include "SelectionService.h"
#include "GizmoService.h"
#include "ToolCameraService.h"
#include "ToolServicesMessages.h"
#include "ToolSnapService.h"

using namespace egf;
using namespace efd;
using namespace ecr;
using namespace egmToolServices;

//-----------------------------------------------------------------------------------------------
TransformGizmo::TransformGizmo(
    efd::ServiceManager* pServiceManager,
    TransformGizmoAdapter* pAdapter)
    : IGizmo(pAdapter)
    , m_spTransformAdapter(pAdapter)
    , m_fDefaultDistance(8.0f)
    , m_spGizmo(NULL)
    , m_usingSpecial(false)
    , m_isActive(false)
    , m_isTransforming(false)
{
    m_pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pPickService = pServiceManager->GetSystemServiceAs<PickService>();
    EE_ASSERT(m_pPickService);

    m_pRenderService = pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);

    m_pSceneGraphService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        pServiceManager->GetSystemServiceAs<SceneGraphService>());
    EE_ASSERT(m_pSceneGraphService);

    m_pGizmoService = pServiceManager->GetSystemServiceAs<GizmoService>();
    EE_ASSERT(m_pGizmoService);

    m_pCameraService = pServiceManager->GetSystemServiceAs<ToolCameraService>();
    EE_ASSERT(m_pCameraService);
}

//-----------------------------------------------------------------------------------------------
TransformGizmo::~TransformGizmo()
{
    m_spGizmo = NULL;
}

//-----------------------------------------------------------------------------------------------
bool TransformGizmo::OnTick(efd::TimeType timeElapsed, RenderSurface* pSurface)
{
    IGizmo::OnTick(timeElapsed, pSurface);

    if (m_spGizmo == NULL)
        return true;

    m_spGizmo->SetTranslate(m_spTransformAdapter->GetOrigin());
    m_spGizmo->SetRotate(m_spTransformAdapter->GetRotation());
    m_spGizmo->Update(0.0f);

    return true;
}

//-----------------------------------------------------------------------------------------------
bool TransformGizmo::OnMouseScroll(
    RenderSurface* pSurface,
    int x,
    int y,
    int dScroll,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dScroll);
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(bIsClosest);

    return false;
}

//-----------------------------------------------------------------------------------------------
bool TransformGizmo::IsActive()
{
    return m_isActive;
}

//-----------------------------------------------------------------------------------------------
void TransformGizmo::TransformToView(RenderSurface* pSurface)
{
    if (m_spGizmo == NULL)
        return;

    NiCamera* pCamera = pSurface->GetCamera();

    if (pCamera)
    {
        if (pCamera->GetViewFrustum().m_bOrtho)
        {
            NiFrustum pkFrustum = pCamera->GetViewFrustum();
            m_spGizmo->SetScale(Abs(pkFrustum.m_fRight * 2.0f) / m_fDefaultDistance);
        }
        else
        {
            efd::Point3 center = m_spTransformAdapter->GetOrigin();

            float fCamDistance = (center - pCamera->GetWorldTranslate()).Length();
            if ((fCamDistance / m_fDefaultDistance) > 0.0f)
            {
                m_spGizmo->SetScale((fCamDistance / m_fDefaultDistance) *
                    pCamera->GetViewFrustum().m_fRight * 2.0f);
            }
            else
            {
                m_spGizmo->SetScale(1.0f);
            }
        }

        m_spGizmo->Update(0.0f);
    }

    m_spGizmo->SetAppCulled(!m_spTransformAdapter->IsVisible());
}

//-----------------------------------------------------------------------------------------------
bool TransformGizmo::MatchSurface(
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    efd::Point3& intersection,
    efd::Point3& rotation)
{
    // TODO NDarnell
    // Need to move this code into an adapter, this is too specific to pickable gamebryo objects.

    bool picked = false;

    // Perform a pick, and use the delta between that and the starting pt
    PickService::PickRecordPtr spPickResult =
        m_pPickService->PerformPick(rayOrigin, rayDirection, true, false, false);

    if (spPickResult)
    {
        NiPick::Record* pFinalRecord = NULL;

        const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
        EE_ASSERT(pPickerResults);
        EE_ASSERT(pPickerResults->GetSize() > 0);

        for (unsigned int i = 0; i < pPickerResults->GetSize(); i++)
        {
            NiPick::Record* pPickRecord = pPickerResults->GetAt(i);
            if (pPickRecord)
            {
                NiAVObject* pPickedObject = pPickRecord->GetAVObject()->GetRoot();

                if (!m_spTransformAdapter->IsSelected(pPickedObject))
                {
                    // Picked entity is not selected; accept it
                    pFinalRecord = pPickRecord;
                    picked = true;
                    break;
                }
            }
        }

        if (pFinalRecord)
        {
            intersection = pFinalRecord->GetIntersection();
            if (m_pGizmoService->GetPlacementMode() == GizmoService::PMODE_ALIGN_TO_SURFACE)
            {
                GizmoService::FindAlignToSurfaceRotation(pFinalRecord->GetNormal(), rotation);
            }
        }
    }

    return picked;
}

//-----------------------------------------------------------------------------------------------
void TransformGizmo::Connect(Ni3DRenderView* pGizmoView)
{
    IGizmo::Connect(pGizmoView);

    if (m_spGizmo != NULL)
        pGizmoView->AppendScene(m_spGizmo);
}

//-----------------------------------------------------------------------------------------------
void TransformGizmo::Disconnect(Ni3DRenderView* pGizmoView)
{
    IGizmo::Disconnect(pGizmoView);

    if (m_spGizmo != NULL)
        pGizmoView->RemoveScene(m_spGizmo);
}

//-----------------------------------------------------------------------------------------------
GizmoAdapter* TransformGizmo::GetAdapter() const
{
    return m_spTransformAdapter;
}

//-----------------------------------------------------------------------------------------------
void TransformGizmo::BeginTransform()
{
    m_spTransformAdapter->BeginTransform();
}

//-----------------------------------------------------------------------------------------------
void TransformGizmo::EndTransform(bool cancel)
{
    m_spTransformAdapter->EndTransform(cancel);
}

//-----------------------------------------------------------------------------------------------
