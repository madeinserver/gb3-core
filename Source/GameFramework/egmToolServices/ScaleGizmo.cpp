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

#include "ScaleGizmo.h"

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/CameraService.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include "GizmoService.h"
#include "ToolCameraService.h"
#include "SelectionService.h"
#include "ToolServicesMessages.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//------------------------------------------------------------------------------------------------
ScaleGizmo::ScaleGizmo(efd::ServiceManager* pServiceManager,
                       TransformGizmoAdapter* pAdapter)
    : TransformGizmo(pServiceManager, pAdapter)
    , m_startDistance(0)
    , m_center(0, 0, 0)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_mouseMoved(false)
{
    m_spGizmo = m_pSceneGraphService->LoadSceneGraph("ScaleGizmo.nif");
}

//-----------------------------------------------------------------------------------------------
ScaleGizmo::~ScaleGizmo()
{
}

//-----------------------------------------------------------------------------------------------
bool ScaleGizmo::OnTick(efd::TimeType timeElapsed, ecr::RenderSurface* pSurface)
{
    TransformGizmo::OnTick(timeElapsed, pSurface);

    if (!IsActive())
        return true;

    if (!m_mouseMoved)
        return true;

    m_mouseMoved = false;

    efd::Float32 currentDistance = GetDistance(pSurface, (float)m_mouseX, (float)m_mouseY);

    efd::Float32 scaleMultiplier = currentDistance / m_startDistance;
    if (m_pGizmoService->ScaleSnapEnabled())
    {
        // Snap to the nearest scale increment
        GizmoService::RoundToIncrement(scaleMultiplier, m_pGizmoService->ScaleSnap());
    }

    for (efd::UInt32 i = 0; i < m_spTransformAdapter->GetTargets(); i++)
    {
        if (m_spTransformAdapter->CanTranslate(i) &&
            m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_WORLD)
        {
            efd::Point3 delta = (m_spTransformAdapter->GetTranslationStart(i) - m_center);
            efd::Point3 scaled_delta = delta * scaleMultiplier;

            Point3 newPosition(m_center + scaled_delta);

            if (m_pGizmoService->TranslationPrecisionEnabled())
            {
                // Round such that the final position matches precision.  For
                // scaling operations, this is only necessary when there is
                // multiple selection.
                GizmoService::RoundToIncrement(newPosition,
                    m_pGizmoService->TranslationPrecision());
            }

            // Local-space scaling doesn't affect positioning
            m_spTransformAdapter->SetTranslation(i, newPosition);
        }

        if (m_spTransformAdapter->CanScale(i))
        {
            float newScale;
            if (m_spTransformAdapter->GetScaleStart(i) > FLT_EPSILON)
                newScale = m_spTransformAdapter->GetScaleStart(i) * scaleMultiplier;
            else
                newScale = scaleMultiplier; // allow recovery from zero scale

            if (m_pGizmoService->ScalePrecisionEnabled())
            {
                // Round such that final scale matches precision
                GizmoService::RoundToIncrement(newScale, m_pGizmoService->ScalePrecision());
            }

            m_spTransformAdapter->SetScale(i, newScale);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool ScaleGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);
    EE_UNUSED_ARG(bIsClosest);

    if (!IsActive())
        return false;

    if (!m_isTransforming)
    {
        if (!m_usingSpecial && m_pGizmoService->IsSpecialActive())
        {
            m_usingSpecial = true;
            m_spTransformAdapter->OnTransformClone();

            BeginTransform();
        }
    }

    m_mouseMoved = true;
    m_isTransforming = true;
    m_mouseX = x;
    m_mouseY = y;

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool ScaleGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    if (!bIsClosest)
        return false;

    if (m_pCameraService->GetIsLooking() || m_pCameraService->GetIsTranslating())
        return false;

    BeginTransform();

    m_mouseX = x;
    m_mouseY = y;

    m_startDistance = GetDistance(pSurface, (float)m_mouseX, (float)m_mouseY);

    m_isActive = true;
    m_mouseMoved = false;

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool ScaleGizmo::OnMouseUp(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    EndTransform(false);

    return true;
}

//-----------------------------------------------------------------------------------------------
void ScaleGizmo::BeginTransform()
{
    TransformGizmo::BeginTransform();

    m_center = m_spTransformAdapter->GetOrigin();
}

//-----------------------------------------------------------------------------------------------
void ScaleGizmo::EndTransform(bool cancel)
{
    if (!m_isActive)
        return;

    m_isActive = false;
    m_usingSpecial = false;
    m_isTransforming = false;
    m_mouseMoved = false;

    TransformGizmo::EndTransform(cancel);
}

//------------------------------------------------------------------------------------------------
bool ScaleGizmo::HitTest(
    ecr::RenderSurface* pSurface,
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    float& outHitDistance)
{
    TransformToView(pSurface);

    // Issue the pick operation to the pick service.
    PickService::PickRecordPtr spPickResult =
        m_pPickService->PerformPick(rayOrigin, rayDirection, m_spGizmo, false, true, false);

    if (!spPickResult)
        return false;

    const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
    outHitDistance = pPickerResults->GetAt(0)->GetDistance();

    return true;
}

//-----------------------------------------------------------------------------------------------
float ScaleGizmo::GetDistance(RenderSurface* pSurface, float fX, float fY)
{
    float camera_x, camera_y;
    if (!pSurface->GetCamera()->WorldPtToScreenPt(m_center, camera_x, camera_y))
    {
        EE_ASSERT(false);
        return 0;
    }

    int screen_x = (int) (pSurface->GetRenderTargetGroup()->GetWidth(0) * camera_x);
    int screen_y = (int) (pSurface->GetRenderTargetGroup()->GetHeight(0) * (1.0f - camera_y));

    float delta_x = screen_x - fX;
    float delta_y = screen_y - fY;

    float distance = NiSqrt(delta_x * delta_x + delta_y * delta_y);

    return distance;
}

//-----------------------------------------------------------------------------------------------
