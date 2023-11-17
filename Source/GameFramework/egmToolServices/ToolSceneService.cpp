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

#include <efd/MessageService.h>
#include <efd/PathUtils.h>
#include <efd/IDs.h>
#include <efd/ServiceManager.h>

#include <egf/Entity.h>
#include <egf/EntityManager.h>
#include <egf/egfBaseIDs.h>
#include <egf/GameTimeClock.h>

#include <NiRoomGroup.h>
#include <NiMeshUpdateProcess.h>

#include "ToolSceneService.h"

#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CoreRuntimeMessages.h>

#include <egf/StandardModelLibraryFlatModelIDs.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolSceneGraphService);

EE_HANDLER(ToolSceneGraphService, OnSettingsChanged, SettingsUpdateMessage);
EE_HANDLER_WRAP(ToolSceneGraphService, OnToolsVisibilityChanged,
    StreamMessage, kMSGID_ToolVisibilityChanged);

//------------------------------------------------------------------------------------------------
// DefaultLightAttacherFunctor
//------------------------------------------------------------------------------------------------
ToolSceneGraphService::DefaultLightAttacherFunctor::DefaultLightAttacherFunctor(
    ToolSceneGraphService* pSceneService, bool attachLights)
    : m_pSceneService(pSceneService)
    , m_attachLights(attachLights)
{
}

ToolSceneGraphService::DefaultLightAttacherFunctor::~DefaultLightAttacherFunctor()
{
}

efd::Bool ToolSceneGraphService::DefaultLightAttacherFunctor::operator()(
    const egf::Entity* pEntity,
    const efd::vector<NiObjectPtr>& objects)
{
    EE_UNUSED_ARG(pEntity);
    EE_ASSERT(objects.size() > 0);

    NiAVObject* pSceneGraph = NiDynamicCast(NiAVObject, objects[0]);

    if (pSceneGraph != NULL)
    {
        if (m_attachLights)
            m_pSceneService->AttachDefaultLights(pSceneGraph, false);
        else
            m_pSceneService->DetachDefaultLights(pSceneGraph, false);
    }

    return false;
}

efd::Bool ToolSceneGraphService::DefaultLightAttacherFunctor::operator()(
    const SceneGraphService::SceneGraphHandle handle,
    const efd::vector<NiObjectPtr>& objects)
{
    EE_UNUSED_ARG(handle);
    EE_ASSERT(objects.size() > 0);

    NiAVObject* pSceneGraph = NiDynamicCast(NiAVObject, objects[0]);

    if (pSceneGraph != NULL)
    {
        if (m_attachLights)
            m_pSceneService->AttachDefaultLights(pSceneGraph, false);
        else
            m_pSceneService->DetachDefaultLights(pSceneGraph, false);
    }

    return false;
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// RecomputeSceneBoundsFunctor
//------------------------------------------------------------------------------------------------
class RecomputeSceneBoundsFunctor :
    public SceneGraphService::EntitySceneGraphFunctor,
    public SceneGraphService::HandleSceneGraphFunctor
{
public:
    RecomputeSceneBoundsFunctor()
        : m_sceneBound(NULL)
    {
    }

    ~RecomputeSceneBoundsFunctor()
    {
        NiDelete m_sceneBound;
    }

    efd::Bool operator()(const egf::Entity* pEntity, const efd::vector<NiObjectPtr>& objects)
    {
        EE_UNUSED_ARG(pEntity);
        EE_ASSERT(objects.size() > 0);

        NiAVObject* pSceneGraph = NiDynamicCast(NiAVObject, objects[0]);

        if (pSceneGraph != NULL)
        {
            const NiBound& kSceneGraphBounds = pSceneGraph->GetWorldBound();

            if (m_sceneBound == NULL)
            {
                m_sceneBound = NiNew NiBound(kSceneGraphBounds);
            }
            else
            {
                m_sceneBound->Merge(&kSceneGraphBounds);
            }
        }

        return false;
    }

    efd::Bool operator()(const SceneGraphService::SceneGraphHandle handle,
        const efd::vector<NiObjectPtr>& objects)
    {
        EE_UNUSED_ARG(handle);
        EE_UNUSED_ARG(objects);

        return true;
    }

    NiBound* m_sceneBound;
};

//------------------------------------------------------------------------------------------------
ToolSceneGraphService::ToolSceneGraphService(
    NiSPWorkflowManager* pWorkflowManager,
    NiTexturePalette* pTexturePalette)
    : SceneGraphService(pWorkflowManager, pTexturePalette, true)
    , m_isShutdown(false)
    , m_recomputeBoundsCounter(0)
    , m_recomputeBoundsEpsilon(50)
    , m_worldScale(1.0f)
    , m_playAnimations(false)
    , m_useDefaultLights(false)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 1500;

    NiRoomGroup::SetPortallingDisabled(true);

    m_pUpdateProcess->SetForceSubmitModifiers(true);

    m_pUpdateProcess->SetTime(0.0f);

    m_spDefaultLight1 = NiNew NiDirectionalLight();
    m_spDefaultLight1->SetName("Default Light 1");
    m_spDefaultLight1->SetAmbientColor(NiColor::BLACK);
    m_spDefaultLight1->SetDiffuseColor(NiColor::WHITE);
    m_spDefaultLight1->SetSpecularColor(NiColor::WHITE);
    m_spDefaultLight1->SetRotate(NiMatrix3(
        NiPoint3(0.44663f, 0.496292f, -0.744438f),
        NiPoint3(0.0f, -0.83205f, -0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f)));
    m_spDefaultLight1->Update(*m_pUpdateProcess);

    m_spDefaultLight2 = NiNew NiDirectionalLight();
    m_spDefaultLight2->SetName("Default Light 2");
    m_spDefaultLight2->SetAmbientColor(NiColor::BLACK);
    m_spDefaultLight2->SetDiffuseColor(NiColor::WHITE);
    m_spDefaultLight2->SetSpecularColor(NiColor::WHITE);
    m_spDefaultLight2->SetRotate(NiMatrix3(
        NiPoint3(-0.44663f, -0.496292f, 0.744438f),
        NiPoint3(0.0f, 0.83205f, 0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f)));
    m_spDefaultLight2->Update(*m_pUpdateProcess);

    m_sceneBound.SetCenterAndRadius(NiPoint3::ZERO, 0);
}

//------------------------------------------------------------------------------------------------
ToolSceneGraphService::~ToolSceneGraphService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* ToolSceneGraphService::GetDisplayName() const
{
    return "ToolSceneGraphService";
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnServiceRegistered(efd::IAliasRegistrar* pAliasRegistrar)
{
    pAliasRegistrar->AddIdentity<SceneGraphService>();
    SceneGraphService::OnServiceRegistered(pAliasRegistrar);
}

//------------------------------------------------------------------------------------------------
SyncResult ToolSceneGraphService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    SceneGraphService::OnPreInit(pDependencyRegistrar);

    efd::MessageService* pMsgService =
        m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(pMsgService);

    pMsgService->Subscribe(this, kCAT_LocalMessage);

    m_spRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_spRenderService);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolSceneGraphService::OnInit()
{
    AsyncResult result = SceneGraphService::OnInit();

    if (result == efd::AsyncResult_Complete)
    {
        m_spRenderService->AddDelegate(this);
    }

    return result;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolSceneGraphService::OnTick()
{
    SceneGraphService::OnTick();

    m_spDefaultLight1->Update(*m_pUpdateProcess);
    m_spDefaultLight2->Update(*m_pUpdateProcess);

    // Update the validate frames call to match playing animations setting
    RenderContext* pActiveContext = m_spRenderService->GetActiveRenderContext();

    if (pActiveContext != NULL)
        pActiveContext->SetValidateFrames(!m_playAnimations);

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::UpdateDynamicEntities(bool, bool, bool)
{
    RenderContext* activeContext = m_spRenderService->GetActiveRenderContext();

    if (activeContext != NULL && !activeContext->GetValidateFrames())
    {
        SceneGraphService::UpdateDynamicEntities(true, true, true);
    }
    else
    {
        SceneGraphService::UpdateDynamicEntities(false, false, true);

        //// Ensure that the SceneGraphsUpdatedMessage is fired off every tick.
        //SceneGraphService::OnSceneGraphsUpdated();

        //// We need to clear the toUpdateOnce list in the Tool SGS, because we have our own way of
        //// updating entities when they change that works for dynamic and static since we have the
        //// ability to stop updating those lists of entities.
        //m_toUpdateOnce.clear();
    }
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolSceneGraphService::OnShutdown()
{
    if (!m_isShutdown)
    {
        m_isShutdown = true;

        m_spRenderService->RemoveDelegate(this);

        efd::MessageService* pMsgService =
            m_pServiceManager->GetSystemServiceAs<efd::MessageService>();

        if (pMsgService != NULL)
        {
            pMsgService->Unsubscribe(this, kCAT_LocalMessage);
        }

        m_spDefaultLight1->DetachAllAffectedNodes();
        m_spDefaultLight2->DetachAllAffectedNodes();

        m_spDefaultLight1 = NULL;
        m_spDefaultLight2 = NULL;
    }

    m_spRenderService = NULL;

    return SceneGraphService::OnShutdown();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnPropertyUpdate(
    const efd::ClassID& callerID,
    egf::Entity* pEntity,
    const egf::PropertyID& propertyID,
    const IProperty* pProperty,
    const efd::UInt32 tags)
{
    SceneGraphService::OnPropertyUpdate(callerID, pEntity, propertyID, pProperty, tags);

    NiAVObject* pSceneGraph = GetSceneGraphFromEntity(pEntity);

    UpdateToolScene(pEntity, pSceneGraph);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::UpdateDirtyProperties(egf::Entity* pEntity)
{
    // Only do work if we know about the scene graph
    SceneGraphData* pSceneGraphData = GetEntityData(pEntity);

    // Ignore entities we don't know about
    if (!pSceneGraphData)
        return;

    // We only update the properties of entities we are managing.
    if (pSceneGraphData->IsAssetExternal())
        return;

    /// We only update the properties of entities we are managing.
    const egf::FlatModel* pModel = pEntity->GetModel();

    // Check if the entity is a proxy
    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_Proxy))
    {
        if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_ProxyScale))
        {
            // If the entity is a proxy, we need to adjust the proxy representation a little bit
            // differently.  We need to adjust the scale based on the WorldScale * ProxyScale.
            float proxyScale;
            if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyScale, proxyScale)
                == PropertyResult_OK)
            {
                NiAVObject* pSceneGraph = pSceneGraphData->GetSceneGraph();
                if (pSceneGraph)
                {
                    pSceneGraph->SetScale(proxyScale * GetWorldScale());
                    pSceneGraph->Update(*m_pUpdateProcess);
                }
            }
        }
    }

    SceneGraphService::UpdateDirtyProperties(pEntity);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::UpdateVolatileProperties(
    egf::Entity* pEntity,
    NiAVObject* pSceneGraph,
    bool initialUpdate)
{
    SceneGraphService::UpdateVolatileProperties(pEntity, pSceneGraph, initialUpdate);

    UpdateToolScene(pEntity, pSceneGraph);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::UpdateAttachedProperty(egf::Entity* pEntity)
{
    SceneGraphService::UpdateAttachedProperty(pEntity);

    NiAVObject* pSceneGraph = GetSceneGraphFromEntity(pEntity);

    UpdateToolScene(pEntity, pSceneGraph);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::UpdateToolScene(
    egf::Entity* pEntity,
    NiAVObject* pSceneGraph)
{
    SceneGraphData* pData = GetEntityData(pEntity);
    if (!pData || pData->IsAssetExternal())
        return;

    // Make sure it's a valid entity (not one with an empty NifAsset)
    if (!pSceneGraph)
        return;

    const egf::FlatModel* pModel = pEntity->GetModel();

    // Check if the entity is a proxy
    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_Proxy))
    {
        // If the entity is a proxy, we need to adjust the proxy representation a little bit
        // differently.  We need to adjust the scale based on the WorldScale * ProxyScale.
        float proxyScale;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyScale, proxyScale)
            == PropertyResult_OK)
        {
            pSceneGraph->SetScale(proxyScale * GetWorldScale());
        }
    }

    // Set the invisibility override.
    bool visible = true;
    for (efd::UInt32 ui = 0; ui < VISIBILITY_MAX; ui++)
    {
        if (m_invisibilityOverride[ui].find(pEntity->GetEntityID(), visible))
        {
            pSceneGraph->SetAppCulled(true);
            break;
        }
    }

    AddToUpdateOnce(pData);

    if (m_recomputeBoundsCounter > m_recomputeBoundsEpsilon)
    {
        RecomputeSceneBoundsFunctor kFunctor;
        ForEachEntitySceneGraph(kFunctor);

        m_sceneBound = *kFunctor.m_sceneBound;

        m_recomputeBoundsCounter = 0;
    }
    else
    {
        const NiBound& kSceneGraphBounds = pSceneGraph->GetWorldBound();
        m_sceneBound.Merge(&kSceneGraphBounds);
        m_recomputeBoundsCounter++;
    }

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSceneGraphAdded(egf::Entity* pEntity, NiAVObject* pSceneGraph)
{
    SceneGraphService::OnSceneGraphAdded(pEntity, pSceneGraph);

    if (m_useDefaultLights)
        AttachDefaultLights(pSceneGraph, true);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSceneGraphRemoved(egf::Entity* pEntity, NiAVObject* pSceneGraph)
{
    SceneGraphService::OnSceneGraphRemoved(pEntity, pSceneGraph);

    if (m_useDefaultLights)
        DetachDefaultLights(pSceneGraph, true);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSceneGraphAdded(SceneGraphHandle handle, NiAVObject* pSceneGraph)
{
    SceneGraphService::OnSceneGraphAdded(handle, pSceneGraph);

    if (m_useDefaultLights)
        AttachDefaultLights(pSceneGraph, true);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSceneGraphRemoved(SceneGraphHandle handle, NiAVObject* pSceneGraph)
{
    SceneGraphService::OnSceneGraphRemoved(handle, pSceneGraph);

    if (m_useDefaultLights)
        DetachDefaultLights(pSceneGraph, true);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::AttachDefaultLights(NiAVObject* pSceneGraph, bool updateLightEffects)
{
    NiNode* pNode = NiDynamicCast(NiNode, pSceneGraph);

    if (pNode != NULL)
    {
        m_spDefaultLight1->AttachAffectedNode(pNode);
        m_spDefaultLight2->AttachAffectedNode(pNode);

        if (updateLightEffects)
        {
            m_spDefaultLight1->UpdateEffects();
            m_spDefaultLight2->UpdateEffects();
        }

        pNode->UpdateEffects();
    }
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::DetachDefaultLights(NiAVObject* pSceneGraph, bool updateLightEffects)
{
    NiNode* pNode = NiDynamicCast(NiNode, pSceneGraph);

    if (pNode != NULL)
    {
        m_spDefaultLight1->DetachAffectedNode(pNode);
        m_spDefaultLight2->DetachAffectedNode(pNode);

        if (updateLightEffects)
        {
            m_spDefaultLight1->UpdateEffects();
            m_spDefaultLight2->UpdateEffects();
        }

        pNode->UpdateEffects();
    }
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSettingsChanged(const SettingsUpdateMessage* pMessage,
    efd::Category targetChannel)
{
    const SettingsUpdateMessage* pSettingsUpdate =
        static_cast<const SettingsUpdateMessage*>(pMessage);

    utf8string kSettingName = pSettingsUpdate->GetSettingName();

    if (kSettingName == "Emergent.WorldBuilder.Lights.UseDefaultLights")
    {
        // Update default lights toggle
        m_useDefaultLights = (pSettingsUpdate->GetSettingValue().compare("True") == 0);

        ToolSceneGraphService::DefaultLightAttacherFunctor kFunctor(this, m_useDefaultLights);
        ForEachEntitySceneGraph(kFunctor);
        ForEachHandleSceneGraph(kFunctor);

        m_spDefaultLight1->UpdateEffects();
        m_spDefaultLight2->UpdateEffects();
    }
    else if (kSettingName == "Emergent.WorldBuilder.World.Scale")
    {
        m_worldScale = (float)atof(pSettingsUpdate->GetSettingValue().c_str());
    }
    else if (kSettingName == "Emergent.WorldBuilder.PlayAnimations")
    {
        m_playAnimations = (pSettingsUpdate->GetSettingValue().compare("True") == 0);

        egf::GameTimeClock* pClock =
            (egf::GameTimeClock*)m_pServiceManager->GetClock(kCLASSID_GameTimeClock);

        if (m_playAnimations)
        {
            if (pClock->IsPaused())
            {
                pClock->Resume();
            }
        }
        else
        {
            if (!pClock->IsPaused())
            {
                pClock->Pause();
            }
        }
    }

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnToolsVisibilityChanged(
    const StreamMessage* pMsg,
    efd::Category targetChannel)
{
    pMsg->ResetForUnpacking();

    // Is this a visibility message or not (0 = false, 1 = true)
    efd::UInt32 visibilityMsgId = 0;
    *pMsg >> visibilityMsgId;

    efd::UInt32 visibilityCategory = 0;
    *pMsg >> visibilityCategory;

    // Get the entity ids from the stream
    efd::UInt32 entityCount = 0;
    *pMsg >> entityCount;

    // If this is a show message, remove the id's from the array and mark the removed
    // entities as dirty
    if (visibilityMsgId == 1)
    {
        efd::ID128 persistentEntityID;
        for (efd::UInt32 ui = 0; ui < entityCount; ui++)
        {
            *pMsg >> persistentEntityID;

            // Add to dirty list
            egf::Entity* entity = m_pEntityManager->LookupEntityByDataFileID(persistentEntityID);
            EE_ASSERT(entity);
            egf::EntityID entityID = entity->GetEntityID();
            m_invisibilityOverride[visibilityCategory].erase(entityID);
            NiAVObject* pObject = GetSceneGraphFromEntity(entity);
            if (pObject)
                UpdateVolatileProperties(entity, pObject);
        }
    }
    // If this is a hide message, add the id's to the array and mark the added entities as dirty
    else
    {
        efd::ID128 persistentEntityID;
        for (efd::UInt32 ui = 0; ui < entityCount; ui++)
        {
            *pMsg >> persistentEntityID;
            bool value = false;
            egf::Entity* entity = m_pEntityManager->LookupEntityByDataFileID(persistentEntityID);
            EE_ASSERT(entity);
            egf::EntityID entityID = entity->GetEntityID();

            if (m_invisibilityOverride[visibilityCategory].find(entityID, value))
            {
            }
            else
            {
                m_invisibilityOverride[visibilityCategory][entityID] = value;

                // Add to dirty list
                NiAVObject* pObject = GetSceneGraphFromEntity(entity);
                if (pObject)
                    UpdateVolatileProperties(entity, pObject);
            }
        }
    }

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
NiAVObjectPtr ToolSceneGraphService::LoadSceneGraph(const efd::utf8string& fileName)
{
    efd::utf8string pathToScene = m_strDataPath;
    pathToScene += efd::PathUtils::GetNativePathSeperator() + fileName;

    NiStream nifStream;
    nifStream.SetTexturePalette(m_spSceneGraphCache->GetTexturePalette());
    bool loaded = nifStream.Load(pathToScene.c_str());
    if (!loaded)
    {
        char* pcDataPath = NULL;
        pcDataPath = EE_EXTERNAL_NEW char[NI_MAX_PATH];
        NiPath::GetExecutableDirectory(pcDataPath, NI_MAX_PATH);

        pathToScene = pcDataPath;
        pathToScene += "Data";
        pathToScene += efd::PathUtils::GetNativePathSeperator() + fileName;

        nifStream.RemoveAllObjects();
        nifStream.ResetLastErrorInfo();
        loaded = nifStream.Load(pathToScene.c_str());

        if (!loaded)
            return NULL;
    }

    efd::vector<NiObjectPtr> objects;
    for (efd::UInt32 ui = 0; ui < nifStream.GetObjectCount(); ++ui)
    {
        if (nifStream.GetObjectAt(ui))
            objects.push_back(nifStream.GetObjectAt(ui));
    }

    AddSceneGraph(objects, true, false);

    return NiDynamicCast(NiAVObject, objects[0]);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSurfacePreDraw(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_UNUSED_ARG(pSurface);
}

//------------------------------------------------------------------------------------------------
void ToolSceneGraphService::OnSurfacePostDraw(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_UNUSED_ARG(pSurface);
}

//------------------------------------------------------------------------------------------------
