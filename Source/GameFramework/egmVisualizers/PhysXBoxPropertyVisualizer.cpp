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

#include "egmVisualizersPCH.h"
#include "PhysXBoxPropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
PhysXBoxPropertyVisualizer::PhysXBoxPropertyVisualizer(egf::ExtraDataPtr spExtraData) :
    CubePropertyVisualizer(spExtraData)
{
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction PhysXBoxPropertyVisualizer::UpdateFromEntity(egf::Entity* pEntity)
{
    PropertyChangeAction action = CubePropertyVisualizer::UpdateFromEntity(pEntity);

    efd::Bool anchorAtBase = true;
    PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "AnchorAtBase",
        "AnchorAtBase",
        anchorAtBase);

    if (anchorAtBase)
    {
        action = PropertyChangeAction_UpdateGeometry;
        m_anchor.z += m_dimensions.z / 2.0f;
    }

    Point3 orientationOffset;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "OrientationOffset",
        "ShapeRotation",
        orientationOffset) == PropertyResult_OK)
    {
        action = PropertyChangeAction_UpdateTransforms;
        m_orientation += orientationOffset;
    }

    return action;
}
//--------------------------------------------------------------------------------------------------
