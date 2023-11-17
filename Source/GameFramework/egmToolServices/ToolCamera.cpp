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

#include "ToolCamera.h"

#include <efd/MessageService.h>

#include <egf/EntityManager.h>

#include <NiInputKeyboard.h>

#include <ecrInput/KeyboardMessages.h>
#include <ecrInput/MouseMessages.h>

#include <ecr/RenderService.h>
#include <ecr/RenderSurface.h>
#include <ecr/CameraService.h> // For view-like math functionality

#include "StandardCameraMode.h"

using namespace egf;
using namespace efd;
using namespace ecr;
using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
ToolCamera::ToolCamera()
    : m_zoom(1)
    , m_fov(0.96f)
    , m_boundEntity(egf::kENTITY_INVALID)
    , m_boundEntityDirty(false)
    , m_pRenderSurface(NULL)
{
    // The default MaxFarNearRatio for NiCamera is 10000, but we may need a bigger ratio.
    // 2^16 should be good in many cases.
    SetMaxFarNearRatio(65536.0f);
}
//--------------------------------------------------------------------------------------------------
void ToolCamera::SetFOV(const efd::Float32 fov)
{
    m_fov = fov;
}
//--------------------------------------------------------------------------------------------------
efd::Float32 ToolCamera::GetFOV() const
{
    return m_fov;
}
//--------------------------------------------------------------------------------------------------
void ToolCamera::Zoom(const efd::Float32 dZoom)
{
    m_zoom = m_zoom * dZoom;

    if (GetViewFrustum().m_bOrtho)
        SetViewFrustum(CameraService::OrthoZoom(dZoom, GetViewFrustum()));
}
//--------------------------------------------------------------------------------------------------
