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

#include "PropertyVisualizationService.h"
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CoreRuntimeMessages.h>

#include <efd/MessageService.h>
#include <efd/PathUtils.h>
#include <efd/IDs.h>
#include <egf/Entity.h>
#include <egf/EntityManager.h>
#include <efd/ecrLogIDs.h>

#include "PropertyVisualizerFactory.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(PropertyVisualizationService);


EE_HANDLER_WRAP(PropertyVisualizationService, HandleEntityDiscoverMessage,
                egf::EntityChangeMessage, kMSGID_OwnedEntityEnterWorld);
EE_HANDLER_WRAP(PropertyVisualizationService, HandleEntityRemovedMessage,
                egf::EntityChangeMessage, kMSGID_OwnedEntityExitWorld);
EE_HANDLER_WRAP(PropertyVisualizationService, HandleEntityUpdatedMessage,
                egf::EntityChangeMessage, kMSGID_OwnedEntityUpdated);

//------------------------------------------------------------------------------------------------
PropertyVisualizationService::PropertyVisualizationService()
    : m_pRenderService(NULL)
    , m_spSceneGraphService(NULL)
    , m_pMessageService(NULL)
    , m_pFlatModelManager(NULL)
    , m_pEntityManager(NULL)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2072;

    m_visualizersEnabled = false;
}

//------------------------------------------------------------------------------------------------
PropertyVisualizationService::~PropertyVisualizationService()
{
}

//------------------------------------------------------------------------------------------------
SyncResult PropertyVisualizationService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<EntityManager>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);

    m_spSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_spSceneGraphService);

    m_pFlatModelManager = m_pServiceManager->GetSystemServiceAs<FlatModelManager>();
    EE_ASSERT(m_pFlatModelManager);

    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<EntityManager>();
    EE_ASSERT(m_pEntityManager);

    RegisterForEntityMessages();

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult PropertyVisualizationService::OnShutdown()
{
    DisableVisualization(true);
    m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);

    m_pMessageService = NULL;
    m_pRenderService = NULL;
    m_spSceneGraphService = NULL;
    m_pFlatModelManager = NULL;
    m_pEntityManager = NULL;

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::RegisterForEntityMessages()
{
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::AddVisualizers(Entity* pEntity)
{
    EE_ASSERT(m_visualizersEnabled);

    FlatModel *model = (FlatModel*)pEntity->GetModel();
    EE_ASSERT(model);

    list<utf8string> mixinNames;
    model->GetMixinNames(mixinNames);

    // Add the current model to the front of the list so we don't need
    // a special case
    mixinNames.push_front(model->GetName());

    for (list<utf8string>::const_iterator
        mixiter = mixinNames.begin();
        mixiter != mixinNames.end();
        mixiter++)
    {
        const FlatModel* pMixin = m_pFlatModelManager->FindOrLoadModel(*mixiter);
        if (!pMixin)
        {
            continue;
        }

        efd::list<ExtraDataPtr> extraData;
        pMixin->GetExtraData("PropertyVisualizer", extraData);

        if (extraData.size() > 0)
        {
            for (efd::list<ExtraDataPtr>::const_iterator
                iter = extraData.begin();
                iter != extraData.end();
                iter++)
            {
                PropertyVisualizerFactory pvf;

                IPropertyVisualizerPtr visualizer;
                if (!m_visualizers[pEntity->GetEntityID()].find((*iter)->GetName(), visualizer))
                {
                    visualizer = pvf.CreatePropertyVisualizer(*iter,
                        m_pEntityManager,
                        &m_connectionTracker,
                        &m_visibilityTracker);
                }

                if (visualizer)
                {
                    visualizer->AddGeometry(m_spSceneGraphService, pEntity);
                    visualizer->UpdateGeometry(m_spSceneGraphService, pEntity);
                    visualizer->SetVisibility(
                        m_visibilityTracker.IsEntityVisible(pEntity->GetEntityID()));

                    m_visualizers[pEntity->GetEntityID()][(*iter)->GetName()] = visualizer;
                }
                else
                {
                    EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                        ("Failed to create visualizer for '%s'", (*iter)->GetName().c_str()));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::HandleEntityDiscoverMessage(const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* entity = pMessage->GetEntity();
    EE_ASSERT(entity);

    m_entities[entity->GetEntityID()] = entity;

    if (!m_visualizersEnabled)
        return;

    // Check visibility
    bool isVisible = true;
    if (entity->GetPropertyValue("IsVisible", isVisible) ==
        egf::PropertyResult_OK)
    {
        SetEntityVisibility(entity->GetEntityID(),
            "IsVisible",
            0,
            isVisible);
    }

    AddVisualizers(entity);

    // Look through the list of visualizers where a valid connection has yet to be
    // established, trying to perform an update.  The update should recognize a
    // newly valid connection and put the visualizer in its appropriate
    // spot in the map.
    list<IPropertyVisualizerPtr> visualizerList;
    m_connectionTracker.GetUnresolvedConnections(visualizerList);
    for (list<IPropertyVisualizerPtr>::const_iterator
        iter = visualizerList.begin();
        iter != visualizerList.end();
        iter++)
    {
        (*iter)->UpdateGeometry(m_spSceneGraphService, entity);
    }

    m_pRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* entity = pMessage->GetEntity();
    EE_ASSERT(entity);

    map<utf8string, IPropertyVisualizerPtr> visualizers;
    m_visualizers.find(entity->GetEntityID(), visualizers);

    for (map<utf8string, IPropertyVisualizerPtr>::const_iterator
        iter = visualizers.begin();
        iter != visualizers.end();
        iter++)
    {
        iter->second->RemoveGeometry(m_spSceneGraphService);
    }

    m_visualizers[entity->GetEntityID()].clear();
    m_visualizers.erase(entity->GetEntityID());

    m_entities.erase(entity->GetEntityID());

    m_connectionTracker.SetupEntityForRemoval(entity->GetEntityID());
    list<IPropertyVisualizerPtr> visualizerList;
    m_connectionTracker.GetConnections(entity->GetEntityID(), visualizerList);
    for (list<IPropertyVisualizerPtr>::const_iterator
        iter = visualizerList.begin();
        iter != visualizerList.end();
        iter++)
    {
        (*iter)->UpdateGeometry(m_spSceneGraphService, entity);
    }
    m_connectionTracker.RemoveEntity();

    m_pRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    if (!m_visualizersEnabled)
        return;

    Entity* em = pMessage->GetEntity();
    EE_ASSERT(em);

    efd::map<efd::utf8string, IPropertyVisualizerPtr> visualizers;

    // Check visibility
    bool isVisible = true;
    if (em->GetPropertyValue("IsVisible", isVisible) ==
        egf::PropertyResult_OK)
    {
        SetEntityVisibility(em->GetEntityID(),
            "IsVisible",
            0,
            isVisible);
    }

    if (m_visualizers.find(em->GetEntityID(), visualizers))
    {

        for (efd::map<efd::utf8string, IPropertyVisualizerPtr>::const_iterator
            iter = visualizers.begin();
            iter != visualizers.end();
            iter++)
        {
            iter->second->UpdateGeometry(m_spSceneGraphService, em);
        }
    }

    // If this entity is part of a connection, the visualizer handling
    // the connection must be updated as well.
    list<IPropertyVisualizerPtr> visualizerList;
    m_connectionTracker.GetConnections(em->GetEntityID(), visualizerList);
    for (list<IPropertyVisualizerPtr>::const_iterator
        iter = visualizerList.begin();
        iter != visualizerList.end();
        iter++)
    {
        (*iter)->UpdateGeometry(m_spSceneGraphService, em);
    }

    m_pRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::EnableVisualization()
{
    if (m_visualizersEnabled)
        return;

    m_visualizersEnabled = true;

    efd::map<egf::EntityID, Entity*>::iterator itr;
    for (itr = m_entities.begin(); itr != m_entities.end(); itr++)
    {
        AddVisualizers(itr->second);
    }
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::DisableVisualization(efd::Bool cleanup)
{
    if (!m_visualizersEnabled)
        return;

    m_visualizersEnabled = false;

    efd::map<egf::EntityID, efd::map<efd::utf8string, IPropertyVisualizerPtr> >::iterator itr;
    for (itr = m_visualizers.begin(); itr != m_visualizers.end(); itr++)
    {
        efd::map<efd::utf8string, IPropertyVisualizerPtr>::iterator itr2;
        for (itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
        {
            itr2->second->RemoveGeometry(m_spSceneGraphService);
        }
    }

    // Only free the visualizers if we want to perform cleanup; otherwise,
    // the memory will still stay around (facilitating quickly turning on/off the
    // visualization)
    if (cleanup)
    {
        m_visualizers.clear();
    }
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::ResetVisualization()
{
    if (m_visualizersEnabled)
    {
        DisableVisualization(true);
        EnableVisualization();
    }
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationService::SetEntityVisibility(
    egf::EntityID entityID,
    const efd::utf8string& source,
    efd::UInt32 category,
    efd::Bool isVisible)
{
    m_visibilityTracker.SetEntityVisibility(entityID, source, category, isVisible);

    isVisible = m_visibilityTracker.IsEntityVisible(entityID);

    for (efd::map<efd::utf8string, IPropertyVisualizerPtr>::iterator
        itr = m_visualizers[entityID].begin();
        itr != m_visualizers[entityID].end();
        itr++)
    {
        itr->second->SetVisibility(isVisible);
    }

    // If an entity is connected to this entity, visualizers requiring connections need to have
    // their visibility changed
    efd::list<IPropertyVisualizerPtr> connections;
    m_connectionTracker.GetConnections(entityID, connections);
    for (efd::list<IPropertyVisualizerPtr>::iterator
        itr = connections.begin();
        itr != connections.end();
        itr++)
    {
        (*itr)->SetVisibility(isVisible);
    }

}

//------------------------------------------------------------------------------------------------
