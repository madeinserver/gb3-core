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

#include "EntitySplinePropertyVisualizer.h"

#include <egf/ExtraData.h>
#include <egf/PrimitiveProperties.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>

using namespace ecr;
using namespace efd;
using namespace egf;
using namespace egmVisualizers;

//------------------------------------------------------------------------------------------------
EntitySplinePropertyVisualizer::EntitySplinePropertyVisualizer(
    ExtraDataPtr pExtraData,
    EntityManager* pEntityManager,
    VisualizerConnectionTracker* pConnectionTracker,
    VisualizerVisibilityTracker* pVisibilityTracker)
    : m_spExtraData(pExtraData)
    , m_spMaterialProperty(NULL)
    , m_owner(kENTITY_INVALID)
    , m_color(0, 0, 0)
    , m_pEntityManager(pEntityManager)
    , m_pConnectionTracker(pConnectionTracker)
    , m_pVisibilityTracker(pVisibilityTracker)
{
    EE_ASSERT(pConnectionTracker);
    EE_ASSERT(pVisibilityTracker);
}

//------------------------------------------------------------------------------------------------
void EntitySplinePropertyVisualizer::AddGeometry(
    SceneGraphService* pSceneGraphService,
    Entity* pEntity)
{
    m_owner = pEntity->GetEntityID();

    m_pSceneGraphService = pSceneGraphService;

    // Create a default material; don't use PropertyVisualizationHelpers helper for this, since 
    // this material needs to be shared among all the connections
    m_spMaterialProperty = NiNew NiMaterialProperty();
    m_spMaterialProperty->SetAmbientColor(NiColor::WHITE);
    m_spMaterialProperty->SetDiffuseColor(NiColor::WHITE);
    m_spMaterialProperty->SetSpecularColor(NiColor::WHITE);
    m_spMaterialProperty->SetEmittance(NiColor::WHITE);
    m_spMaterialProperty->SetShineness(0.0f);
    m_spMaterialProperty->SetAlpha(1.0f);

    // Defer most work normally done here to UpdateGeometry, which should be called after this.
}

//------------------------------------------------------------------------------------------------
void EntitySplinePropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    EE_UNUSED_ARG(pSceneGraphService);

    for (ConnectionMap::iterator itr = m_connections.begin(); itr != m_connections.end(); ++itr)
    {
        m_pConnectionTracker->RemoveConnection(itr->second.m_entityID, this);
        if (itr->second.m_connectionHandle != SceneGraphService::kInvalidHandle)
            pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionHandle);
    }

    m_connections.clear();
    m_oldConnections.clear();
}

//------------------------------------------------------------------------------------------------
void EntitySplinePropertyVisualizer::UpdateGeometry(
    SceneGraphService* pSceneGraphService,
    egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    PropertyChangeAction action = UpdateFromEntity(pEntity);

    if (m_connections.size() <= 1)
        return;

    // Tear down and recreate all the geometry when the list changes
    if (action == PropertyChangeAction_UpdateGeometry)
    {
        // Recreate current meshes;  UpdateFromEntity should've handled freeing the old ones.
        ConnectionMap::iterator itr = m_connections.begin();
        ConnectionMap::iterator firstItr = itr;
        int iLast = m_connections.size();

        for(ConnectionMap::iterator quickItr = m_connections.begin();
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

                firstItr->second.m_spMesh->AttachProperty(m_spMaterialProperty);
            }

            PropertyVisualizationHelpers::CreateThickLine(firstItr->second.m_spMesh,
                firstItr->second.m_point, itr->second.m_point, 0.4f);

            if (firstItr->second.m_connectionHandle == SceneGraphService::kInvalidHandle)
            {
                //Entity* pConnectedEntity = 
                //    m_pEntityManager->LookupEntity(firstItr->second.m_entityID);
                firstItr->second.m_connectionHandle = 
                    PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                    firstItr->second.m_spMesh, pSceneGraphService);
            }

            firstItr->second.m_spMesh->RecomputeBounds();
            firstItr->second.m_spMesh->Update(0.0f);

            firstItr = itr;
        }
        while (itr != m_connections.end());

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

        for(ConnectionMap::iterator quickItr = m_connections.begin();
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
            while(m_connections.count(nextIndex) == 0 && iNext < iLast);

            if (iNext == iLast)
                break;

            itr = m_connections.find(nextIndex);

            PropertyVisualizationHelpers::CreateThickLine(firstItr->second.m_spMesh, 
                firstItr->second.m_point, itr->second.m_point, 0.4f);

            firstItr->second.m_spMesh->RecomputeBounds();
            firstItr->second.m_spMesh->Update(0.0f);

            firstItr = itr;
        }
        while(itr != m_connections.end());

        SetVisibility(true);
    }

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);
}

//------------------------------------------------------------------------------------------------
PropertyChangeAction EntitySplinePropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    EE_UNUSED_ARG(pEntity);

    Entity* pOwner = m_pEntityManager->LookupEntity(m_owner);
    EE_ASSERT(pOwner);

    PropertyChangeAction action = UpdateConnectionList(pOwner, pEntity);

    Point3 point;
    for (ConnectionMap::iterator itr = m_connections.begin(); itr != m_connections.end(); itr++)
    {
        Entity* pConnectedEntity = m_pEntityManager->LookupEntity(itr->second.m_entityID);
        if (pConnectedEntity && PropertyVisualizationHelpers::GetDependency(
            m_spExtraData,
            pConnectedEntity,
            "Position",
            "Position",
            point) == PropertyResult_OK)
        {
            itr->second.m_point = NiPoint3(point.x, point.y, point.z);

            if (action != PropertyChangeAction_UpdateGeometry)
                action = PropertyChangeAction_UpdateTransforms;
        }
    }

    Color color;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Line Color",
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

//------------------------------------------------------------------------------------------------
void EntitySplinePropertyVisualizer::SetVisibility(bool isVisible)
{
    isVisible = m_pVisibilityTracker->IsEntityVisible(m_owner);

    // All connections become invisible
    if (!isVisible)
    {
        for (ConnectionMap::iterator itr = m_connections.begin();
            itr != m_connections.end();
            itr++)
        {
            if (itr->second.m_spMesh)
                itr->second.m_spMesh->SetAppCulled(true);
        }
    }
    // Only some of the visibility should change
    else
    {
        for (ConnectionMap::iterator itr = m_connections.begin();
            itr != m_connections.end();
            itr++)
        {
            isVisible = m_pVisibilityTracker->IsEntityVisible(
                itr->second.m_entityID);

            if (itr->second.m_spMesh)
                itr->second.m_spMesh->SetAppCulled(!isVisible);
        }
    }
}
//------------------------------------------------------------------------------------------------
PropertyChangeAction EntitySplinePropertyVisualizer::UpdateConnectionList(Entity* pOwner,
    Entity* pEntity)
{
    EE_ASSERT(m_pSceneGraphService);

    PropertyChangeAction action = PropertyChangeAction_None;

    efd::map<utf8string, egf::EntityID> connections;
    if (PropertyVisualizationHelpers::GetAssocArrayDependency(
        m_spExtraData,
        pOwner,
        "Entity List",
        "Entity List",
        connections) == PropertyResult_OK)
    {
        // If the connection list hasn't changed, early-out.
        // This is fairly expensive, but Entity->IsDirty seems to be unreliable for our 
        // purposes, and in most cases this will be less expensive than recreating all the 
        // geometry.
        if (connections == m_oldConnections &&
            !m_pConnectionTracker->IsEntityBeingRemoved(pEntity->GetEntityID()))
            return action;

        m_oldConnections = connections;

        // Free old geometry & connections and prepare for recreation
        for (ConnectionMap::iterator itr = m_connections.begin();
             itr != m_connections.end();
             itr++)
        {
            if(itr->second.m_connectionHandle != SceneGraphService::kInvalidHandle)
                m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionHandle);

            m_pConnectionTracker->RemoveConnection(itr->second.m_entityID, this);
        }

        // Clear the list, which will free the meshes
        m_connections.clear();

        // Setup the new connection map
        for (efd::map<utf8string, egf::EntityID>::iterator itr = connections.begin();
             itr != connections.end();
             itr++)
        {
            // Only add in the new entity if it is not in the middle of being
            // removed from the scene.
            if (!m_pConnectionTracker->IsEntityBeingRemoved(itr->second))
            {
                m_connections[itr->first].m_entityID = itr->second;

                m_connections[itr->first].m_connectionHandle = SceneGraphService::kInvalidHandle;
            }
        }

        // Register all new connections with the connection tracker
        for (ConnectionMap::iterator itr = m_connections.begin();
             itr != m_connections.end();
             itr++)
        {
            m_pConnectionTracker->AddConnection(itr->second.m_entityID, this);
        }

        action = PropertyChangeAction_UpdateGeometry;
    }

    return action;
}
//------------------------------------------------------------------------------------------------
