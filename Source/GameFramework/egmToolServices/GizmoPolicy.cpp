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

#include "GizmoPolicy.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(GizmoPolicy);

//-----------------------------------------------------------------------------------------------
void GizmoPolicy::AddGizmo(const efd::utf8string& gizmoName, IGizmo* pGizmo)
{
    m_gizmoMap[gizmoName] = pGizmo;
}

//-----------------------------------------------------------------------------------------------
void GizmoPolicy::RemoveGizmo(const efd::utf8string& gizmoName)
{
    m_gizmoMap.erase(gizmoName);
}

//-----------------------------------------------------------------------------------------------