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

//--------------------------------------------------------------------------------------------------
inline const NiFixedString& NiControllerSequence::GetName() const
{
    EE_ASSERT(m_spSeqData);
    return m_spSeqData->GetName();
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiControllerSequence::GetActivationID() const
{
    return m_uiActivationID;
}

//--------------------------------------------------------------------------------------------------
inline NiSequenceData* NiControllerSequence::GetSequenceData() const
{
    return m_spSeqData;
}

//--------------------------------------------------------------------------------------------------
inline NiTextKeyExtraData* NiControllerSequence::GetTextKeys() const
{
    return m_spTextKeys;
}

//--------------------------------------------------------------------------------------------------
inline NiControllerSequence* NiControllerSequence::GetTimeSyncSequence() const
{
    return m_spPartnerSequence;
}

//--------------------------------------------------------------------------------------------------
inline NiPoseBuffer* NiControllerSequence::GetPoseBuffer() const
{
    return m_spPoseBuffer;
}

//--------------------------------------------------------------------------------------------------
inline NiScratchPad* NiControllerSequence::GetScratchPad() const
{
    return m_spScratchPad;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::ResetSequence()
{
    m_fOffset = -NI_INFINITY;
}

//--------------------------------------------------------------------------------------------------
inline NiAnimState NiControllerSequence::GetState() const
{
    return m_eState;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::IsAdditiveBlend() const
{
    return m_spAdditiveRefFrame != NULL;
}

//--------------------------------------------------------------------------------------------------
inline NiPoseBuffer* NiControllerSequence::GetAdditiveRefFrame() const
{
    return m_spAdditiveRefFrame;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetOffset() const
{
    return m_fOffset;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::SetOffset(float fOffset)
{
    m_fOffset = fOffset;
}

//--------------------------------------------------------------------------------------------------
inline int NiControllerSequence::GetPriority() const
{
    return m_iPriority;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetWeight() const
{
    return m_fWeight;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::SetWeight(float fWeight)
{
    m_fWeight = NiMax(fWeight, 0.0f);
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetDuration() const
{
    return m_fDuration;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetDurationDivFreq() const
{
    return m_fDuration / m_fFrequency;
}

//--------------------------------------------------------------------------------------------------
inline NiTimeController::CycleType NiControllerSequence::GetCycleType()
    const
{
    return m_eCycleType;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::SetCycleType(
    NiTimeController::CycleType eCycleType)
{
    if (eCycleType == NiTimeController::CLAMP ||
        eCycleType == NiTimeController::LOOP)
    {
        m_eCycleType = eCycleType;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetFrequency() const
{
    return m_fFrequency;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::SetFrequency(float fFrequency)
{
    m_fFrequency = fFrequency;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::TimeDivFreq(float fTime) const
{
    return (fTime / m_fFrequency);
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::TimeMultFreq(float fTime) const
{
    return (fTime * m_fFrequency);
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetLastTime() const
{
    return m_fLastTime;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetLastScaledTime() const
{
    return m_fLastScaledTime;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetTimeAt(const NiFixedString& kTextKey,
    float fCurrentTime)
{
    NiTextKeyMatch kMatchObject(kTextKey);
    return GetTimeAt(&kMatchObject, fCurrentTime);
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetKeyTimeAt(
    const NiFixedString& kTextKey) const
{
    if (!kTextKey.Exists())
    {
        return INVALID_TIME;
    }
    else
    {
        NiTextKeyMatch kMatchObject(kTextKey);
        return GetKeyTimeAt(&kMatchObject);
    }
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiControllerSequence::GetNumEvaluators() const
{
    EE_ASSERT(m_spSeqData);
    return m_spSeqData->GetNumEvaluators();
}

//--------------------------------------------------------------------------------------------------
inline NiEvaluator* NiControllerSequence::GetEvaluatorAt(
    unsigned int uiIndex) const
{
    EE_ASSERT(uiIndex < GetNumEvaluators());
    EE_ASSERT(m_spSeqData);
    return m_spSeqData->GetEvaluatorAt(uiIndex);
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::AddActivationCallback(
    NiSequenceData::ActivationCallback* pkCallback)
{
    m_kActivationCallbacks.AddFirstEmpty(pkCallback);
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::RemoveActivationCallback(
    NiSequenceData::ActivationCallback* pkCallback)
{
    m_kActivationCallbacks.Remove(pkCallback);
    m_kActivationCallbacks.Compact();
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiControllerSequence::GetActivationCallbackCount() const
{
    return m_kActivationCallbacks.GetSize();
}

//--------------------------------------------------------------------------------------------------
inline NiSequenceData::ActivationCallback*
    NiControllerSequence::GetActivationCallbackAt(unsigned int uiIndex)
{
    return m_kActivationCallbacks.GetAt(uiIndex);
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::AddTextKeyCallback(
    NiControllerSequence::TextKeyCallback* pkCallback,
    NiTextKeyMatch* pkMatch)
{
    unsigned int uiIdx =
        m_kTextKeyCallbacks.Add(TextKeyCallbackData());

    m_kTextKeyCallbacks[uiIdx].m_pkCallback = pkCallback;
    m_kTextKeyCallbacks[uiIdx].m_spMatch = pkMatch;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::RemoveTextKeyCallback(
    const NiControllerSequence::TextKeyCallback* pkCallback,
    const NiTextKeyMatch* pkMatch)
{
    for (unsigned int i = 0; i < m_kTextKeyCallbacks.GetSize(); i++)
    {
        TextKeyCallbackData& kData = m_kTextKeyCallbacks.GetAt(i);
        if (kData.m_pkCallback == pkCallback &&
            kData.m_spMatch == pkMatch)
        {
            m_kTextKeyCallbacks.RemoveAtAndFill(i);
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::RemoveTextKeyCallback(
    const NiControllerSequence::TextKeyCallback* pkCallback,
    const NiFixedString& kTextKey)
{
    for (unsigned int i = 0; i < m_kTextKeyCallbacks.GetSize(); i++)
    {
        TextKeyCallbackData& kData = m_kTextKeyCallbacks.GetAt(i);
        if (kData.m_pkCallback == pkCallback &&
            kData.m_spMatch->IsKeyMatch(kTextKey))
        {
            m_kTextKeyCallbacks.RemoveAtAndFill(i);
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------
inline NiControllerManager* NiControllerSequence::GetOwner() const
{
    return m_pkOwner;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::SetOwner(NiControllerManager* pkOwner)
{
    if (pkOwner && m_pkOwner)
    {
        return false;
    }

    m_pkOwner = pkOwner;
    return true;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::SetSequenceData(
    NiSequenceData* pkSequenceData)
{
    if (pkSequenceData && m_spSeqData)
    {
        return false;
    }

    m_spSeqData = pkSequenceData;
    return true;
}

//--------------------------------------------------------------------------------------------------
inline NiControllerSequence* NiControllerSequence::StartBlend(
    NiSequenceData* pkDestSeqData,
    float fDuration, const char* pcDestTextKey, int iPriority,
    float fSourceWeight, float fDestWeight,
    NiControllerSequence* pkDestTimeSyncSeq, float fDestFrequency)
{
    // Determine the destination frame.
    float fDestFrame = 0.0f;
    if (pcDestTextKey)
    {
        // The blend is to some point in the animation - not necessarily
        // the start. We must first find where in the animation we will
        // be starting.
        float fKeyTime = pkDestSeqData->GetKeyTimeAt(pcDestTextKey);
        if (fKeyTime != INVALID_TIME)
        {
            fDestFrame = fKeyTime;
        }
    }

    return StartBlend(pkDestSeqData, fDuration, fDestFrame, iPriority,
        fSourceWeight, fDestWeight, pkDestTimeSyncSeq, fDestFrequency);
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetSpinnerScaledWeight() const
{
    return m_fEaseSpinner * m_fTransSpinner * m_fWeight;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetEaseSpinner() const
{
    return m_fEaseSpinner;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetEaseEndTime() const
{
    return m_fEndTime;
}

//--------------------------------------------------------------------------------------------------
inline float NiControllerSequence::GetDestFrame() const
{
    return m_fDestFrame;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::IsMoreImportantThan(
    const NiControllerSequence* pkSequence) const
{
    // This function returns true when this NiControllerSequence is more
    // important than pkSequence in terms of animation state, priority,
    // spinner values, and weight; otherwise, false is returned.

    EE_ASSERT(pkSequence);

    // Active sequences are more important than inactive sequences.
    if (m_eState == INACTIVE)
    {
        // All inactive sequences have the same level of importance.
        return false;
    }
    if (pkSequence->GetState() == INACTIVE)
    {
        return true;
    }

    // Additive sequences are always applied, so always take priority over non-additive sequences.
    if (IsAdditiveBlend() ^ pkSequence->IsAdditiveBlend())
    {
        return IsAdditiveBlend();
    }

    // A higher priority is more important.
    if (m_iPriority < pkSequence->GetPriority())
    {
        return false;
    }
    if (m_iPriority > pkSequence->GetPriority())
    {
        return true;
    }

    if (IsAdditiveBlend() && pkSequence->IsAdditiveBlend())
    {
        // Order of additive sequences matters, so provide a stable sort.
        // This may change as sequences are deactivated and reactivated,
        // but for a given set of active sequences, the sort order will
        // be deterministic.
        return GetActivationID() > pkSequence->GetActivationID();
    }

    // Both sequences have the same priority.

    // A higher ease spinner value is more important.
    if (m_fEaseSpinner < pkSequence->GetEaseSpinner())
    {
        return false;
    }
    if (m_fEaseSpinner > pkSequence->GetEaseSpinner())
    {
        return true;
    }

    // Both sequences have the same ease spinner.

    // A higher scaled weight (including spinner values) is more important.
    if (GetSpinnerScaledWeight() > pkSequence->GetSpinnerScaledWeight())
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::SetLastAccumTransform(
    const NiQuatTransform& kTransform)
{
    EE_ASSERT(m_spScratchPad);
    m_spScratchPad->SetLastAccumTransform(kTransform);
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::ClearAccumTransformData()
{
    EE_ASSERT(m_spScratchPad);
    m_spScratchPad->ClearAccumTransformData();
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::VerifyDependencies(
    NiControllerSequence* pkSequence)
{
    NiControllerSequence* pkPartnerSequence = pkSequence->m_spPartnerSequence;
    if (pkPartnerSequence)
    {
        if (pkPartnerSequence == this)
        {
            return false;
        }
        else
        {
            return VerifyDependencies(pkPartnerSequence);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
inline NiControllerSequence::TextKeyCallbackData::TextKeyCallbackData(int)
{
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::TextKeyCallbackData::operator==(
    const TextKeyCallbackData& kRHS) const
{
    return kRHS.m_pkCallback == m_pkCallback && kRHS.m_spMatch == m_spMatch;
}

//--------------------------------------------------------------------------------------------------
inline bool NiControllerSequence::TextKeyCallbackData::operator!=(
    const TextKeyCallbackData& kRHS) const
{
    return kRHS.m_pkCallback != m_pkCallback || kRHS.m_spMatch != m_spMatch;
}

//--------------------------------------------------------------------------------------------------
inline void NiControllerSequence::Unpause(float fTime)
{
    float fAdjust = fTime - m_fLastUpdateTime;

    m_fLastUpdateTime = fTime;

    if (m_fOffset == -NI_INFINITY)
        return;

    m_fOffset -= fAdjust;
    m_fStartTime += fAdjust;
    m_fEndTime += fAdjust;
}

//--------------------------------------------------------------------------------------------------