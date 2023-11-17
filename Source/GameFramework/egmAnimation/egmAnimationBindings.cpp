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

#include "egmAnimationBindings.h"
#include "AnimationService.h"

#include <egf/ScriptContext.h>

using namespace egf;
using namespace egmAnimation;

//--------------------------------------------------------------------------------------------------
void egmAnimationBindings::SetTargetAnimationByName(
    egf::EntityID entityID,
    const char* sequenceName)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    pAnimationService->SetTargetAnimation(entityID, sequenceName);
}
//--------------------------------------------------------------------------------------------------
void egmAnimationBindings::SetTargetAnimationById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    pAnimationService->SetTargetAnimation(entityID, sequenceID);
}
//--------------------------------------------------------------------------------------------------
const char* egmAnimationBindings::GetAnimationNameFromId(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetAnimationNameFromId(entityID, sequenceID).c_str();
}
//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID egmAnimationBindings::GetAnimationIdFromName(
    egf::EntityID entityID,
    const char* sequenceName)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetAnimationIdFromName(entityID, sequenceName);
}
//--------------------------------------------------------------------------------------------------
const char* egmAnimationBindings::GetCurrentAnimationName(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetCurrentAnimation(entityID).c_str();
}
//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID egmAnimationBindings::GetCurrentAnimationId(
    egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetCurrentAnimationId(entityID);
}
//--------------------------------------------------------------------------------------------------
const char* egmAnimationBindings::GetNextAnimationName(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetNextAnimation(entityID).c_str();
}
//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID egmAnimationBindings::GetNextAnimationId(
    egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetNextAnimationId(entityID);
}
//--------------------------------------------------------------------------------------------------
const char* egmAnimationBindings::GetTargetAnimationName(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetTargetAnimation(entityID).c_str();
}
//--------------------------------------------------------------------------------------------------
NiActorManager::SequenceID egmAnimationBindings::GetTargetAnimationId(
    egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetTargetAnimationId(entityID);
}
//--------------------------------------------------------------------------------------------------
egmAnimationBindings::TransitionState egmAnimationBindings::GetTransitionState(
    egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return (TransitionState)pAnimationService->GetTransitionState(entityID);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::ListenForTextKeyEvents(
    egf::EntityID entityID,
    const char* behaviorName,
    const char* textKeyName,
    const char* seqName)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->ListenForTextKeyEvents(entityID, behaviorName, textKeyName, seqName);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::ClearRegisteredTextKeys(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->ClearRegisteredTextKeys(entityID);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::ActivateLayeredSequenceById(
    egf::EntityID entityID,
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
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->ActivateLayeredSequenceById(
        entityID,
        sequenceID,
        autoDeactivate,
        priority,
        weight,
        easeInTime,
        easeOutTime,
        timeSyncID,
        freq,
        startFrame,
        additiveBlend,
        additiveRefFrame);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::ActivateLayeredSequence(
    egf::EntityID entityID,
    const char* sequenceName,
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
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID sequenceID =
        pAnimationService->GetAnimationIdFromName(entityID, sequenceName);
    return pAnimationService->ActivateLayeredSequenceById(
        entityID,
        sequenceID,
        autoDeactivate,
        priority,
        weight,
        easeInTime,
        easeOutTime,
        timeSyncID,
        freq,
        startFrame,
        additiveBlend,
        additiveRefFrame);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::SetSequenceFrequencyById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32 frequency)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->SetSequenceFrequencyById(entityID, sequenceID, frequency);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceFrequencyById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32& frequency)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetSequenceFrequencyById(entityID, sequenceID, frequency);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::SetSequenceFrequency(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32 frequency)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID id = pAnimationService->GetAnimationIdFromName(entityID,
        sequenceName);
    return pAnimationService->SetSequenceFrequencyById(entityID, id, frequency);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceFrequency(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32& frequency)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID id = pAnimationService->GetAnimationIdFromName(
        entityID,
        sequenceName);
    return pAnimationService->GetSequenceFrequencyById(entityID, id, frequency);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceDurationById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32& duration)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetSequenceDurationById(entityID, sequenceID, duration);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceDuration(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32& duration)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID id = pAnimationService->GetAnimationIdFromName(
        entityID,
        sequenceName);
    return pAnimationService->GetSequenceDurationById(entityID, id, duration);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::SetSequenceWeightById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32 weight)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->SetSequenceWeightById(entityID, sequenceID, weight);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceWeightById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32& weight)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->GetSequenceWeightById(entityID, sequenceID, weight);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::SetSequenceWeight(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32 weight)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID id = pAnimationService->GetAnimationIdFromName(entityID,
        sequenceName);
    return pAnimationService->SetSequenceWeightById(entityID, id, weight);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::GetSequenceWeight(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32& weight)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID sequenceID = pAnimationService->GetAnimationIdFromName(
        entityID,
        sequenceName);
    return pAnimationService->GetSequenceWeightById(entityID, sequenceID, weight);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::DeactivateLayeredSequenceById(
    egf::EntityID entityID,
    NiActorManager::SequenceID sequenceID,
    efd::Float32 easeOutTime)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->DeactivateLayeredSequenceById(entityID, sequenceID, easeOutTime);
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::DeactivateLayeredSequence(
    egf::EntityID entityID,
    const char* sequenceName,
    efd::Float32 easeOutTime)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    NiActorManager::SequenceID sequenceID =
        pAnimationService->GetAnimationIdFromName(entityID, sequenceName);
    return pAnimationService->DeactivateLayeredSequenceById(entityID, sequenceID, easeOutTime);
}
//--------------------------------------------------------------------------------------------------
efd::list<efd::utf8string> egmAnimationBindings::GetActiveSequences(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    efd::vector<efd::utf8string> sequences;
    pAnimationService->GetActiveSequences(entityID, sequences);

    efd::list<efd::utf8string> sequenceList(sequences.begin(), sequences.end());
    return sequenceList;
}
//--------------------------------------------------------------------------------------------------
efd::Bool egmAnimationBindings::IsPaused(egf::EntityID entityID)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    return pAnimationService->IsPaused(entityID);
}
//--------------------------------------------------------------------------------------------------
void egmAnimationBindings::SetPaused(egf::EntityID entityID, efd::Bool isPaused)
{
    AnimationService* pAnimationService = g_bapiContext.GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    pAnimationService->SetPaused(entityID, isPaused);
}
//--------------------------------------------------------------------------------------------------