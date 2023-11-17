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

#include "PanCameraMode.h"

#include <efd/MemTracker.h>
#include <efd/MessageService.h>
#include <efd/ParseHelper.h>

#include <egf/EntityManager.h>

#include <ecr/RenderSurface.h>
#include <ecr/CameraService.h> // For view-like math functionality

#include "ToolCameraService.h"
#include "ToolServicesMessages.h"

using namespace egf;
using namespace efd;
using namespace egmToolServices;
using namespace ecrInput;
using namespace ecr;

//--------------------------------------------------------------------------------------------------
PanCameraMode::PanCameraMode(efd::ServiceManager* pServiceManager)
    : StandardCameraMode(pServiceManager)
{
}
//--------------------------------------------------------------------------------------------------
PanCameraMode::~PanCameraMode()
{
}
//--------------------------------------------------------------------------------------------------
efd::Bool PanCameraMode::OnMouseMove(
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    ToolCamera* pCamera)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    const NiMatrix3 rotation = pCamera->GetRotate();

    Float32 panSpeedX = (Float32)(dx) * m_panSpeedX;
    Float32 panSpeedY = (Float32)(dy) * m_panSpeedY;

    if (m_pCameraService->GetIsLooking() || m_pCameraService->GetIsPanning())
    {
        m_pRenderService->InvalidateRenderContexts();

        NiPoint3 newTranslation = ecr::CameraService::Pan(
            panSpeedX,
            panSpeedY,
            pCamera->GetTranslate(),
            rotation);

        pCamera->SetTranslate(newTranslation);
    }
    else
    {
        return false;
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
efd::Bool PanCameraMode::OnMouseDown(
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

    const NiFrustum& frustum = pCamera->GetViewFrustum();

    float pickDistance;
    Bool pickHit = GetPickDistance(x, y, pickDistance);

    if ((pickHit) && (!frustum.m_bOrtho))
    {
        RenderSurface* pActiveSurface = m_pRenderService->GetActiveRenderSurface();

        // if there was a pick result, pan proportional to the distance
        m_panSpeedX = pickDistance /
            (float)pActiveSurface->GetRenderTargetGroup()->GetWidth(0);
        m_panSpeedY = pickDistance /
            (float)pActiveSurface->GetRenderTargetGroup()->GetHeight(0);
    }
    else if (pickHit)
    {
        RenderSurface* pActiveSurface = m_pRenderService->GetActiveRenderSurface();

        // if there was a pick result in ortho pan proportional to the frustum width
        m_panSpeedX = frustum.m_fRight * 2.0f /
            (float)pActiveSurface->GetRenderTargetGroup()->GetWidth(0);
        m_panSpeedY =  frustum.m_fTop * 2.0f /
            (float)pActiveSurface->GetRenderTargetGroup()->GetHeight(0);
    }
    else
    {
        m_panSpeedX = m_mousePanScalar;
        m_panSpeedY = m_mousePanScalar;
    }

    m_panSpeedX *= m_pCameraService->GetLookScale();
    m_panSpeedY *= m_pCameraService->GetLookScale();

    m_cameraStart = pCamera->GetTranslate();

    return true;
}
//--------------------------------------------------------------------------------------------------
efd::Bool PanCameraMode::OnMouseUp(
    MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    ToolCamera* pCamera)
{
    return StandardCameraMode::OnMouseUp(button, x, y, pCamera);
}
//--------------------------------------------------------------------------------------------------
