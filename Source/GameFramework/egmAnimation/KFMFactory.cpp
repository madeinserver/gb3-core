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

#include "KFMFactory.h"
#include "KFMFactoryResponse.h"
#include <efd/ecrLogIDs.h>
#include <efd/SmartCriticalSection.h>
#include <efd/GenericAssetLoadResponse.h>
#include <NiMemStream.h>

using namespace efd;
using namespace egmAnimation;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(KFMFactory);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(KFMFactory::ResponseData);

EE_HANDLER_SUBCLASS(
    AssetFactoryManager,
    AssetLoadRequestHandler,
    AssetLoadRequest,
    KFMFactoryRequest);

//------------------------------------------------------------------------------------------------
KFMFactory::KFMFactory(
    AssetFactoryManager* pFactoryManager,
    MessageService* pMessageService)
    : m_spMessageService(pMessageService)
    , m_spAssetFactoryManager(pFactoryManager)
{
    EE_ASSERT(m_spMessageService);
    EE_ASSERT(m_spAssetFactoryManager);

    // These factories are not registered with the asset factory manager.  LoadAsset is called
    // on them directly as background<->background messaging is not supported.
    m_spGenericAssetFactory = EE_NEW GenericAssetFactory();
}

//------------------------------------------------------------------------------------------------
KFMFactory::~KFMFactory()
{
}

//------------------------------------------------------------------------------------------------
void KFMFactory::Shutdown()
{
    m_spMessageService = NULL;
    m_spAssetFactoryManager = NULL;
    m_spGenericAssetFactory = NULL;
}

//------------------------------------------------------------------------------------------------
IAssetFactory::LoadStatus KFMFactory::LoadAsset(
    AssetFactoryManager* pFactoryManager,
    const AssetLoadRequest* pRequest,
    AssetLoadResponsePtr& pResponse)
{
    EE_UNUSED_ARG(pFactoryManager);

    ResponseDataPtr spResponseData;
    GetResponseDataAs<KFMFactory::ResponseData>(pRequest, spResponseData);

    if (!spResponseData)
    {
        // A new request has come in. Process it.
        m_pendingRequestLock.Lock();
        spResponseData = EE_NEW ResponseData();
        m_pendingRequestMap[pRequest] = spResponseData;
        m_pendingRequestLock.Unlock();

        spResponseData->m_spResponse = EE_NEW KFMFactoryResponse(
            pRequest->GetURN(),
            pRequest->GetResponseCategory(),
            AssetLoadResponse::ALR_Success,
            pRequest->GetAssetPath(),
            pRequest->GetIsReload());
    }

    EE_ASSERT(spResponseData);

    const KFMFactoryRequest* pKFMRequest = EE_DYNAMIC_CAST(KFMFactoryRequest, pRequest);

    IAssetFactory::LoadStatus status;
    if (pKFMRequest)
    {
        status = LoadKFMAsset(pKFMRequest, spResponseData);
    }
    else
    {
        // Incorrect request type.
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("KFMFactory: Invalid request type."));
        spResponseData->m_spResponse->SetResult(AssetLoadResponse::ALR_UnknownError);
        status = LOAD_COMPLETE;
    }

    if (status == LOAD_COMPLETE)
    {
        if (spResponseData->m_spResponse->GetResult() != AssetLoadResponse::ALR_Success &&
            spResponseData->m_spResponse->GetResult() != AssetLoadResponse::ALR_PartialSuccess)
        {
            // Delete actor on failure.
            spResponseData->m_spResponse->SetActor(NULL);
        }

        pResponse = spResponseData->m_spResponse;
        RemoveResponseData(pRequest);
    }

    return status;
}

//------------------------------------------------------------------------------------------------
IAssetFactory::LoadStatus KFMFactory::LoadKFMAsset(
    const KFMFactoryRequest* pRequest,
    KFMFactory::ResponseData* pResponseData)
{
    EE_ASSERT(pRequest);
    EE_ASSERT(pResponseData);

    // Check on the KFM load first.  If the actor manager doesn't exist, the KFM load is pending.
    if (!pResponseData->m_spResponse->GetActor())
    {
        // KFM is still loading.  Poll completion.
        AssetLoadResponsePtr pGenericResponse;
        IAssetFactory::LoadStatus status =
            m_spGenericAssetFactory->LoadAsset(m_spAssetFactoryManager, pRequest, pGenericResponse);
        if (status != IAssetFactory::LOAD_COMPLETE)
        {
            return status;
        }

        GenericAssetLoadResponsePtr spKFMResponse =
            EE_DYNAMIC_CAST(GenericAssetLoadResponse, pGenericResponse);
        EE_ASSERT(spKFMResponse);

        AssetLoadResponse::AssetLoadResult loadResult = spKFMResponse->GetResult();

        if (loadResult != AssetLoadResponse::ALR_Success &&
            loadResult != AssetLoadResponse::ALR_PartialSuccess)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("KFMFactory: Error during loading: %s", pRequest->GetAssetPath().c_str()));
            pResponseData->m_spResponse->SetResult(loadResult);
            return LOAD_COMPLETE;
        }

        // Load succeeded.  Create KFM tool and load KFs.
        NiKFMToolPtr spKFMTool = EE_NEW NiKFMTool;
        NiMemStream stream(spKFMResponse->GetAssetData(), spKFMResponse->GetAssetSize());
        NiKFMTool::KFM_RC returnCode = spKFMTool->LoadFromStream(
            &stream,
            pRequest->GetAssetPath().c_str());

        if (returnCode != NiKFMTool::KFM_SUCCESS)
        {
            // KFM parsing failed.
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("KFMFactory: KFM parsing failed: %s", pRequest->GetAssetPath().c_str()));
            pResponseData->m_spResponse->SetResult(AssetLoadResponse::ALR_ParseError);
            return LOAD_COMPLETE;
        }

        EE_ASSERT(loadResult == AssetLoadResponse::ALR_Success);
        pResponseData->LoadKFMTool(spKFMTool, pRequest);
        pResponseData->m_spResponse->SetResult(AssetLoadResponse::ALR_Success);
    }

    // Increment numWaitingSequences for any sequence that is still loading
    // but is still running.

    // Check KF loading.
    UInt32 numReqs = pResponseData->m_sequences.size();
    UInt32 numWaitingSequences = 0;

    NiActorManager* pActor = pResponseData->m_spResponse->GetActor();
    EE_ASSERT(pActor);

    for (UInt32 req = 0; req < numReqs; req++)
    {
        KFRequest* pKFRequest = pResponseData->m_sequences[req];
        if (!pKFRequest)
            continue;

        {
            NiStream stream;
            if (stream.Load(pKFRequest->GetAssetPath().c_str()))
            {
                UInt32 numSeqs = pKFRequest->m_sequenceIDs.size();
                for (UInt32 seq = 0; seq < numSeqs; seq++)
                {
                    UInt32 seqID = pKFRequest->m_sequenceIDs[seq];
                    pActor->AddSequenceData(seqID, stream, false);
                }
            }
            else
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("KFMFactory: KF load failed: %s", pKFRequest->GetAssetPath().c_str()));
                pResponseData->m_spResponse->SetResult(AssetLoadResponse::ALR_PartialSuccess);
            }
        }

        // If loaded, remove from array.
        pResponseData->m_sequences[req] = NULL;
    }

    if (numWaitingSequences > 0)
    {
        return LOAD_RUNNING;
    }
    else
    {
        return LOAD_COMPLETE;
    }
}

//------------------------------------------------------------------------------------------------
IAssetFactory::ThreadSafetyDescription KFMFactory::GetThreadSafety() const
{
    return IAssetFactory::LOADER_FG_AND_BG;
}

//------------------------------------------------------------------------------------------------
KFMFactory::ResponseData::ResponseData()
    : m_spResponse(NULL)
{
}

//------------------------------------------------------------------------------------------------
KFMFactory::ResponseData::~ResponseData()
{
    for (UInt32 i = 0; i < m_sequences.size(); i++)
    {
        m_sequences[i] = NULL;
    }
}

//------------------------------------------------------------------------------------------------
void KFMFactory::ResponseData::LoadKFMTool(
    NiKFMTool* pKFMTool,
    const KFMFactoryRequest* pRequest)
{
    bool accumulation = pRequest->GetAccumulation();

    NiActorManagerPtr pActor =
        NiActorManager::Create(pKFMTool, pKFMTool->GetBaseKFMPath(), accumulation, false);
    EE_ASSERT(pActor);
    if (!pActor)
        return;

    m_spResponse->SetActor(pActor);

    UInt32 numIDs;
    UInt32* pSequenceIDs;
    pKFMTool->GetSequenceIDs(pSequenceIDs, numIDs);
    m_sequences.reserve(numIDs);
    for (UInt32 i = 0; i < numIDs; i++)
    {
        UInt32 seqID = pSequenceIDs[i];

        // Get KF filename.
        const NiFixedString& filename = pKFMTool->GetFullKFFilename(seqID);
        EE_ASSERT(filename.Exists());

        bool found = false;

        // Check for shared filenames to minimize KF loads.
        UInt32 numReqs = m_sequences.size();
        for (UInt32 req = 0; req < numReqs; req++)
        {
            if (filename == m_sequences[req]->m_filename)
            {
                m_sequences[req]->m_sequenceIDs.push_back(seqID);
                found = true;
                break;
            }
        }

        if (!found)
        {
            KFRequestPtr newRequest = EE_NEW KFRequest(filename);
            newRequest->SetIsBackground(pRequest->GetIsBackground());
            newRequest->SetIsPreemptive(pRequest->GetIsPreemptive());
            newRequest->m_sequenceIDs.push_back(seqID);
            m_sequences.push_back(newRequest);
        }
    }

    EE_FREE(pSequenceIDs);
}

//------------------------------------------------------------------------------------------------
KFMFactory::KFRequest::KFRequest(const NiFixedString& filename)
    : AssetLoadRequest("", k_invalidMessageClassID, (const char*)filename)
    , m_filename(filename)
{
}

//------------------------------------------------------------------------------------------------
