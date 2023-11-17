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
#include "CubePropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
CubePropertyVisualizer::CubePropertyVisualizer(ExtraDataPtr spExtraData) :
    m_cubeHandle(SceneGraphService::kInvalidHandle),
    m_solidCubeHandle(SceneGraphService::kInvalidHandle)
{
    m_spExtraData = spExtraData;
}

//-----------------------------------------------------------------------------------------------
void CubePropertyVisualizer::AddGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    UpdateFromEntity(pEntity);

    if (!m_spMesh)
    {
        m_spMesh = NiNew NiMesh();
        PropertyVisualizationHelpers::CreateWireCube(
            m_spMesh,
            m_dimensions.x,
            m_dimensions.y,
            m_dimensions.z);

        m_spMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(m_spMesh);

        m_spMaterialProperty->SetEmittance(m_color);
        m_spMaterialProperty->SetDiffuseColor(m_color);

        // Add solid clickable mesh
        m_spSolidMesh = NiNew NiMesh();
        PropertyVisualizationHelpers::CreateSolidCube(
            m_spSolidMesh,
            m_dimensions.x,
            m_dimensions.y,
            m_dimensions.z);

        m_spSolidMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(
            m_spSolidMesh);
        m_spSolidMaterialProperty->SetAlpha(0.1f);
        NiAlphaPropertyPtr pAlpha = NiNew NiAlphaProperty();
        pAlpha->SetAlphaBlending(true);
        pAlpha->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
        pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        m_spSolidMesh->AttachProperty(pAlpha);
    }

    m_spMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spMesh->SetScale(m_scale);

    if (m_cubeHandle == SceneGraphService::kInvalidHandle)
    {
        m_cubeHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(
            m_spMesh,
            pSceneGraphService);
    }

    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);

    m_spSolidMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spSolidMesh->SetScale(m_scale);

    if (m_solidCubeHandle == SceneGraphService::kInvalidHandle)
    {
        m_solidCubeHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(m_spSolidMesh,
            pSceneGraphService);
    }

    m_spSolidMesh->RecomputeBounds();
    m_spSolidMesh->Update(0.0f);

}

//-----------------------------------------------------------------------------------------------
void CubePropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    UpdateFromEntity(pEntity);

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);

    PropertyVisualizationHelpers::CreateWireCube(
        m_spMesh,
        m_dimensions.x,
        m_dimensions.y,
        m_dimensions.z);

    m_spMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spMesh->SetScale(m_scale);

    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(m_orientation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(m_orientation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(m_orientation.z * -EE_DEGREES_TO_RADIANS);
    m_spMesh->SetRotate(kXRot * kYRot * kZRot);

    m_spMesh->RecomputeBounds();
    m_spMesh->Update(0.0f);

    PropertyVisualizationHelpers::CreateSolidCube(
        m_spSolidMesh,
        m_dimensions.x,
        m_dimensions.y,
        m_dimensions.z);

    m_spSolidMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spSolidMesh->SetScale(m_scale);

    kXRot.MakeXRotation(m_orientation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(m_orientation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(m_orientation.z * -EE_DEGREES_TO_RADIANS);
    m_spSolidMesh->SetRotate(kXRot * kYRot * kZRot);

    m_spSolidMesh->RecomputeBounds();
    m_spSolidMesh->Update(0.0f);
}

//-----------------------------------------------------------------------------------------------
void CubePropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    pSceneGraphService->RemoveSceneGraph(m_cubeHandle);
    m_cubeHandle = SceneGraphService::kInvalidHandle;

    pSceneGraphService->RemoveSceneGraph(m_solidCubeHandle);
    m_solidCubeHandle = SceneGraphService::kInvalidHandle;
}

//-----------------------------------------------------------------------------------------------
PropertyChangeAction CubePropertyVisualizer::UpdateFromEntity(Entity* pEntity)
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
        m_anchor = point;
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

    Point3 orientation;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Orientation",
        "Rotation",
        orientation) == PropertyResult_OK)
    {
        m_orientation = orientation;
        action = PropertyChangeAction_UpdateTransforms;
    }

    Point3 dimensions;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Dimensions",
        "Dimensions",
        dimensions) == PropertyResult_OK)
    {
        m_dimensions = dimensions;
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

//-----------------------------------------------------------------------------------------------
void CubePropertyVisualizer::SetVisibility(bool isVisible)
{
    if (m_spMesh)
        m_spMesh->SetAppCulled(!isVisible);

    if(m_spSolidMesh)
        m_spSolidMesh->SetAppCulled(!isVisible);
}

//-----------------------------------------------------------------------------------------------
