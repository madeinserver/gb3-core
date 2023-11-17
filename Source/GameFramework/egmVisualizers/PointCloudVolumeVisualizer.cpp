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

#include "PointCloudVolumeVisualizer.h"

#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//------------------------------------------------------------------------------------------------
PointCloudVolumeVisualizer::PointCloudVolumeVisualizer(ExtraDataPtr spExtraData) :
    CubePropertyVisualizer(spExtraData)
{
    m_divisionHandle = SceneGraphService::kInvalidHandle;
    m_iXDivisions = m_iYDivisions = m_iZDivisions = 0;
    m_spDivisionMesh = NULL;
}
//------------------------------------------------------------------------------------------------
void PointCloudVolumeVisualizer::AddGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    CubePropertyVisualizer::AddGeometry(pSceneGraphService, pEntity);

    if (!m_spDivisionMesh)
    {
        CreateDivisionMesh(pSceneGraphService);
    }
}
//------------------------------------------------------------------------------------------------
void PointCloudVolumeVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    CubePropertyVisualizer::UpdateGeometry(pSceneGraphService, pEntity);

    // check the divisions here so we know if we need to update the geometry or not
    PropertyChangeAction action = PropertyChangeAction_None;
    efd::UInt32 xDivisions;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "XDivisions",
        "No Default",
        xDivisions) == PropertyResult_OK)
    {
        if (xDivisions != m_iXDivisions)
        {
            m_iXDivisions = xDivisions;
            action = PropertyChangeAction_UpdateGeometry;
        }
    }

    efd::UInt32 yDivisions;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "YDivisions",
        "No Default",
        yDivisions) == PropertyResult_OK)
    {
        if (yDivisions != m_iYDivisions)
        {
            m_iYDivisions = yDivisions;
            action = PropertyChangeAction_UpdateGeometry;
        }
    }

    efd::UInt32 zDivisions;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "ZDivisions",
        "No Default",
        zDivisions) == PropertyResult_OK)
    {
        if (zDivisions != m_iZDivisions)
        {
            m_iZDivisions = zDivisions;
            action = PropertyChangeAction_UpdateGeometry;
        }
    }

    // update the divisions here

    // check to see if dimensions updated..
    egf::ExtraDataEntryPtr entry = m_spExtraData->GetEntry("Dimensions");
    if (entry && (entry->m_value != "Default") && pEntity->IsDirty(entry->m_value))
    {
        action = PropertyChangeAction_UpdateGeometry;
    }

    // check to see if Orientation updated..
    entry = m_spExtraData->GetEntry("Orientation");
    if (entry && (entry->m_value != "Default") && pEntity->IsDirty(entry->m_value))
    {
        action = PropertyChangeAction_UpdateGeometry;
    }

    if (action == PropertyChangeAction_UpdateGeometry)
    {
        CreateDivisionMesh(pSceneGraphService);
    }

    // update this all the time, we have no idea what was updated in the base class...
    m_spDivisionsMaterialProperty->SetEmittance(m_color);
    m_spDivisionsMaterialProperty->SetDiffuseColor(m_color);
    m_spDivisionMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spDivisionMesh->SetScale(m_scale);
    m_spDivisionMesh->RecomputeBounds();
    m_spDivisionMesh->Update(0.0f);
}
//------------------------------------------------------------------------------------------------
void PointCloudVolumeVisualizer::CreateDivisionMesh(SceneGraphService* pSceneGraphService)
{
    // delete previous mesh...
    if (m_spDivisionMesh)
    {
        m_spDivisionMesh = NULL;
        m_spDivisionsMaterialProperty = NULL;
        pSceneGraphService->RemoveSceneGraph(m_divisionHandle);
        m_divisionHandle = SceneGraphService::kInvalidHandle;
    }
    m_spDivisionMesh = NiNew NiMesh();

    m_spDivisionMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    efd::UInt32 iXDivisions = m_iXDivisions + 2; // for the ends
    efd::UInt32 iYDivisions = m_iYDivisions + 2;
    efd::UInt32 iZDivisions = m_iZDivisions + 1;

    int iTotalPoints = ((iXDivisions * 2) + (iYDivisions * 2)) * (iZDivisions);

    NiDataStreamRef* pStreamRef = m_spDivisionMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = m_spDivisionMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3, iTotalPoints,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);
    }
    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);
    NiPoint3* pTPoints = pPoints;

    if(!pPoints) {
        m_spDivisionMesh = NULL;
    }

    efd::Point3 vNegHalfSpace = -(m_dimensions / 2.0f);
    efd::Point3 vDelta;
    vDelta.x = m_dimensions.x / (iXDivisions - 1);
    vDelta.y = m_dimensions.y / (iYDivisions - 1);
    vDelta.z = m_dimensions.z / ((iZDivisions == 1) ? iZDivisions : (iZDivisions - 1));

    efd::Point3 tPos;
    efd::Point3 vTDelta;

    NiMatrix3 mRotation;
    mRotation.FromEulerAnglesXYZ(
        m_orientation.x * -EE_DEGREES_TO_RADIANS, 
        m_orientation.y * -EE_DEGREES_TO_RADIANS, 
        m_orientation.z * -EE_DEGREES_TO_RADIANS);

    // create the grid of lines
    for (efd::UInt32 z = 0; z < iZDivisions; z++)
    {
        vTDelta.z = vDelta.z * z;

        for (efd::UInt32 x = 0; x < iXDivisions; x++)
        {
            vTDelta.x = vDelta.x * x;
            vTDelta.y = 0.0f;

            // -y point
            // position in local space of object
            tPos = vTDelta + vNegHalfSpace;
            // rotate the local space position
            tPos = mRotation * tPos;

            // +y point
            // write pos and increment
            *pTPoints++ = tPos;

            tPos = vTDelta + vNegHalfSpace;
            tPos.y = -vNegHalfSpace.y;

            tPos = mRotation * tPos;
            // write pos and increment
            *pTPoints++ = tPos;
        }

        for (efd::UInt32 y = 0; y < iYDivisions; y++)
        {
            vTDelta.y = vDelta.y * y;
            vTDelta.x = 0.0f;

            // -x point
            // position in local space of object
            tPos = vTDelta + vNegHalfSpace;
            // rotate the local space position
            tPos = mRotation * tPos;

            // write pos and increment
            *pTPoints++ = tPos;


            // +x point
            tPos = vTDelta + vNegHalfSpace;
            tPos.x = -vNegHalfSpace.x;

            tPos = mRotation * tPos;

            // write pos and increment
            *pTPoints++ = tPos;
        }
    }

    pStream->Unlock(NiDataStream::LOCK_WRITE);

    m_spDivisionsMaterialProperty = PropertyVisualizationHelpers::AttachDefaultMaterial(
        m_spDivisionMesh);
    m_spDivisionsMaterialProperty->SetEmittance(m_color);
    m_spDivisionsMaterialProperty->SetDiffuseColor(m_color);
    m_spDivisionMesh->SetTranslate(m_anchor.x, m_anchor.y, m_anchor.z);
    m_spDivisionMesh->SetScale(m_scale);
    m_spDivisionMesh->RecomputeBounds();
    m_spDivisionMesh->Update(0.0f);

    if (m_divisionHandle == SceneGraphService::kInvalidHandle)
    {
        m_divisionHandle = PropertyVisualizationHelpers::AddMeshToSceneGraphService(
            m_spDivisionMesh,
            pSceneGraphService);
    }
}
//------------------------------------------------------------------------------------------------
void PointCloudVolumeVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    pSceneGraphService->RemoveSceneGraph(m_divisionHandle);
    m_divisionHandle = SceneGraphService::kInvalidHandle;

    CubePropertyVisualizer::RemoveGeometry(pSceneGraphService);
}
//------------------------------------------------------------------------------------------------
PropertyChangeAction PointCloudVolumeVisualizer::UpdateFromEntity(Entity* pEntity)
{
    PropertyChangeAction action = CubePropertyVisualizer::UpdateFromEntity(pEntity);

    // not checking the data here because i need to know if we need to update the geometry or not

    return action;
}
//------------------------------------------------------------------------------------------------
void PointCloudVolumeVisualizer::SetVisibility(bool isVisible)
{
    if(m_spDivisionMesh)
        m_spDivisionMesh->SetAppCulled(!isVisible);

    CubePropertyVisualizer::SetVisibility(isVisible);
}
//------------------------------------------------------------------------------------------------
