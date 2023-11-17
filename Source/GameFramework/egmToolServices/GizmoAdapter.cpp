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

#include "GizmoAdapter.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(GizmoAdapter);

//-----------------------------------------------------------------------------------------------
GizmoAdapter::GizmoAdapter()
    : m_transforming(false)
{
}

//-----------------------------------------------------------------------------------------------
void GizmoAdapter::OnAdded()
{
}

//-----------------------------------------------------------------------------------------------
void GizmoAdapter::OnTick()
{
}

//-----------------------------------------------------------------------------------------------
void GizmoAdapter::OnRemoved()
{
}

//-----------------------------------------------------------------------------------------------
void GizmoAdapter::BeginTransform()
{
    m_transforming = true;
}

//-----------------------------------------------------------------------------------------------
void GizmoAdapter::EndTransform(bool cancel)
{
    EE_UNUSED_ARG(cancel);

    m_transforming = false;
}

//-----------------------------------------------------------------------------------------------