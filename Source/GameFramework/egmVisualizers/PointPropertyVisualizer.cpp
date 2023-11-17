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
#include "PointPropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
PointPropertyVisualizer::PointPropertyVisualizer(ExtraDataPtr pExtraData) :
    m_pointHandle(SceneGraphService::kInvalidHandle)
{
    m_spExtraData = pExtraData;
}
//--------------------------------------------------------------------------------------------------
void PointPropertyVisualizer::AddGeometry(
    SceneGraphService* pSceneGraphService,
    Entity* pEntity)
{
    if (!m_spMesh)
    {
        m_spMesh = NiNew NiMesh();

        PropertyVisualizationHelpers::CreateAsterisk(m_spMesh);

        m_spMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(m_spMesh);

        m_spMaterialProperty->SetEmittance(m_color);
        m_spMaterialProperty->SetDiffuseColor(m_color);
    }

    if (m_pointHandle == SceneGraphService::kInvalidHandle)
    {
        m_pointHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(
            m_spMesh,
            pSceneGraphService);
    }

    UpdateFromEntity(pEntity);

    m_spMesh->SetTranslate(m_point);

    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
void PointPropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    pSceneGraphService->RemoveSceneGraph(m_pointHandle);
    m_pointHandle = SceneGraphService::kInvalidHandle;
}
//--------------------------------------------------------------------------------------------------
void PointPropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    UpdateFromEntity(pEntity);

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);

    m_spMesh->SetTranslate(m_point);
    m_spMesh->Update(0.0f);
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction PointPropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    PropertyChangeAction action = PropertyChangeAction_None;

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

    Point3 point;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "Point",
        "Position",
        point) == PropertyResult_OK)
    {
        m_point = point;
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
        m_point += offset;
        action = PropertyChangeAction_UpdateTransforms;
    }

    return action;
}
//--------------------------------------------------------------------------------------------------
void PointPropertyVisualizer::SetVisibility(bool isVisible)
{
    if (m_spMesh)
        m_spMesh->SetAppCulled(!isVisible);
}
//--------------------------------------------------------------------------------------------------
