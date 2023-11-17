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
#include "ConnectionPropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>
#include <egf/PrimitiveProperties.h>

using namespace ecr;
using namespace egf;
using namespace efd;
using namespace egmVisualizers;

//--------------------------------------------------------------------------------------------------
ConnectionPropertyVisualizer::ConnectionPropertyVisualizer(
    ExtraDataPtr pExtraData,
    EntityManager* pEntityManager,
    VisualizerConnectionTracker* pConnectionTracker,
    VisualizerVisibilityTracker* pVisibilityTracker) :

    m_spExtraData(pExtraData),
    m_spMaterialProperty(NULL),
    m_owner(kENTITY_INVALID),
    m_color(0,0,0),
    m_sourcePoint(0,0,0),
    m_pEntityManager(pEntityManager),
    m_pConnectionTracker(pConnectionTracker),
    m_pVisibilityTracker(pVisibilityTracker)
{
    EE_ASSERT(pConnectionTracker);
    EE_ASSERT(pVisibilityTracker);
}

//--------------------------------------------------------------------------------------------------
void ConnectionPropertyVisualizer::AddGeometry(SceneGraphService* pSceneGraphService, Entity* pEntity)
{
    m_owner = pEntity->GetEntityID();

    m_pSceneGraphService = pSceneGraphService;

    // Create a default material; don't use PropertyVisualizationHelpers helper for this, since this
    // material needs to be shared among all the connections
    m_spMaterialProperty = NiNew NiMaterialProperty();
    m_spMaterialProperty->SetAmbientColor(NiColor::WHITE);
    m_spMaterialProperty->SetDiffuseColor(NiColor::WHITE);
    m_spMaterialProperty->SetSpecularColor(NiColor::WHITE);
    m_spMaterialProperty->SetEmittance(NiColor::WHITE);
    m_spMaterialProperty->SetShineness(0.0f);
    m_spMaterialProperty->SetAlpha(1.0f);

    // Defer most work normally done here to UpdateGeometry, which should be called after this.
}

//--------------------------------------------------------------------------------------------------
void ConnectionPropertyVisualizer::RemoveGeometry(SceneGraphService* pSceneGraphService)
{
    EE_UNUSED_ARG(pSceneGraphService);

    for (ConnectionMap::iterator itr = m_connections.begin();
        itr != m_connections.end();
        itr ++)
    {
        m_pConnectionTracker->RemoveConnection(itr->second.m_entityID, this);
        pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionHandle);
        pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionLineHandle);
    }

    m_connections.clear();
}

//--------------------------------------------------------------------------------------------------
void ConnectionPropertyVisualizer::UpdateGeometry(SceneGraphService* pSceneGraphService,
                                             egf::Entity* pEntity)
{
    EE_UNUSED_ARG(pSceneGraphService);

    PropertyChangeAction action = UpdateFromEntity(pEntity);

    // Tear down and recreate all the geometry when the list changes
    if (action == PropertyChangeAction_UpdateGeometry)
    {
        // Recreate current meshes;  UpdateFromEntity should've handled freeing the old ones.
        for (ConnectionMap::iterator itr = m_connections.begin();
            itr != m_connections.end();
            ++itr)
        {
            if (!itr->second.m_spMesh)
            {
                itr->second.m_spMesh = NiNew NiMesh();
                itr->second.m_spLineMesh = NiNew NiMesh();

                itr->second.m_spMesh->AttachProperty(m_spMaterialProperty);
                itr->second.m_spLineMesh->AttachProperty(m_spMaterialProperty);
            }

            PropertyVisualizationHelpers::CreateThickLine(
                itr->second.m_spMesh, m_sourcePoint, itr->second.m_point, 0.4f);
            PropertyVisualizationHelpers::CreateLine(
                itr->second.m_spMesh, m_sourcePoint, itr->second.m_point);

            if (itr->second.m_connectionHandle == SceneGraphService::kInvalidHandle)
            {
                itr->second.m_connectionHandle =
                    PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                    itr->second.m_spMesh, pSceneGraphService);
                itr->second.m_connectionLineHandle =
                    PropertyVisualizationHelpers::AddMeshToSceneGraphService(
                    itr->second.m_spLineMesh, pSceneGraphService);
            }

            itr->second.m_spLineMesh->RecomputeBounds();
            itr->second.m_spLineMesh->Update(0.0f);

            itr->second.m_spMesh->RecomputeBounds();
            itr->second.m_spMesh->Update(0.0f);
        }

        SetVisibility(true);
    }

    // If the targets changed, we don't need to tear down the geometry,
    // just change the vertices.
    // This is a different meaning of UpdateTransforms than we're used
    // to, but with lines it's the closest equivalent
    else if (action == PropertyChangeAction_UpdateTransforms)
    {
        for (ConnectionMap::iterator itr = m_connections.begin();
            itr != m_connections.end();
            itr++)
        {
            PropertyVisualizationHelpers::CreateThickLine(
                itr->second.m_spMesh, m_sourcePoint, itr->second.m_point, 0.4f);
            PropertyVisualizationHelpers::CreateLine(
                itr->second.m_spLineMesh, m_sourcePoint, itr->second.m_point);

            itr->second.m_spLineMesh->RecomputeBounds();
            itr->second.m_spLineMesh->Update(0.0f);

            itr->second.m_spMesh->RecomputeBounds();
            itr->second.m_spMesh->Update(0.0f);
        }

        SetVisibility(true);
    }

    m_spMaterialProperty->SetEmittance(m_color);
    m_spMaterialProperty->SetDiffuseColor(m_color);
}

//--------------------------------------------------------------------------------------------------
PropertyChangeAction ConnectionPropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    EE_UNUSED_ARG(pEntity);

    Entity* pOwner = m_pEntityManager->LookupEntity(m_owner);
    EE_ASSERT(pOwner);

    PropertyChangeAction action = UpdateConnectionList(pOwner, pEntity);

    Point3 point;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Position",
        "Position",
        point) == PropertyResult_OK)
    {
        m_sourcePoint = NiPoint3(point.x, point.y, point.z);
        action = PropertyChangeAction_UpdateGeometry;
    }
    else
    {
        m_sourcePoint = NiPoint3(0.0f, 0.0f, 0.0f);
    }

    Point3 offset;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Offset",
        "No Default",
        offset) == PropertyResult_OK)
    {
        m_sourcePoint += offset;

        if (action != PropertyChangeAction_UpdateGeometry)
            action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        offset = Point3(0.0f, 0.0f, 0.0f);
    }

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
        else
        {
            itr->second.m_point = m_sourcePoint;
        }

        itr->second.m_point += offset;
    }

    Color color;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
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
void ConnectionPropertyVisualizer::SetVisibility(bool isVisible)
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
            if (itr->second.m_spLineMesh)
                itr->second.m_spLineMesh->SetAppCulled(true);
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

            if (itr->second.m_spLineMesh)
                itr->second.m_spLineMesh->SetAppCulled(!isVisible);
            if (itr->second.m_spMesh)
                itr->second.m_spMesh->SetAppCulled(!isVisible);
        }
    }
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction ConnectionPropertyVisualizer::UpdateConnectionList(Entity* pOwner,
    Entity* pEntity)
{
    EE_ASSERT(m_pSceneGraphService);

    PropertyChangeAction action = PropertyChangeAction_None;

    EntityID entityID;
    egf::PropertyResult result = PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pOwner,
        "Connection",
        "Connection",
        entityID);

    if (result == PropertyResult_OK)
    {
        // If the connection list hasn't changed, early-out.
        if (m_connections.size() == 1 && m_connections["OnlyConnection"].m_entityID == entityID &&
            !m_pConnectionTracker->IsEntityBeingRemoved(pEntity->GetEntityID()))
            return action;

        m_oldConnections["OnlyConnection"] = entityID;

        for (ConnectionMap::iterator itr = m_connections.begin();
            itr != m_connections.end();
            itr++)
        {
            m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionHandle);
            m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionLineHandle);

            m_pConnectionTracker->RemoveConnection(itr->second.m_entityID, this);
        }

        m_connections.clear();

        if (!m_pConnectionTracker->IsEntityBeingRemoved(entityID))
        {
            m_connections["OnlyConnection"].m_entityID = entityID;
            m_connections["OnlyConnection"].m_connectionHandle =
                SceneGraphService::kInvalidHandle;
            m_connections["OnlyConnection"].m_connectionLineHandle =
                SceneGraphService::kInvalidHandle;

            m_pConnectionTracker->AddConnection(m_connections["OnlyConnection"].m_entityID, this);
        }

        action = PropertyChangeAction_UpdateGeometry;
    }
    else if (result == PropertyResult_PropertyNotScalar)
    {
        efd::map<utf8string, egf::EntityID> connections;
        if (PropertyVisualizationHelpers::GetAssocArrayDependency(
            m_spExtraData,
            pOwner,
            "Connection",
            "Connection",
            connections) == PropertyResult_OK)
        {
            // If the connection list hasn't changed, early-out.
            // This is fairly expensive, but Entity->IsDirty seems to be unreliable for our purposes, and
            // in most cases this will be less expensive than recreating all the geometry.
            if (connections == m_oldConnections &&
                !m_pConnectionTracker->IsEntityBeingRemoved(pEntity->GetEntityID()))
                return action;

            m_oldConnections = connections;

            // Free old geometry & connections and prepare for recreation
            for (ConnectionMap::iterator
                itr = m_connections.begin();
                itr != m_connections.end();
                itr++)
            {
                m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionHandle);
                m_pSceneGraphService->RemoveSceneGraph(itr->second.m_connectionLineHandle);

                m_pConnectionTracker->RemoveConnection(itr->second.m_entityID, this);
            }

            // Clear the list, which will free the meshes
            m_connections.clear();

            // Setup the new connection map
            for (efd::map<utf8string, egf::EntityID>::iterator
                itr = connections.begin();
                itr != connections.end();
                itr++)
            {
                // Only add in the new entity if it is not in the middle of being
                // removed from the scene.
                if (!m_pConnectionTracker->IsEntityBeingRemoved(itr->second))
                {
                    m_connections[itr->first].m_entityID = itr->second;

                    m_connections[itr->first].m_connectionHandle =
                        SceneGraphService::kInvalidHandle;
                    m_connections[itr->first].m_connectionLineHandle =
                        SceneGraphService::kInvalidHandle;
                }
            }

            // Register all new connections with the connection tracker
            for (ConnectionMap::iterator
                itr = m_connections.begin();
                itr != m_connections.end();
                itr++)
            {
                m_pConnectionTracker->AddConnection(itr->second.m_entityID, this);
            }

            action = PropertyChangeAction_UpdateGeometry;
        }
    }

    return action;
}
//--------------------------------------------------------------------------------------------------
