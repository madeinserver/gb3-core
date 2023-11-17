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

#include "egmToolServicesPCH.h"
#include "egmToolServicesLibType.h"

#include <egf/StandardModelLibraryPropertyIDs.h>
#include "BoxSelectionFunctor.h"

#include "EntitySelectionAdapter.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(EntitySelectionAdapter);

EE_HANDLER_WRAP(EntitySelectionAdapter,
                HandleEntityUpdatedMessage,
                EntityChangeMessage,
                kMSGID_OwnedEntityUpdated);
EE_HANDLER_WRAP(EntitySelectionAdapter,
                HandleEntityRemovedMessage,
                EntityChangeMessage,
                kMSGID_OwnedEntityRemoved);

EE_HANDLER_WRAP(EntitySelectionAdapter,
                OnSelectabilityChanged,
                efd::IMessage,
                kMSGID_ToolSelectabilityChanged);

EE_HANDLER(EntitySelectionAdapter, OnSceneGraphAdded, SceneGraphAddedMessage);
EE_HANDLER(EntitySelectionAdapter, OnSceneGraphRemoved, SceneGraphRemovedMessage);

//-----------------------------------------------------------------------------------------------
EntitySelectionAdapter::EntitySelectionAdapter(efd::ServiceManager* pServiceManager)
    : m_centerInvalid(false)
    , m_center(Point3::ZERO)
{
    m_pMessageService = pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pEntityManager = pServiceManager->GetSystemServiceAs<EntityManager>();
    EE_ASSERT(m_pEntityManager);

    m_pSceneGraphService = pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_pSceneGraphService);

    m_pPickService = pServiceManager->GetSystemServiceAs<PickService>();
    EE_ASSERT(m_pPickService);

    m_pSelectionService = pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(m_pSelectionService);

    m_pRenderService = pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);
}

//-----------------------------------------------------------------------------------------------
EntitySelectionAdapter::~EntitySelectionAdapter()
{
    m_selectedEntities.clear();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::OnAdded()
{
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::OnRemoved()
{
    m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::HasSelection() const
{
    return !m_selectedEntities.empty();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::ClearSelection()
{
    BeginSelection();
    ClearSelectedEntities();
    EndSelection();
}

//-----------------------------------------------------------------------------------------------
NiBound EntitySelectionAdapter::GetBound()
{
    NiBound kSelectionBound;

    bool setFirstBound = true;
    for (efd::map<egf::EntityID, NiAVObjectPtr>::const_iterator iter = m_selectedEntities.begin();
        iter != m_selectedEntities.end();
        ++iter)
    {
        const NiAVObject* pScene = iter->second;

        NiBound kBound;
        if (pScene != NULL)
        {
            kBound = pScene->GetWorldBound();
        }
        else
        {
            const egf::EntityID entityId = iter->first;
            egf::Entity* pEntity = m_pEntityManager->LookupEntity(entityId);
            EE_ASSERT(pEntity);

            if (pEntity != NULL)
            {
                efd::Point3 position;
                if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Position, position)
                    != egf::PropertyResult_OK)
                {
                    continue;
                }

                kBound.SetCenter(position.x, position.y, position.z);
                kBound.SetRadius(1);
            }
        }

        if (setFirstBound)
        {
            setFirstBound = false;
            kSelectionBound = kBound;
        }
        else
        {
            kSelectionBound.Merge(&kBound);
        }
    }

    return kSelectionBound;
}

//-----------------------------------------------------------------------------------------------
efd::Point3 EntitySelectionAdapter::GetCenter()
{
    if (!m_centerInvalid)
        return m_center;

    m_centerInvalid = false;

    efd::Point3 totalTranslation = efd::Point3::ZERO;

    int size = static_cast<int>(m_selectedEntities.size());

    if (size == 0)
        return totalTranslation;

    for (efd::map<egf::EntityID, NiAVObjectPtr>::const_iterator iter = m_selectedEntities.begin();
        iter != m_selectedEntities.end();
        ++iter)
    {
        egf::Entity* pEntity = m_pEntityManager->LookupEntity(iter->first);
        EE_ASSERT(pEntity);

        if (pEntity != NULL)
        {
            efd::Point3 position;
            if (pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position)
                == egf::PropertyResult_OK)
            {
                totalTranslation += position;
            }
        }
    }

    m_center = (totalTranslation / (float)size);
    return m_center;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::Select(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    const efd::Point3& origin,
    const efd::Point3& direction)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    BeginSelection();

    NiAVObject* pSelectedObject;
    egf::Entity* pEntity;
    if (PickEntity(origin, direction, pEntity, pSelectedObject))
    {
        if (pEntity != NULL)
        {
            ToggleSelectedEntity(pEntity, pSelectedObject);
        }

        EndSelection();
        return true;
    }
    else
    {
        if (m_pSelectionService->IsSelectingMultiple() ||
            m_pSelectionService->IsSelectingAlternative())
        {
            EndSelection();
            return true;
        }

        ClearSelectedEntities();
    }

    EndSelection();
    return false;
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::SelectFrustum(NiCamera* pCamera, NiMeshCullingProcess* pCuller)
{
    BeginSelection();

    BoxSelectionFunctor boxSelector(pCamera, pCuller);

    m_pSceneGraphService->ForEachEntitySceneGraph(boxSelector);

    SelectionService::SelectionMode selectionMode = m_pSelectionService->GetSelectionMode();

    if (selectionMode == SelectionService::SELECTIONMODE_REPLACE)
        ClearSelectedEntities();

    if (boxSelector.GetSelections().size())
    {
        const efd::vector<const Entity*> &selections = boxSelector.GetSelections();
        efd::vector<const Entity*>::const_iterator i;
        NiAVObjectPtr pObj;

        for (i = selections.begin(); i != selections.end(); i++)
        {
            const Entity* pEntity = *i;

            if (!IsFrozenEntity(pEntity))
            {
                switch (selectionMode)
                {
                case SelectionService::SELECTIONMODE_REPLACE:
                case SelectionService::SELECTIONMODE_ADD:
                    AddSelectedEntity(pEntity);
                    break;
                case SelectionService::SELECTIONMODE_SUBTRACT:
                    RemoveSelectedEntity(pEntity);
                    break;
                case SelectionService::SELECTIONMODE_INVERT:
                    if (m_selectedEntities.find(pEntity->GetEntityID(), pObj))
                    {
                        RemoveSelectedEntity(pEntity);
                    }
                    else
                    {
                        AddSelectedEntity(pEntity);
                    }
                    break;
                }
            }
        }
    }

    EndSelection();

    return true;
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::RenderSelection(
    ecr::RenderService* pRenderService,
    unsigned int uiFrameID)
{
    EE_UNUSED_ARG(pRenderService);
    EE_UNUSED_ARG(uiFrameID);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::IsSelected(const egf::EntityID& entityId)
{
    NiAVObjectPtr spObject;
    return m_selectedEntities.find(entityId, spObject);
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::PickEntity(
    const efd::Point3& rayStart,
    const efd::Point3& rayDir,
    egf::Entity*& pEntity,
    NiAVObject*& pSelectedObject)
{
    EE_ASSERT(m_pPickService);

    pEntity = NULL;

    // Issue the pick operation to the pick service.
    PickService::PickRecordPtr spPickResult = m_pPickService->PerformPick(rayStart, rayDir);

    if (spPickResult)
    {
        const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
        
        EE_ASSERT(pPickerResults);
        EE_ASSERT(pPickerResults->GetSize() > 0);

        for (unsigned int ui = 0; ui < pPickerResults->GetSize(); ui++)
        {
            pSelectedObject = pPickerResults->GetAt(ui)->GetAVObject()->GetRoot();

            pEntity = m_pSceneGraphService->GetEntityFromSceneGraph(pSelectedObject);

            if (!IsFrozenEntity(pEntity))
                return true;
        }

        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::SelectEntities(const list<Entity*>* pEntities)
{
    BeginSelection();

    for (list<Entity*>::const_iterator i = pEntities->begin();
        i != pEntities->end();
        ++i)
    {
        AddSelectedEntity(*i);
    }

    EndSelection();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::SelectEntities(const list<ID128>* pEntities)
{
    BeginSelection();

    for (list<ID128>::const_iterator i = pEntities->begin();
         i != pEntities->end();
         ++i)
    {
        const efd::ID128 id = *i;

        egf::Entity* pEntity = m_pEntityManager->LookupEntityByDataFileID(id);
        EE_ASSERT(pEntity);

        if (pEntity)
            AddSelectedEntity(pEntity);
    }

    EndSelection();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::DeselectEntities(const list<Entity*>* pEntities)
{
    BeginSelection();

    for (list<Entity*>::const_iterator i = pEntities->begin();
         i != pEntities->end();
         ++i)
    {
        RemoveSelectedEntity(*i);
    }

    EndSelection();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::DeselectEntities(const list<ID128>* pEntities)
{
    BeginSelection();

    for (list<ID128>::const_iterator i = pEntities->begin();
         i != pEntities->end();
         ++i)
    {
        const ID128 id = *i;

        egf::Entity* pEntity = m_pEntityManager->LookupEntityByDataFileID(id);
        EE_ASSERT(pEntity);

        RemoveSelectedEntity(pEntity);
    }

    EndSelection();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::BeginSelection()
{
    // Double check that anyone that calls BeginSelection calls EndSelection.
    EE_ASSERT(m_tempAdded.empty());
    EE_ASSERT(m_tempRemoved.empty());

    m_tempAdded.clear();
    m_tempRemoved.clear();
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::EndSelection()
{
    m_centerInvalid = true;

    SelectEntitiesMessagePtr spSelectMessage = EE_NEW SelectEntitiesMessage();
    spSelectMessage->SetSelectionCleared(m_tempCleared);

    list<ID128>* pAddedEntities = spSelectMessage->GetAddedEntities();
    for (EntityList::iterator it = m_tempAdded.begin(); it != m_tempAdded.end(); ++it)
    {
        const Entity* pEntity = *it;
        pAddedEntities->push_back(pEntity->GetDataFileID());
    }

    list<ID128>* pRemovedEntities = spSelectMessage->GetRemovedEntities();
    for (EntityList::iterator it = m_tempRemoved.begin(); it != m_tempRemoved.end(); ++it)
    {
        const Entity* pEntity = *it;
        pRemovedEntities->push_back(pEntity->GetDataFileID());
    }

    m_pMessageService->SendImmediate(spSelectMessage, 
        ToolMessagesConstants::ms_fromFrameworkCategory);

    m_tempCleared = false;
    m_tempAdded.clear();
    m_tempRemoved.clear();

    m_pRenderService->InvalidateRenderContexts();
}

//-----------------------------------------------------------------------------------------------
efd::Bool EntitySelectionAdapter::ToggleSelectedEntity(
    const egf::Entity* pEntity, 
    NiAVObject* pSelectedNode)
{
    if (m_selectedEntities.find(pEntity->GetEntityID()) == m_selectedEntities.end())
    {
        if (!m_pSelectionService->IsSelectingMultiple())
            ClearSelectedEntities();

        AddSelectedEntity(pEntity, pSelectedNode);

        return true;
    }

    if (!m_pSelectionService->IsSelectingMultiple())
    {
        ClearSelectedEntities();
    }
    else
    {
        RemoveSelectedEntity(pEntity, pSelectedNode);
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::AddSelectedEntity(const egf::Entity* pEntity)
{
    NiAVObject* pSelectedNode = m_pSceneGraphService->GetSceneGraphFromEntity(pEntity);

    AddSelectedEntity(pEntity, pSelectedNode);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::AddSelectedEntity(
    const egf::Entity* pEntity,
    NiAVObject* pSelectedNode)
{
    // Remove entity with this id first to prevent double addition to the render view
    RemoveSelectedEntity(pEntity);

    m_selectedEntities[pEntity->GetEntityID()] = pSelectedNode;

    if (pSelectedNode)
    {
        // Add the selected entity's scene graph to the selection render
        // view. Entities in the selected render view are rendered
        // separately from the unselected entities.
        m_pSelectionService->GetSelectionView()->AppendScene(pSelectedNode);
    }

    m_tempAdded.push_back(pEntity);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::RemoveSelectedEntity(const egf::Entity* pEntity)
{
    NiAVObject* pSelectedNode = m_pSceneGraphService->GetSceneGraphFromEntity(pEntity);

    RemoveSelectedEntity(pEntity, pSelectedNode);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::RemoveSelectedEntity(
    const egf::Entity* pEntity,
    NiAVObject* pSelectedNode)
{
    if (pSelectedNode)
        m_pSelectionService->GetSelectionView()->RemoveScene(pSelectedNode);

    // if there is a selection which is not the one passed in
    // clear that one as well... this is for the visualizer selection removing
    NiAVObjectPtr spScene;
    m_selectedEntities.find(pEntity->GetEntityID(), spScene);
    if (spScene && pSelectedNode != spScene)
    {
        m_pSelectionService->GetSelectionView()->RemoveScene(spScene);
    }

    m_selectedEntities.erase(pEntity->GetEntityID());

    m_tempRemoved.push_back(pEntity);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::ClearSelectedEntities()
{
    m_tempCleared = true;

    m_tempAdded.clear();
    m_tempRemoved.clear();

    Ni3DRenderView* pRenderView = m_pSelectionService->GetSelectionView();

    while (!m_selectedEntities.empty())
    {
        EntityAVMap::iterator i = m_selectedEntities.begin();
        if(i->second)
        {
            pRenderView->RemoveScene(i->second);
        }
        m_selectedEntities.erase(i);
    }
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();

    NiAVObjectPtr spScene;
    m_selectedEntities.find(pEntity->GetEntityID(), spScene);

    if (spScene)
        m_pSelectionService->GetSelectionView()->RemoveScene(spScene);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();

    if (m_selectedEntities.find(pEntity->GetEntityID()) == m_selectedEntities.end())
        return;

    if (pEntity->IsDirty("Position"))
        m_centerInvalid = true;
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::OnSceneGraphAdded(
    const SceneGraphAddedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    NiAVObject* pObject = 0;
    if (pMessage->GetEntity())
    {
        pObject = m_pSceneGraphService->GetSceneGraphFromEntity(pMessage->GetEntity());
    }
    else
    {
        return;
    }

    NiNode* pNewScene = NiDynamicCast(NiNode, pObject);

    if (pNewScene != NULL)
    {
        const EntityID& entityId = pMessage->GetEntity()->GetEntityID();

        // Note, this code exists to resolve any scene graphs that may have been loaded after the
        // selection service knows about the actual entity.
        NiAVObjectPtr spScene;
        if (!m_selectedEntities.find(entityId, spScene))
            return;

        // Remove the old scene in case the selection service already had a scene mapped to the
        // entity.
        if (spScene)
            m_pSelectionService->GetSelectionView()->RemoveScene(spScene);

        // Now we can associate the new scene with the existing entity we know about.
        m_selectedEntities[entityId] = pNewScene;
        m_pSelectionService->GetSelectionView()->AppendScene(pNewScene);
    }
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::OnSceneGraphRemoved(
    const SceneGraphRemovedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    if (!pMessage->GetEntity())
        return;

    const Entity* pEntity = pMessage->GetEntity();

    NiAVObject* pObject = m_pSceneGraphService->GetSceneGraphFromEntity(pEntity);
    NiNode* pNewScene = NiDynamicCast(NiNode, pObject);
    if (!pNewScene)
        return;

    // Note, this code exists to resolve any scene graphs that may have been loaded after the
    // selection service knows about the actual entity.
    NiAVObjectPtr spScene;
    if (!m_selectedEntities.find(pEntity->GetEntityID(), spScene))
        return;

    // Remove the old scene in case the selection service already had a scene mapped to the
    // entity.
    if (spScene)
        m_pSelectionService->GetSelectionView()->RemoveScene(spScene);
}

//-----------------------------------------------------------------------------------------------
void EntitySelectionAdapter::OnSelectabilityChanged(
    const IMessage* pMsg,
    efd::Category targetChannel)
{
    // Check to see if this is a stream message
    efd::StreamMessage* pStreamMsg = EE_DYNAMIC_CAST(efd::StreamMessage,
        const_cast<efd::IMessage*>(pMsg));

    if (!pStreamMsg)
        return;

    pStreamMsg->ResetForUnpacking();

    // Is this a visibility message or not (0 = false, 1 = true)
    efd::UInt32 selectabilityMsgId = 0;
    *pStreamMsg >> selectabilityMsgId;

    efd::UInt32 selectabilityCategory = 0;
    *pStreamMsg >> selectabilityCategory;

    // Get the entity ids from the stream
    efd::UInt32 entityCount = 0;
    *pStreamMsg >> entityCount;

    // If this is a selectable message, remove the id's from the map
    if (selectabilityMsgId == 1)
    {
        efd::ID128 persistentEntityID;
        for (efd::UInt32 ui = 0; ui < entityCount; ui++)
        {
            *pStreamMsg >> persistentEntityID;

            // Add to dirty list
            egf::Entity* entity = m_pEntityManager->LookupEntityByDataFileID(persistentEntityID);
            EE_ASSERT(entity);
            egf::EntityID entityID = entity->GetEntityID();
            m_unselectableEntities[selectabilityCategory].erase(entityID);
        }
    }
    // If this is an unselectable message, add the id's to the map
    else
    {
        efd::ID128 persistentEntityID;
        for (efd::UInt32 ui = 0; ui < entityCount; ui++)
        {
            *pStreamMsg >> persistentEntityID;
            bool value = false;
            egf::Entity* entity = m_pEntityManager->LookupEntityByDataFileID(persistentEntityID);
            EE_ASSERT(entity);
            egf::EntityID entityID = entity->GetEntityID();

            if (m_unselectableEntities[selectabilityCategory].find(entityID, value))
            {
            }
            else
            {
                m_unselectableEntities[selectabilityCategory][entityID] = value;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
bool EntitySelectionAdapter::IsFrozenEntity(const Entity* pEntity) const
{
    bool frozen = false;

    for (efd::UInt32 ui = 0; ui < UNSELECTABLE_MAX; ui++)
    {
        if (m_unselectableEntities[ui].find(pEntity->GetEntityID(), frozen))
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
const efd::map<egf::EntityID, NiAVObjectPtr>& EntitySelectionAdapter::GetSelectedEntities() const
{
    return m_selectedEntities;
}

//-----------------------------------------------------------------------------------------------
