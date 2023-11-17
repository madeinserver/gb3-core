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

// Pre-compiled header
#include "egmAnimationPCH.h"

#include "AnimationService.h"
#include "KFMFactory.h"
#include "KFMFactoryRequest.h"
#include "KFMFactoryResponse.h"
#include "ActorModel.h"

#include <NiActorManager.h>

#include <efd/ConfigManager.h>
#include <efd/IDs.h>
#include <efd/MessageService.h>
#include <efd/PathUtils.h>
#include <efd/TimeType.h>
#include <efd/ecrLogIDs.h>
#include <efd/ServiceManager.h>

#include <efd/AssetConfigService.h>
#include <efd/AssetLocatorService.h>
#include <efd/AssetCacheResponse.h>

#include <egf/EntityManager.h>
#include <egf/EntityChangeMessage.h>
#include <egf/egfClassIDs.h>
#include <egf/StandardModelLibraryPropertyIDs.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>
#include <egf/PlaceableModel.h>

#include <ecr/SceneGraphService.h>

#include <egf/egfLogIDs.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmAnimation;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(AnimationService);

// Register for changes to locally owned entities:

EE_HANDLER_WRAP(AnimationService, HandleEntityEnterWorldMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityEnterWorld);
EE_HANDLER_WRAP(AnimationService, HandleEntityExitWorldMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityExitWorld);
EE_HANDLER_WRAP(AnimationService, HandleEntityUpdatedMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityUpdated);

// Register for asset change related messages:
EE_HANDLER(AnimationService, HandleSceneGraphAddedMessage, ecr::SceneGraphAddedMessage);
EE_HANDLER(AnimationService, HandleSceneGraphRemovedMessage, ecr::SceneGraphRemovedMessage);
EE_HANDLER(AnimationService, HandleSceneGraphCacheResponse, ecr::SceneGraphCacheResponse);

// Register for asset load messages
EE_HANDLER(AnimationService, HandleCacheResponse, KFMCacheResponse);

// Register for entity preload messages
EE_HANDLER(AnimationService, HandlePreloadRequest, EntityPreloadRequest);


NiImplementRTTI(AnimationService::TextKeyBehavior, NiTextKeyMatch);
NiImplementRTTI(AnimationService::TextKeyMatchAll, TextKeyBehavior);

const efd::UInt32 AnimationService::ActorEntry::kInvalidUpdateIndex = (efd::UInt32)(-1);

//------------------------------------------------------------------------------------------------
AnimationService::TextKeyBehavior::TextKeyBehavior(
    const char* match,
    const char* behavior) :
    NiTextKeyMatch(match),
    m_behavior(behavior)
{
}

//------------------------------------------------------------------------------------------------
void AnimationService::TextKeyBehavior::InvokeBehavior(
    egf::Entity* pEntity,
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    const NiFixedString& textKey,
    float eventTime)
{
    // Don't send text key callbacks from replicated entities, as the owned entity
    // will receive one per replicated copy.
    if (!pEntity || !GetBehavior().Exists() || !pEntity->IsOwned())
        return;

    efd::ParameterListPtr spStream = EE_NEW efd::ParameterList();
    spStream->AddParameter("Sequence", sequenceID);
    spStream->AddParameter("KeyName", efd::utf8string(textKey));
    spStream->AddParameter("UpdateTime", pActorMgr->GetLastUpdateTime());
    spStream->AddParameter("EventTime", eventTime);

    pEntity->SendEvent(
        pEntity->GetEntityID(),
        pEntity->GetModelName().c_str(),
        GetBehavior(),
        spStream);
}

//------------------------------------------------------------------------------------------------
AnimationService::TextKeyMatchAll::TextKeyMatchAll(const char* behavior) :
    TextKeyBehavior("", behavior)
{
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::TextKeyMatchAll::IsKeyMatch(const NiFixedString& textKey)
{
    SetLastMatchedKey(textKey);
    return true;
}

//------------------------------------------------------------------------------------------------
AnimationService::AnimationEventCallback::AnimationEventCallback()
{
}

//------------------------------------------------------------------------------------------------
void AnimationService::AnimationEventCallback::AnimActivated(
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    NiControllerSequence* pSeq,
    float eventTime)
{
    EE_UNUSED_ARG(pActorMgr);
    EE_UNUSED_ARG(sequenceID);
    EE_UNUSED_ARG(pSeq);
    EE_UNUSED_ARG(eventTime);

    // We do not respond in any way to this event, so leave it empty.
}

//------------------------------------------------------------------------------------------------
void AnimationService::AnimationEventCallback::AnimDeactivated(
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    NiControllerSequence* pSeq,
    float eventTime)
{
    EE_UNUSED_ARG(pActorMgr);
    EE_UNUSED_ARG(sequenceID);
    EE_UNUSED_ARG(pSeq);
    EE_UNUSED_ARG(eventTime);

    // We do not respond in any way to this event, so leave it empty.
}

//------------------------------------------------------------------------------------------------
void AnimationService::AnimationEventCallback::AnimCompleted(
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    NiControllerSequence* pSeq,
    float eventTime)
{
    EE_UNUSED_ARG(pActorMgr);
    EE_UNUSED_ARG(sequenceID);
    EE_UNUSED_ARG(pSeq);
    EE_UNUSED_ARG(eventTime);

    // We do not respond in any way to this event, so leave it empty.
}

//------------------------------------------------------------------------------------------------
void AnimationService::AnimationEventCallback::TextKeyEvent(
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    const NiFixedString& textKey,
    const NiTextKeyMatch* pMatchObject,
    NiControllerSequence* pSeq,
    float eventTime)
{
    EE_UNUSED_ARG(pSeq);

    TextKeyBehavior* pCompare = NiDynamicCast(TextKeyBehavior, pMatchObject);
    EE_ASSERT(pCompare);
    EE_ASSERT(m_pEntity);
    if (pCompare)
        pCompare->InvokeBehavior(m_pEntity, pActorMgr, sequenceID, textKey, eventTime);
}

//------------------------------------------------------------------------------------------------
void AnimationService::AnimationEventCallback::EndOfSequence(
    NiActorManager* pActorMgr,
    NiActorManager::SequenceID sequenceID,
    NiControllerSequence* pSeq,
    float eventTime)
{
    EE_UNUSED_ARG(pSeq);
    EE_UNUSED_ARG(eventTime);
    EE_ASSERT(pActorMgr);

    SequenceTimeMap::iterator itor = m_easeOutTimes.find(sequenceID);
    if (itor != m_easeOutTimes.end())
        pActorMgr->DeactivateSequence(sequenceID, itor->second);
    else
        pActorMgr->DeactivateSequence(sequenceID);
}

//------------------------------------------------------------------------------------------------
AnimationService::AnimationService(const bool toolMode) :
    m_pMessageService(NULL),
    m_pEntityManager(NULL),
    m_pSceneGraphService(NULL),
    m_kAnimationTag("gamebryo-animation"),
    m_kAnimationKFTag("gamebryo-sequence-file"),
    m_assetLoadCategory(kCAT_INVALID),
    m_assetRefreshCategory(kCAT_INVALID),
    m_cacheAddCategory(kCAT_INVALID),
    m_cacheRemoveCategory(kCAT_INVALID),
    m_assetPreloadRequestCategory(kCAT_INVALID),
    m_assetPreloadResponseCategory(kCAT_INVALID),
    m_kfmCacheCategory(kCAT_INVALID),
    m_sceneGraphCacheCategory(kCAT_INVALID),
    m_actorMessageCategory(kCAT_INVALID),
    m_toolMode(toolMode)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 1800;

    m_spKFMCache = EE_NEW KFMCache();
}

//------------------------------------------------------------------------------------------------
AnimationService::~AnimationService()
{
}

//------------------------------------------------------------------------------------------------
const char* AnimationService::GetDisplayName() const
{
    return "AnimationService";
}

//------------------------------------------------------------------------------------------------
SyncResult AnimationService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<SceneGraphService>();
    // Because we register an IAssetFactory (via the KFMCache that we manage) we must depend on the
    // AssetFactoryManager class.
    pDependencyRegistrar->AddDependency<AssetFactoryManager>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);
    if (!m_pMessageService)
        return SyncResult_Failure;

    AssetLocatorService* pALS = m_pServiceManager->GetSystemServiceAs<efd::AssetLocatorService>();
    AssetFactoryManager* pAFM = m_pServiceManager->GetSystemServiceAs<efd::AssetFactoryManager>();
    m_spKFMCache->Init(m_pMessageService, pALS, pAFM);

    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);
    if (!m_pEntityManager)
        return SyncResult_Failure;

    m_pSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_pSceneGraphService);
    if (!m_pSceneGraphService)
        return SyncResult_Failure;

    if (!SubscribeToMessages())
        return SyncResult_Failure;

    egf::FlatModelManager* pFlatModelManager =
        m_pServiceManager->GetSystemServiceAs<egf::FlatModelManager>();
    if (pFlatModelManager)
    {
        pFlatModelManager->RegisterBuiltinModelFactory(
            "Actor",
            kFlatModelID_StandardModelLibrary_Actor,
            ActorModel::ActorModelFactory);
    }

    // Register for callbacks from various built-in models
    PlaceableModel::AddCallback(this);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnInit()
{
    // Register for preloading in OnInit when we know the EntityManager will have
    // been initialized ready to handle this method.
    m_pEntityManager->RegisterPreloadService(m_assetPreloadResponseCategory);

    // Set up a callback to the reload service so the KFMCache gets refresh requests.
    // This is to facilitate rapid iteration on animation assets.
    ReloadManager* pReloadMgr = m_pServiceManager->GetSystemServiceAs<ReloadManager>();
    if (pReloadMgr)
    {
        pReloadMgr->RegisterAssetChangeHandler(m_kAnimationTag, m_spKFMCache);
        pReloadMgr->RegisterAssetChangeHandler(m_kAnimationKFTag, m_spKFMCache);
    }

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnTick()
{
    // Ticking the Animation Service causes all update-able actors to be updated.
    float currTime = (float)m_pServiceManager->GetTime(kCLASSID_GameTimeClock);

    efd::UInt32 numToUpdate = static_cast<efd::UInt32>(m_toUpdate.size());
    for (efd::UInt32 ui = 0; ui < numToUpdate; ++ui)
    {
        ActorEntry* pActor = m_toUpdate[ui];
        if (pActor->m_spActorManager)
        {
            pActor->m_spActorManager->Update(currTime);
        }
    }

    // Return pending to indicate the service should continue to be ticked.
    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult AnimationService::OnShutdown()
{
    if (m_pMessageService != NULL)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
        m_pMessageService->Unsubscribe(this, m_assetLoadCategory);
        m_pMessageService->Unsubscribe(this, m_assetRefreshCategory);
        m_pMessageService->Unsubscribe(this, m_cacheAddCategory);
        m_pMessageService->Unsubscribe(this, m_cacheRemoveCategory);
        m_pMessageService->Unsubscribe(this, m_assetPreloadRequestCategory);

        m_pMessageService->Unsubscribe(this, m_kfmCacheCategory);
        m_pMessageService->Unsubscribe(this, m_sceneGraphCacheCategory);
        m_pMessageService = NULL;
    }

    if (m_pEntityManager)
    {
        m_pEntityManager->UnregisterPreloadService(m_assetPreloadResponseCategory);
        m_pEntityManager = NULL;
    }

    PlaceableModel::RemoveCallback(this);

    ActorMap::iterator entityIter = m_entityData.begin();
    while (entityIter != m_entityData.end())
    {
        entityIter->second = 0;
        ++entityIter;
    }
    m_entityData.clear();

    AssetMap::iterator assetIDIter = m_assetsByPhysicalID.begin();
    while (assetIDIter != m_assetsByPhysicalID.end())
    {
        if (assetIDIter->second->m_cacheHandle)
        {
            m_spKFMCache->UnCacheKFM(assetIDIter->second->m_cacheHandle);
            assetIDIter->second->m_cacheHandle = 0;
        }
        assetIDIter->second = 0;
        ++assetIDIter;
    }
    m_assetsByPhysicalID.clear();

    m_toUpdate.clear();

    assetIDIter = m_pendingAssetLoads.begin();
    while (assetIDIter != m_pendingAssetLoads.end())
    {
        assetIDIter->second = 0;
        ++assetIDIter;
    }
    m_pendingAssetLoads.clear();

    m_spKFMCache->Shutdown();
    m_spKFMCache = 0;

    m_pSceneGraphService = NULL;

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::HasKFMAssetProperty(
    const egf::Entity* pEntity,
    efd::AssetID& kfmAssetID)
{
    if (!pEntity->GetModel()->ContainsModel(kFlatModelID_StandardModelLibrary_Actor))
    {
        return false;
    }

    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_KfmAsset, kfmAssetID) !=
        PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("KFMAsset property missing for entity model: %s",
            pEntity->GetModelName().c_str()));
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void AnimationService::AddEntityWithActorModel(egf::Entity* pEntity)
{
    EE_LOG(efd::kEntity, efd::ILogger::kLVL3,
        ("%s: entity ID: %s",
        __FUNCTION__,
        pEntity->GetEntityID().ToString().c_str()));

    // Create an actor entry to track this entity.
    ActorEntry* pData = CreateEntityData(pEntity);

    if (!pData)
    {
        EE_LOG(efd::kEntity, efd::ILogger::kERR2,
            ("%s: entity added twice! ID: %s",
            __FUNCTION__,
            pEntity->GetEntityID().ToString().c_str()));
        return;
    }

    // All entities found this way start off with the same in/out world status as their entity
    pData->SetIsInWorld(pEntity->IsInWorld());

    // Get the asset ID
    efd::AssetID assetID;
    EE_VERIFY(HasKFMAssetProperty(pEntity, assetID));

    if (assetID.GetURN().empty())
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("KfmAsset property value empty for entity ID: %s",
            pEntity->GetEntityID().ToString().c_str()));
        return;
    }

    AssetData* pAssetData = FindAssetData(assetID.GetURN());
    if (!pAssetData)
    {
        // This will launch an asset load request for the asset
        pAssetData = CreateAssetData(assetID.GetURN());
    }

    if (!pAssetData)
        return;

    // This will add the entity to the asset data. If the asset is ready, it will set up
    // this entities objects, otherwise we just record that the entity needs the asset.
    // This function may generate messages when the asset is ready.
    AddEntityToAssetData(pAssetData, pData);
}

//------------------------------------------------------------------------------------------------
void AnimationService::RemoveEntityWithActorModel(egf::Entity* pEntity)
{
    RemoveActorEntity(pEntity);
}

//------------------------------------------------------------------------------------------------
void AnimationService::OnPropertyUpdate(
    const egf::FlatModelID& modelID,
    egf::Entity* pEntity,
    const egf::PropertyID& propertyID,
    const egf::IProperty* pProperty,
    const efd::UInt32 tags)
{
    EE_UNUSED_ARG(propertyID);
    EE_UNUSED_ARG(pProperty);
    EE_UNUSED_ARG(tags);

    ActorMap::iterator entityIter = m_entityData.find(pEntity->GetEntityID());
    if (entityIter == m_entityData.end())
        return;

    if (modelID == kFlatModelID_StandardModelLibrary_Placeable)
        UpdateTransform(entityIter->second);
}

//------------------------------------------------------------------------------------------------
void AnimationService::UpdateTransform(ActorEntry* pActorEntry)
{
    NiActorManager* pActorManager = pActorEntry->m_spActorManager;
    if (!pActorManager)
        return;

    NiAVObject* pNifRoot = pActorManager->GetNIFRoot();
    if (!pNifRoot)
        return;

    const PlaceableModel* pPlaceable = static_cast<const PlaceableModel*>(
        pActorEntry->m_pEntity->FindBuiltinModel(kFlatModelID_StandardModelLibrary_Placeable));
    EE_ASSERT(pPlaceable);

    if (!m_toolMode && pActorManager->GetAccumRoot() && 
        pActorManager->GetControllerManager()->GetCumulativeAnimations())
    {
        if (pPlaceable->GetScale() == 0.0f)
            return;

        // Get the desired transform as a quaternion
        NiQuatTransform desiredTransform;
        desiredTransform.SetScale(pPlaceable->GetScale());
        desiredTransform.SetTranslate(pPlaceable->GetPosition());
        Point3 rotation = pPlaceable->GetRotation();
        NiMatrix3 kXRot, kYRot, kZRot;
        kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS); // Roll  +x
        kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS); // Pitch +y
        kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS); // Yaw   +z
        desiredTransform.SetRotate(kXRot * kYRot * kZRot);

        // Compute the required delta transform, use quaternions to keep the rotation orthonormal
        NiAVObject* pAccumRoot = pActorManager->GetAccumRoot();
        NiQuatTransform currentTransform;
        currentTransform.SetTranslate(pAccumRoot->GetWorldTranslate());
        currentTransform.SetRotate(pAccumRoot->GetWorldRotate());
        currentTransform.SetScale(pAccumRoot->GetWorldScale());

        NiQuatTransform inverseTransform;
        currentTransform.HierInvert(inverseTransform);

        NiQuatTransform delta = desiredTransform.HierApply(inverseTransform);

        // Get the NIF Root transform
        NiQuatTransform rootTransform;
        rootTransform.SetTranslate(pNifRoot->GetWorldTranslate());
        rootTransform.SetRotate(pNifRoot->GetWorldRotate());
        rootTransform.SetScale(pNifRoot->GetWorldScale());

        // Compute the new NIF Root transform
        desiredTransform = delta.HierApply(rootTransform);

        // Apply it.
        if (pNifRoot->GetParent())
        {
            NiQuatTransform parentTransform;
            parentTransform.SetTranslate(pNifRoot->GetParent()->GetWorldTranslate());
            parentTransform.SetRotate(pNifRoot->GetParent()->GetWorldRotate());
            parentTransform.SetScale(pNifRoot->GetParent()->GetWorldScale());
            parentTransform.HierInvert(inverseTransform);
            desiredTransform = inverseTransform.HierApply(desiredTransform);
        }
        pNifRoot->SetTranslate(desiredTransform.GetTranslate());
        pNifRoot->SetScale(desiredTransform.GetScale());
        pNifRoot->SetRotate(desiredTransform.GetRotate());
    }
    else
    {
        pNifRoot->SetTranslate(pPlaceable->GetPosition());
        Point3 rotation = pPlaceable->GetRotation();
        NiMatrix3 kXRot, kYRot, kZRot;
        kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS); // Roll  +x
        kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS); // Pitch +y
        kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS); // Yaw   +z
        pNifRoot->SetRotate(kXRot * kYRot * kZRot);
        pNifRoot->SetScale(pPlaceable->GetScale());
    }

}

//------------------------------------------------------------------------------------------------
void AnimationService::EntityEnterWorld(ActorEntry* pData)
{
    if (pData->IsInWorld())
        return;

    if (pData->m_spActorManager)
    {
        AddToUpdate(pData);
    }

    pData->SetIsInWorld(true);
}

//------------------------------------------------------------------------------------------------
void AnimationService::EntityExitWorld(ActorEntry* pData)
{
    if (!pData->IsInWorld())
        return;

    if (pData->m_spActorManager)
    {
        RemoveFromUpdate(pData);
    }

    pData->SetIsInWorld(false);
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleEntityEnterWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    ActorEntry* pActorEntry = 0;
    if (ContainsEntity(pEntity->GetEntityID(), pActorEntry))
    {
        // Properties for the entity may have changed before entering the world, and we will not
        // receive any update for them. So we need to check now.
        UpdateDiscoveredEntity(pActorEntry);

        EntityEnterWorld(pActorEntry);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleEntityExitWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    ActorEntry* pActorEntry = 0;
    if (ContainsEntity(pEntity->GetEntityID(), pActorEntry))
    {
        EntityExitWorld(pActorEntry);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    // Look for a change in the actor.
    Entity* pEntity = pMessage->GetEntity();
    efd::AssetID actorName;
    if (HasKFMAssetProperty(pEntity, actorName) &&
        pEntity->IsDirty(kPropertyID_StandardModelLibrary_KfmAsset))
    {
        // Only do work if we know about the scene graph
        ActorEntry* pActorEntry = 0;
        if (!ContainsEntity(pEntity->GetEntityID(), pActorEntry))
            return;

        // Get the asset data
        AssetData* pAssetData = pActorEntry->m_spAssetData;

        // If the asset ID is invalid, remove the actor data
        if (!actorName.IsValidURN())
        {
            if (pAssetData)
            {
                RemoveEntityFromAssetData(pActorEntry);
            }
        }
        else
        {
            // Look for asset data for the current name
            AssetData* pNewAssetData = FindAssetData(actorName);
            if (!pNewAssetData || (pNewAssetData != pAssetData))
            {
                ReplaceAsset(pActorEntry, pNewAssetData, actorName);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleCacheResponse(
    const KFMCacheResponse* pMessage,
    efd::Category targetChannel)
{
    if (targetChannel == m_assetRefreshCategory)
    {
        EE_ASSERT(pMessage->GetRequestData() == 0);
        ProcessKFMCacheReload(pMessage);
        return;
    }

    KFMCacheRequestDataPtr spCacheData = (KFMCacheRequestData*)pMessage->GetRequestData();
    EE_ASSERT(spCacheData);

    if (pMessage->GetHandleCount() == 0)
    {
        // This situation only arises if the desired file cannot be located at all, in which case
        // it is not in the cache and we should remove all references to it, as it no longer
        // exists.
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR1,
            ("%s: Found no assets when looking for identifier '%s'. "
            "Reloads or other change notifications for this identifier will be ignored and "
            "existing assets using this identifer will have their actors removed. ",
            __FUNCTION__,
            pMessage->GetIdentifier().c_str()));

        m_pendingAssetLoads.erase(pMessage->GetIdentifier());

        // Remove any existing asset data for this asset
        if (spCacheData->m_pAssetData)
        {
            OnAssetLoadFailure(spCacheData->m_pAssetData);
        }

        // Some operations still need to send responses, so don't always exit.
        if (!spCacheData->m_pResponse)
        {
            return;
        }
    }

    if (spCacheData->m_pAssetData) // This was a load for a specific known asset
    {
        if (pMessage->GetHandleCount() > 1)
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kERR1,
                ("%s: Found %d assets when looking for tag '%s'. "
                "Ignoring all but the first.",
                __FUNCTION__,
                pMessage->GetHandleCount(),
                pMessage->GetIdentifier().c_str()));
        }

        spCacheData->m_pAssetData =
            SetAssetDataFromKFMCache(spCacheData->m_pAssetData, pMessage->GetHandle(0), false);
    }

    // Process any cache requests that result. Note that the asset may have been loaded for an
    // entity and loaded for the cache.
    if (spCacheData->m_pResponse)
    {
        if (pMessage->GetHandleCount() == 1)
        {
            m_requestIDToPhysicalID[pMessage->GetIdentifier()] =
                m_spKFMCache->GetPhysicalID(pMessage->GetHandle(0));
        }

        vector<KFMCache::KFMCacheHandle> handles;
        for (UInt32 ui = 0; ui < pMessage->GetHandleCount(); ++ui)
            handles.push_back(pMessage->GetHandle(ui));

        CacheHandles(spCacheData, handles);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::ProcessKFMCacheReload(const KFMCacheResponse* pMessage)
{
    // We only get these for reload requests
    EE_ASSERT(pMessage->GetHandleCount() == 1);

    const utf8string& physicalID = m_spKFMCache->GetPhysicalID(pMessage->GetHandle(0));
    const utf8string& fileName = m_spKFMCache->GetFilePath(pMessage->GetHandle(0));
    AssetDataPtr spAssetData = FindAssetData(physicalID);
    if (!spAssetData)
    {
        spAssetData = FindAssetData(fileName);
    }
    if (!spAssetData)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kLVL3,
            ("%s: Asset Refresh requested for an asset that we do not know about. "
            "Asset ID: %s, file name: %s",
            __FUNCTION__,
            physicalID.c_str(),
            fileName.c_str()));
        return;
    }
    EE_ASSERT(spAssetData->m_cacheHandle == pMessage->GetHandle(0));

    CacheSequenceNames(spAssetData);

    // Reset the actor on all entities using this asset
    efd::UInt32 numEntities = static_cast<efd::UInt32>(spAssetData->m_entities.size());
    for (efd::UInt32 ui = 0; ui < numEntities; ++ui)
    {
        ActorEntry* pEntityData = spAssetData->m_entities[ui];
        EE_ASSERT(pEntityData);

        ResetActorManager(pEntityData);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleSceneGraphAddedMessage(
    const SceneGraphAddedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    EE_ASSERT(pMessage);
    EE_ASSERT(m_pSceneGraphService);

    egf::Entity* pEntity = pMessage->GetEntity();
    if (!pEntity)
        return;

    const egf::EntityID& entityID = pEntity->GetEntityID();

    ActorMap::iterator entItor = m_entityData.find(entityID);
    if (entItor == m_entityData.end())
        return;

    EE_ASSERT(entItor->second->m_pEntity == pEntity);

    NiAVObject* pAVObject = m_pSceneGraphService->GetSceneGraphFromEntity(pEntity);
    EE_ASSERT(pAVObject);

    SetActorSceneGraph(entItor->second, pAVObject);
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleSceneGraphRemovedMessage(
    const SceneGraphRemovedMessage* pMessage,
    Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    EE_ASSERT(pMessage);
    EE_ASSERT(m_pSceneGraphService);

    egf::Entity* pEntity = pMessage->GetEntity();
    if (!pEntity)
        return;

    const egf::EntityID& entityID = pEntity->GetEntityID();

    ActorMap::iterator entItor = m_entityData.find(entityID);
    if (entItor == m_entityData.end())
        return;

    EE_ASSERT(entItor->second->m_pEntity == pEntity);

    UnsetActorSceneGraph(entItor->second);
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandleSceneGraphCacheResponse(
    const SceneGraphCacheResponse* pMessage,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    EE_ASSERT(pMessage);

    AssetData* pAssetData = (AssetData*)pMessage->GetRequestData();
    EE_ASSERT(pAssetData);
    EE_ASSERT(pAssetData->m_isCached);
    EE_ASSERT(pAssetData->m_spCacheLoadData);
    EE_ASSERT(pAssetData->m_spCacheLoadData->m_numOutstandingNIFs > 0);

    if (pMessage->GetHandleCount() > 0)
    {
        EE_ASSERT(pMessage->GetHandleCount() == 1);
        pAssetData->m_nifCacheHandle = pMessage->GetHandle(0);
    }

    KFMCacheRequestDataPtr spCacheData = pAssetData->m_spCacheLoadData;

    spCacheData->m_numOutstandingNIFs--;

    CheckCompletedCacheRequest(spCacheData);
}

//------------------------------------------------------------------------------------------------
void AnimationService::HandlePreloadRequest(
    const egf::EntityPreloadRequest* pRequest,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    // Examine the entity for assets we need to preload
    Entity* pEntity = pRequest->GetEntity();
    EE_LOG(efd::kEntity, efd::ILogger::kLVL3,
        ("Animation handling preload of entity %s",
         pEntity->GetModel()->GetName().c_str()));

    // We must have seen this entity via its Actor built-in
    ActorEntry* pEntityData = 0;
    if (!ContainsEntity(pEntity->GetEntityID(), pEntityData))
    {
        // Not an actor, so nothing to preload
        EntityPreloadResponse* pResponse = EE_NEW EntityPreloadResponse;
        pResponse->m_pEntity = pEntity;
        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
        return;
    }

    efd::AssetID kfmAssetID;
    EE_VERIFY(HasKFMAssetProperty(pEntity, kfmAssetID));
    if (!kfmAssetID.GetURN().empty())
    {
        // Make asset data if it doesn't already exist. It should, but maybe the asset was changed
        // between Actor built-in model initialization and preload.
        AssetData* pAssetData = FindAssetData(kfmAssetID);
        if (pAssetData != pEntityData->m_spAssetData)
        {
            ReplaceAsset(pEntityData, pAssetData, kfmAssetID);
        }

        if (pEntityData->m_spActorManager)
        {
            // Pass the preload request on to the scene graph service
            utf8string nifPath =
                (const char*)pEntityData->m_spActorManager->GetKFMTool()->GetFullModelPath();

            m_pSceneGraphService->Preload(pEntity, nifPath, m_assetPreloadResponseCategory);
        }
        else
        {
            // Record that we need to respond to a preload when the actor manager is ready.
            EntityPreloadData* pData = EE_NEW EntityPreloadData;
            pData->m_pEntityData = pEntityData;
            pData->m_responseCategory = m_assetPreloadResponseCategory;

            pEntityData->m_spAssetData->m_preloads.push_back(pData);
        }
    }
    else
    {
        // No actor asset ID, so nothing to preload
        EntityPreloadResponse* pResponse = EE_NEW EntityPreloadResponse;
        pResponse->m_pEntity = pEntity;
        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
        return;
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::UpdateDiscoveredEntity(ActorEntry* pActorEntry)
{
    // Get the asset data
    AssetData* pAssetData = pActorEntry->m_spAssetData;

    // Look for a change in the actor asset.
    efd::AssetID kfmName;
    if (HasKFMAssetProperty(pActorEntry->m_pEntity, kfmName))
    {
        // If the asset ID is invalid, remove the actor data
        if (!kfmName.IsValidURN())
        {
            if (pAssetData)
            {
                RemoveEntityFromAssetData(pActorEntry);

                return;
            }
        }
        else
        {
            // Look for asset data for the current name
            AssetData* pNewAssetData = FindAssetData(kfmName);

            if (!pNewAssetData || (pNewAssetData != pAssetData))
            {
                ReplaceAsset(pActorEntry, pNewAssetData, kfmName);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::ActivateLayeredSequenceById(
    const egf::EntityID& entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Bool autoDeactivate,
    efd::SInt32 priority,
    efd::Float32 weight,
    efd::Float32 easeInTime,
    efd::Float32 easeOutTime,
    NiActorManager::SequenceID timeSyncID,
    efd::Float32 freq,
    efd::Float32 startFrame,
    efd::Bool additiveBlend,
    efd::Float32 additiveRefFrame)
{
    ActorEntry* pEntry;
    if (!ContainsEntity(entityID, pEntry))
        return false;

    if (!pEntry->m_spActorManager)
        return false;

    NiControllerSequence* pCtrlSequence = pEntry->m_spActorManager->ActivateSequence(
        sequenceID,
        priority,
        weight,
        easeInTime,
        timeSyncID,
        freq,
        startFrame,
        additiveBlend,
        additiveRefFrame);

    if (!pCtrlSequence)
        return false;

    // If the user wishes to automatically deactivate the animation sequence when it's complete add
    // it to our listener object and add notify the actor manager.
    if (autoDeactivate)
    {
        pEntry->m_eventCallback.m_easeOutTimes[sequenceID] = easeOutTime;
        pEntry->m_spActorManager->RegisterCallback(NiActorManager::END_OF_SEQUENCE, sequenceID);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::DeactivateLayeredSequenceById(
    const egf::EntityID& entityID,
    NiActorManager::SequenceID id,
    efd::Float32 easeOutTime)
{
    NiActorManager* pActorManager = GetActorManager(entityID);
    if (!pActorManager)
        return false;

    return pActorManager->DeactivateSequence(id, easeOutTime);
}




//------------------------------------------------------------------------------------------------
// Caching
//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::Cache(
    const efd::AssetID& assetID,
    KFMCache::IKFMCacheRequestData* pRequestData,
    const efd::Category responseCategory,
    efd::vector<KFMCache::KFMCacheHandle>& o_handles)
{
    // Make a cache request data object, we need it regardless of what we do next.
    KFMCacheRequestData* pCacheData = EE_NEW KFMCacheRequestData;
    pCacheData->m_pAssetData = 0;
    pCacheData->m_pResponse = EE_NEW KFMCacheResponse(
            assetID.GetURN(),
            responseCategory,
            pRequestData,
            AssetCacheResponse::ACR_Success,
            AssetCacheResponse::ACT_CacheAdd);

    // See if we already know of this ID
    utf8string physicalID;
    AssetDataPtr spAssetData = FindAssetData(assetID.GetURN());
    if (spAssetData && spAssetData->m_state == AssetData::ADS_Ready)
    {
        o_handles.push_back(spAssetData->m_cacheHandle);
        return CacheHandles(pCacheData, o_handles);
    }

    AssetID filtered = assetID;
    if (assetID.GetURN().find(m_kAnimationTag) == efd::utf8string::npos)
    {
        // Append animation tag.  If this is not here, then asset locator overrides may cause
        // some assets to not be returned, i.e. if "reaper.kfm" and "reaper.nif" both exist
        // and the animation tag is not there, only one will be returned by the AssetLocate.
        utf8string urn = filtered.GetURN();
        urn += ":";
        urn += m_kAnimationTag;
        filtered.SetURN(urn);
    }

    // Send the request on to the cache. Note we may have already requested the asset for
    // another purpose, but requesting an as-yet-unready asset again ensures that messages
    // are correctly sent.
    if (m_spKFMCache->CacheKFM(
        filtered,
        m_kfmCacheCategory,
        pCacheData,
        o_handles))
    {
        return CacheHandles(pCacheData, o_handles);
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool AnimationService::CacheHandles(
    KFMCacheRequestDataPtr spCacheData,
    efd::vector<KFMCache::KFMCacheHandle>& handles)
{
    for (UInt32 ui = 0; ui < handles.size(); ++ui)
    {
        KFMCache::KFMCacheHandle handle = handles[ui];
        AssetDataPtr spAssetData = 0;
        if (!m_assetsByPhysicalID.find(m_spKFMCache->GetPhysicalID(handle), spAssetData))
        {
            spAssetData = CreateAssetDataWithHandle(handle);
        }
        else if (spAssetData->m_spEntityLoadData)
        {
            // We are already loading this, must be for an entity.
            // We know the asset data will be unchanged (no merging of data) because we did
            // not find the data in the map find above.
            spAssetData = SetAssetDataFromKFMCache(spAssetData, handle, true);
        }

        // An asset that is already cached should not be returned by another cache request
        if (!spAssetData->m_isCached)
        {
            spCacheData->m_pResponse->AddHandle(handle);
            spAssetData->m_isCached = true;

            // Request the NIF be cached if we have an actor manager
            NiActorManager* pActorManager = m_spKFMCache->GetActorManager(handle);
            if (pActorManager)
            {
                // Request caching of the NIF
                utf8string nifPath =
                    (const char*)pActorManager->GetKFMTool()->GetFullModelPath();

                vector<SceneGraphCache::SceneGraphCacheHandle> nifHandles;
                if (m_pSceneGraphService->CacheSceneGraphFileName(
                    nifPath,
                    spAssetData,
                    m_sceneGraphCacheCategory,
                    nifHandles))
                {
                    spAssetData->m_nifCacheHandle = nifHandles[0];
                }
                else
                {
                    spAssetData->m_spCacheLoadData = spCacheData;
                    spCacheData->m_numOutstandingNIFs++;
                }
            }
        }
    }

    // This will send a response if all the NIFs are loaded
    return CheckCompletedCacheRequest(spCacheData);
}

//------------------------------------------------------------------------------------------------
void AnimationService::UnCacheHandles(efd::vector<KFMCache::KFMCacheHandle>& i_handles)
{
    for (UInt32 ui = 0; ui < i_handles.size(); ++ui)
    {
        AssetData* pAssetData = FindAssetData(m_spKFMCache->GetPhysicalID(i_handles[ui]));
        if (pAssetData)
        {
            pAssetData->m_isCached = false;

            if (pAssetData->m_nifCacheHandle != SceneGraphCache::k_invalidHandle)
            {
                vector<SceneGraphCache::SceneGraphCacheHandle> nifHandles;
                nifHandles.push_back(pAssetData->m_nifCacheHandle);
                m_pSceneGraphService->UnCacheHandles(nifHandles);
                pAssetData->m_nifCacheHandle = SceneGraphCache::k_invalidHandle;
            }

            RemoveUnusedAssetData(pAssetData);
        }
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::RemoveAllFromCache()
{
    efd::vector<KFMCache::KFMCacheHandle> handles;
    efd::map<efd::utf8string, AssetDataPtr>::iterator iter = m_assetsByPhysicalID.begin();
    while (iter != m_assetsByPhysicalID.end())
    {
        if (iter->second->m_isCached)
            handles.push_back(iter->second->m_cacheHandle);
        iter++;
    }

    UnCacheHandles(handles);
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::SubscribeToMessages()
{
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    m_assetPreloadRequestCategory = m_pEntityManager->GetEntityPreloadCategory();
    m_pMessageService->Subscribe(this, m_assetPreloadRequestCategory);

    m_assetPreloadResponseCategory = m_pMessageService->GetGloballyUniqueCategory();

    m_assetLoadCategory = GenerateAssetResponseCategory();
    m_pMessageService->Subscribe(this, m_assetLoadCategory);
    m_assetRefreshCategory = m_spKFMCache->GetReloadCategory();
    m_pMessageService->Subscribe(this, m_assetRefreshCategory);
    m_cacheAddCategory = GenerateAssetResponseCategory();
    m_pMessageService->Subscribe(this, m_cacheAddCategory);
    m_cacheRemoveCategory = GenerateAssetResponseCategory();
    m_pMessageService->Subscribe(this, m_cacheRemoveCategory);

    m_kfmCacheCategory = m_pMessageService->GetGloballyUniqueCategory();
    m_pMessageService->Subscribe(this, m_kfmCacheCategory);

    m_sceneGraphCacheCategory = m_pMessageService->GetGloballyUniqueCategory();
    m_pMessageService->Subscribe(this, m_sceneGraphCacheCategory);

    m_actorMessageCategory = m_pMessageService->GetGloballyUniqueCategory();

    return true;
}

//------------------------------------------------------------------------------------------------
AnimationService::ActorEntry* AnimationService::CreateEntityData(egf::Entity* pEntity)
{
    ActorEntry* pNewEntityData = 0;
    if (ContainsEntity(pEntity->GetEntityID(), pNewEntityData))
    {
        return NULL;
    }

    // Create the data
    pNewEntityData = EE_NEW ActorEntry();
    pNewEntityData->m_pEntity = pEntity;

    // Get some rarely-changed properties from the mesh model or other models.
    bool isShared = true;
    if (pEntity->GetModel()->ContainsModel(kFlatModelID_StandardModelLibrary_Actor))
        pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_IsKfmAssetShared, isShared);
    pNewEntityData->SetIsAssetShared(isShared);

    bool isAccumulated = true;
    if (pEntity->GetModel()->ContainsModel(kFlatModelID_StandardModelLibrary_Actor))
    {
        pEntity->GetPropertyValue(
            kPropertyID_StandardModelLibrary_AccumulatesTransforms,
            isAccumulated);
    }
    pNewEntityData->SetIsAccumulated(isAccumulated);

    /// Set the callback entity.
    pNewEntityData->m_eventCallback.m_pEntity = pEntity;

    // Add to the map of known entities
    m_entityData[pEntity->GetEntityID()] = pNewEntityData;

    return pNewEntityData;
}

//------------------------------------------------------------------------------------------------
void AnimationService::RemoveActorEntity(egf::Entity* pEntity)
{
    ActorEntry* pActorEntry = 0;
    if (!ContainsEntity(pEntity->GetEntityID(), pActorEntry))
    {
        return;
    }

    RemoveEntityFromAssetData(pActorEntry);

    m_entityData.erase(pEntity->GetEntityID());
}

//------------------------------------------------------------------------------------------------
void AnimationService::SetActorManager(AssetData* pAssetData, ActorEntry* pActorData)
{
    EE_ASSERT(!pActorData->m_spActorManager);

    NiActorManager* pAssetActor = m_spKFMCache->GetActorManager(pAssetData->m_cacheHandle);

    if (!pAssetActor)
        return;

    NiActorManager* pActorManager =
            pAssetActor->CloneOnlyAnimation(NULL, pActorData->IsAccumulated());
    EE_ASSERT(pActorManager);

    pActorManager->SetCallbackObject(&(pActorData->m_eventCallback));

    // Update the target animation
    NiActorManager::SequenceID nextSequenceID = pActorData->m_currentSequenceID;
    if (nextSequenceID == NiActorManager::INVALID_SEQUENCE_ID)
    {
        efd::utf8string activeSequence;
        pActorData->m_pEntity->GetPropertyValue(
            kPropertyID_StandardModelLibrary_InitialSequence,
            activeSequence);
        efd::map<efd::utf8string, NiActorManager::SequenceID>::iterator itor =
            pAssetData->m_sequenceIDs.find(activeSequence.c_str());

        if (itor != pAssetData->m_sequenceIDs.end())
             pActorData->m_currentSequenceID = itor->second;
    }

    pActorData->m_spActorManager = pActorManager;

    // Request the NIF
    utf8string nifPath = (const char*)pActorManager->GetKFMTool()->GetFullModelPath();
    m_pSceneGraphService->CreateSceneGraphFileName(
        pActorData->m_pEntity,
        nifPath,
        false,
        !pActorData->IsInWorld(),
        false,
        false);

    // Remaining work is done in HandleSceneGraphAddedMessage
}

//------------------------------------------------------------------------------------------------
void AnimationService::UnsetActorManager(ActorEntry* pActorData)
{
    /// Get the existing actor manager and NIF root, if they exist
    NiActorManagerPtr spExistingManager = pActorData->m_spActorManager;

    if (spExistingManager)
    {
        // Pull the current animation, but only if the NIF Root is valid. If invalid, we do not
        // want to over-write the value we stored when the actor was created or when the scene
        // graph was removed.
        if (spExistingManager->GetNIFRoot())
        {
            pActorData->m_currentPaused = spExistingManager->IsPaused();
            pActorData->m_currentSequenceID = spExistingManager->GetTargetAnimation();
            if (pActorData->m_currentSequenceID == NiActorManager::INVALID_SEQUENCE_ID)
                pActorData->m_currentSequenceID = spExistingManager->GetCurAnimation();
        }

        // Remove from update list
        RemoveFromUpdate(pActorData);

        // Remove the entity's scene graph
        m_pSceneGraphService->RemoveSceneGraph(pActorData->m_pEntity);

        // Unregister the callback from the old actor
        spExistingManager->SetCallbackObject(NULL);

        pActorData->m_spActorManager = 0;
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::ResetActorManager(ActorEntry* pActorData)
{
    /// Keep a smart pointer to the existing actor manager so we can copy data off it
    NiActorManagerPtr spExistingManager = pActorData->m_spActorManager;

    UnsetActorManager(pActorData);

    SetActorManager(pActorData->m_spAssetData, pActorData);

    if (spExistingManager && pActorData->m_spActorManager)
        pActorData->m_spActorManager->CopyCallbacks(spExistingManager);
}

//------------------------------------------------------------------------------------------------
void AnimationService::ReplaceAsset(
    ActorEntry* pActorData,
    AssetData*& pNewAssetData,
    const efd::utf8string& newActorName)
{
    if (pActorData->m_spAssetData)
    {
        RemoveEntityFromAssetData(pActorData);
    }

    if (!pNewAssetData)
    {
        // This will launch an asset load request for the asset
        pNewAssetData = CreateAssetData(newActorName);
    }

    // This will add the entity to the asset data. If the asset is ready, it will set up
    // this entities objects, otherwise we just record that the entity needs the asset.
    // This function may generate messages when the asset is ready.
    AddEntityToAssetData(pNewAssetData, pActorData);
}

//------------------------------------------------------------------------------------------------
void AnimationService::SetActorSceneGraph(ActorEntry* pActorData, NiAVObject* pSceneGraph)
{
    NiActorManager* pActor = pActorData->m_spActorManager;
    if (!pActor)
        return;

    // When the scene graph was removed we would have cached the active animation
    if (!pActor->ChangeNIFRoot(pSceneGraph))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Actor root not found for kfm: %s. Actor will not be animated.",
            m_spKFMCache->GetFilePath(pActorData->m_spAssetData->m_cacheHandle).c_str()));
    }

    if (pActorData->m_currentSequenceID != NiActorManager::INVALID_SEQUENCE_ID)
        pActor->SetTargetAnimation(pActorData->m_currentSequenceID);
    pActor->SetPaused(pActorData->m_currentPaused);

    // Set up Placeable model feedback with the Scene Graph Service
    if (!m_toolMode && pActor->GetAccumRoot() &&
        pActor->GetControllerManager()->GetCumulativeAnimations())
    {
        NiAVObject* pAccumRoot = pActor->GetAccumRoot();

        PlaceableModel* pPlaceable = static_cast<PlaceableModel*>(
            pActorData->m_pEntity->FindBuiltinModel(kFlatModelID_StandardModelLibrary_Placeable));
        m_pSceneGraphService->EnablePlaceableFeedback(
            pPlaceable,
            pActorData->m_pEntity,
            pAccumRoot,
            this);
    }

    if (pSceneGraph)
    {
        UpdateTransform(pActorData);

        // Add to update list, if needed
        if (pActorData->IsInWorld())
        {
            AddToUpdate(pActorData);
        }

        // Inform other services.
        OnActorAdded(pActorData->m_pEntity);
    }

}

//------------------------------------------------------------------------------------------------
void AnimationService::UnsetActorSceneGraph(ActorEntry* pActorData)
{
    NiActorManager* pActor = pActorData->m_spActorManager;
    if (!pActor)
        return;

    // Changing the NIF Root resets the actor manager, so we need to remember the current
    // target animation so that we can switch to it again.
    pActorData->m_currentPaused = pActor->IsPaused();
    pActorData->m_currentSequenceID = pActor->GetTargetAnimation();
    if (pActorData->m_currentSequenceID == NiActorManager::INVALID_SEQUENCE_ID)
        pActorData->m_currentSequenceID = pActor->GetCurAnimation();

    if (pActor->GetNIFRoot())
    {
        RemoveFromUpdate(pActorData);

        // We are effectively removing the actor manager. Inform other services.
        OnActorRemoved(pActorData->m_pEntity);
        pActor->ChangeNIFRoot(NULL);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::OnActorAdded(egf::Entity* pEntity)
{
    ActorAddedMessagePtr spMessage = EE_NEW ActorAddedMessage();
    spMessage->m_pEntity = pEntity;

    m_pMessageService->SendLocal(spMessage, m_actorMessageCategory);
}

//------------------------------------------------------------------------------------------------
void AnimationService::OnActorRemoved(egf::Entity* pEntity)
{
    ActorRemovedMessagePtr spMessage = EE_NEW ActorRemovedMessage();
    spMessage->m_pEntity = pEntity;

    m_pMessageService->SendLocal(spMessage, m_actorMessageCategory);
}

//------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------
// AssetData management methods
//------------------------------------------------------------------------------------------------
AnimationService::AssetData* AnimationService::FindAssetData(
    const efd::utf8string& identifier) const
{
    // Look for asset data by any name it might be found.

    AssetDataPtr spData = 0;
    if (m_assetsByPhysicalID.find(identifier, spData))
    {
        return spData;
    }

    if (m_pendingAssetLoads.find(identifier, spData))
    {
        return spData;
    }

    utf8string physicalID;
    if (m_requestIDToPhysicalID.find(identifier, physicalID))
    {
        if (m_assetsByPhysicalID.find(physicalID, spData))
            return spData;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------
AnimationService::AssetData* AnimationService::CreateAssetData(const utf8string& identifier)
{
    EE_ASSERT(FindAssetData(identifier) == 0);

    AssetDataPtr spAssetData = EE_NEW AssetData;
    spAssetData->m_identifier = identifier;

    spAssetData->m_spEntityLoadData = EE_NEW KFMCacheRequestData;
    spAssetData->m_spEntityLoadData->m_pAssetData = spAssetData;
    spAssetData->m_spEntityLoadData->m_pResponse = 0;

    vector<KFMCache::KFMCacheHandle> handles;
    if (m_spKFMCache->CacheKFM(
        identifier,
        m_kfmCacheCategory,
        spAssetData->m_spEntityLoadData,
        handles))
    {
        if (handles.size() == 0)
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kERR1,
                ("%s: Found no assets when looking for tag '%s', or load error. ",
                __FUNCTION__,
                identifier.c_str()));

            spAssetData = 0;

            return 0;
        }

        if (handles.size() > 1)
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kERR1,
                ("%s: Found %d assets when looking for tag '%s'. "
                "Ignoring all but the first.",
                __FUNCTION__,
                handles.size(),
                identifier.c_str()));
        }

        spAssetData = SetAssetDataFromKFMCache(spAssetData, handles[0], true);

        return spAssetData;
    }

    m_pendingAssetLoads[identifier] = spAssetData;

    return spAssetData;
}

//------------------------------------------------------------------------------------------------
AnimationService::AssetData* AnimationService::CreateAssetDataWithHandle(
    KFMCache::KFMCacheHandle handle)
{
    EE_ASSERT(handle);

    const utf8string& physicalID = m_spKFMCache->GetPhysicalID(handle);
    EE_ASSERT(FindAssetData(physicalID) == 0);

    AssetData* pAssetData = EE_NEW AssetData;

    pAssetData->m_state = AssetData::ADS_Ready;
    pAssetData->m_physicalID = physicalID;
    pAssetData->m_cacheHandle = handle;
    m_assetsByPhysicalID[physicalID] = pAssetData;

    // Cache the sequence names
    CacheSequenceNames(pAssetData);

    return pAssetData;
}

//------------------------------------------------------------------------------------------------
AnimationService::AssetData* AnimationService::SetAssetDataFromKFMCache(
    AssetDataPtr spAssetData,
    KFMCache::KFMCacheHandle cacheHandle,
    bool usagePending)
{
    EE_ASSERT(cacheHandle);

    // It may be that this asset was also requested for a cache, in which case we cleared the
    // load data when we received the cache load response.
    if (!spAssetData->m_spEntityLoadData)
        return spAssetData;

    // Clear any entity load data on the asset data as we are done loading for entities.
    spAssetData->m_spEntityLoadData = 0;

    // Get the physical ID that we are setting from.
    const utf8string& physicalID = m_spKFMCache->GetPhysicalID(cacheHandle);

    EE_ASSERT(spAssetData->m_state == AssetData::ADS_Loading);

    // Add it to the set of known identifier->physical IDs and remove it from pending loads.
    m_pendingAssetLoads.erase(spAssetData->m_identifier);
    m_requestIDToPhysicalID[spAssetData->m_identifier] = physicalID;
    spAssetData->m_identifier.clear();

    // Now that we have a physical ID, we might discover we already have the NIF for
    // this request; we previously loaded it under a different request
    AssetData* pExistingData = FindAssetData(physicalID);
    if (pExistingData && pExistingData != spAssetData)
    {
        // We do indeed have an existing asset for this physical ID. Merge this one
        // with the existing one, which will do much of the same work as this function.
        MergeAssetData(pExistingData, spAssetData);
        return pExistingData;
    }

    // Set up data we now have from the cache handle.
    spAssetData->m_physicalID = physicalID;
    spAssetData->m_cacheHandle = cacheHandle;
    m_assetsByPhysicalID[physicalID] = spAssetData;

    // We have new data and are ready for use.
    spAssetData->m_state = AssetData::ADS_Ready;

    // Cache the sequence names
    CacheSequenceNames(spAssetData);

    // We are also assuming that at this point no entity using this
    // asset has an actor manager in use.

    // Set the actor manager on all entities
    efd::UInt32 numEntities = static_cast<efd::UInt32>(spAssetData->m_entities.size());
    for (efd::UInt32 ui = 0; ui < numEntities; ++ui)
    {
        ActorEntry* pEntityData = spAssetData->m_entities[ui];
        EE_ASSERT(pEntityData);

        SetActorManager(spAssetData, pEntityData);
    }

    // Mark all the preloads for this asset as completed.
    MarkPreloadsAsFound(spAssetData);

    // In theory this asset data is used, but remove it if it isn't.
    if (!usagePending)
        RemoveUnusedAssetData(spAssetData);

    return spAssetData;
}

//------------------------------------------------------------------------------------------------
void AnimationService::AddEntityToAssetData(AssetData* pAssetData, ActorEntry* pEntityData)
{
    EE_ASSERT(pEntityData->m_spAssetData == 0);

    pAssetData->m_entities.push_back(pEntityData);
    pEntityData->m_spAssetData = pAssetData;

    if (pAssetData->m_state == AssetData::ADS_Ready)
    {
        SetActorManager(pAssetData, pEntityData);
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::RemoveEntityFromAssetData(ActorEntry* pEntityData)
{
    AssetData* pAssetData = pEntityData->m_spAssetData;
    if (!pAssetData)
        return;

    UnsetActorManager(pEntityData);

    efd::vector<ActorEntry*>::iterator iter = pAssetData->m_entities.find(pEntityData);
    EE_ASSERT(iter != pAssetData->m_entities.end());
    pAssetData->m_entities.erase(iter);

    pAssetData->RemoveEntityFromPreload(pEntityData->m_pEntity);

    RemoveUnusedAssetData(pAssetData);

    pEntityData->m_spAssetData = 0;
}

//------------------------------------------------------------------------------------------------
void AnimationService::MergeAssetData(AssetData *pExistingData, AssetData *pRedundantData)
{
    EE_ASSERT(pExistingData != pRedundantData);

    // Go through all the objects on the redundant data and shift them to the new data.
    while (pRedundantData->m_entities.size() > 0)
    {
        ActorEntry* pEntityData = pRedundantData->m_entities.back();
        EE_ASSERT(pEntityData);

        EE_ASSERT(pEntityData->IsWaitingOnAsset());

        pEntityData->m_spAssetData = pExistingData;
        pExistingData->m_entities.push_back(pEntityData);
        pRedundantData->m_entities.pop_back();

        if (pExistingData->m_state == AssetData::ADS_Ready)
        {
            SetActorManager(pExistingData, pEntityData);
        }
    }

    // Transfer any preload info. If the existing data has been loaded, then the preloads are
    // all found. If not, we move them to the list for the existing data.
    if (pExistingData->m_state == AssetData::ADS_Ready)
    {
        MarkPreloadsAsFound(pRedundantData);
    }
    else
    {
        while (pRedundantData->m_preloads.size() > 0)
        {
            pExistingData->m_preloads.push_back(pRedundantData->m_preloads.back());
            pRedundantData->m_preloads.pop_back();
        }
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::RemoveUnusedAssetData(AssetDataPtr spAssetData)
{
    if (!spAssetData->InUse())
    {
        if (spAssetData->m_cacheHandle && spAssetData->m_state == AssetData::ADS_Ready)
        {
            m_spKFMCache->UnCacheKFM(spAssetData->m_cacheHandle);
            m_assetsByPhysicalID.erase(spAssetData->m_physicalID);

            spAssetData->m_state = AssetData::ADS_Unloaded;
        }
        // else we should be still loading and we will remove once loaded
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::MarkPreloadsAsFound(AssetData* pAssetData)
{
    NiActorManager* pActorManager = m_spKFMCache->GetActorManager(pAssetData->m_cacheHandle);

    while (pAssetData->m_preloads.size() > 0)
    {
        EntityPreloadData* pPreloadData = pAssetData->m_preloads.back();

        if (!pActorManager)
        {
            // The asset load failed, so send responses
            if (pPreloadData->m_responseCategory.IsValid())
            {
                EntityPreloadResponse* pPreloadResponse = EE_NEW EntityPreloadResponse;
                pPreloadResponse->m_pEntity = pPreloadData->m_pEntityData->m_pEntity;
                m_pMessageService->SendLocal(pPreloadResponse, pPreloadData->m_responseCategory);
            }
        }
        else
        {
            // Request preload of the NIF. The SceneGraphService will send the response.
            Entity* pEntity = pPreloadData->m_pEntityData->m_pEntity;
            const utf8string& nifPath =
                (const char*)pActorManager->GetKFMTool()->GetFullModelPath();
            m_pSceneGraphService->Preload(pEntity, nifPath, pPreloadData->m_responseCategory);
        }

        EE_DELETE pPreloadData;

        pAssetData->m_preloads.pop_back();
    }
}

//------------------------------------------------------------------------------------------------
void AnimationService::CacheSequenceNames(AssetData* pAssetData)
{
    EE_ASSERT(pAssetData);
    EE_ASSERT(pAssetData->m_cacheHandle != KFMCache::k_invalidHandle);

    pAssetData->m_sequenceIDs.clear();
    pAssetData->m_sequenceNames.clear();

    NiActorManager* pActorMgr = m_spKFMCache->GetActorManager(pAssetData->m_cacheHandle);
    if (!pActorMgr)
        return;

    // Extract all of the sequence IDs and sequence names in order to build a fast look up map.
    efd::UInt32 numSequences;
    efd::UInt32* pSequences;

    pActorMgr->GetKFMTool()->GetSequenceIDs(pSequences, numSequences);
    for (efd::UInt32 ui = 0; ui < numSequences; ui++)
    {
        efd::UInt32 uiSequenceID = pSequences[ui];

        NiSequenceData* pSeqData = pActorMgr->GetSequenceData(uiSequenceID);
        if (!pSeqData)
        {
            // It's possible that there was a KF load failure.
            pAssetData->m_sequenceNames[uiSequenceID] = utf8string::NullString();
            continue;
        }

        EE_ASSERT(pSeqData->GetName());
        efd::utf8string seqName = static_cast<const efd::Char*>(pSeqData->GetName());
        EE_ASSERT(!seqName.empty());

        pAssetData->m_sequenceIDs[seqName] = uiSequenceID;
        pAssetData->m_sequenceNames[uiSequenceID] = seqName;
    }

    EE_FREE(pSequences);
}

//------------------------------------------------------------------------------------------------
void AnimationService::OnAssetLoadFailure(AssetDataPtr spAssetData)
{
    // Mark all preloads as done, because we will never get their asset
    MarkPreloadsAsFound(spAssetData);

    // Uncache it now so that it is removed when the last dependent is removed.
    spAssetData->m_isCached = false;

    // Clear the scene graph data from all entities, attachments and handles
    while (spAssetData->m_entities.size() > 0)
    {
        RemoveEntityFromAssetData(spAssetData->m_entities.back());
    }

    EE_ASSERT(!spAssetData->InUse());
    EE_ASSERT(!FindAssetData(spAssetData->m_physicalID));
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::ListenForTextKeyEvents(
    const egf::EntityID& entityID,
    const char* behaviorName,
    const char* textKey,
    const char* seqName)
{
    ActorEntry* pEntry;
    if (!ContainsEntity(entityID, pEntry))
        return false;

    if (!pEntry->m_spActorManager)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Unable to register for text key '%s', behavior '%s', sequence '%s'. "
            "Actor manager does not exist. "
            "Request Preload and wait on the OnAssetsLoaded behavior to "
            "ensure the existance of the actor manager.",
            textKey,
            behaviorName,
            seqName));
        return false;
    }

    NiActorManager::SequenceID seqID;
    if (seqName)
        seqID = pEntry->m_spActorManager->FindSequenceID(seqName);
    else
        seqID = NiActorManager::ANY_SEQUENCE_ID;

    if (seqID == NiActorManager::INVALID_SEQUENCE_ID)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Unable to register for text key '%s', behavior '%s' in non-existent sequence '%s'",
            textKey,
            behaviorName,
            seqName));
        return false;
    }

    NiTextKeyMatch* pMatch;
    if (textKey)
        pMatch = EE_NEW TextKeyBehavior(textKey, behaviorName);
    else
        pMatch = EE_NEW TextKeyMatchAll(behaviorName);

    return pEntry->m_spActorManager->RegisterCallback(
        NiActorManager::TEXT_KEY_EVENT,
        seqID,
        pMatch);
}

//------------------------------------------------------------------------------------------------
bool AnimationService::CheckCompletedCacheRequest(KFMCacheRequestData* pRequestData)
{
    if (pRequestData && pRequestData->m_numOutstandingNIFs == 0)
    {
        m_pMessageService->SendLocal(
            pRequestData->m_pResponse,
            pRequestData->m_pResponse->GetResponseCategory());
        pRequestData->m_pResponse = 0;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
AnimationService::AssetData::AssetData()
    : m_state(ADS_Loading)
    , m_cacheHandle(KFMCache::k_invalidHandle)
    , m_spEntityLoadData(0)
    , m_spCacheLoadData(0)
    , m_nifCacheHandle(SceneGraphCache::k_invalidHandle)
    , m_isCached(false)
{
}

//------------------------------------------------------------------------------------------------
AnimationService::AssetData::~AssetData()
{
    // Clear the smart pointers
    m_cacheHandle = 0;
    m_nifCacheHandle = 0;
    m_spEntityLoadData = 0;
    m_spCacheLoadData = 0;
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::AssetData::InUse() const
{
    return (m_isCached || !m_entities.empty() || !m_preloads.empty());
}

//------------------------------------------------------------------------------------------------
efd::Bool AnimationService::AssetData::RemoveEntityFromPreload(Entity* pEntity)
{
    efd::vector<EntityPreloadData*>::iterator iter = m_preloads.begin();
    while (iter != m_preloads.end())
    {
        if ((*iter)->m_pEntityData->m_pEntity == pEntity)
        {
            EE_DELETE(*iter);
            m_preloads.erase(iter);
            return true;
        }

        ++iter;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
AnimationService::ActorEntry::~ActorEntry()
{
    m_spActorManager = 0;
    m_spAssetData = 0;
}

//------------------------------------------------------------------------------------------------
AnimationService::KFMCacheRequestData::KFMCacheRequestData()
    : m_pAssetData(0)
    , m_pResponse(0)
    , m_numOutstandingNIFs(0)
{
}

//------------------------------------------------------------------------------------------------
AnimationService::KFMCacheRequestData::~KFMCacheRequestData()
{
    if (m_pResponse)
    {
        EE_DELETE m_pResponse;
    }
}

//------------------------------------------------------------------------------------------------
