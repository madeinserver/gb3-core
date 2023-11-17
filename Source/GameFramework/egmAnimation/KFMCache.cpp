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

#include "egmAnimationPCH.h"

#include <egmAnimation/KFMCache.h>

#include <egmAnimation/KFMCacheResponse.h>
#include <efd/ecrLogIDs.h>
#include <efd/AssetLoadRequest.h>
#include <egmAnimation/KFMFactory.h>
#include <egmAnimation/KFMFactoryResponse.h>

using namespace efd;
using namespace ecr;
using namespace egmAnimation;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(KFMCache);

// Register for asset locator response messages:
EE_HANDLER_WRAP(KFMCache, HandleAssetLocatorResponse,
    AssetLocatorResponse, kMSGID_AssetLocatorResponse);

// Register for asset load responses. We register for two types in order to catch error
// conditions.
EE_HANDLER(KFMCache, HandleAssetLoadResponse, AssetLoadResponse);
EE_HANDLER_SUBCLASS(
    KFMCache,
    HandleAssetLoadResponse,
    AssetLoadResponse,
    KFMFactoryResponse);

const KFMCache::KFMCacheHandle KFMCache::k_invalidHandle = 0;

//------------------------------------------------------------------------------------------------
KFMCache::KFMCache()
    : m_pMessageService(NULL)
    , m_pAssetLocatorService(NULL)
    , m_pAssetFactoryManager(NULL)
    , m_kAnimationTag("gamebryo-animation")
    , m_kAnimationKFTag("gamebryo-sequence-file")
    , m_subscribed(false)
{
}

//------------------------------------------------------------------------------------------------
KFMCache::~KFMCache()
{
}

//------------------------------------------------------------------------------------------------
void KFMCache::Init(
    efd::MessageService* pMessageService,
    efd::AssetLocatorService* pAssetLocator,
    efd::AssetFactoryManager* pAssetFactory)
{
    m_pMessageService = pMessageService;
    m_pAssetLocatorService = pAssetLocator;
    m_pAssetFactoryManager = pAssetFactory;

    m_assetLoadCategory = m_pMessageService->GetGloballyUniqueCategory();
    m_assetReloadCategory = m_pMessageService->GetGloballyUniqueCategory();
    m_assetLocateCategory = GenerateAssetResponseCategory();
    m_assetRefreshCategory = GenerateAssetResponseCategory();
    m_assetReloadResponseCategory = m_pMessageService->GetGloballyUniqueCategory();

    KFMFactory* pFactory = EE_NEW KFMFactory(m_pAssetFactoryManager, m_pMessageService);
    m_pAssetFactoryManager->RegisterAssetFactory(m_assetLoadCategory, pFactory);
    m_pAssetFactoryManager->RegisterAssetFactory(m_assetReloadCategory, pFactory);
}

//------------------------------------------------------------------------------------------------
void KFMCache::Shutdown()
{
    m_pAssetFactoryManager->UnregisterAssetFactory(m_assetLoadCategory);
    m_pAssetFactoryManager->UnregisterAssetFactory(m_assetReloadCategory);

    // Unregister for messages
    if (m_subscribed)
    {
        m_pMessageService->Unsubscribe(this, m_assetLoadCategory);
        m_pMessageService->Unsubscribe(this, m_assetReloadCategory);
        m_pMessageService->Unsubscribe(this, m_assetLocateCategory);
        m_pMessageService->Unsubscribe(this, m_assetRefreshCategory);
        m_subscribed = false;
    }

    // Empty the cache
    efd::map<utf8string, KFMCacheDataPtr>::iterator iter = m_cache.begin();
    while (iter != m_cache.end())
    {
        iter->second = 0;
        ++iter;
    }
    m_cache.clear();

    // Clear any outstanding requests
    efd::map<utf8string, PendingRequestDataPtr>::iterator pendingIter =  m_pendingRequests.begin();
    while (pendingIter != m_pendingRequests.end())
    {
        pendingIter->second = 0;
        ++pendingIter;
    }
    m_pendingRequests.clear();

    efd::map<utf8string, FilePathVector*>::iterator identIter = m_identifierCache.begin();
    while (identIter != m_identifierCache.end())
    {
        EE_DELETE identIter->second;
        ++identIter;
    }
    m_identifierCache.clear();
}

//------------------------------------------------------------------------------------------------
bool KFMCache::CacheKFM(
    const efd::utf8string& identifier,
    const efd::Category responseCategory,
    IKFMCacheRequestData* pResponseData,
    vector<KFMCacheHandle>& handles)
{
    FilePathVector* filePaths = 0;
    KFMCacheDataPtr spData = 0;

    filePaths = FindFilePaths(identifier);

    if (filePaths)
    {
        bool foundAll = true;
        for (UInt32 ui = 0; ui < filePaths->size(); ++ui)
        {
            if (!m_cache.find((*filePaths)[ui], spData))
            {
                foundAll = false;
                handles.clear();

                /// We have seen this identifier before, but some of its files have been uncached.
                /// For simplicity, pretend we have never seen it before.
                break;
            }
            handles.push_back(spData);
        }
        if (foundAll)
        {
            return true;
        }
    }

    if (identifier.find("urn:") == utf8string::npos)
    {
        // See if we already have it cached
        efd::map<utf8string, KFMCacheDataPtr>::iterator cacheIter = m_cache.find(identifier);
        if (cacheIter != m_cache.end())
        {
            handles.clear();
            handles.push_back(cacheIter->second);
            return true;
        }

        RequestFilePath(identifier, identifier, responseCategory, pResponseData);

        return false;
    }

    RequestURN(identifier, responseCategory, pResponseData);

    return false;
}

//------------------------------------------------------------------------------------------------
void KFMCache::RequestFilePath(
    const efd::utf8string& identifier,
    const efd::utf8string& fileName,
    const efd::Category responseCategory,
    IKFMCacheRequestData* pResponseData)
{
    // Create a request data object
    RequestData* pRequestData = EE_NEW RequestData;
    pRequestData->m_identifier = identifier;
    pRequestData->m_responseCategory = responseCategory;
    pRequestData->m_pResponseData = pResponseData;

    // Look for an existing pending request, and create it if there is not one
    PendingRequestDataPtr spPendingData = 0;
    if (!m_pendingRequests.find(identifier, spPendingData) || !spPendingData)
    {
        spPendingData = EE_NEW PendingRequestData;
        spPendingData->m_status = AS_LoadingKFM;
        spPendingData->m_identifier = identifier;
        m_pendingRequests[identifier] = spPendingData;
        spPendingData->m_filePaths.push_back(fileName);
        spPendingData->m_requestList.push_back(pRequestData);
    }
    else
    {
        EE_ASSERT(spPendingData->m_filePaths.size() == 1);

        // Add this request to the set for this identifier
        if (spPendingData->m_filePaths[0] == fileName)
        {
            // The name hasn't changed while processing
            spPendingData->m_requestList.push_back(pRequestData);
            return;
        }
        else
        {
            // The name for this identifier has changed while still loading.
            // Not sure how this can happen, but if it does we cancel the existing load
            // for the identifier and create a new one.

            // First, get the file data. We should have it but handle it if we don't.
            KFMCacheDataPtr spPendingFile = 0;
            if (!m_pendingFiles.find(spPendingData->m_filePaths[0], spPendingFile) ||
                !spPendingFile)
            {
                // Change the stored file name
                spPendingData->m_filePaths.clear();
                spPendingData->m_filePaths.push_back(fileName);
                spPendingData->m_requestList.push_back(pRequestData);

                // Fall through to make the missing file pending data
            }
            else
            {
                // Find the request in the file data and remove it. This prevents us
                // from sending messages for the over-written file
                for (efd::list<PendingRequestDataPtr>::iterator requestIter =
                    spPendingFile->m_requestList.begin();
                    requestIter != spPendingFile->m_requestList.end();
                    requestIter++)
                {
                    if (spPendingData == *requestIter)
                    {
                        spPendingFile->m_requestList.erase(requestIter);
                        break;
                    }
                }

                // Send failed messages for all the existing requests for this identifier
                for (efd::list<RequestDataPtr>::iterator requestIter =
                    spPendingData->m_requestList.begin();
                    requestIter != spPendingData->m_requestList.end();
                    ++requestIter)
                {
                    KFMCacheResponse* pCacheResp = EE_NEW KFMCacheResponse(
                        (*requestIter)->m_identifier,
                        (*requestIter)->m_responseCategory,
                        (*requestIter)->m_pResponseData,
                        AssetCacheResponse::ACR_FileNotFound,
                        AssetCacheResponse::ACT_CacheAdd);

                    m_pMessageService->SendLocal(pCacheResp, (*requestIter)->m_responseCategory);

                    // release the RequestDataPtr
                    *requestIter = 0;
                }

                // Clear the old request
                m_pendingRequests[identifier] = 0;
                m_pendingRequests.erase(identifier);

                // Create a new one
                spPendingData = EE_NEW PendingRequestData;
                spPendingData->m_status = AS_LoadingKFM;
                spPendingData->m_identifier = identifier;
                m_pendingRequests[identifier] = spPendingData;
                spPendingData->m_filePaths.push_back(fileName);
                spPendingData->m_requestList.push_back(pRequestData);
            }
        }
    }

    // Look for an existing request for the file name, and create one if it does not exist
    KFMCacheDataPtr spPendingFile = 0;
    if (!m_pendingFiles.find(fileName, spPendingFile))
    {
        spPendingFile = EE_NEW KFMCacheData;
        spPendingFile->m_llid.clear();
        spPendingFile->m_physicalID = identifier;
        spPendingFile->m_filePath = fileName;
        spPendingFile->m_status = AS_LoadingKFM;
        m_pendingFiles[fileName] = spPendingFile;

        if (!m_subscribed)
            Subscribe();

        AssetLoadRequest* pLoadRequest =
            EE_NEW AssetLoadRequest(identifier, m_assetLoadCategory, fileName);
        m_pMessageService->SendLocal(pLoadRequest, AssetFactoryManager::MessageCategory());
    }

    // Add this request to the list for the file.
    spPendingFile->m_requestList.push_back(spPendingData);
}

//------------------------------------------------------------------------------------------------
void KFMCache::RequestURN(
    const efd::utf8string& identifier,
    const efd::Category responseCategory,
    IKFMCacheRequestData* pResponseData)
{
    // Create a request data object
    RequestData* pRequestData = EE_NEW RequestData;
    pRequestData->m_identifier = identifier;
    pRequestData->m_responseCategory = responseCategory;
    pRequestData->m_pResponseData = pResponseData;

    // Look for an existing pending request, and create it if there is not one
    PendingRequestDataPtr spPendingData = 0;
    if (!m_pendingRequests.find(identifier, spPendingData))
    {
        spPendingData = EE_NEW PendingRequestData;
        spPendingData->m_status = AS_Locating;
        spPendingData->m_identifier = identifier;
        m_pendingRequests[identifier] = spPendingData;

        if (!m_subscribed)
            Subscribe();

        // Locate the file names for this request
        m_pAssetLocatorService->AssetLocate(identifier, m_assetLocateCategory);
    }

    // Add this request to the set for this identifier
    spPendingData->m_requestList.push_back(pRequestData);
}

//------------------------------------------------------------------------------------------------
bool KFMCache::UnCacheKFM(KFMCacheHandle handle)
{
    /// Check it's in the cache
    efd::map<utf8string, KFMCacheDataPtr>::iterator cacheIter =
        m_cache.find(handle->m_filePath);
    if (cacheIter == m_cache.end())
        return false;

    // Check for ref count of three. One for the caller of this function, one for the
    // handle argument, and one for the cache.
    if (handle->GetRefCount() == 3)
    {
        m_cache.erase(cacheIter);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool KFMCache::ReloadKFM(
    KFMCacheHandle handle,
    efd::Category responseCategory,
    IKFMCacheRequestData* pResponseData)
{
    /// Check it's in the cache
    efd::map<utf8string, KFMCacheDataPtr>::iterator cacheIter =
        m_cache.find(handle->m_filePath);
    if (cacheIter == m_cache.end())
        return false;

    // Create a request data object so we remember who to respond to
    RequestData* pData = EE_NEW RequestData;
    pData->m_identifier = handle->m_filePath;
    pData->m_responseCategory = responseCategory;
    pData->m_pResponseData = pResponseData;

    PendingRequestData* pPendingData = EE_NEW PendingRequestData;
    pPendingData->m_requestList.push_back(pData);

    handle->m_requestList.push_back(pPendingData);

    // The asset is being reloaded
    handle->m_status = AS_Reloading;

    if (!m_subscribed)
        Subscribe();

    AssetLoadRequest* pLoadRequest =
        EE_NEW AssetLoadRequest(handle->m_filePath, m_assetReloadCategory, handle->m_filePath);
    m_pMessageService->SendLocal(pLoadRequest, AssetFactoryManager::MessageCategory());

    return true;
}

//------------------------------------------------------------------------------------------------
bool KFMCache::ReplaceKFM(
    KFMCacheHandle handle,
    const efd::utf8string& newFileName,
    efd::Category responseCategory,
    IKFMCacheRequestData* pResponseData)
{
    /// Check it's in the cache
    efd::map<utf8string, KFMCacheDataPtr>::iterator cacheIter =
        m_cache.find(handle->m_filePath);
    if (cacheIter == m_cache.end())
        return false;

    // Request a file name load.
    RequestFilePath(handle->m_physicalID, newFileName, responseCategory, pResponseData);

    return true;
}

//------------------------------------------------------------------------------------------------
void KFMCache::HandleAssetChange(
    const efd::utf8string& physicalID,
    const efd::utf8string& tags)
{
    // Make sure the tags on the asset contain our animation tag indicating the asset that changed
    // on disk is an animation file we need to track.
    if (tags.find(m_kAnimationTag) != efd::utf8string::npos ||
        tags.find(m_kAnimationKFTag) != efd::utf8string::npos)
    {
        // Issue a request to the asset service to locate our asset.
        m_pAssetLocatorService->AssetLocate(physicalID, m_assetRefreshCategory);
    }
}

//------------------------------------------------------------------------------------------------
void KFMCache::HandleReloadRequest(const efd::AssetLocatorResponse* pMessage)
{
    // Get the original request URI
    const AssetLocatorResponse::AssetURLMap& assets = pMessage->GetAssetURLMap();

    if (assets.size() == 0)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR1,
            ("%s: Found 0 assets when trying to reload '%s'. Keeping the old asset.",
            __FUNCTION__,
            pMessage->GetResponseURI().c_str()));
        return;
    }

    if (assets.size() > 1)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR1,
            ("%s: Found %d assets when trying to reload '%s'. "
            "Ignoring all but the first.",
            __FUNCTION__,
            assets.size(),
            pMessage->GetResponseURI().c_str()));
    }

    AssetLocatorResponse::AssetURLMap::const_iterator assetIterator = assets.begin();
    const efd::utf8string& assetID = assetIterator->first;
    const efd::utf8string& assetPath = assetIterator->second.url;
    const utf8string& tagSet = assetIterator->second.tagset;

    if (assetPath.empty())
        return;

    // We may have either a .kf sequence file to reload or a .kfm animation file to reload.
    if (tagSet.find(m_kAnimationTag) != efd::utf8string::npos)
        RefreshKFMAsset(assetID, assetPath);
    else if (tagSet.find(m_kAnimationKFTag) != efd::utf8string::npos)
        RefreshKFAsset(assetID, assetPath);
}

//------------------------------------------------------------------------------------------------
bool KFMCache::RefreshKFMAsset(
    const efd::utf8string& physicalID,
    const efd::utf8string& kfmPath)
{
    // Look through the cache for a matching physical ID or file name
    KFMCacheDataPtr spCacheData = 0;
    FilePathVector* filePaths;
    if (m_identifierCache.find(physicalID, filePaths))
    {
        if (filePaths->size() == 0)
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kLVL3,
                ("%s: Found ID %s but no file paths. "
                "Assuming we have a pending load and not reloading.",
                __FUNCTION__,
                physicalID.c_str()));
            return false;
        }
        if (filePaths->size() > 1)
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kERR1,
                ("%s: Found %d assets when trying to reload '%s'. "
                "Doing nothing, as this should never occur.",
                __FUNCTION__,
                filePaths->size(),
                physicalID.c_str()));
            return false;
        }

        EE_VERIFY(m_cache.find((*filePaths)[0], spCacheData));
    }
    else
    {
        // We need to iterate looking for a physical ID or file path match, because the
        // physical ID might now be representing a different file path.
        efd::map<efd::utf8string, KFMCacheDataPtr>::iterator cacheIter = m_cache.begin();
        while (cacheIter != m_cache.end())
        {
            spCacheData = cacheIter->second;
            if (spCacheData->m_physicalID == physicalID ||
                spCacheData->m_filePath == kfmPath)
            {
                break;
            }

            ++cacheIter;
        }

        if (cacheIter == m_cache.end())
        {
            EE_LOG(efd::kGamebryoGeneral0,
                efd::ILogger::kLVL3,
                ("%s: Found no ID %s nor path %s in the cache.",
                __FUNCTION__,
                physicalID.c_str(),
                kfmPath.c_str()));
            return false;
        }
    }

    EE_ASSERT(spCacheData);

    if (kfmPath == spCacheData->m_filePath)
    {
        // Existing file, so treat it as a reload.
        // Create a request data object so we remember who to respond to
        RequestData* pData = EE_NEW RequestData;
        pData->m_identifier = spCacheData->m_filePath;
        pData->m_responseCategory = m_assetReloadResponseCategory;
        pData->m_pResponseData = NULL;

        PendingRequestData* pPendingData = EE_NEW PendingRequestData;
        pPendingData->m_requestList.push_back(pData);

        spCacheData->m_requestList.push_back(pPendingData);

        // The asset is being reloaded
        spCacheData->m_status = AS_Reloading;

        if (!m_subscribed)
            Subscribe();

        AssetLoadRequest* pLoadRequest = EE_NEW KFMFactoryRequest(
            spCacheData->m_filePath,
            m_assetReloadCategory,
            spCacheData->m_filePath);
        m_pMessageService->SendLocal(pLoadRequest, AssetFactoryManager::MessageCategory());
    }
    else
    {
        // New file path, so start again from scratch
        RequestFilePath(
            spCacheData->m_physicalID,
            kfmPath,
            m_assetReloadResponseCategory,
            NULL);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool KFMCache::RefreshKFAsset(
    const efd::utf8string& physicalID,
    const efd::utf8string& kfPath)
{
    EE_UNUSED_ARG(physicalID);

    // Our cache maps .kfm based assets to entities/actor managers. Since we are dealing with
    // .kf based assets we have to iterate through our cache one by one.
    bool updatedAsset = false;
    for (efd::map<efd::utf8string, KFMCacheDataPtr>::iterator cacheIter = m_cache.begin();
        cacheIter != m_cache.end();
        ++cacheIter)
    {
        KFMCacheData* pCacheData = cacheIter->second;
        NiActorManager* pActorManager = pCacheData->m_spActorManager;

        if (!pActorManager)
            continue;

        UInt32* pSequenceIDs = 0;
        UInt32 idCount = 0;
        pActorManager->GetKFMTool()->GetSequenceIDs(pSequenceIDs, idCount);

        bool foundEntry = false;
        for (UInt32 ui = 0; ui < idCount; ++ui)
        {
            efd::utf8string kfPathname = static_cast<const efd::Char*>(
                pActorManager->GetKFMTool()->GetFullKFFilename(pSequenceIDs[ui]));
            efd::utf8string kfFilename =
                efd::PathUtils::IsAbsolutePath(kfPathname) ?
                kfPathname : efd::PathUtils::ConvertToAbsolute(kfPathname);
            kfFilename = efd::PathUtils::PathMakeNative(kfFilename);

            if (!kfFilename.icompare(efd::PathUtils::PathMakeNative(kfPath)))
            {
                foundEntry = true;
                break;
            }
        }
        EE_FREE(pSequenceIDs);

        if (!foundEntry)
            continue;

        // We found a .kf file that matches the .kf file that changed.
        if (!pActorManager->GetControllerManager())
            continue;

        pActorManager->ReloadKFFile(kfPath.c_str());

        KFMCacheResponse* pCacheResp = EE_NEW KFMCacheResponse(
            pCacheData->m_physicalID,
            m_assetReloadResponseCategory,
            NULL,
            AssetCacheResponse::ACR_Success,
            AssetCacheResponse::ACT_CacheReload);
        pCacheResp->AddHandle(pCacheData);

        m_pMessageService->SendLocal(pCacheResp, m_assetReloadResponseCategory);

        updatedAsset = true;
    }

    return updatedAsset;
}

//------------------------------------------------------------------------------------------------
void KFMCache::HandleAssetLocatorResponse(
    const efd::AssetLocatorResponse* pMessage,
    efd::Category targetChannel)
{
    // Check for refrsh request
    if (targetChannel == m_assetRefreshCategory)
    {
        HandleReloadRequest(pMessage);
        return;
    }

    const utf8string& identifier = pMessage->GetResponseURI();
    const AssetLocatorResponse::AssetURLMap& assets = pMessage->GetAssetURLMap();

    // Find our request data
    PendingRequestDataPtr spPendingData = 0;
    if (!m_pendingRequests.find(identifier, spPendingData))
    {
        // The user must have cancelled the request
        return;
    }
    EE_ASSERT(spPendingData);
    EE_ASSERT(spPendingData->m_status == AS_Locating);

    // No found assets requires a response message for all requests
    if (assets.size() == 0)
    {
        for (efd::list<RequestDataPtr>::iterator requestIter =
            spPendingData->m_requestList.begin();
            requestIter != spPendingData->m_requestList.end();
            ++requestIter)
        {
            KFMCacheResponse* pCacheResp = EE_NEW KFMCacheResponse(
                (*requestIter)->m_identifier,
                (*requestIter)->m_responseCategory,
                (*requestIter)->m_pResponseData,
                AssetCacheResponse::ACR_AssetNotFound,
                AssetCacheResponse::ACT_CacheAdd);

            m_pMessageService->SendLocal(pCacheResp, (*requestIter)->m_responseCategory);

            // release the RequestDataPtr
            *requestIter = 0;
        }

        m_pendingRequests[identifier] = 0;
        m_pendingRequests.erase(identifier);

        return;
    }

    // Work through the file paths found
    AssetLocatorResponse::AssetURLMap::const_iterator assetIter;
    for (assetIter = assets.begin(); assetIter != assets.end(); assetIter++)
    {
        // Add the file to the set we need for this request
        spPendingData->m_filePaths.push_back(assetIter->second.url);

        KFMCacheDataPtr spFileData = 0;
        if (m_cache.find(assetIter->second.url, spFileData))
        {
            // This file is already cached, so we can increment the set of found files for
            // the requests.
            ++(spPendingData->m_cachedCount);
        }
        else
        {
            if (!m_pendingFiles.find(assetIter->second.url, spFileData))
            {
                // We're here if we know nothing of this file. So we need to request it.
                spFileData = EE_NEW KFMCacheData;
                spFileData->m_status = AS_LoadingKFM;
                spFileData->m_llid = assetIter->second.llid;
                spFileData->m_physicalID = assetIter->first;
                spFileData->m_filePath = assetIter->second.url;
                m_pendingFiles[spFileData->m_filePath] = spFileData;

                AssetLoadRequest* pLoadRequest = EE_NEW KFMFactoryRequest(
                    assetIter->second.url,
                    m_assetLoadCategory,
                    assetIter->second.url);
                m_pMessageService->SendLocal(pLoadRequest, AssetFactoryManager::MessageCategory());
            }
            spFileData->m_requestList.push_back(spPendingData);
        }
    }

    /// Check in case we have found all the files in the cache
    if (CheckCompleteRequest(spPendingData))
        return;

    /// Not done, so must be loading.
    spPendingData->m_status = AS_LoadingKFM;
}

//------------------------------------------------------------------------------------------------
void KFMCache::HandleAssetLoadResponse(
    const efd::AssetLoadResponse* pResponse,
    efd::Category targetChannel)
{
    // Find the request
    KFMCacheDataPtr spData = 0;
    if (targetChannel == m_assetLoadCategory)
    {
        if (!m_pendingFiles.find(pResponse->GetAssetPath(), spData))
        {
            // The request may have been cancelled. Just exit.
            return;
        }
    }
    else if (targetChannel == m_assetReloadCategory)
    {
        if (!m_cache.find(pResponse->GetAssetPath(), spData))
        {
            // The asset may have been removed from the cache. Just exit.
            return;
        }
    }
    else
    {
        // We hit this assert if a message comes to this handler on a category we don't
        // know about. For that to occur, we would have to somehow have been subscribed for
        // a different category.
        EE_FAIL("KFMCache::HandleAssetLoadResponse: Load response on an unknown category");
        return;
    }

    // Now we should have valid data at this point
    EE_ASSERT(spData);
    EE_ASSERT(spData->m_filePath == pResponse->GetAssetPath());

    AssetLoadResponse::AssetLoadResult loadResult = pResponse->GetResult();
    AssetCacheResponse::AssetCacheResult cacheResult = AssetCacheResponse::ACR_Success;
    if (loadResult != AssetLoadResponse::ALR_Success &&
        loadResult != AssetLoadResponse::ALR_PartialSuccess)
    {
        // Some kind of error that prevents us from caching the asset. An error was logged by the
        // KFM Factory, so we just state what we will do about it.
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Failed to load KFM asset %s. Cached copy will have no objects.",
            spData->m_filePath.c_str()));
        cacheResult = loadResult == AssetLoadResponse::ALR_FileAccessDenied ?
            AssetCacheResponse::ACR_FileAccessDenied :
            AssetCacheResponse::ACR_ParseError;
    }

    const KFMFactoryResponse* pKFMResponse = EE_DYNAMIC_CAST(KFMFactoryResponse, pResponse);
    EE_ASSERT(pKFMResponse);

    if (targetChannel == m_assetLoadCategory)
    {
        EE_ASSERT(spData->m_spActorManager == 0);
        spData->m_spActorManager = pKFMResponse->GetActor();

        m_cache[spData->m_filePath] = spData;
        m_pendingFiles.erase(spData->m_filePath);
        spData->m_status = AS_Ready;

        // Add the asset to the identifier cache if it has an llid
        if (!spData->m_llid.empty())
        {
            FilePathVector filePaths;
            filePaths.push_back(spData->m_filePath);
            AddFilePaths(spData->m_llid, filePaths);
        }

        // Go through all the requests and increment the number of cached files
        for (efd::list<PendingRequestDataPtr>::iterator requestIter =
            spData->m_requestList.begin();
            requestIter != spData->m_requestList.end();
            requestIter++)
        {
            PendingRequestDataPtr spPendingData = *requestIter;
            EE_ASSERT(spPendingData);
            ++(spPendingData->m_cachedCount);
            CheckCompleteRequest(spPendingData);
            *requestIter = 0;
        }
        spData->m_requestList.clear();
    }
    else if (targetChannel == m_assetReloadCategory)
    {
        spData->m_spActorManager = pKFMResponse->GetActor();

        // The reload is done
        spData->m_status = AS_Ready;

        // Send all the responses
        for (efd::list<PendingRequestDataPtr>::iterator pendingIter =
            spData->m_requestList.begin();
            pendingIter != spData->m_requestList.end();
            pendingIter++)
        {
            PendingRequestDataPtr spPendingData = *pendingIter;
            EE_ASSERT(spPendingData);

            for (efd::list<RequestDataPtr>::iterator requestIter =
                spPendingData->m_requestList.begin();
                requestIter != spPendingData->m_requestList.end();
                ++requestIter)
            {
               RequestData* pData = *requestIter;

               KFMCacheResponse* pCacheResp = EE_NEW KFMCacheResponse(
                    pData->m_identifier,
                    pData->m_responseCategory,
                    pData->m_pResponseData,
                    cacheResult,
                    AssetCacheResponse::ACT_CacheReload);
                pCacheResp->AddHandle(spData);

                m_pMessageService->SendLocal(pCacheResp, pData->m_responseCategory);
            }
        }
        spData->m_requestList.clear();
    }
}

//------------------------------------------------------------------------------------------------
void KFMCache::Subscribe()
{
    // You hit this assert if you did not call Init.
    EE_ASSERT(m_pMessageService);

    m_pMessageService->Subscribe(this, m_assetLoadCategory);
    m_pMessageService->Subscribe(this, m_assetReloadCategory);
    m_pMessageService->Subscribe(this, m_assetLocateCategory);
    m_pMessageService->Subscribe(this, m_assetRefreshCategory);

    m_subscribed = true;
}

//------------------------------------------------------------------------------------------------
KFMCache::FilePathVector* KFMCache::FindFilePaths(const efd::utf8string& identifier)
{
    map<utf8string, FilePathVector*>::iterator iter = m_identifierCache.find(identifier);
    if (iter == m_identifierCache.end())
        return 0;

    return iter->second;
}

//------------------------------------------------------------------------------------------------
void KFMCache::AddFilePaths(const efd::utf8string& identifier, const FilePathVector& paths)
{
    map<utf8string, FilePathVector*>::iterator iter = m_identifierCache.find(identifier);
    if (iter != m_identifierCache.end())
    {
        if (paths == *iter->second)
        {
            // Existing entry matches new entry, avoid allocations by keeping existing entry.
            return;
        }
        // Existing entry is different, update by copying new vector into old vector. This should
        // minimize the allocations required to perform the update.
        *iter->second = paths;
    }
    else
    {
        FilePathVector* pPaths = EE_EXTERNAL_NEW FilePathVector(paths);
        m_identifierCache[identifier] = pPaths;
    }
}

//------------------------------------------------------------------------------------------------
void KFMCache::RemoveFilePaths(const efd::utf8string& identifier)
{
    map<utf8string, FilePathVector*>::iterator iter = m_identifierCache.find(identifier);
    if (iter != m_identifierCache.end())
    {
        EE_EXTERNAL_DELETE iter->second;
        m_identifierCache.erase(iter);
    }
}

//------------------------------------------------------------------------------------------------
bool KFMCache::CheckCompleteRequest(PendingRequestData* pPendingRequest)
{
    bool isComplete = (pPendingRequest->m_cachedCount == pPendingRequest->m_filePaths.size());

    if (!isComplete)
        return false;

    // Remove all traces from the pending list, we're done
    AddFilePaths(pPendingRequest->m_identifier, pPendingRequest->m_filePaths);

    // Send all the responses
    for (efd::list<RequestDataPtr>::iterator requestIter = pPendingRequest->m_requestList.begin();
        requestIter != pPendingRequest->m_requestList.end();
        ++requestIter)
    {
       RequestData* pData = *requestIter;

       KFMCacheResponse* pCacheResp = EE_NEW KFMCacheResponse(
            pData->m_identifier,
            pData->m_responseCategory,
            pData->m_pResponseData,
            AssetCacheResponse::ACR_Success,
            AssetCacheResponse::ACT_CacheAdd);

        for (UInt32 ui = 0; ui < pPendingRequest->m_filePaths.size(); ++ui)
        {
            // Find the cached file data. It must be present.
            KFMCacheDataPtr spCacheData = 0;
            EE_VERIFY(m_cache.find(pPendingRequest->m_filePaths[ui], spCacheData));

            // Add the file to the list of handles to return to the requestor
            pCacheResp->AddHandle(spCacheData);
        }

        m_pMessageService->SendLocal(pCacheResp, pData->m_responseCategory);
    }

    m_pendingRequests[pPendingRequest->m_identifier] = 0;
    m_pendingRequests.erase(pPendingRequest->m_identifier);

    return true;
}

//------------------------------------------------------------------------------------------------
KFMCache::KFMCacheData::KFMCacheData()
    : m_status(AS_Invalid)
    , m_spActorManager(0)
{
}

//------------------------------------------------------------------------------------------------
KFMCache::KFMCacheData::~KFMCacheData()
{
    efd::list<PendingRequestDataPtr>::iterator requestIter = m_requestList.begin();
    while (requestIter != m_requestList.end())
    {
        *requestIter = 0;
        ++requestIter;
    }

    m_spActorManager = 0;
}

//------------------------------------------------------------------------------------------------
KFMCache::PendingRequestData::PendingRequestData()
    : m_status(AS_Invalid)
    , m_cachedCount(0)
{
}

//------------------------------------------------------------------------------------------------
KFMCache::PendingRequestData::~PendingRequestData()
{
    efd::list<RequestDataPtr>::iterator requestIter = m_requestList.begin();
    while (requestIter != m_requestList.end())
    {
        *requestIter = 0;
        ++requestIter;
    }
}

//------------------------------------------------------------------------------------------------
KFMCache::RequestData::RequestData()
    : m_responseCategory(efd::kCAT_INVALID)
    , m_pResponseData(0)
{
}

//------------------------------------------------------------------------------------------------
KFMCache::RequestData::~RequestData()
{
}
