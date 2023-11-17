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

#include <efd/MemTracker.h>
#include <efd/MessageService.h>
#include <egf/EntityManager.h>

#include <ecr/CameraService.h>

#include "ToolCameraService.h"
#include "OrbitCameraMode.h"

using namespace egf;
using namespace efd;
using namespace egmToolServices;
using namespace ecrInput;

//--------------------------------------------------------------------------------------------------
OrbitCameraMode::OrbitCameraMode(efd::ServiceManager* pServiceManager)
    : StandardCameraMode(pServiceManager)
{
    m_mouseLookScalar = 0.01f;
}
//--------------------------------------------------------------------------------------------------
OrbitCameraMode::~OrbitCameraMode()
{

}
//--------------------------------------------------------------------------------------------------
efd::Bool OrbitCameraMode::OnMouseMove(
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    ToolCamera* pCamera)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    if (m_pCameraService->GetIsLooking())
    {
        Float32 lookScalar = m_mouseLookScalar * m_pCameraService->GetLookScale();

        NiPoint3 newLocation;
        NiMatrix3 newRotation;
        ecr::CameraService::Orbit(
            (Float32)dx * lookScalar,
            (Float32)dy * lookScalar,
            pCamera->GetTranslate(),
            pCamera->GetRotate(),
            m_orbitCenter,
            m_upAxis,
            newLocation,
            newRotation);

        pCamera->SetTranslate(newLocation);
        pCamera->SetRotate(newRotation);

        if (pCamera->GetViewFrustum().m_bOrtho)
        {
            AdjustOrthoDistance(pCamera);
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
    else
    {
        return false;
    }

    m_pRenderService->InvalidateRenderContexts();

    return true;
}
//--------------------------------------------------------------------------------------------------
efd::Bool OrbitCameraMode::OnMouseDown(
    MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    ToolCamera* pCamera)
{
    if (!StandardCameraMode::OnMouseDown(button, x, y, pCamera))
        return false;

    m_pRenderService->InvalidateRenderContexts();

    SetupOrbit(x, y, pCamera);

    if (pCamera->GetViewFrustum().m_bOrtho)
        AdjustOrthoDistance(pCamera);

    return true;
}
//--------------------------------------------------------------------------------------------------
efd::Bool OrbitCameraMode::OnMouseUp(
    MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    ToolCamera* pCamera)
{
    return StandardCameraMode::OnMouseUp(button, x, y, pCamera);
}
//--------------------------------------------------------------------------------------------------
