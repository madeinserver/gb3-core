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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiActorManager.h"
#include "NiAnimationMetrics.h"
#include "NiCloningProcess.h"
#include <NiFilename.h>


const NiActorManager::SequenceID NiActorManager::ANY_SEQUENCE_ID =
    (unsigned int) -1;
const NiActorManager::SequenceID NiActorManager::INVALID_SEQUENCE_ID =
    (unsigned int) -2;
const float NiActorManager::INVALID_TIME = -FLT_MAX;
const float NiActorManager::SEQUENCE_DATA_FREQUENCY =
    NiControllerSequence::SEQUENCE_DATA_FREQUENCY;

const NiActorManager::SequenceID NiActorManager::ANY_EVENT_CODE =
    NiActorManager::ANY_SEQUENCE_ID;
const NiActorManager::SequenceID NiActorManager::INVALID_EVENT_CODE =
    NiActorManager::INVALID_SEQUENCE_ID;

//--------------------------------------------------------------------------------------------------
NiActorManager::NiActorManager(NiKFMTool* pkKFMTool,
    bool bCumulativeAnimations) :
    m_spKFMTool(pkKFMTool),
    m_spNIFRoot(NULL),
    m_spManager(NULL),
    m_pkCallbackObject(NULL),
    m_eTargetID(0),
    m_fTime(0.0f),
    m_fPauseTime(-NI_INFINITY),
    m_bStartTransition(false),
    m_eTransitionState(NO_TRANSITION),
    m_fTransStartTime(0.0f),
    m_fTransEndTime(0.0f),
    m_fTransStartFrameTime(0.0f),
    m_pcTargetKey(NULL),
    m_eCurID(0),
    m_eNextID(0),
    m_fNextChainTime(0.0f),
    m_bChainActive(false),
    m_bCumulativeAnimationsRequested(bCumulativeAnimations)
{
    Reset();
    m_kCallbacks.SetSize(3);
    m_kCallbacks.SetGrowBy(3);
}

//--------------------------------------------------------------------------------------------------
NiActorManager::~NiActorManager()
{
    // Don't process callbacks during destruction.
    m_pkCallbackObject = NULL;

    // Remove all active sequences from the controller manager, but don't
    // deactivate them since that would trigger their activation callbacks.
    if (m_spManager)
    {
        m_spManager->SetMaxRecycledSequences(0);
        m_spManager->RemoveAllSequences();
    }

    // Reset to clear various data members.
    Reset();

    // Release held objects.
    m_spNIFRoot = NULL;
    m_spManager = NULL;
    m_spKFMTool = NULL;
}

//--------------------------------------------------------------------------------------------------
NiActorManager* NiActorManager::Create(const char* pcKFMFilename,
    bool bCumulativeAnimations, bool bLoadFilesFromDisk,
    NiStream* pkStream, NiPoseBinding* pkPoseBinding)
{
    EE_ASSERT(pcKFMFilename);

    NiKFMTool::KFM_RC eRC, eSuccess = NiKFMTool::KFM_SUCCESS;

    // Create KFM tool and load KFM file.
    NiKFMToolPtr spKFMTool = NiNew NiKFMTool;
    eRC = spKFMTool->LoadFile(pcKFMFilename);
    if (eRC != eSuccess)
    {
        return NULL;
    }

    return Create(spKFMTool, pcKFMFilename, bCumulativeAnimations,
        bLoadFilesFromDisk, pkStream, pkPoseBinding);
}

//--------------------------------------------------------------------------------------------------
NiActorManager* NiActorManager::Create(NiKFMTool* pkKFMTool,
    const char* pcKFMFilePath, bool bCumulativeAnimations,
    bool bLoadFilesFromDisk, NiStream* pkStream, NiPoseBinding* pkPoseBinding)
{
    EE_ASSERT(pkKFMTool && pcKFMFilePath);

    // Build the KFM path.
    NiFilename kFilename(pcKFMFilePath);
    char acKFMPath[NI_MAX_PATH];
    NiSprintf(acKFMPath, NI_MAX_PATH, "%s%s", kFilename.GetDrive(),
        kFilename.GetDir());
    pkKFMTool->SetBaseKFMPath(acKFMPath);

    // Create actor manager.
    NiActorManager* pkActorManager = NiNew NiActorManager(pkKFMTool,
        bCumulativeAnimations);

    // Initialize actor manager.
    if (bLoadFilesFromDisk)
    {
        // Create the stream if not provided.
        bool bDeleteStream = false;
        if (!pkStream)
        {
            pkStream = NiNew NiStream;
            bDeleteStream = true;
        }

        // Initialize the actor manager and load all files.
        if (!pkActorManager->Initialize(*pkStream, pkPoseBinding))
        {
            NiDelete pkActorManager;
            return NULL;
        }

        // Delete stream if created earlier.
        if (bDeleteStream)
        {
            NiDelete pkStream;
        }
    }
    else
    {
        // Create a dummy controller manager so that sequences can be added.
        // During a later ChangeNIFRoot() call, these will be copied to the real manager.
        pkActorManager->m_spManager = EE_NEW NiControllerManager(NULL);
    }

    return pkActorManager;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::Initialize(NiStream& kStream,
    NiPoseBinding* pkPoseBinding)
{
    if (!LoadNIFFile(kStream, true, pkPoseBinding))
    {
        return false;
    }
    LoadAllSequenceData(&kStream);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::LoadNIFFile(NiStream& kStream, bool bLoadNIFFile,
    NiPoseBinding* pkPoseBinding)
{
    // Get model path.
    const char* pcModelPath = m_spKFMTool->GetModelPath();
    if (!pcModelPath)
    {
        return false;
    }

    // Load NIF file, if indicated.
    if (bLoadNIFFile)
    {
        // Get NIF filename.
        const char* pcNIFFilename = m_spKFMTool->GetFullModelPath();
        EE_ASSERT(pcNIFFilename);

        // Load NIF file.
        bool bSuccess = kStream.Load(pcNIFFilename);
        if (!bSuccess)
        {
            return false;
        }
    }

    // Get NIF root.
    NiAVObject* pkNIFRoot = NiDynamicCast(NiAVObject,
        kStream.GetObjectAt(0));
    if (!pkNIFRoot)
    {
        return false;
    }

    return ChangeNIFRoot(pkNIFRoot, pkPoseBinding);
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::LoadAllSequenceData(NiStream* pkStream)
{
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    // Iterate through entire array of sequence IDs.
    bool bAllSuccessful = true;
    SequenceID* pSequenceIDs;
    unsigned int uiNumIDs;
    m_spKFMTool->GetSequenceIDs(pSequenceIDs, uiNumIDs);
    char acLastKFFilename[NI_MAX_PATH];
    acLastKFFilename[0] = '\0';
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        SequenceID eSequenceID = pSequenceIDs[ui];

        // Get KF filename.
        NiFixedString kKFFilename = m_spKFMTool->GetFullKFFilename(
            eSequenceID);
        EE_ASSERT(kKFFilename.Exists());

        // Determine whether or not to load file.
        bool bLoadKFFile = false;
        if (strcmp(acLastKFFilename, kKFFilename) != 0)
        {
            bLoadKFFile = true;
            NiStrcpy(acLastKFFilename, NI_MAX_PATH, kKFFilename);
        }

        if (!AddSequenceData(eSequenceID, *pkStream, bLoadKFFile))
        {
            bAllSuccessful = false;
        }
    }
    NiFree(pSequenceIDs);

    if (bDeleteStream)
    {
        NiDelete pkStream;
    }

    return bAllSuccessful;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::AddSequenceData(SequenceID eSequenceID,
    NiStream& kStream, bool bLoadKFFile)
{
    if (!m_spManager)
    {
        return false;
    }

    // Get sequence information.
    NiKFMTool::Sequence* pkKFMSequence = m_spKFMTool->GetSequence(
        eSequenceID);
    if (!pkKFMSequence)
    {
        return false;
    }

    // Get KF filename.
    NiFixedString kKFFilename = m_spKFMTool->GetFullKFFilename(eSequenceID);
    EE_ASSERT(kKFFilename.Exists());

    // Load file, if specified.
    if (bLoadKFFile)
    {
        bool bSuccess = kStream.Load(kKFFilename);
        if (!bSuccess)
        {
            NILOG("NiActorManager: Failed to load "
                "KF File: %s\n", (const char*) kKFFilename);
            return false;
        }
    }

    // Note: as of Gamebryo 2.5.0, NiKFMTool was modified to store
    // filename/sequencename pairs instead of filename/index pairs to allow
    // for the possibility of sequences being added or removed in a DCC app
    // without having to modify the KFM file.  For backwards compatibility,
    // if there is no name, then we will use the animation index.
    NiSequenceDataPtr spSeqData;
    const NiFixedString& kSeqName = pkKFMSequence->GetSequenceName();

    if (kSeqName.Exists())
    {
        spSeqData = NiSequenceData::CreateSequenceDataFromFile(kStream,
            kSeqName);

        if (!spSeqData)
        {
            NILOG("NiActorManager: Failed to add "
                "sequence with name %s in %s\n", (const char*)kSeqName,
                (const char*) kKFFilename);
            return false;
        }
    }
    else
    {
        int iAnimIndex = pkKFMSequence->GetAnimIndex();
        spSeqData =
            NiSequenceData::CreateSequenceDataFromFile(kStream, iAnimIndex);
        if (!spSeqData)
        {
            NILOG("NiActorManager: Failed to add "
                "sequence at index %d in %s\n", iAnimIndex,
                (const char*) kKFFilename);
            return false;
        }
    }

    return ChangeSequenceData(eSequenceID, spSeqData);
}

//--------------------------------------------------------------------------------------------------
NiActorManager* NiActorManager::Clone(NiCloningProcess* pkCloningProcess)
{
    // Clone the NIF root.
    bool bDeleteCloningProcess = false;
    if (!pkCloningProcess)
    {
        pkCloningProcess = NiNew NiCloningProcess();
        bDeleteCloningProcess = true;
    }
    pkCloningProcess->m_eCopyType = NiObjectNET::COPY_EXACT;
    NiAVObject* pkNIFRoot = (NiAVObject*) m_spNIFRoot->Clone(
        *pkCloningProcess);
    if (bDeleteCloningProcess)
    {
        NiDelete pkCloningProcess;
    }

    // Get the controller manager from the clone.
    NiControllerManager* pkManager =
        NiControllerManager::FindControllerManager(pkNIFRoot);
    EE_ASSERT(pkManager);

    // Create new actor manager.
    NiActorManager* pkActorManager = NiNew NiActorManager(m_spKFMTool,
        m_bCumulativeAnimationsRequested);
    pkActorManager->m_spNIFRoot = pkNIFRoot;
    pkActorManager->m_spManager = pkManager;

    // Copy member data.
    pkActorManager->m_pkCallbackObject = m_pkCallbackObject;
    NiTMapIterator kIter = m_kSeqDataMap.GetFirstPos();
    while (kIter)
    {
        SequenceID eSequenceID;
        NiSequenceDataPtr spOrigSeqData;
        m_kSeqDataMap.GetNext(kIter, eSequenceID, spOrigSeqData);

        NiSequenceData* pkNewSeqData = pkActorManager->m_spManager
            ->GetSequenceDataByName(spOrigSeqData->GetName());
        pkActorManager->m_kSeqDataMap.SetAt(eSequenceID,
            pkNewSeqData);
    }
    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);
        if (pkData)
        {
            pkActorManager->m_kCallbacks.AddFirstEmpty(pkData);
        }
    }

    return pkActorManager;
}

//--------------------------------------------------------------------------------------------------
NiActorManager* NiActorManager::CloneOnlyAnimation(NiAVObject* pkAVObject)
{
    return CloneOnlyAnimation(pkAVObject, m_bCumulativeAnimationsRequested);
}

//--------------------------------------------------------------------------------------------------
NiActorManager* NiActorManager::CloneOnlyAnimation(
    NiAVObject* pkAVObject,
    bool bCumulativeAnimations)
{
    // Get the actor root
    const char* pcModelRoot = m_spKFMTool->GetModelRoot();
    if (!pcModelRoot)
    {
        return NULL;
    }

    // Find the actor root, may be NULL.
    NiAVObject* pkActorRoot = pkAVObject ? pkAVObject->GetObjectByName(pcModelRoot) : NULL;

    // Create the controller manager.
    NiControllerManager* pkManager = NiNew NiControllerManager(pkActorRoot, bCumulativeAnimations);

    // Create new actor manager.
    NiActorManager* pkActorManager = NiNew NiActorManager(m_spKFMTool, bCumulativeAnimations);

    // Copy member data.
    pkActorManager->m_pkCallbackObject = m_pkCallbackObject;
    unsigned int uiCount = m_spManager->GetSequenceDataCount();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        // Get sequence data: skip temporary poses.
        NiSequenceData* pkSeqData = m_spManager->GetSequenceDataAt(ui);
        if (!pkSeqData || pkSeqData->GetTempPose())
            continue;

        // Share original sequence data.
        pkManager->AddSequenceData(pkSeqData);
    }

    NiTMapIterator kIter = m_kSeqDataMap.GetFirstPos();
    while (kIter)
    {
        SequenceID eSequenceID;
        NiSequenceDataPtr spOrigSeqData;
        m_kSeqDataMap.GetNext(kIter, eSequenceID, spOrigSeqData);

        NiSequenceData* pkNewSeqData = pkManager->GetSequenceDataByName(spOrigSeqData->GetName());
        pkActorManager->m_kSeqDataMap.SetAt(eSequenceID, pkNewSeqData);
    }

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);
        if (pkData)
        {
            pkActorManager->m_kCallbacks.AddFirstEmpty(pkData);
        }
    }

    pkActorManager->m_spNIFRoot = pkAVObject;
    pkActorManager->m_spManager = pkManager;

    return pkActorManager;
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::Update(float fTime)
{
    NIMETRICS_ANIMATION_SCOPETIMER(AM_UPDATE_TIME);

    NiKFMTool::KFM_RC eRC;

    m_fTime = fTime;

    if (IsPaused())
        return;

    if (m_fPauseTime != -NI_INFINITY)
    {
        if (m_eTransitionState != NO_TRANSITION)
        {
            float fAdjust = m_fTime - m_fPauseTime;
            m_fTransStartTime += fAdjust;
            m_fTransEndTime += fAdjust;
        }

        m_fPauseTime = -NI_INFINITY;
    }

    ProcessExtraSeqCallbacks(m_fTime);

    if (m_eTransitionState != NO_TRANSITION)
    {
        if (m_bStartTransition && m_fTime >= m_fTransStartTime)
        {
            if (m_eTransitionState == BLENDING)
            {
                m_spCurSequence->Update(m_fTransStartTime, 0, false, true);
                float fDuration = m_fTransEndTime - m_fTransStartTime;
                m_spNextSequence = m_spManager->BlendFromSequence(
                    m_spCurSequence, m_spNextSeqData, fDuration,
                    m_pcTargetKey);
                EE_ASSERT(m_spNextSequence != NULL);
                m_spNextSequence->Update(m_fTransStartTime, 0, false, false);

                AddCallbacks(m_eNextID, m_spNextSequence);

                RaiseAnimDeactivatedEvents(m_eCurID,
                    m_spCurSequence, m_fTransStartTime);
            }
            m_bStartTransition = false;
        }

        if (m_fTime >= m_fTransEndTime)
        {
            if (m_spCurSequence && m_spCurSequence->GetState() != INACTIVE)
            {
                m_spCurSequence->Update(m_fTransEndTime, 0, false, true);
                m_spManager->DeactivateSequence(m_spCurSequence);
            }

            RaiseAnimActivatedEvents(m_eNextID, m_spNextSequence,
                m_fTransEndTime);

            m_eTransitionState = NO_TRANSITION;
            m_eCurID = m_eNextID;
            m_spCurSequence = m_spNextSequence;
            m_eNextID = INVALID_SEQUENCE_ID;
            m_spNextSeqData = NULL;
            m_spNextSequence = NULL;
        }
        else
        {
            return;
        }
    }

    SequenceID eDesiredID = m_eCurID;
    float fTransStartTime = m_fTime;

    // Handle animation chains.
    if (m_bChainActive)
    {
        if (m_fNextChainTime == INVALID_TIME)
        {
            EE_ASSERT(!m_kChainIDs.IsEmpty() && !m_kChainDurations.IsEmpty());
            eDesiredID = m_kChainIDs.RemoveHead();
            float fDuration = m_kChainDurations.RemoveHead();
            if (!m_kChainIDs.IsEmpty())
            {
                if (fDuration == NiKFMTool::MAX_DURATION)
                {
                    EE_ASSERT(GetSequenceData(eDesiredID) ==
                        m_spCurSequence->GetSequenceData());

                    // Get the time until the end of the sequence.
                    EE_ASSERT(eDesiredID == m_eCurID);
                    fDuration = m_spCurSequence->GetTimeAt(
                        NiAnimationConstants::GetEndTextKey(),
                        m_fTime) - m_fTime;
                }
                m_fNextChainTime = m_fTransEndTime + fDuration;
            }
            else
            {
                EE_ASSERT(m_kChainDurations.IsEmpty());
                m_fNextChainTime = INVALID_TIME;
                m_bChainActive = false;
            }
        }

        if (m_fTime >= m_fNextChainTime && m_fNextChainTime != INVALID_TIME)
        {
            EE_ASSERT(!m_kChainIDs.IsEmpty() && !m_kChainDurations.IsEmpty());
            eDesiredID = m_kChainIDs.GetHead();
            fTransStartTime = m_fNextChainTime;
            m_fNextChainTime = INVALID_TIME;
        }
    }
    else
    {
        eDesiredID = m_eTargetID;
    }

    // If desired sequence is already animating, don't do anything.
    if (eDesiredID == m_eCurID)
    {
        return;
    }

    // If target sequence is invalid, deactivate all sequences.
    if (eDesiredID == INVALID_SEQUENCE_ID)
    {
        RaiseAnimDeactivatedEvents(m_eCurID, m_spCurSequence, m_fTime);
        RaiseAnimCompletedEvents(m_eCurID, m_spCurSequence, m_fTime);

        m_eCurID = INVALID_SEQUENCE_ID;
        if (m_spCurSequence)
        {
            m_spManager->DeactivateSequence(m_spCurSequence);
            m_spCurSequence = NULL;
        }
        return;
    }

    // If there is no current sequence, activate the desired one and return.
    if (m_eCurID == INVALID_SEQUENCE_ID)
    {
        NiSequenceData* pkDesiredSeqData = GetSequenceData(eDesiredID);
        EE_ASSERT(pkDesiredSeqData);
        NiControllerSequence* pkDesiredSequence =
            m_spManager->ActivateSequence(pkDesiredSeqData, 0);
        EE_ASSERT(pkDesiredSequence);

        m_eCurID = eDesiredID;
        m_spCurSequence = pkDesiredSequence;

        pkDesiredSequence->Update(m_fTime, 0, false, false);
        AddCallbacks(m_eCurID, m_spCurSequence);
        RaiseAnimActivatedEvents(m_eCurID, m_spCurSequence, m_fTime);

        return;
    }

    // Determine if transition is allowed.
    bool bTransAllowed = false;
    eRC = m_spKFMTool->IsTransitionAllowed(m_eCurID, eDesiredID,
        bTransAllowed);
    EE_ASSERT(eRC == NiKFMTool::KFM_SUCCESS);
    if (bTransAllowed)
    {
        // Get desired sequence data.
        NiSequenceData* pkDesiredSeqData = GetSequenceData(eDesiredID);
        EE_ASSERT(pkDesiredSeqData);

        // Retrieve the desired transition.
        NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
            m_eCurID, eDesiredID);
        EE_ASSERT(pkTransition);

        // Store the chain animations.
        if (pkTransition->GetType() == NiKFMTool::TYPE_CHAIN)
        {
            // Assert that a chain is not already active. Nested chains are
            // not supported.
            EE_ASSERT(!m_bChainActive);

            // Assert that this transition does not contain an infinite chain.
            // This check can be expensive, so only do for debug builds.
            EE_ASSERT(m_spKFMTool->IsValidChainTransition(m_eCurID, eDesiredID,
                pkTransition));

            m_kChainIDs.RemoveAll();
            m_kChainDurations.RemoveAll();

            unsigned int uiNumChainAnims =
                pkTransition->GetChainInfo().GetSize();
            EE_ASSERT(uiNumChainAnims > 0);

            for (unsigned int ui = 0; ui < uiNumChainAnims; ui++)
            {
                unsigned int uiChainID = pkTransition->GetChainInfo()
                    .GetAt(ui).GetSequenceID();
                float fDuration = pkTransition->GetChainInfo().GetAt(ui)
                    .GetDuration();

                m_kChainIDs.AddTail(uiChainID);
                m_kChainDurations.AddTail(fDuration);
            }
            m_kChainIDs.AddTail(eDesiredID);
            m_kChainDurations.AddTail(NiKFMTool::MAX_DURATION);

            eDesiredID = m_kChainIDs.GetHead();
            pkDesiredSeqData = GetSequenceData(eDesiredID);
            EE_ASSERT(pkDesiredSeqData);
            pkTransition = m_spKFMTool->GetTransition(m_eCurID, eDesiredID);
            EE_ASSERT(pkTransition);
            m_fNextChainTime = INVALID_TIME;
            m_bChainActive = true;
        }

        // Determine start time of transition.
        m_fTransStartTime = fTransStartTime;
        m_spCurSequence->Update(m_fTransStartTime, 0, false, true);
        m_pcTargetKey = NULL;
        if (pkTransition->GetType() == NiKFMTool::TYPE_BLEND)
        {
            if (pkTransition->GetBlendPairs().GetSize() > 0)
            {
                // This update is to ensure that GetTimeAt returns an
                // accurate time.
                m_spCurSequence->Update(m_fTime, 0, false, true);

                m_fTransStartTime = NI_INFINITY;
                for (unsigned int ui = 0;
                    ui < pkTransition->GetBlendPairs().GetSize(); ui++)
                {
                    NiKFMTool::Transition::BlendPair* pkPair =
                        pkTransition->GetBlendPairs().GetAt(ui);

                    if (!pkPair->GetStartKey())
                    {
                        m_fTransStartTime = m_fTime;
                        m_pcTargetKey = pkPair->GetTargetKey();
                        break;
                    }

                    float fTempTime = m_spCurSequence->GetTimeAt(
                        pkPair->GetStartKey(), m_fTime);
                    if (fTempTime != NiControllerSequence::INVALID_TIME &&
                        fTempTime >= m_fTime &&
                        fTempTime < m_fTransStartTime)
                    {
                        m_fTransStartTime = fTempTime;
                        m_pcTargetKey = pkPair->GetTargetKey();
                    }
                }
            }
        }

        // Determine end time of transition.
        EE_ASSERT(pkTransition->GetDuration() > 0.0f);
        m_fTransEndTime = m_fTransStartTime + pkTransition->GetDuration();

        // Desired sequence is not yet active.
        NiControllerSequence* pkDesiredSequence = NULL;

        // Start appropriate transition.
        switch (pkTransition->GetType())
        {
            case NiKFMTool::TYPE_BLEND:
                m_fTransStartFrameTime = m_spCurSequence->GetLastScaledTime();
                if (m_fTransStartTime == m_fTime)
                {
                    pkDesiredSequence = m_spManager->BlendFromSequence(
                        m_spCurSequence, pkDesiredSeqData,
                        pkTransition->GetDuration(), m_pcTargetKey);
                    EE_ASSERT(pkDesiredSequence != NULL);
                }
                else
                {
                    m_fTransStartFrameTime += m_spCurSequence
                        ->ComputeScaledTime(m_fTransStartTime - m_fTime,
                        false);
                    m_bStartTransition = true;
                }
                m_eTransitionState = BLENDING;
                break;
            case NiKFMTool::TYPE_MORPH:
            {
                m_fTransStartFrameTime = m_spCurSequence->GetLastScaledTime();
                pkDesiredSequence = m_spManager->Morph(m_spCurSequence,
                    pkDesiredSeqData, pkTransition->GetDuration());
                EE_ASSERT(pkDesiredSequence != NULL);
                m_eTransitionState = MORPHING;

                RaiseAnimDeactivatedEvents(m_eCurID,
                    m_spCurSequence, m_fTransStartTime);
                break;
            }
            case NiKFMTool::TYPE_CROSSFADE:
            {
                m_fTransStartFrameTime = m_spCurSequence->GetLastScaledTime();
                pkDesiredSequence = m_spManager->CrossFade(m_spCurSequence,
                    pkDesiredSeqData, pkTransition->GetDuration());
                EE_ASSERT(pkDesiredSequence != NULL);
                m_eTransitionState = CROSSFADING;

                RaiseAnimDeactivatedEvents(m_eCurID,
                    m_spCurSequence, m_fTransStartTime);
                break;
            }
            default:
                EE_ASSERT(false);
                break;
        }

        m_eNextID = eDesiredID;
        m_spNextSeqData = pkDesiredSeqData;
        m_spNextSequence = pkDesiredSequence;

        if (pkDesiredSequence)
        {
            AddCallbacks(eDesiredID, pkDesiredSequence);
            pkDesiredSequence->Update(m_fTransStartTime, 0, false, false);
        }
    }

}

//--------------------------------------------------------------------------------------------------
NiControllerSequence* NiActorManager::GetActiveSequence(
    SequenceID eSequenceID,
    bool bCheckExtraSequences,
    bool bCheckStateSequences) const
{
    if (bCheckExtraSequences)
    {
        // check extra sequences
        NiControllerSequence* pkSequence = GetExtraSequence(eSequenceID);
        if (pkSequence && pkSequence->GetState() != INACTIVE)
        {
            return pkSequence;
        }
    }

    if (bCheckStateSequences)
    {
        // check current sequence
        if (m_eCurID == eSequenceID &&
            m_spCurSequence && m_spCurSequence->GetState() != INACTIVE)
        {
            return m_spCurSequence;
        }

        // check next sequence
        if (m_eNextID == eSequenceID &&
            m_spNextSequence && m_spNextSequence->GetState() != INACTIVE)
        {
            return m_spNextSequence;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID NiActorManager::GetSequenceID(
    NiControllerSequence* pkSequence, bool bCheckExtraSequences,
    bool bCheckStateSequences) const
{
    EE_ASSERT(pkSequence);

    if (bCheckExtraSequences)
    {
        // check extra sequences
        NiTMapIterator pos_seq = m_kExtraSequenceMap.GetFirstPos();
        while (pos_seq)
        {
            SequenceID eSequenceID;
            NiControllerSequencePtr spSequence;
            m_kExtraSequenceMap.GetNext(pos_seq, eSequenceID, spSequence);
            if (pkSequence == spSequence)
            {
                return eSequenceID;
            }
        }
    }

    if (bCheckStateSequences)
    {
        // check current sequence
        if (pkSequence == m_spCurSequence)
        {
            return m_eCurID;
        }

        // check next sequence
        if (pkSequence == m_spNextSequence)
        {
            return m_eNextID;
        }
    }

    return INVALID_SEQUENCE_ID;
}

//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID NiActorManager::FindSequenceID(
    const char* pcName) const
{
    // pcName must be non-null
    EE_ASSERT(pcName);

    NiTMapIterator kItr = m_kSeqDataMap.GetFirstPos();
    while (kItr)
    {
        SequenceID eID;
        NiSequenceDataPtr spSeqData;
        m_kSeqDataMap.GetNext(kItr, eID, spSeqData);
        if (spSeqData && spSeqData->GetName() && !strcmp(pcName, spSeqData->GetName()))
        {
            // found the first one, return it
            return eID;
        }
    }

    // didn't find any with that name
    return INVALID_SEQUENCE_ID;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::RegisterCallback(EventType eEventType,
    SequenceID eSequenceID, const NiFixedString& kTextKey)
{
    NiTextKeyMatch* pkMatch = NULL;

    if (eEventType == TEXT_KEY_EVENT)
    {
        if (kTextKey.Exists())
            pkMatch = NiNew NiTextKeyMatch(kTextKey);
        else
            return false;
    }

    return RegisterCallback(eEventType, eSequenceID, pkMatch);
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::UnregisterCallback(EventType eEventType,
    SequenceID eSequenceID, const NiFixedString& kTextKey)
{
    if (eEventType == END_OF_SEQUENCE)
    {
        UnregisterCallback(eEventType, eSequenceID,
            NiAnimationConstants::GetEndOfSequenceMatch());
    }

    if (eEventType != TEXT_KEY_EVENT)
        return;

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);

        if (pkData == NULL)
            continue;

        if (pkData->m_eEventType == eEventType &&
            pkData->m_eSequenceID == eSequenceID)
        {
            NiTextKeyMatch* pkCurrMatch = pkData->GetMatchObject();

            // pkCurrMatch will only exist if it is not a text key event,
            // in which case we do not need to check for a match.
            if (pkCurrMatch == NULL ||
                (pkCurrMatch != NULL && pkCurrMatch->IsKeyMatch(kTextKey)))
            {
                m_kCallbacks.RemoveAt(ui);
                break;
            }
        }
    }

    if (eSequenceID == ANY_SEQUENCE_ID)
    {
        for (unsigned int i = 0; i < m_spManager->GetSequenceCount(); i++)
        {
            NiControllerSequence* pkSeq = m_spManager->GetSequenceAt(i);
            pkSeq->RemoveTextKeyCallback(this, kTextKey);
        }
    }
    else
    {
        NiSequenceData* pkSeqData = GetSequenceData(eSequenceID);
        if (!pkSeqData)
            return;
        NiControllerSequence* pkSeq =
            m_spManager->GetSequenceBySequenceData(pkSeqData);
        if (!pkSeq)
            return;
        pkSeq->RemoveTextKeyCallback(this, kTextKey);
    }
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::InitCallback(CallbackData* pkCallback)
{
    EE_ASSERT(pkCallback);
    m_kCallbacks.AddFirstEmpty(pkCallback);

    if (!pkCallback->GetMatchObject())
        return true;

    if (pkCallback->m_eSequenceID == ANY_SEQUENCE_ID)
    {
        for (unsigned int i = 0; i < m_spManager->GetSequenceCount(); i++)
        {
            NiControllerSequence* pkSeq = m_spManager->GetSequenceAt(i);
            pkSeq->AddTextKeyCallback(this, pkCallback->GetMatchObject());
        }
    }
    else
    {
        NiSequenceData* pkSeqData = GetSequenceData(pkCallback->m_eSequenceID);
        if (!pkSeqData)
            return false;
        NiControllerSequence* pkSeq =
            m_spManager->GetSequenceBySequenceData(pkSeqData);
        // If the controller sequence doesn't exist yet, we will
        // add the text key callback when it gets created.
        if (!pkSeq)
            return true;
        pkSeq->AddTextKeyCallback(this, pkCallback->GetMatchObject());
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::RegisterCallback(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject)
{
    if (eEventType == TEXT_KEY_EVENT)
    {
        if (pkMatchObject == NULL)
            return false;
    }
    else if (eEventType == END_OF_SEQUENCE)
    {
        pkMatchObject = NiAnimationConstants::GetEndOfSequenceMatch();
    }
    else
    {
        pkMatchObject = NULL;
    }

    UnregisterCallback(eEventType, eSequenceID, pkMatchObject);
    CallbackDataPtr spData = NiNew CallbackData(eEventType, eSequenceID, pkMatchObject);
    return InitCallback(spData);
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::UnregisterCallback(EventType eEventType,
    SequenceID eSequenceID, NiTextKeyMatch* pkMatchObject)
{
    if (eEventType == END_OF_SEQUENCE)
    {
        pkMatchObject = NiAnimationConstants::GetEndOfSequenceMatch();
    }
    else if (eEventType != TEXT_KEY_EVENT)
    {
        pkMatchObject = NULL;
    }

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkData = m_kCallbacks.GetAt(ui);

        if (pkData && pkData->m_eEventType == eEventType &&
            pkData->m_eSequenceID == eSequenceID &&
            pkData->GetMatchObject() == pkMatchObject)
        {
            m_kCallbacks.RemoveAt(ui);
            break;
        }
    }

    if (!pkMatchObject)
        return;

    if (eSequenceID == ANY_SEQUENCE_ID)
    {
        for (unsigned int i = 0; i < m_spManager->GetSequenceCount(); i++)
        {
            NiControllerSequence* pkSeq = m_spManager->GetSequenceAt(i);
            pkSeq->RemoveTextKeyCallback(this, pkMatchObject);
        }
    }
    else
    {
        NiSequenceData* pkSeqData = GetSequenceData(eSequenceID);
        if (!pkSeqData)
            return;
        NiControllerSequence* pkSeq =
            m_spManager->GetSequenceBySequenceData(pkSeqData);
        if (!pkSeq)
            return;
        pkSeq->RemoveTextKeyCallback(this, pkMatchObject);
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::CopyCallbacks(NiActorManager* pkSource)
{
    ClearAllRegisteredCallbacks();

    if (!pkSource)
        return;

    for (unsigned int i = 0; i < pkSource->m_kCallbacks.GetSize(); i++)
    {
        NiActorManager::CallbackData* pData = pkSource->m_kCallbacks.GetAt(i);
        if (!pData)
            continue;

        InitCallback(pData);
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::TextKeyEvent(NiControllerSequence* pkSequence,
    const NiTextKeyMatch* pkMatchObject,
    const NiFixedString& kTextKey,
    float fEventTime)
{
    if (!m_pkCallbackObject)
        return;

    SequenceID eSequenceID = GetSequenceID(pkSequence, true, true);
    if (pkMatchObject == NiAnimationConstants::GetEndOfSequenceMatch())
    {
        m_pkCallbackObject->EndOfSequence(this, eSequenceID, pkSequence,
            fEventTime);
    }
    else
    {
        m_pkCallbackObject->TextKeyEvent(this, eSequenceID, kTextKey,
            pkMatchObject, pkSequence, fEventTime);
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::AddCallbacks(SequenceID eSeqID,
    NiControllerSequence* pkSeq)
{
    EE_ASSERT(pkSeq);
    pkSeq->AddActivationCallback(this);

    for (unsigned int i = 0; i < m_kCallbacks.GetSize(); i++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(i);

        if (!pkCallbackData)
            continue;

        if (pkCallbackData->m_eSequenceID != ANY_SEQUENCE_ID &&
            pkCallbackData->m_eSequenceID != eSeqID)
        {
            continue;
        }

        if (pkCallbackData->m_eEventType == TEXT_KEY_EVENT)
        {
            pkSeq->AddTextKeyCallback(this,
                pkCallbackData->GetMatchObject());
        }
        else if (pkCallbackData->m_eEventType == END_OF_SEQUENCE)
        {
            pkSeq->AddTextKeyCallback(this,
                NiAnimationConstants::GetEndOfSequenceMatch());
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::ProcessExtraSeqCallbacks(float fEndTime)
{
    if (!m_pkCallbackObject)
        return;

    NiTMapIterator kItr = m_kExtraSequenceMap.GetFirstPos();
    NiActorManager::SequenceID eSeqID;
    NiControllerSequencePtr spSeq;
    while (kItr)
    {
        m_kExtraSequenceMap.GetNext(kItr, eSeqID, spSeq);
        EE_ASSERT(spSeq);
        // If inactive, this sequence should have been removed via
        // NiActorManager::ActivationChanged.
        EE_ASSERT(spSeq->GetState() != INACTIVE);

        // If we are easing in and the ease-in will happen between the
        // previous time and the next update.
        if (spSeq->GetState() == EASEIN &&
            spSeq->GetEaseEndTime() <= fEndTime)
        {
            RaiseAnimActivatedEvents(eSeqID, spSeq, spSeq->GetEaseEndTime());
        }
    }
}

//--------------------------------------------------------------------------------------------------
NiControllerSequence* NiActorManager::ActivateSequence(
    SequenceID eSequenceID, int iPriority, float fWeight,
    float fEaseInTime, SequenceID eTimeSyncSeqID,
    float fFrequency, float fStartFrame,
    bool bAdditiveBlend, float fAdditiveRefFrame)
{
    NiSequenceData* pkSeqData = GetSequenceData(eSequenceID);
    if (!pkSeqData)
    {
        return NULL;
    }

    // Find active time sync sequence.
    NiControllerSequence* pkTimeSyncSeq = NULL;
    if (eTimeSyncSeqID != NiKFMTool::SYNC_SEQUENCE_ID_NONE)
    {
        pkTimeSyncSeq = GetActiveSequence(eTimeSyncSeqID, true, true);
        if (!pkTimeSyncSeq)
        {
            return NULL;
        }
    }

    // If there is an active sequence already, then return that.
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);
    if (spExtraSeq)
    {
        if (spExtraSeq->GetState() == ANIMATING)
        {
            // Can't activate a fully active animation.
            return NULL;
        }
        else if (spExtraSeq->GetState() != INACTIVE)
        {
            if (fEaseInTime == 0.0f)
            {
                // If the ease-in  time is 0.0, we need to immediately raise any
                // AnimActivated events for the sequence.
                RaiseAnimActivatedEvents(eSequenceID, spExtraSeq, m_fTime);
            }

            spExtraSeq->Reactivate(iPriority, fWeight, fEaseInTime,
                pkTimeSyncSeq, NiControllerSequence::CURRENT_FRAME, false, true);

            if (m_fTime != INVALID_TIME)
            {
                spExtraSeq->Update(m_fTime, 0, false, false);
            }

            return spExtraSeq;
        }
    }

    // Activate a new extra sequence for the specified ID.
    NiControllerSequence* pkSequence = m_spManager->ActivateSequenceInternal(
        pkSeqData, iPriority, fWeight, fEaseInTime, pkTimeSyncSeq,
        fFrequency, fStartFrame, false, false, bAdditiveBlend, fAdditiveRefFrame);
    if (pkSequence != NULL)
    {
        // Add the new extra sequence at the specified ID.
        m_kExtraSequenceMap.SetAt(eSequenceID, pkSequence);

        AddCallbacks(eSequenceID, pkSequence);

        if (m_fTime != INVALID_TIME && !IsPaused())
            pkSequence->Update(m_fTime, 0, false, false);

        if (fEaseInTime == 0.0f)
        {
            // If the ease-in  time is 0.0, we need to immediately raise any
            // AnimActivated events for the sequence.
            RaiseAnimActivatedEvents(eSequenceID, pkSequence, m_fTime);
        }
    }

    return pkSequence;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::DeactivateSequence(SequenceID eSequenceID,
    float fEaseOutTime)
{
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);
    if (!spExtraSeq)
    {
        return false;
    }

    if (spExtraSeq->GetState() == INACTIVE)
    {
        return false;
    }

    if (spExtraSeq->GetState() != EASEOUT)
    {
        // Regardless of the ease out time, if we are deactivating this sequence,
        // at this point in time it is no longer fully active, so raise the
        // deactivated event.
        RaiseAnimDeactivatedEvents(eSequenceID, spExtraSeq, m_fTime);
    }

    // The previously checked pre-conditions should ensure success.
    EE_VERIFY(m_spManager->DeactivateSequence(spExtraSeq, fEaseOutTime));

    if (m_fTime != INVALID_TIME)
    {
        spExtraSeq->Update(m_fTime, 0, false, true);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::ReloadKFFile(const char* pcFile)
{
    NiSequenceDataPointerArray kSeqDatas;
    if (!NiSequenceData::CreateAllSequenceDatasFromFile(pcFile, kSeqDatas))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < kSeqDatas.GetSize(); ui++)
    {
        NiSequenceData* pkSeqData = kSeqDatas.GetAt(ui);
        if (!pkSeqData)
            continue;

        SequenceID eSeqID = FindSequenceID(pkSeqData->GetName());
        if (eSeqID != INVALID_SEQUENCE_ID)
        {
            ChangeSequenceData(eSeqID, pkSeqData);
        }
        else
        {
            // This actor manager does not know anything about this
            // sequence, so it has no sequence ID.  Add it to the
            // controller manager in case some later actor manager
            // (cloned from this one) knows what to do with it.
            m_spManager->AddSequenceData(pkSeqData);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiKFMTool::KFM_RC NiActorManager::ChangeSequenceID(SequenceID eOldID,
    SequenceID eNewID)
{
    // Update KFM tool.
    NiKFMTool::KFM_RC eRC = m_spKFMTool->UpdateSequenceID(eOldID, eNewID);

    // Update stored IDs.
    if (m_eTargetID == eOldID)
    {
        m_eTargetID = eNewID;
    }
    if (m_eCurID == eOldID)
    {
        m_eCurID = eNewID;
    }
    if (m_eNextID == eOldID)
    {
        m_eNextID = eNewID;
    }

    // Update chain IDs.
    NiTListIterator pos_chain = m_kChainIDs.GetHeadPos();
    while (pos_chain)
    {
        unsigned int uiSequenceID = m_kChainIDs.Get(pos_chain);
        if (uiSequenceID == eOldID)
        {
            NiTListIterator pos_current = m_kChainIDs.InsertAfter(pos_chain,
                eNewID);
            m_kChainIDs.RemovePos(pos_chain);
            pos_chain = pos_current;
        }
        pos_chain = m_kChainIDs.GetNextPos(pos_chain);
    }

    // Update sequence map.
    NiTMapIterator pos_seq = m_kSeqDataMap.GetFirstPos();
    while (pos_seq)
    {
        SequenceID eSequenceID;
        NiSequenceDataPtr spSeqData;
        m_kSeqDataMap.GetNext(pos_seq, eSequenceID, spSeqData);
        if (eSequenceID == eOldID)
        {
            m_kSeqDataMap.RemoveAt(eSequenceID);
            m_kSeqDataMap.SetAt(eNewID, spSeqData);
        }
    }

    // Update callback data.
    unsigned int ui;
    for (ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData && pkCallbackData->m_eSequenceID == eOldID)
        {
            pkCallbackData->m_eSequenceID = eNewID;
        }
    }

    // Update extra sequences.
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eOldID);
    if (spExtraSeq != NULL)
    {
        m_kExtraSequenceMap.RemoveAt(eOldID);
        m_kExtraSequenceMap.SetAt(eNewID, spExtraSeq);
    }

    return eRC;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::ReloadNIFFile(NiStream* pkStream, bool bLoadNIFFile,
    NiPoseBinding* pkPoseBinding)
{
    if (!bLoadNIFFile && !pkStream)
    {
        return false;
    }

    // Create stream if not provided.
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    bool bSuccess = LoadNIFFile(*pkStream, bLoadNIFFile, pkPoseBinding);

    // Delete stream if created earlier.
    if (bDeleteStream)
    {
        NiDelete pkStream;
    }

    return bSuccess;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::ChangeNIFRoot(NiAVObject* pkNIFRoot,
    NiPoseBinding* pkPoseBinding)
{
    EE_ASSERT(m_spKFMTool);

    // Check for NIF related data
    bool bHaveNif = true;
    if (!pkNIFRoot)
    {
        bHaveNif = false;
    }

    // Get model root name.
    const char* pcModelRoot = m_spKFMTool->GetModelRoot();
    if (!pcModelRoot)
    {
        bHaveNif = false;
    }

    // Find the actor root.
    NiAVObject* pkActorRoot = bHaveNif ? pkNIFRoot->GetObjectByName(pcModelRoot) : NULL;
    if (!pkActorRoot)
    {
        bHaveNif = false;
    }

    // Turn off all sequences.
    Reset();

    // Create the controller manager.
    NiControllerManager* pkManager = NiNew NiControllerManager(pkActorRoot,
        m_bCumulativeAnimationsRequested, pkPoseBinding);

    // Copy all sequence datas from one manager to another.
    if (m_spManager)
    {
        unsigned int uiCount = m_spManager->GetSequenceDataCount();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            // Get sequence data: skip temporary poses.
            NiSequenceData* pkSeqData =
                m_spManager->GetSequenceDataAt(ui);
            if (!pkSeqData || pkSeqData->GetTempPose())
                continue;

            // Share original sequence data.
            pkManager->AddSequenceData(pkSeqData);
        }

        // Clean up previous controller manager.
        m_spManager->RemoveAllSequences();
        m_spManager->RemoveAllSequenceDatas();
    }

    m_spNIFRoot = pkNIFRoot;
    m_spManager = pkManager;

    return bHaveNif;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::LoadSequenceData(SequenceID eSequenceID,
    bool bLoadKFFile, NiStream* pkStream)
{
    if (!bLoadKFFile && !pkStream)
    {
        return false;
    }

    // Create stream if not provided.
    bool bDeleteStream = false;
    if (!pkStream)
    {
        pkStream = NiNew NiStream;
        bDeleteStream = true;
    }

    bool bSuccess = AddSequenceData(eSequenceID, *pkStream, bLoadKFFile);

    // Delete stream if created earlier.
    if (bDeleteStream)
    {
        NiDelete pkStream;
    }

    return bSuccess;
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::ChangeSequenceData(SequenceID eSequenceID,
    NiSequenceData* pkSeqData)
{
    // Remove old sequence data mapped to this ID, if it exists.
    UnloadSequenceData(eSequenceID);

    bool bSuccess = m_spManager->AddSequenceData(pkSeqData);
    if (bSuccess)
        m_kSeqDataMap.SetAt(eSequenceID, pkSeqData);
    else
        m_kSeqDataMap.SetAt(eSequenceID, NULL);

    return bSuccess;
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::UnloadSequenceData(SequenceID eSequenceID)
{
    EE_ASSERT(m_spManager);

    NiSequenceData* pkSeqData = GetSequenceData(eSequenceID);
    if (!pkSeqData)
    {
        return;
    }

    // Deactivate any active instance.
    DeactivateSequence(eSequenceID, 0.0f);

    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);

    if (eSequenceID == m_eCurID || eSequenceID == m_eNextID ||
        eSequenceID == m_eTargetID || spExtraSeq != NULL)
    {
        Reset();
        m_spManager->RemoveAllInactiveSequences();
    }

    m_spManager->RemoveSequenceData(pkSeqData);
    m_kSeqDataMap.RemoveAt(eSequenceID);

    // m_kExtraSequenceMap is emptied by Reset, so the sequence need not be
    // removed from it here.
}

//--------------------------------------------------------------------------------------------------
bool NiActorManager::FindTimeForAnimationToCompleteTransition(
    unsigned int uiTransSrcID, unsigned int uiTransDesID,
    NiKFMTool::Transition* pkTransition, float fBeginFrame,
    float fDesiredTransitionFrame, NiActorManager::CompletionInfo* pkInfoOut)
{
    EE_ASSERT(pkInfoOut);

    EE_ASSERT(pkTransition);
    NiSequenceData* pkSrcSeqData = GetSequenceData(uiTransSrcID);
    NiSequenceData* pkDesSeqData = GetSequenceData(uiTransDesID);
    EE_ASSERT(pkSrcSeqData && pkDesSeqData);

    EE_ASSERT(fDesiredTransitionFrame >= 0.0);
    EE_ASSERT(fDesiredTransitionFrame >= fBeginFrame);

    switch (pkTransition->GetType())
    {
        case NiKFMTool::TYPE_BLEND:
        {
            bool bImmediateBlend;
            if (pkTransition->GetBlendPairs().GetSize() == 0)
            {
                bImmediateBlend = true;
            }
            else if (pkTransition->GetBlendPairs().GetSize() == 1 &&
                !pkTransition->GetBlendPairs().GetAt(0)->
                GetStartKey().Exists())
            {
                bImmediateBlend = true;
            }
            else
            {
                bImmediateBlend = false;
            }

            if (bImmediateBlend)
            {
                // Handle immediate blend.
                float fBeginToTransitionFrameTime =
                    fDesiredTransitionFrame - fBeginFrame;

                const char* pcImmediateOffsetTextKey =
                    NiAnimationConstants::GetStartTextKey();
                if (pkTransition->GetBlendPairs().GetSize() == 1)
                {
                    pcImmediateOffsetTextKey = pkTransition->GetBlendPairs()
                        .GetAt(0)->GetTargetKey();
                }

                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    pkDesSeqData->GetKeyTimeAt(pcImmediateOffsetTextKey);

                // TimeToCompleteTransition is in RealTime
                pkInfoOut->m_fTimeToCompleteTransition =
                    pkSrcSeqData->TimeDivFreq(fBeginToTransitionFrameTime) +
                    pkTransition->GetDuration();

                pkInfoOut->m_fFrameTransitionOccursInSrc =
                    fDesiredTransitionFrame;
            }
            else
            {
                // Handle delayed blend.

                // Given the time and minimal step time, determine what the
                // next blend pair will be and the time it'll take to reach
                // it.
                float fActualTransitionUnboundedFrame = 0;
                NiKFMTool::Transition::BlendPair* pkBlendPair =
                    GetNextBlendPair(uiTransSrcID, uiTransDesID,
                    pkTransition, fDesiredTransitionFrame,
                    fActualTransitionUnboundedFrame);
                pkInfoOut->m_pkBlendPair = pkBlendPair;

                if (pkBlendPair == NULL)
                {
                    // This could happen if the transition time was beyond
                    // the length of the sequence and the sequence was
                    // CLAMPED.

                    pkInfoOut->m_fFrameTransitionOccursInSrc =
                        pkSrcSeqData->GetDuration();

                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes = 0.0f;

                    pkInfoOut->m_fTimeToCompleteTransition =
                        pkSrcSeqData->TimeDivFreq(
                        fActualTransitionUnboundedFrame - fBeginFrame) +
                        pkTransition->GetDuration();
                }
                else
                {
                    const char* pcStartKey = pkBlendPair->GetStartKey();
                    if (pcStartKey == NULL)
                    {
                        pcStartKey = NiAnimationConstants::GetStartTextKey();
                    }

                    float fActualTransitionFrame =
                        pkSrcSeqData->GetKeyTimeAt(pcStartKey);
                    EE_ASSERT(fActualTransitionFrame !=
                        NiControllerSequence::INVALID_TIME);

                    pkInfoOut->m_fFrameTransitionOccursInSrc =
                        fActualTransitionFrame;

                    const char* pcTargetKey = pkBlendPair->GetTargetKey();
                    if (pcTargetKey == NULL)
                    {
                        pcTargetKey = NiAnimationConstants::GetStartTextKey();
                    }
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                        pkDesSeqData->GetKeyTimeAt(pcTargetKey);

                    pkInfoOut->m_fTimeToCompleteTransition =
                        pkSrcSeqData->TimeDivFreq(
                        fActualTransitionUnboundedFrame - fBeginFrame) +
                        pkTransition->GetDuration();
                }
            }

            break;
        }
        case NiKFMTool::TYPE_MORPH:
        {
            pkInfoOut->m_fFrameTransitionOccursInSrc =
                fDesiredTransitionFrame;
            pkInfoOut->m_fTimeToCompleteTransition =
                pkSrcSeqData->TimeDivFreq(fDesiredTransitionFrame -
                fBeginFrame) + pkTransition->GetDuration();

            float fDesiredTransitionTime =
                pkSrcSeqData->TimeDivFreq(fDesiredTransitionFrame);

            float fDestTransitionStartFrame =
                pkDesSeqData->FindCorrespondingMorphFrame(
                    pkSrcSeqData, fDesiredTransitionTime);

            float fPartialDestSeqFrameLength =
                pkDesSeqData->GetDuration()- fDestTransitionStartFrame;

            float fTranDuration = pkTransition->GetDuration();
            float fTranFrameDuration;

            if (fTranDuration == NiKFMTool::MAX_DURATION)
            {
                // The time of transition duration should never be
                // negative... This code is being pessimistic...
                // we should never hit this block.
                EE_ASSERT(fTranDuration != NiKFMTool::MAX_DURATION);
                fTranFrameDuration = pkDesSeqData->GetDuration() - fBeginFrame;
            }
            else
            {
                 fTranFrameDuration = pkDesSeqData->TimeMultFreq(
                     fTranDuration);
            }


            if (fTranFrameDuration > fPartialDestSeqFrameLength)
            {
                // We have to loop ... not as easy as a cross fade because
                // we can start anywhere in the destination sequence.
                float fDesFrameLength = pkDesSeqData->GetDuration();

                float fRemainingFrameDuration = fTranFrameDuration -
                    fPartialDestSeqFrameLength;

                float fCeiling =
                    ceilf(fRemainingFrameDuration / fDesFrameLength);

                float fTimeOfDestLoops = (fCeiling * fDesFrameLength);

                float fFinal = (fDesFrameLength - fDestTransitionStartFrame) +
                    fTimeOfDestLoops;
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fDesFrameLength - (fFinal - fTranFrameDuration);
            }
            else
            {
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fDestTransitionStartFrame + fTranFrameDuration;
            }

            break;
        }
        case NiKFMTool::TYPE_CROSSFADE:
        {
            pkInfoOut->m_fFrameTransitionOccursInSrc =
                fDesiredTransitionFrame;

            float fTranDuration = pkTransition->GetDuration();
            pkInfoOut->m_fTimeToCompleteTransition =
                pkSrcSeqData->TimeDivFreq(fDesiredTransitionFrame -
                fBeginFrame) + fTranDuration;

            // Assume that we always start cross fade at start of destination.
            // If a cross fade has a duration that is longer than the
            // destination sequence, then the destination sequence should loop
            // and our time to start in the destination will depend on where
            // the duration ends within the loop...
            float fDesFrameLength = pkDesSeqData->GetDuration();
            float fTranFrameDuration = pkDesSeqData->TimeMultFreq(
                fTranDuration);

            if (fTranFrameDuration > fDesFrameLength)
            {
                if (pkDesSeqData->GetCycleType() == NiTimeController::CLAMP)
                {
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                        fDesFrameLength;
                }
                else
                {
                    float fCeiling = ceilf(fTranFrameDuration /
                        fDesFrameLength);
                    float fTimeOfDestLoops = (fCeiling * fDesFrameLength);

                    // Note that this is an unbounded frame time
                    pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                        fDesFrameLength - (fTimeOfDestLoops -
                        fTranFrameDuration);
                }
            }
            else
            {
                pkInfoOut->m_fFrameInDestWhenTransitionCompletes =
                    fTranFrameDuration;
            }

            break;
        }
        case NiKFMTool::TYPE_CHAIN:
        {
            float fTotalTime = 0.0f;

            // Ensure that Chains exist
            unsigned int uiChainInfoSize =
                pkTransition->GetChainInfo().GetSize();

            EE_ASSERT(uiChainInfoSize > 0);

            // Get first chain and its transition information
            NiSequenceData* pkChainSrcSeqData = pkSrcSeqData;
            unsigned int uiSrcID = uiTransSrcID;
            NiKFMTool::Transition::ChainInfo* pkChainInfo;

            // Find completion info of first transition of chain
            float fSrcBeginFrame = 0.0f;
            ChainCompletionInfo* pkCCI;
            float fFrameDuration = fDesiredTransitionFrame;

            unsigned int uiIndex;
            for (uiIndex = 0; uiIndex < uiChainInfoSize; uiIndex++)
            {
                pkChainInfo = &pkTransition->GetChainInfo().GetAt(uiIndex);
                unsigned int uiDesID = pkChainInfo->GetSequenceID();
                NiSequenceData* pkChainDesSeqData = GetSequenceData(uiDesID);

                pkCCI =
                    FillChainComplete(uiSrcID, uiDesID, fFrameDuration,
                    fSrcBeginFrame, fTotalTime);

                pkInfoOut->m_kChainCompletionInfoSet.Add(pkCCI);

                // now make this chain the source...
                pkChainSrcSeqData = pkChainDesSeqData;
                uiSrcID = uiDesID;

                // Place Duration in unbounded frame time (a bit weird)
                fFrameDuration = pkChainInfo->GetDuration();
                if (fFrameDuration == NiKFMTool::MAX_DURATION)
                {
                    fFrameDuration = pkChainSrcSeqData->GetDuration() -
                        fSrcBeginFrame;
                }
                else
                {
                    // scale duration so that it is in frame time
                        fFrameDuration = pkChainDesSeqData->TimeMultFreq(
                            fFrameDuration);
                }
            }

            pkCCI = FillChainComplete(uiSrcID, uiTransDesID,
                fFrameDuration, fSrcBeginFrame, fTotalTime);
            pkInfoOut->m_kChainCompletionInfoSet.Add(pkCCI);

            pkChainSrcSeqData = GetSequenceData(uiSrcID);
            fTotalTime +=
                pkDesSeqData->GetDurationDivFreq() -
                pkChainSrcSeqData->TimeDivFreq(fSrcBeginFrame);
            pkInfoOut->m_fTimeForChainToComplete = fTotalTime;
            break;
        }
        default:
            EE_ASSERT(false);
            return false;
            break;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
float NiActorManager::GetNextAnimActivatedTime(SequenceID eSequenceID)
{
    // This function returns the next time that the indicated animation will
    // be fully activated (i.e., not being blended or morphed). If the
    // animation is already active or the next activation time cannot be
    // determined, INVALID_TIME is returned.

    if (eSequenceID == INVALID_SEQUENCE_ID)
        return INVALID_TIME;

    EE_ASSERT(m_fTime != INVALID_TIME);

    // Search for sequence in the extra sequences.
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);
    if (spExtraSeq != NULL)
    {
        // If sequence is inactive, remove it.
        if (spExtraSeq->GetState() == INACTIVE)
        {
            spExtraSeq->RemoveActivationCallback(this);
            m_kExtraSequenceMap.RemoveAt(eSequenceID);
            return INVALID_TIME;
        }

        // If sequence is easing in, return the time when the ease-in will
        // be complete.
        if (spExtraSeq->GetState() == EASEIN)
        {
            float fEndTime = spExtraSeq->GetEaseEndTime();
            if (fEndTime > m_fTime)
            {
                return fEndTime;
            }
            else
            {
                return INVALID_TIME;
            }
        }

        // Extra sequence is already activated and will not be activated
        // again in the foreseeable future.
        return INVALID_TIME;
    }

    if (eSequenceID == m_eCurID)
    {
        return INVALID_TIME;
    }

    if (eSequenceID == m_eNextID)
    {
        return m_fTransEndTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        !m_kChainIDs.FindPos(eSequenceID))
    {
        return INVALID_TIME;
    }

    float fAnimActivatedTime = m_fTransEndTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(m_eCurID,
        m_eNextID);
    EE_ASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }
    CompletionInfo kInfo;
    EE_VERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));

    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = m_spNextSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_spNextSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        EE_ASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    SequenceID eSrcID = m_eNextID;
    NiSequenceData* pkSrcSeqData = GetSequenceData(m_eNextID);
    for (;;)
    {
        if (eSequenceID == eSrcID)
        {
            return fAnimActivatedTime;
        }

        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        EE_ASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiSequenceData* pkChainSeqData = GetSequenceData(eChainID);
        EE_ASSERT(pkChainSeqData);

        NiKFMTool::Transition* pkToolTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        EE_ASSERT(pkToolTransition &&
            pkToolTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        EE_VERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkToolTransition, fStartFrame, fTransitionFrame, &kInfo));
        fAnimActivatedTime += kInfo.m_fTimeToCompleteTransition;

        eSrcID = eChainID;
        pkSrcSeqData = pkChainSeqData;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = pkChainSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
    }
}

//--------------------------------------------------------------------------------------------------
float NiActorManager::GetNextAnimDeactivatedTime(SequenceID eSequenceID)
{
    // This function returns the next time that the indicated animation will
    // be fully deactivated (i.e., not being blended or morphed). If the next
    // deactivation time cannot be determined, INVALID_TIME is returned.

    if (eSequenceID == INVALID_SEQUENCE_ID)
        return INVALID_TIME;

    EE_ASSERT(m_fTime != INVALID_TIME);

    // Search for sequence in the extra sequences.
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);
    if (spExtraSeq != NULL)
    {
        // If sequence is inactive, remove it.
        if (spExtraSeq->GetState() == INACTIVE)
        {
            spExtraSeq->RemoveActivationCallback(this);
            m_kExtraSequenceMap.RemoveAt(eSequenceID);
            return INVALID_TIME;
        }

        // If sequence is easing out, return the time when the ease-out will
        // be complete.
        if (spExtraSeq->GetState() == EASEOUT)
        {
            float fEndTime = spExtraSeq->GetEaseEndTime();
            if (fEndTime > m_fTime)
            {
                return fEndTime;
            }
            else
            {
                return INVALID_TIME;
            }
        }

        // Extra sequences are deactivated manually, so we cannot predict when
        // that will happen.
        return INVALID_TIME;
    }

    if (eSequenceID == m_eCurID)
    {
        if (m_eNextID == INVALID_SEQUENCE_ID)
        {
            return INVALID_TIME;
        }

        if (m_fTime > m_fTransStartTime)
        {
            return INVALID_TIME;
        }

        return m_fTransStartTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        (eSequenceID == m_eNextID && m_kChainIDs.IsEmpty()) ||
        !m_kChainIDs.FindPos(eSequenceID))
    {
        return INVALID_TIME;
    }

    float fAnimDeactivatedTime = m_fTransStartTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
        m_eCurID,m_eNextID);
    EE_ASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }

    CompletionInfo kInfo;
    EE_VERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));

    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    float fTransitionDuration = pkTransition->GetDuration();
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = m_spNextSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_spNextSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        EE_ASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    SequenceID eSrcID = m_eNextID;
    NiSequenceData* pkSrcSeqData = GetSequenceData(m_eNextID);
    for (;;)
    {
        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        EE_ASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiSequenceData* pkChainSeqData = GetSequenceData(eChainID);
        EE_ASSERT(pkChainSeqData);

        NiKFMTool::Transition* pkToolTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        EE_ASSERT(pkToolTransition &&
            pkToolTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        EE_VERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkToolTransition, fStartFrame, fTransitionFrame, &kInfo));
        fAnimDeactivatedTime += (kInfo.m_fFrameTransitionOccursInSrc -
            fStartFrame) + fTransitionDuration;

        if (eSequenceID == eSrcID)
        {
            return fAnimDeactivatedTime;
        }

        eSrcID = eChainID;
        pkSrcSeqData = pkChainSeqData;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = pkChainSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
        fTransitionDuration = pkToolTransition->GetDuration();
    }
}

//--------------------------------------------------------------------------------------------------
float NiActorManager::GetNextTextKeyEventTime(SequenceID eSequenceID,
    const NiFixedString& kTextKey)
{
    NiTextKeyMatch kMatchObject(kTextKey);
    return GetNextTextKeyEventTime(eSequenceID, &kMatchObject);
}

//--------------------------------------------------------------------------------------------------
float NiActorManager::GetNextTextKeyEventTime(SequenceID eSequenceID,
    NiTextKeyMatch* pkMatchObject)
{
    // This function returns the next time that the specified text key in the
    // indicated animation will occur. If the next text key event time cannot
    // be determined or the text key cannot be found, INVALID_TIME is
    // returned.

    if (eSequenceID == INVALID_SEQUENCE_ID)
        return INVALID_TIME;

    EE_ASSERT(pkMatchObject);
    EE_ASSERT(m_fTime != INVALID_TIME);

    float fTextKeyTime;

    // Search for sequence in the extra sequences.
    NiControllerSequencePtr spExtraSeq = GetExtraSequence(eSequenceID);
    if (spExtraSeq != NULL)
    {
        if (spExtraSeq->GetState() == INACTIVE)
        {
            spExtraSeq->RemoveActivationCallback(this);
            m_kExtraSequenceMap.RemoveAt(eSequenceID);
            return INVALID_TIME;
        }

        // Find next time of match object in extra sequence.
        fTextKeyTime = spExtraSeq->GetTimeAt(pkMatchObject, m_fTime);
        if (fTextKeyTime == NiControllerSequence::INVALID_TIME ||
            fTextKeyTime < m_fTime ||
            (spExtraSeq->GetState() == EASEOUT &&
            fTextKeyTime > spExtraSeq->GetEaseEndTime()))
        {
            return INVALID_TIME;
        }

        return fTextKeyTime;
    }

    if (eSequenceID == m_eCurID)
    {
        fTextKeyTime = m_spCurSequence->GetTimeAt(pkMatchObject, m_fTime);
        if (fTextKeyTime == NiControllerSequence::INVALID_TIME ||
            fTextKeyTime < m_fTime ||
            (fTextKeyTime == m_fTime &&
                m_spCurSequence->GetKeyTimeAt(pkMatchObject) ==
                m_spCurSequence->GetDuration()))
        {
            return INVALID_TIME;
        }

        if (m_eNextID != INVALID_SEQUENCE_ID &&
            fTextKeyTime > m_fTransStartTime)
        {
            return INVALID_TIME;
        }

        return fTextKeyTime;
    }

    if (m_eNextID == INVALID_SEQUENCE_ID ||
        (eSequenceID != m_eNextID && m_kChainIDs.IsEmpty()) ||
        (eSequenceID != m_eNextID && !m_kChainIDs.FindPos(eSequenceID)))
    {
        return INVALID_TIME;
    }

    fTextKeyTime = m_fTransEndTime;
    NiKFMTool::Transition* pkTransition = m_spKFMTool->GetTransition(
        m_eCurID, m_eNextID);
    EE_ASSERT(pkTransition && pkTransition->GetType() != NiKFMTool::TYPE_CHAIN);
    float fTransStartFrameTime = m_fTransStartFrameTime;
    if (fTransStartFrameTime == INVALID_TIME)
    {
        fTransStartFrameTime = 0.0f;
    }
    CompletionInfo kInfo;
    EE_VERIFY(FindTimeForAnimationToCompleteTransition(
        m_eCurID, m_eNextID, pkTransition, fTransStartFrameTime,
        fTransStartFrameTime, &kInfo));

    float fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    float fTransitionFrame = 0.0f;
    NiTListIterator posID = m_kChainIDs.GetHeadPos();
    NiTListIterator posDuration = m_kChainDurations.GetHeadPos();
    if (!m_kChainDurations.IsEmpty())
    {
        float fDuration = m_kChainDurations.GetHead();
        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = m_spNextSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = m_spNextSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;

        EE_ASSERT(posID && posDuration);
        posID = m_kChainIDs.GetNextPos(posID);
        posDuration = m_kChainDurations.GetNextPos(posDuration);
    }

    SequenceID eSrcID = m_eNextID;
    NiSequenceData* pkSrcSeqData = GetSequenceData(m_eNextID);
    for (;;)
    {
        if (eSequenceID == eSrcID)
        {
            float fTextKeyFrame = pkSrcSeqData->GetKeyTimeAt(pkMatchObject);
            if (fTextKeyFrame == NiControllerSequence::INVALID_TIME ||
                (posID && fTextKeyFrame > fTransitionFrame))
            {
                return INVALID_TIME;
            }

            float fDelayTime = fTextKeyFrame - fStartFrame;
            if (fDelayTime < 0.0f)
            {
                fDelayTime += pkSrcSeqData->GetDuration();
            }
            float fUnscaledTime = fDelayTime / pkSrcSeqData->GetFrequency();
            fTextKeyTime += fUnscaledTime;

            return fTextKeyTime;
        }

        if (!posID)
        {
            // This statement will break out of this loop when there are no
            // more intermediate animations.
            return INVALID_TIME;
        }
        EE_ASSERT(posDuration);

        SequenceID eChainID = m_kChainIDs.GetNext(posID);
        float fDuration = m_kChainDurations.GetNext(posDuration);
        NiSequenceData* pkChainSeqData = GetSequenceData(eChainID);
        EE_ASSERT(pkChainSeqData);

        NiKFMTool::Transition* pkToolTransition = m_spKFMTool->GetTransition(
            eSrcID, eChainID);
        EE_ASSERT(pkToolTransition &&
            pkToolTransition->GetType() != NiKFMTool::TYPE_CHAIN);
        EE_VERIFY(FindTimeForAnimationToCompleteTransition(eSrcID, eChainID,
            pkToolTransition, fStartFrame, fTransitionFrame, &kInfo));
        fTextKeyTime += kInfo.m_fTimeToCompleteTransition;

        eSrcID = eChainID;
        pkSrcSeqData = pkChainSeqData;
        fStartFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

        if (fDuration == NiKFMTool::MAX_DURATION)
        {
            fDuration = pkChainSeqData->GetDuration() - fStartFrame;
        }
        else
        {
            // scale duration so that it is in frame time
            fDuration = pkChainSeqData->TimeMultFreq(fDuration);
        }

        fTransitionFrame = fStartFrame + fDuration;
    }
}

//--------------------------------------------------------------------------------------------------
NiActorManager::ChainCompletionInfo* NiActorManager::FillChainComplete(
    unsigned int uiSrcID, unsigned int uiDesID,
    float fFrameDuration, float& fSrcBeginFrame, float& fTotalTime)
{
    NiSequenceData* pkSrcSeqData = GetSequenceData(uiSrcID);
    NiSequenceData* pkDesSeqData = GetSequenceData(uiDesID);

    // Get the Transition... call recurively
    NiKFMTool::Transition* pkCurTransition =
        m_spKFMTool->GetTransition(uiSrcID, uiDesID);
    EE_ASSERT(pkCurTransition);

    float fNewTransitionFrame;
    if (fFrameDuration == NiKFMTool::MAX_DURATION)
    {
        fNewTransitionFrame = pkSrcSeqData->GetDuration();
        if (fNewTransitionFrame < fSrcBeginFrame)
            fNewTransitionFrame = fSrcBeginFrame;
    }
    else
    {
        fNewTransitionFrame =
            fSrcBeginFrame + fFrameDuration;
    }

    CompletionInfo kInfo;
    EE_VERIFY(FindTimeForAnimationToCompleteTransition(uiSrcID, uiDesID,
        pkCurTransition, fSrcBeginFrame, fNewTransitionFrame, &kInfo));

    ChainCompletionInfo* pkCCI = NiNew ChainCompletionInfo;
    pkCCI->SetName(pkSrcSeqData->GetName());
    pkCCI->m_fSeqStart = fTotalTime;
    pkCCI->m_fInSeqBeginFrame = fSrcBeginFrame;

    fTotalTime += kInfo.m_fTimeToCompleteTransition;

    pkCCI->m_fTransStart = fTotalTime - pkCurTransition
        ->GetDuration();
    pkCCI->m_fTransEnd = fTotalTime;
    pkCCI->m_pkTransition = pkCurTransition;
    pkCCI->m_uiSrcID = uiSrcID;
    pkCCI->m_uiDesID = uiDesID;
    pkCCI->SetNextName(pkDesSeqData->GetName());

    fSrcBeginFrame = kInfo.m_fFrameInDestWhenTransitionCompletes;

    return pkCCI;
}

//--------------------------------------------------------------------------------------------------
NiKFMTool::Transition::BlendPair* NiActorManager::GetNextBlendPair(
    unsigned int uiTransSrcID, unsigned int,
    NiKFMTool::Transition* pkTransition, float fFrameTime,
    float& fActualUnboundedFrame)
{
    // This function finds the lowest blend pair after fFrameTime.

    if (pkTransition->GetType() != NiKFMTool::TYPE_BLEND ||
        pkTransition->GetBlendPairs().GetSize() == 0 ||
        !pkTransition->GetBlendPairs().GetAt(0)->GetStartKey().Exists())
    {
        return NULL;
    }

    NiSequenceData* pkSrcSeqData = GetSequenceData(uiTransSrcID);

    float fSrcFrameLength = pkSrcSeqData->GetDuration();

    EE_ASSERT(fFrameTime >= 0.0f);
    fActualUnboundedFrame = fFrameTime;
    if (fFrameTime > fSrcFrameLength)
    {
        if (pkSrcSeqData->GetCycleType() == NiTimeController::CLAMP)
        {
            return NULL;
        }
        else
        {
            // this can happen, for example with durations of chains...
            float fFloor = floorf(fFrameTime / fSrcFrameLength);
            float fTimeForLooping = (fFloor * fSrcFrameLength);
            fFrameTime = fFrameTime - fTimeForLooping;
        }
    }

    NiKFMTool::Transition::BlendPair* pkSmallestBP = NULL;
    float fSmallestBPFrameTime = NI_INFINITY;

    for (unsigned int ui = 0; ui < pkTransition->GetBlendPairs().GetSize();
        ui++)
    {
        NiKFMTool::Transition::BlendPair* pkBP = pkTransition
            ->GetBlendPairs().GetAt(ui);
        float fBPFrameTime = pkSrcSeqData->GetKeyTimeAt(pkBP->GetStartKey())
            - fFrameTime;
        if (fBPFrameTime < 0.0f)
        {
            fBPFrameTime += fSrcFrameLength;
        }

        if (fBPFrameTime < fSmallestBPFrameTime)
        {
            fSmallestBPFrameTime = fBPFrameTime;
            pkSmallestBP = pkBP;
        }
    }

    fActualUnboundedFrame += fSmallestBPFrameTime;

    EE_ASSERT(pkSmallestBP != NULL);
    return pkSmallestBP;
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::RaiseAnimActivatedEvents(SequenceID eEventSeqID,
    NiControllerSequence* pkSeq, float fEventTime)
{
    // This function is only called when no animation is active and an
    // an animation is activated. It catches a case that is not handled by
    // the standard event handling code.

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            if (pkCallbackData->m_eEventType == ANIM_ACTIVATED)
            {
                SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
                if (eSequenceID == ANY_SEQUENCE_ID)
                {
                    if (GetExtraSequence(eEventSeqID) != NULL)
                    {
                        m_pkCallbackObject->AnimActivated(this,
                            eEventSeqID, pkSeq, fEventTime);
                    }
                }
                else if (eSequenceID == eEventSeqID)
                {
                    m_pkCallbackObject->AnimActivated(this, eEventSeqID,
                        pkSeq, fEventTime);
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::RaiseAnimDeactivatedEvents(SequenceID eEventSeqID,
    NiControllerSequence* pkSeq, float fEventTime)
{
    // This function is only called when the last remaining animation is
    // deactivated on the manager. It catches a case that is not handled by
    // the standard event handling code.

    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            if (pkCallbackData->m_eEventType == ANIM_DEACTIVATED)
            {
                SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
                if (eSequenceID == ANY_SEQUENCE_ID)
                {
                    if (GetExtraSequence(eEventSeqID) != NULL)
                    {
                        m_pkCallbackObject->AnimDeactivated(this,
                            eEventSeqID, pkSeq, fEventTime);
                    }
                }
                else if (eSequenceID == eEventSeqID)
                {
                    m_pkCallbackObject->AnimDeactivated(this, eEventSeqID,
                        pkSeq, fEventTime);
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::RaiseAnimCompletedEvents(SequenceID eEventSeqID,
    NiControllerSequence* pkSeq, float fEventTime)
{
    for (unsigned int ui = 0; ui < m_kCallbacks.GetSize(); ui++)
    {
        CallbackData* pkCallbackData = m_kCallbacks.GetAt(ui);
        if (pkCallbackData)
        {
            if (pkCallbackData->m_eEventType == ANIM_COMPLETED)
            {
                SequenceID eSequenceID = pkCallbackData->m_eSequenceID;
                if (eSequenceID == ANY_SEQUENCE_ID)
                {
                    m_pkCallbackObject->AnimCompleted(this,
                        eEventSeqID, pkSeq, fEventTime);
                }
                else if (eSequenceID == eEventSeqID)
                {
                    m_pkCallbackObject->AnimCompleted(this, eEventSeqID,
                        pkSeq, fEventTime);
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::RemoveInactiveExtraSequences()
{
    NiTMapIterator pos_seq = m_kExtraSequenceMap.GetFirstPos();
    while (pos_seq)
    {
        SequenceID eSequenceID;
        NiControllerSequencePtr spSequence;
        m_kExtraSequenceMap.GetNext(pos_seq, eSequenceID, spSequence);
        if (spSequence->GetState() == INACTIVE)
        {
            spSequence->RemoveActivationCallback(this);
            m_kExtraSequenceMap.RemoveAt(eSequenceID);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NiActorManager::ActivationChanged(NiControllerSequence* pkSequence,
    NiAnimState eState)
{
    // Check if the sequence is now inactive (i.e. it has completed).
    if (eState == INACTIVE)
    {
        bool bExtraSequence = true;
        SequenceID eSequenceID = GetSequenceID(pkSequence, true, false);
        if (eSequenceID == INVALID_SEQUENCE_ID)
        {
            bExtraSequence = false;
            eSequenceID = GetSequenceID(pkSequence, false, true);
        }
        if (eSequenceID != INVALID_SEQUENCE_ID)
        {
            // Check callback list.
            unsigned int uiNumCallbacks = m_kCallbacks.GetSize();
            for (unsigned int ui = 0; ui < uiNumCallbacks; ui++)
            {
                CallbackData* pkData = m_kCallbacks.GetAt(ui);
                if (pkData)
                {
                    if (pkData->m_eEventType == ANIM_COMPLETED &&
                        (pkData->m_eSequenceID == eSequenceID ||
                         pkData->m_eSequenceID == ANY_SEQUENCE_ID))
                    {
                        // The sequence has completed. Raise any
                        // AnimCompleted events for the sequence.
                        RaiseAnimCompletedEvents(eSequenceID,
                            pkSequence, m_fTime);
                        break;
                    }
                }
            }

            // Remove activation callback.
            pkSequence->RemoveActivationCallback(this);

            // Remove extra sequence.
            if (bExtraSequence)
            {
                m_kExtraSequenceMap.RemoveAt(eSequenceID);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
