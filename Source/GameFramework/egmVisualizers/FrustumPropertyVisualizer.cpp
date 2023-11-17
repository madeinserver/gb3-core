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
#include "FrustumPropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
FrustumPropertyVisualizer::FrustumPropertyVisualizer(egf::ExtraDataPtr spExtraData) :
    m_frustumHandle(SceneGraphService::kInvalidHandle)
{
    m_spExtraData = spExtraData;

    m_near = 1.0f;
    m_far = 100.0f;
    m_fov = 60.0f;
}
//--------------------------------------------------------------------------------------------------
void FrustumPropertyVisualizer::AddGeometry(SceneGraphService* pSceneGraphService, Entity* pEntity)
{
    UpdateFromEntity(pEntity);

    if (!m_spMesh)
    {
        m_spMesh = NiNew NiMesh();
        PropertyVisualizationHelpers::CreateFrustum(m_spMesh, m_fov, m_near, m_far, 1.3f);

        m_spMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(m_spMesh);

        m_spMaterialProperty->SetEmittance(m_color);
        m_spMaterialProperty->SetDiffuseColor(m_color);
    }

    UpdateGeometry(pSceneGraphService, pEntity);

    if (m_frustumHandle == SceneGraphService::kInvalidHandle)
    {
        m_frustumHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(
            m_spMesh,
            pSceneGraphService);
    }

    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
void FrustumPropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    pSceneGraphService->RemoveSceneGraph(m_frustumHandle);
    m_frustumHandle = SceneGraphService::kInvalidHandle;
}
//--------------------------------------------------------------------------------------------------
void FrustumPropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    PropertyChangeAction action = UpdateFromEntity(pEntity);

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);

    if (action == PropertyChangeAction_UpdateGeometry)
    {
        PropertyVisualizationHelpers::CreateFrustum(m_spMesh, m_fov, m_near, m_far, 1.3f);
    }

    m_spMesh->SetTranslate(m_position.x, m_position.y, m_position.z);
    m_spMesh->SetScale(m_scale);

    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(m_rotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(m_rotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(m_rotation.z * -EE_DEGREES_TO_RADIANS);
    m_spMesh->SetRotate(kXRot * kYRot * kZRot);

    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction FrustumPropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    PropertyChangeAction action = PropertyChangeAction_None;

    Point3 position;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Anchor",
        "Position",
        position) == PropertyResult_OK)
    {
        m_position = position;
        action = PropertyChangeAction_UpdateTransforms;
    }

    Point3 rotation;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Orientation",
        "Rotation",
        rotation) == PropertyResult_OK)
    {
        m_rotation = rotation;
        action = PropertyChangeAction_UpdateTransforms;
    }

    efd::Float32 fov;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "FOV",
        "FOV",
        fov) == PropertyResult_OK)
    {
        m_fov = fov;
        action = PropertyChangeAction_UpdateGeometry;
    }

    efd::Float32 neardist;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Near",
        "NearPlane",
        neardist) == PropertyResult_OK)
    {
        m_near = neardist;
        action = PropertyChangeAction_UpdateGeometry;
    }

    efd::Float32 fardist;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Far",
        "FarPlane",
        fardist) == PropertyResult_OK)
    {
        m_far = fardist;
        action = PropertyChangeAction_UpdateGeometry;
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
        " ",
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
void FrustumPropertyVisualizer::SetVisibility(bool isVisible)
{
    if (m_spMesh)
        m_spMesh->SetAppCulled(!isVisible);
}
//--------------------------------------------------------------------------------------------------
