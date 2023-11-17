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
#include "PropertyVisualizerFactory.h"
#include <ecr/RenderService.h>
#include <egf/ExtraData.h>

#include "PointPropertyVisualizer.h"
#include "SpherePropertyVisualizer.h"
#include "RadiusPropertyVisualizer.h"
#include "CubePropertyVisualizer.h"
#include "PointCloudVolumeVisualizer.h"
#include "FrustumPropertyVisualizer.h"
#include "ConnectionPropertyVisualizer.h"
#include "EntitySplinePropertyVisualizer.h"
#include "SplinePropertyVisualizer.h"
#include "PhysXBoxPropertyVisualizer.h"
#include "AttenuationPropertyVisualizer.h"

using namespace efd;
using namespace egf;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
IPropertyVisualizerPtr PropertyVisualizerFactory::CreatePropertyVisualizer(
        egf::ExtraDataPtr spVisualizerExtraData,
        egf::EntityManager* pEntityManager,
        VisualizerConnectionTracker* pConnectionTracker,
        VisualizerVisibilityTracker* pVisibilityTracker)
{
    EE_ASSERT(pConnectionTracker);
    EE_ASSERT(pVisibilityTracker);

    ExtraDataEntry* typeEntry = spVisualizerExtraData->GetEntry("Type");
    utf8string type = typeEntry->m_value;

    if (type == "Point")
    {
        return NiNew PointPropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Sphere")
    {
        return NiNew SpherePropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Radius")
    {
        return NiNew RadiusPropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Cube")
    {
        return NiNew CubePropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Frustum")
    {
        return NiNew FrustumPropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Connection")
    {
        return NiNew ConnectionPropertyVisualizer(spVisualizerExtraData,
            pEntityManager, pConnectionTracker, pVisibilityTracker);
    }
    if (type == "Spline")
    {
        return NiNew SplinePropertyVisualizer(spVisualizerExtraData, 
            pEntityManager, pConnectionTracker, pVisibilityTracker);
    }
    if (type == "EntitySpline")
    {
        return NiNew EntitySplinePropertyVisualizer(spVisualizerExtraData, 
            pEntityManager, pConnectionTracker, pVisibilityTracker);
    }
    if (type == "PointCloud")
    {
        return NiNew PointCloudVolumeVisualizer(spVisualizerExtraData);
    }
    if (type == "PhysXBox")
    {
        return NiNew PhysXBoxPropertyVisualizer(spVisualizerExtraData);
    }
    if (type == "Attenuation")
    {
        return NiNew AttenuationPropertyVisualizer(spVisualizerExtraData);
    }

    return NULL;
}
//--------------------------------------------------------------------------------------------------
