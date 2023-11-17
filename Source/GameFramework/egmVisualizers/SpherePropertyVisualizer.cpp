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
#include "SpherePropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
SpherePropertyVisualizer::SpherePropertyVisualizer(ExtraDataPtr spExtraData) :
    m_sphereHandle(SceneGraphService::kInvalidHandle)
{
    m_spExtraData = spExtraData;
}
//--------------------------------------------------------------------------------------------------
void SpherePropertyVisualizer::AddGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    UpdateFromEntity(pEntity);

    if (!m_spMesh)
    {
        m_spMesh = NiNew NiMesh();
        PropertyVisualizationHelpers::CreateWireSphere(m_spMesh, 1.0f, 16, 16);
        m_spMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(m_spMesh);

        m_spMaterialProperty->SetEmittance(m_color);
        m_spMaterialProperty->SetDiffuseColor(m_color);
    }

    m_spMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spMesh->SetScale(m_scale * m_radius);

    if (m_sphereHandle == SceneGraphService::kInvalidHandle)
    {
        m_sphereHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(
            m_spMesh,
            pSceneGraphService);
    }

    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
void SpherePropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    pSceneGraphService->RemoveSceneGraph(m_sphereHandle);
    m_sphereHandle = SceneGraphService::kInvalidHandle;
}
//--------------------------------------------------------------------------------------------------
void SpherePropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    UpdateFromEntity(pEntity);

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);

    m_spMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spMesh->SetScale(m_scale * m_radius);
    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction SpherePropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    PropertyChangeAction action = PropertyChangeAction_None;

    Point3 point;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Anchor",
        "Position",
        point) == PropertyResult_OK)
    {
        m_anchor = NiPoint3(point.x, point.y, point.z);
        action = PropertyChangeAction_UpdateTransforms;
    }

    Point3 offset;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Offset",
        "No Default",
        offset) == PropertyResult_OK)
    {
        m_anchor += offset;
        action = PropertyChangeAction_UpdateTransforms;
    }

    efd::Float32 radius;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Radius",
        "Radius",
        radius) == PropertyResult_OK && radius >= 0.0f)
    {
        m_radius = radius;
        action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        // crash prevention
        m_radius = 0.0f;
    }

    efd::Float32 scale;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Scale",
        "No Default",
        scale) == PropertyResult_OK && scale >= 0.0f)
    {
        m_scale = scale;
        action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        // crash prevention
        m_scale = 1.0f;
    }

    Color color;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Color",
        "No Default",
        color) == PropertyResult_OK)
    {
        m_color = color;
    }
    else
    {
        m_color = Color(1.0f, 1.0f, 1.0f);
    }

    return action;
}
//--------------------------------------------------------------------------------------------------
void SpherePropertyVisualizer::SetVisibility(bool isVisible)
{
    if (m_spMesh)
    {
        m_spMesh->SetAppCulled(!isVisible);
    }
}
//--------------------------------------------------------------------------------------------------
