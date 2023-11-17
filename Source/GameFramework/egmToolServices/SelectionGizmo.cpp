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

#include "SelectionGizmo.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//-----------------------------------------------------------------------------------------------
SelectionGizmo::SelectionGizmo(efd::ServiceManager* pServiceManager,
                               TransformGizmoAdapter* pAdapter)
    : TransformGizmo(pServiceManager, pAdapter)
{
    m_spGizmo = m_pSceneGraphService->LoadSceneGraph("SelectGizmo.nif");
}

//-----------------------------------------------------------------------------------------------
SelectionGizmo::~SelectionGizmo()
{
}

//-----------------------------------------------------------------------------------------------
efd::Bool SelectionGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);
    EE_UNUSED_ARG(bIsClosest);

    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool SelectionGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(button);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool SelectionGizmo::OnMouseUp(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(button);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    return false;
}

//-----------------------------------------------------------------------------------------------
bool SelectionGizmo::HitTest(
    ecr::RenderSurface* pSurface,
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    float& outHitDistance)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(rayOrigin);
    EE_UNUSED_ARG(rayDirection);
    EE_UNUSED_ARG(outHitDistance);

    return false;
}

//-----------------------------------------------------------------------------------------------