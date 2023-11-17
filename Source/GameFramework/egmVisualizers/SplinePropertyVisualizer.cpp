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

#include "SplinePropertyVisualizer.h"

#include <egf/ExtraData.h>
#include <egf/PrimitiveProperties.h>

#include "PropertyVisualizationHelpers.h"

using namespace ecr;
using namespace egf;
using namespace efd;
using namespace egmVisualizers;

//-----------------------------------------------------------------------------------------------
SplinePropertyVisualizer::SplinePropertyVisualizer(
    ExtraDataPtr pExtraData,
    EntityManager* pEntityManager,
    VisualizerConnectionTracker* pConnectionTracker,
    VisualizerVisibilityTracker* pVisibilityTracker) :

    m_spExtraData(pExtraData),
    m_spLineMaterialProperty(NULL),
    m_spPointMaterialProperty(NULL),
    m_owner(kENTITY_INVALID),
    m_lineColor(1,1,1),
    m_pointColor(1,1,1),
    m_pEntityManager(pEntityManager),
    m_pConnectionTracker(pConnectionTracker),
    m_pVisibilityTracker(pVisibilityTracker)
{
    EE_ASSERT(pConnectionTracker);
    EE_ASSERT(pVisibilityTracker);
}

//-----------------------------------------------------------------------------------------------
void SplinePropertyVisualizer::AddGeometry(SceneGraphService* pSceneGraphService, Entity* pEntity)
{
    m_owner = pEntity->GetEntityID();

    m_pSceneGraphService = pSceneGraphService;

    // Create a default material; don't use PropertyVisualizationHelpers helper for this, since this
    // material needs to be shared among all the connections
    m_spLineMaterialProperty = NiNew NiMaterialProperty();
    m_spLineMaterialProperty->SetAmbientColor(NiColor::WHITE);
    m_spLineMaterialProperty->SetDiffuseColor(NiColor::WHITE);
    m_spLineMaterialProperty->SetSpecularColor(NiColor::WHITE);
    m_spLineMaterialProperty->SetEmittance(NiColor::WHITE);
    m_spLineMaterialProperty->SetShineness(0.0f);
    m_spLineMaterialProperty->SetAlpha(1.0f);

    m_spPointMaterialProperty = NiNew NiMaterialProperty();
    m_spPointMaterialProperty->SetAmbientColor(NiColor::WHITE);
    m_spPointMaterialProperty->SetDiffuseColor(NiColor::WHITE);
    m_spPointMaterialProperty->SetSpecularColor(NiColor::WHITE);
    m_spPointMaterialProperty->SetEmittance(NiColor::WHITE);
    m_spPointMaterialProperty->SetShineness(0.0f);
    m_spPointMaterialProperty->SetAlpha(1.0f);

    // Defer most work normally done here to UpdateGeometry, which should be called after this.
}

//-----------------------------------------------------------------------------------------------
void SplinePropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    EE_UNUSED_ARG(pSceneGraphService);

    for (ConnectionMap::iterator itr = m_connections.begin(); itr != m_connections.end(); itr ++)
    {
        SplineData* data = &(itr->second);
        if (data->m_meshHandle != SceneGraphService::kInvalidHandle)
            pSceneGraphService->RemoveSceneGraph(data->m_meshHandle);
        if (data->m_connectionLineHandle != SceneGraphService::kInvalidHandle)
            pSceneGraphService->RemoveSceneGraph(data->m_connectionLineHandle);
    }

    m_connections.clear();
    m_oldConnections.clear();
}

//-----------------------------------------------------------------------------------------------
void SplinePropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    PropertyChangeAction action = UpdateFromEntity(pEntity);

    if (m_connections.size() <= 1)
    {
        return;
    }

    // Tear down and recreate all the geometry when the list changes
    if (action == PropertyChangeAction_UpdateGeometry)
    {
        // Recreate current meshes;  UpdateFromEntity should've handled freeing the old ones.
        ConnectionMap::iterator itr = m_connections.begin();
        ConnectionMap::iterator firstItr = itr;
        int iLast = m_connections.size();
        
        // TODO NDarnell add back the mesh linking capability
        //Entity* pOwner = m_pEntityManager->LookupEntity(m_owner);

        for (ConnectionMap::iterator quickItr = m_connections.begin();
             quickItr != m_connections.end();
             quickItr++)
        {
            int iTemp = atoi(quickItr->first.c_str());
            if(iTemp > iLast)
            {
                iLast = iTemp;
            }
        }

        do
        {
            int iFirst = atoi(firstItr->first.c_str());
            int iNext = iFirst;
            char nextIndex[128];

            // find the first next valid index
            do
            {
                iNext++;
                efd::Sprintf(nextIndex, 128, "%i", iNext);
            }
            while (m_connections.count(nextIndex) == 0 && iNext < iLast);

            if (iNext == iLast)
                break;

            itr = m_connections.find(nextIndex);

            if (!firstItr->second.m_spMesh)
            {
                firstItr->second.m_spMesh = NiNew NiMesh();
                PropertyVisualizationHelpers::CreateWireSphere(
                    firstItr->second.m_spMesh, 1.0f, 8, 8);

                firstItr->second.m_spMesh->AttachProperty(m_spPointMaterialProperty);
            }

            if (!firstItr->second.m_spLineMesh)
            {
                firstItr->second.m_spLineMesh = NiNew NiMesh();
                PropertyVisualizationHelpers::CreateThickLine(firstItr->second.m_spLineMesh, 
                    efd::Point3(0.0f, 0.0f, 0.0f),
                    (itr->second.m_point - firstItr->second.m_point),
                    0.4f);

                firstItr->second.m_spLineMesh->AttachProperty(m_spLineMaterialProperty);
            }


            firstItr->second.m_spLineMesh->SetTranslate(firstItr->second.m_point);
            firstItr->second.m_spMesh->SetTranslate(firstItr->second.m_point);

            if (firstItr->second.m_meshHandle == SceneGraphService::kInvalidHandle)
            {
                firstItr->second.m_meshHandle = 
                    PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                    firstItr->second.m_spMesh, pSceneGraphService);
            }

            if (firstItr->second.m_connectionLineHandle == SceneGraphService::kInvalidHandle)
            {
                firstItr->second.m_connectionLineHandle = 
                    PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                    firstItr->second.m_spLineMesh, pSceneGraphService);
            }

            firstItr->second.m_spLineMesh->RecomputeBounds();
            firstItr->second.m_spLineMesh->Update(0.0f);

            firstItr->second.m_spMesh->RecomputeBounds();
            firstItr->second.m_spMesh->Update(0.0f);

            firstItr = itr;
        }
        while (itr != m_connections.end());

        // update the last point with a visualizer
        if(!itr->second.m_spMesh)
        {
            itr->second.m_spMesh = NiNew NiMesh();
            itr->second.m_spMesh->AttachProperty(m_spPointMaterialProperty);
        }

        PropertyVisualizationHelpers::CreateWireSphere(itr->second.m_spMesh, 1.0f, 8, 8);
        itr->second.m_spMesh->SetTranslate(itr->second.m_point);

        if (firstItr->second.m_meshHandle == SceneGraphService::kInvalidHandle)
        {
            firstItr->second.m_meshHandle = 
                PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                firstItr->second.m_spMesh, pSceneGraphService);
        }

        itr->second.m_spMesh->RecomputeBounds();
        itr->second.m_spMesh->Update(0.0f);

        SetVisibility(true);
    }

    // If the targets changed, we don't need to tear down the geometry,
    // just change the vertices.
    // This is a different meaning of UpdateTransforms than we're used
    // to, but with lines it's the closest equivalent
    else if (action == PropertyChangeAction_UpdateTransforms)
    {
        ConnectionMap::iterator itr = m_connections.begin();
        ConnectionMap::iterator firstItr = itr;
        int iLast = m_connections.size();

        for (ConnectionMap::iterator quickItr = m_connections.begin();
             quickItr != m_connections.end();
             quickItr++)
        {
            int iTemp = atoi(quickItr->first.c_str());
            if(iTemp > iLast)
            {
                iLast = iTemp;
            }
        }

        do
        {
            int iFirst = atoi(firstItr->first.c_str());
            int iNext = iFirst;
            char nextIndex[128];

            // find the first next valid index
            do
            {
                iNext++;
                efd::Sprintf(nextIndex, 128, "%i", iNext);
            }
            while (m_connections.count(nextIndex) == 0 && iNext < iLast);

            if (iNext == iLast)
                break;

            itr = m_connections.find(nextIndex);

            // dont need to re-create the sphere, just change the translation...
            // .. could probably do this for the points too if this becomes any bit of a
            // bottleneck
            firstItr->second.m_spMesh->SetTranslate(firstItr->second.m_point);
            PropertyVisualizationHelpers::CreateThickLine(firstItr->second.m_spLineMesh, 
                efd::Point3(0.0f, 0.0f, 0.0f),
                (itr->second.m_point - firstItr->second.m_point),
                0.4f);

            firstItr->second.m_spLineMesh->SetTranslate(firstItr->second.m_point);

            firstItr->second.m_spLineMesh->RecomputeBounds();
            firstItr->second.m_spLineMesh->Update(0.0f);

            firstItr->second.m_spMesh->RecomputeBounds();
            firstItr->second.m_spMesh->Update(0.0f);

            firstItr = itr;
        }
        while(itr != m_connections.end());

        // update the last point with a visualizer
        itr->second.m_spMesh->SetTranslate(itr->second.m_point);
        itr->second.m_spMesh->RecomputeBounds();
        itr->second.m_spMesh->Update(0.0f);

        SetVisibility(true);
    }

    m_spLineMaterialProperty->SetEmittance(m_lineColor);
    m_spLineMaterialProperty->SetDiffuseColor(m_lineColor);

    m_spPointMaterialProperty->SetEmittance(m_pointColor);
    m_spPointMaterialProperty->SetDiffuseColor(m_pointColor);
}

//-----------------------------------------------------------------------------------------------
PropertyChangeAction SplinePropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    EE_UNUSED_ARG(pEntity);

    Entity* pOwner = m_pEntityManager->LookupEntity(m_owner);
    EE_ASSERT(pOwner);

    PropertyChangeAction action = UpdateConnectionList(pOwner, pEntity);

    m_position = Point3(0.0f, 0.0f, 0.0f);
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Offset",
        "No Default",
        m_position) == PropertyResult_OK)
    {
        if (action != PropertyChangeAction_UpdateGeometry)
            action = PropertyChangeAction_UpdateTransforms;
    }

    for (ConnectionMap::iterator itr = m_connections.begin(); itr != m_connections.end(); itr++)
    {
        itr->second.m_point += m_position;
    }

    Color color;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Line Color",
        "No Default",
        color) == PropertyResult_OK)
    {
        m_lineColor = color;
    }
    else
    {
        m_lineColor = Color(1.0f, 1.0f, 1.0f);
    }

    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Point Color",
        "No Default",
        color) == PropertyResult_OK)
    {
        m_pointColor = color;
    }
    else
    {
        m_pointColor = Color(1.0f, 1.0f, 1.0f);
    }

    return action;
}

//-----------------------------------------------------------------------------------------------
void SplinePropertyVisualizer::SetVisibility(bool isVisible)
{
    isVisible = m_pVisibilityTracker->IsEntityVisible(m_owner);

    // All connections become invisible
    for (ConnectionMap::iterator itr = m_connections.begin();
        itr != m_connections.end();
        itr++)
    {
        if (itr->second.m_spMesh)
            itr->second.m_spMesh->SetAppCulled(!isVisible);
        if (itr->second.m_spLineMesh)
            itr->second.m_spLineMesh->SetAppCulled(!isVisible);
    }
}

//-----------------------------------------------------------------------------------------------
PropertyChangeAction SplinePropertyVisualizer::UpdateConnectionList(Entity* pOwner,
    Entity* pEntity)
{
    EE_UNUSED_ARG(pEntity);
    EE_ASSERT(m_pSceneGraphService);

    PropertyChangeAction action = PropertyChangeAction_None;
    if (m_pConnectionTracker->IsEntityBeingRemoved(pEntity->GetEntityID()))
        return action;

    efd::Point3 tPoint;
    egf::PropertyResult result = PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Point List",
        "Point List",
        tPoint);

    if (result == PropertyResult_OK)
    {
        action = PropertyChangeAction_UpdateGeometry;
    }
    else if (result == PropertyResult_PropertyNotScalar)
    {
        efd::map<utf8string, efd::Point3> connections;

        if (PropertyVisualizationHelpers::GetAssocArrayDependency(
            m_spExtraData,
            pOwner,
            "Point List",
            "Point List",
            connections) == PropertyResult_OK)
        {
            // only do this if the list has actually changed, otherwise
            // just update transforms and spline sub-meshes
            if(m_oldConnections.size() != connections.size()) 
            {
                m_oldConnections = connections;

                // Free old geometry & connections and prepare for recreation
                for (ConnectionMap::iterator itr = m_connections.begin();
                     itr != m_connections.end();
                     itr++)
                {
                    if(itr->second.m_meshHandle != SceneGraphService::kInvalidHandle)
                        m_pSceneGraphService->RemoveSceneGraph(itr->second.m_meshHandle);
                    if(itr->second.m_connectionLineHandle != SceneGraphService::kInvalidHandle)
                        m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionLineHandle);
                }

                // Clear the list, which will free the meshes
                m_connections.clear();

                // Setup the new connection map
                for (efd::map<utf8string, efd::Point3>::iterator itr = connections.begin();
                     itr != connections.end();
                     itr++)
                {
                    m_connections[itr->first].m_point = itr->second;

                    m_connections[itr->first].m_meshHandle = SceneGraphService::kInvalidHandle;
                    m_connections[itr->first].m_connectionLineHandle =
                        SceneGraphService::kInvalidHandle;
                }

                action = PropertyChangeAction_UpdateGeometry;
            }
            else
            {
                m_oldConnections = connections;

                // Setup the new connection map
                for (efd::map<utf8string, efd::Point3>::iterator itr = connections.begin();
                     itr != connections.end();
                     itr++)
                {
                    m_connections[itr->first].m_point = itr->second;
                }

                action = PropertyChangeAction_UpdateTransforms;
            }
        }
    }

    return action;
}
//-----------------------------------------------------------------------------------------------
