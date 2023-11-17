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

#include "IGizmo.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(IGizmo);

//-----------------------------------------------------------------------------------------------
IGizmo::IGizmo(GizmoAdapter* pAdapter)
    : m_spAdapter(pAdapter)
{
}

//-----------------------------------------------------------------------------------------------
IGizmo::~IGizmo()
{
    m_spAdapter = NULL;
}

//-----------------------------------------------------------------------------------------------
void IGizmo::Connect(Ni3DRenderView* pGizmoView)
{
    EE_UNUSED_ARG(pGizmoView);

    if (m_spAdapter)
        m_spAdapter->OnAdded();
}

//-----------------------------------------------------------------------------------------------
efd::Bool IGizmo::OnTick(efd::TimeType timeElapsed, ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(timeElapsed);
    EE_UNUSED_ARG(pSurface);

    if (m_spAdapter)
        m_spAdapter->OnTick();

    return true;
}

//-----------------------------------------------------------------------------------------------
void IGizmo::Disconnect(Ni3DRenderView* pGizmoView)
{
    EE_UNUSED_ARG(pGizmoView);

    if (m_spAdapter)
        m_spAdapter->OnRemoved();
}

//-----------------------------------------------------------------------------------------------
GizmoAdapter* IGizmo::GetAdapter() const
{
    return m_spAdapter;
}

//-----------------------------------------------------------------------------------------------
void IGizmo::TransformToView(ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pSurface);
}

//-----------------------------------------------------------------------------------------------