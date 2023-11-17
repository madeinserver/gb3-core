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

#include "StandardCameraMode.h"

#include <efd/MemTracker.h>
#include <efd/ParseHelper.h>
#include <efd/MessageService.h>
#include <efd/ServiceManager.h>

#include <egf/EntityManager.h>

#include <NiCommonGraphCallbackObjects.h>
#include <NiVisualTracker.h>
#include <NiVisualTrackerRenderClick.h>

#include <ecr/RenderSurface.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/PickService.h>
#include <ecr/CameraService.h>

#include <ecrInput/KeyboardMessages.h>
#include <ecrInput/MouseMessages.h>

#include "SelectionService.h"
#include "ToolCameraService.h"
#include "ToolSceneService.h"
#include "ToolServicesMessages.h"

#include "EntitySelectionAdapter.h"

using namespace egf;
using namespace efd;
using namespace ecr;
using namespace egmToolServices;
using namespace ecrInput;

//-----------------------------------------------------------------------------------------------
StandardCameraMode::StandardCameraMode(efd::ServiceManager* pServiceManager)
    : m_pServiceManager(pServiceManager)
    , m_upAxis(NiPoint3::UNIT_Z)
    , m_scrollSpeed(40.0f)
    , m_mouseLookScalar(0.002f)
    , m_mousePanScalar(1.0f)
    , m_orbitCenter(0,0,0)
    , m_orbitStickDistance(500.0f)
    , m_orthoMovementScale(0.3f)
{
    m_pPickService = m_pServiceManager->GetSystemServiceAs<PickService>();
    EE_ASSERT(m_pPickService);

    m_pRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);

    m_pToolSceneService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        m_pServiceManager->GetSystemServiceAs<SceneGraphService>());
    EE_ASSERT(m_pToolSceneService);

    m_pCameraService = m_pServiceManager->GetSystemServiceAs<ToolCameraService>();
    EE_ASSERT(m_pCameraService);

    m_pSelectionService = pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(m_pSelectionService);
}

//-----------------------------------------------------------------------------------------------
StandardCameraMode::~StandardCameraMode()
{
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::OnTick(efd::TimeType timeElapsed, ToolCamera* pCamera)
{
    // Limit delta--when frame rate is very low we don't want large jumps.
    const Float32 worstCaseFPS = 5.f;
    Float32 delta = efd::Clamp((Float32)timeElapsed, 0.0f, 1.0f / worstCaseFPS);

    Float32 strafe = 0;
    Float32 forward = 0;

    if (m_pCameraService->MovingLeft())
        strafe = -1;
    else if (m_pCameraService->MovingRight())
        strafe = 1;

    if (m_pCameraService->MovingBackward())
        forward = -1;
    else if (m_pCameraService->MovingForward())
        forward = 1;

    NiPoint2 rayMove(strafe, forward);
    rayMove.Unitize();

    strafe = rayMove.x;
    forward = rayMove.y;

    // Fit the near and far planes to match the new angle of the camera with the scene.
    NiBound sceneBounds = m_pToolSceneService->GetSceneBounds();
    pCamera->FitNearAndFarToBound(sceneBounds);
    if (!pCamera->GetViewFrustum().m_bOrtho)
    {
        if (pCamera->GetViewFrustum().m_fFar < m_pCameraService->GetFarPlane())
        {
            NiFrustum frustum(pCamera->GetViewFrustum());
            frustum.m_fNear = m_pCameraService->GetNearPlane();
            frustum.m_fFar = m_pCameraService->GetFarPlane();

            pCamera->SetViewFrustum(frustum);
        }
    }

    if (forward == 0 && strafe == 0)
        return true;

    m_pRenderService->InvalidateRenderContexts();

    if (pCamera->GetViewFrustum().m_bOrtho)
    {
        Float32 speed = m_pCameraService->GetKeyboardBaseMovement() * m_orthoMovementScale;
        if (m_pCameraService->MovingFast())
            speed *= m_pCameraService->GetTurboScale();

        Float32 movement = speed * delta * m_pCameraService->GetMovementScale();

        const NiFrustum& frustum = pCamera->GetViewFrustum();

        RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();
        NiRenderTargetGroup* pRenderTarget = pSurface->GetRenderTargetGroup();

        float orthoScaleX = (frustum.m_fRight * 2.0f / (Float32)pRenderTarget->GetWidth(0));
        float orthoScaleY = (frustum.m_fTop * 2.0f / (Float32)pRenderTarget->GetHeight(0));

        // Scale movement in orthographic mode based on the ratio of view frustum to screen size.
        forward = forward * movement * orthoScaleX;
        strafe = strafe * movement * orthoScaleY;

        NiPoint3 translation = pCamera->GetTranslate() +
            (pCamera->GetWorldUpVector() * forward) +
            (pCamera->GetWorldRightVector() * strafe);

        pCamera->SetTranslate(translation);

        AdjustOrthoDistance(pCamera);
    }
    else
    {
        Float32 speed = m_pCameraService->GetKeyboardBaseMovement();
        if (m_pCameraService->MovingFast())
            speed *= m_pCameraService->GetTurboScale();

        Float32 movement = speed * delta * m_pCameraService->GetMovementScale();

        NiPoint3 kWorldDirection = pCamera->GetWorldDirection();
        NiPoint3 kRightVector = pCamera->GetWorldRightVector();
        NiPoint3 kUpVector = pCamera->GetWorldUpVector();

        if (m_pCameraService->IsFlyingLevel())
        {
            // DT32636 Change when we allow for changing the up vector.
            kWorldDirection.z = kRightVector.z = 0.0f;
            kWorldDirection.Unitize();
            kRightVector.Unitize();
        }

        NiPoint3 translation = pCamera->GetTranslate() +
            (kWorldDirection * forward * movement) +
            (kRightVector * strafe * movement);

        pCamera->SetTranslate(translation);
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::OnMouseScroll(
   efd::SInt32 x,
   efd::SInt32 y,
   efd::SInt32 dScroll,
   ToolCamera* pCamera)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    m_pRenderService->InvalidateRenderContexts();

    if (!pCamera->GetViewFrustum().m_bOrtho)
    {
        Float32 dollyDelta;
        Float32 pickDistance;

        RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

        SInt32 centerX = pSurface->GetRenderTargetGroup()->GetWidth(0) / 2;
        SInt32 centerY = pSurface->GetRenderTargetGroup()->GetHeight(0) / 2;

        if (GetPickDistance(centerX, centerY, pickDistance))
        {
            // If you are too close to the object and moving in reverse allow the camera to back
            // out.
            if (pickDistance == 0 && dScroll < 0)
                pickDistance = 1;

            // if we picked, make the speed proportional to distance
            dollyDelta = pickDistance * m_scrollSpeed * (1.0f - NiPow(0.999f, (Float32)(dScroll)));

            // Make sure the user can't scroll pass the near plane.
            Float32 safeNear = pCamera->GetViewFrustum().m_fNear + pCamera->GetMinNearPlaneDist();
            if ((pickDistance - dollyDelta) <= safeNear)
                dollyDelta = 0;
        }
        else
        {
            dollyDelta = (Float32)(dScroll) * m_scrollSpeed;
        }

        if (m_pCameraService->MovingFast())
            dollyDelta *= m_pCameraService->GetTurboScale();

        NiPoint3 kTranslate = CameraService::Dolly(
            dollyDelta,
            pCamera->GetTranslate(),
            pCamera->GetRotate());

        pCamera->SetTranslate(kTranslate);
    }
    else
    {
        Float32 factor = ((dScroll > 0) ?
            Pow(1.1f, Abs((Float32)dScroll)) : -Pow(1.1f, Abs((Float32)dScroll))) * 3;

        if (m_pCameraService->MovingFast())
            factor *= m_pCameraService->GetTurboScale();

        //EE_ASSERT(factor > 1 || factor < -1);
        //EE_ASSERT(factor < 1000 && factor > -1000);

        pCamera->Zoom(factor);
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::OnMouseMove(
     efd::SInt32 x,
     efd::SInt32 y,
     efd::SInt32 dx,
     efd::SInt32 dy,
     ToolCamera* pCamera)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    m_pRenderService->InvalidateRenderContexts();

    Float32 lookScale = m_mouseLookScalar * m_pCameraService->GetLookScale();

    if (m_pCameraService->GetIsLooking() && m_pCameraService->GetIsTranslating())
    {
        if (pCamera->GetViewFrustum().m_bOrtho)
        {
            Float32 factor = ((dy > 0) ?
                Pow(1.1f, Abs((Float32)dy)) : -Pow(1.1f, Abs((Float32)dy))) * 3;

            if (m_pCameraService->MovingFast())
                factor *= m_pCameraService->GetTurboScale();

            //EE_ASSERT(factor > 1 || factor < -1);
            //EE_ASSERT(factor < 1000 && factor > -1000);

            pCamera->Zoom(factor);
        }
        else
        {
            Float32 speed = m_pCameraService->GetMouseBaseMovement();
            if (m_pCameraService->MovingFast())
                speed *= m_pCameraService->GetTurboScale();

            Float32 move_dy = -speed * dy * m_pCameraService->GetMovementScale();
            Float32 move_dx = speed * dx * m_pCameraService->GetMovementScale();

            NiPoint3 translation = pCamera->GetTranslate() +
                (m_upAxis * move_dy) + (pCamera->GetWorldRightVector() * move_dx);

            pCamera->SetTranslate(translation);
        }
    }
    else if (m_pCameraService->GetIsLooking())
    {
        if (pCamera->GetViewFrustum().m_bOrtho)
        {
            NiPoint3 newLocation;
            NiMatrix3 newRotation;
            CameraService::Orbit(
                dx * lookScale,
                dy * lookScale,
                pCamera->GetTranslate(),
                pCamera->GetRotate(),
                m_orbitCenter,
                m_upAxis,
                newLocation,
                newRotation);

            pCamera->SetTranslate(newLocation);
            pCamera->SetRotate(newRotation);

            AdjustOrthoDistance(pCamera);
        }
        else
        {
            NiMatrix3 newRotation;
            newRotation = CameraService::Look(
                dx * lookScale,
                dy * lookScale,
                pCamera->GetRotate(),
                m_upAxis);

            pCamera->SetRotate(newRotation);
        }
    }
    else if (m_pCameraService->GetIsTranslating())
    {
        if (pCamera->GetViewFrustum().m_bOrtho)
        {
            const NiMatrix3 rotation = pCamera->GetRotate();

            Float32 panSpeedX = (Float32)(dx) * m_panSpeedX;
            Float32 panSpeedY = (Float32)(dy) * m_panSpeedY;

            NiPoint3 newTranslation = ecr::CameraService::Pan(
                panSpeedX,
                panSpeedY,
                pCamera->GetTranslate(),
                rotation);

            pCamera->SetTranslate(newTranslation);
        }
        else
        {
            Float32 speed = m_pCameraService->GetMouseBaseMovement();
            if (m_pCameraService->MovingFast())
                speed *= m_pCameraService->GetTurboScale();

            Float32 move_dy = -speed * dy * m_pCameraService->GetMovementScale();

            NiPoint3 kWorldDirection = pCamera->GetWorldDirection();
            NiPoint3 kRightVector = pCamera->GetWorldRightVector();
            NiPoint3 kUpVector = pCamera->GetWorldUpVector();

            // DT32636 Change when we allow for changing the up vector.
            kWorldDirection.z = kRightVector.z = 0.0f;
            kWorldDirection.Unitize();

            NiPoint3 translation = pCamera->GetTranslate() +
                (kWorldDirection * move_dy);

            pCamera->SetTranslate(translation);

            NiMatrix3 newRotation;
            newRotation = CameraService::Look(
                dx * lookScale,
                0,
                pCamera->GetRotate(),
                m_upAxis);

            pCamera->SetRotate(newRotation);
        }
    }
    else if (m_pCameraService->GetIsPanning())
    {
        const NiMatrix3 rotation = pCamera->GetRotate();

        Float32 panSpeedX = (Float32)(dx) * m_panSpeedX;
        Float32 panSpeedY = (Float32)(dy) * m_panSpeedY;

        NiPoint3 newTranslation = ecr::CameraService::Pan(
            panSpeedX,
            panSpeedY,
            pCamera->GetTranslate(),
            rotation);

        pCamera->SetTranslate(newTranslation);
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::OnMouseDown(
    MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    ToolCamera* pCamera)
{
    if (!(button == MouseDownMessage::MBUTTON_LEFT ||
          button == MouseDownMessage::MBUTTON_MIDDLE))
    {
        return false;
    }

    m_pRenderService->InvalidateRenderContexts();

    if (button == MouseDownMessage::MBUTTON_LEFT)
    {
        if (pCamera->GetViewFrustum().m_bOrtho)
            AdjustOrthoDistance(pCamera);
    }

    SetupOrbit(x, y, pCamera);

    if (button == MouseDownMessage::MBUTTON_MIDDLE)
    {
        RenderSurface* pActiveSurface = m_pRenderService->GetActiveRenderSurface();
        NiRenderTargetGroup* pTarget = pActiveSurface->GetRenderTargetGroup();

        Float32 pickDistance;
        if (GetPickDistance(x, y, pickDistance))
        {
            const NiFrustum& frustum = pCamera->GetViewFrustum();

            if (!frustum.m_bOrtho)
            {
                // if there was a pick result, pan proportional to the distance.
                m_panSpeedX = pickDistance / (Float32)pTarget->GetWidth(0);
                m_panSpeedY = pickDistance / (Float32)pTarget->GetHeight(0);
            }
            else
            {
                // if there was a pick result in ortho pan proportional to the frustum width.
                m_panSpeedX = frustum.m_fRight * 2.0f / (Float32)pTarget->GetWidth(0);
                m_panSpeedY = frustum.m_fTop * 2.0f / (Float32)pTarget->GetHeight(0);
            }

            // If the pan speed is ever 0, set it back to the default.
            if (m_panSpeedX == 0 || m_panSpeedY == 0)
            {
                m_panSpeedX = m_mousePanScalar;
                m_panSpeedY = m_mousePanScalar;
            }
        }
        else
        {
            m_panSpeedX = m_mousePanScalar;
            m_panSpeedY = m_mousePanScalar;
        }

        m_panSpeedX *= m_pCameraService->GetLookScale();
        m_panSpeedY *= m_pCameraService->GetLookScale();
    }

    m_cameraStart = pCamera->GetTranslate();

    return true;
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::OnMouseUp(
    MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    ToolCamera* pCamera)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(pCamera);

    if (!(button == MouseUpMessage::MBUTTON_LEFT ||
          button == MouseUpMessage::MBUTTON_MIDDLE))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
void StandardCameraMode::Reset(ToolCamera* pCamera)
{
    EE_UNUSED_ARG(pCamera);

    m_pRenderService->InvalidateRenderContexts();

    if (pCamera->GetViewFrustum().m_bOrtho)
    {
        AdjustOrthoDistance(pCamera);
        AdjustOrthoDistance(pCamera);
    }
}

//-----------------------------------------------------------------------------------------------
Bool StandardCameraMode::GetPickDistance(efd::SInt32 x, efd::SInt32 y, Float32& pickDistance)
{
    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();
    NiCamera* pCamera = pSurface->GetCamera();

    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    CameraService::MouseToRay(
        (Float32)x,
        (Float32)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pCamera,
        rayOrigin,
        rayDirection);

    PickService::PickRecordPtr spPickResult =
        m_pPickService->PerformPick(rayOrigin, rayDirection);

    if (spPickResult)
    {
        const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
        EE_ASSERT(pPickerResults);
        EE_ASSERT(pPickerResults->GetSize() > 0);

        pickDistance = pPickerResults->GetAt(0)->GetDistance();

        if (pickDistance <= pCamera->GetMinNearPlaneDist())
            pickDistance = 0;

        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
void StandardCameraMode::AdjustOrthoDistance(ToolCamera* pCamera)
{
    const NiPoint3& origin = pCamera->GetTranslate();
    const NiMatrix3& rotation = pCamera->GetRotate();

    // Fit the near and far planes to match the new angle of the camera with the scene.
    const NiBound& sceneBounds = m_pToolSceneService->GetSceneBounds();

    NiPoint3 dir;
    rotation.GetCol(0, dir);

    // project a pt onto the plane tangent to the sphere and our dir
    NiPoint3 tangentPoint = sceneBounds.GetCenter() - sceneBounds.GetRadius() * dir;
    Float32 distance = (tangentPoint - origin).Dot(dir);
    NiPoint3 destination = origin + distance * dir;

    // Only adjust the distance if this is not a user camera
    if (pCamera->m_boundEntity == egf::kENTITY_INVALID)
        pCamera->SetTranslate(destination);

    pCamera->FitNearAndFarToBound(sceneBounds);
    pCamera->Update(0);
}

//-----------------------------------------------------------------------------------------------
void StandardCameraMode::SetupOrbit(efd::SInt32 x, efd::SInt32 y, ToolCamera* pCamera)
{
    if (m_pSelectionService->GetAdapter<EntitySelectionAdapter>()->HasSelection())
    {
        EntitySelectionAdapter* pAdapter = 
            m_pSelectionService->GetAdapter<EntitySelectionAdapter>();
        NiPoint3 center = pAdapter->GetCenter();

        NiPoint3 delta = pCamera->GetTranslate() - center;
        if ((delta.x > 0.1f) || (delta.x < -0.1f) ||
            (delta.y > 0.1f) || (delta.y < -0.1f) ||
            (delta.z > 0.1f) || (delta.z < -0.1f))
        {
            m_orbitCenter = center;
            return;
        }
    }

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    EE_ASSERT(pSurface != NULL);

    // we will pick to figure the appropriate orbit distance
    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    CameraService::MouseToRay(
        (Float32)x,
        (Float32)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        rayOrigin,
        rayDirection);

    PickService::PickRecordPtr spPickResult = 
        m_pPickService->PerformPick(rayOrigin, rayDirection);

    if (spPickResult)
    {
        const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
        EE_ASSERT(pPickerResults);
        EE_ASSERT(pPickerResults->GetSize() > 0);

        NiPick::Record* pPickRecord = pPickerResults->GetAt(0);

        if (pPickRecord)
        {
            m_orbitCenter = pPickRecord->GetIntersection();
        }
    }
    else if (pCamera->GetViewFrustum().m_bOrtho)
    {
        NiBound bound = m_pToolSceneService->GetSceneBounds();
        m_orbitCenter = bound.GetCenter();
    }
    else
    {
        m_orbitCenter = rayOrigin + m_orbitStickDistance * rayDirection;
    }
}

//-----------------------------------------------------------------------------------------------
