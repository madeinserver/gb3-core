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

#include "NiConstPoint3Evaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiConstPoint3Evaluator, NiEvaluator);

//--------------------------------------------------------------------------------------------------
NiConstPoint3Evaluator::NiConstPoint3Evaluator() :
    m_kPoint3Value(INVALID_POINT3)
{
    // No need to adjust the eval channel types.
}

//--------------------------------------------------------------------------------------------------
NiConstPoint3Evaluator::NiConstPoint3Evaluator(const NiPoint3& kPoseValue) :
    m_kPoint3Value(kPoseValue)
{
    SetEvalChannelTypes();
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::GetChannelPosedValue(unsigned int uiChannel,
    void* pvResult) const
{
    EE_ASSERT(uiChannel == POINT3);
    EE_ASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        EE_ASSERT(!IsEvalChannelInvalid(uiChannel));
        *(NiPoint3*)pvResult = m_kPoint3Value;
        return true;
    }

    EE_ASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::UpdateChannel(float, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    EE_ASSERT(uiChannel == POINT3);
    EE_ASSERT(pkEvalSPData);
    EE_ASSERT(pkEvalSPData->GetEvaluator() == this);
    EE_ASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    EE_ASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        EE_ASSERT(!IsEvalChannelInvalid(uiChannel));
        NiScratchPad::CubicPoint3SegmentData* pkCubicSeg =
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        EE_ASSERT(pkCubicSeg);
        *(NiPoint3*)pvResult = pkCubicSeg->m_kValue0;
        EE_ASSERT(pkCubicSeg->m_kOutTangent0 == NiPoint3::ZERO);
        EE_ASSERT(pkCubicSeg->m_kA0 == NiPoint3::ZERO);
        EE_ASSERT(pkCubicSeg->m_kB0 == NiPoint3::ZERO);
        return true;
    }

    EE_ASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::GetChannelScratchPadInfo(unsigned int uiChannel,
    bool bForceAlwaysUpdate, NiAVObjectPalette*,
    unsigned int& uiFillSize, bool& bSharedFillData,
    NiScratchPadBlock& eSPBSegmentData,
    NiBSplineBasisData*& pkBasisData) const
{
    EE_ASSERT(uiChannel == POINT3);

    if (IsEvalChannelInvalid(uiChannel) ||
        (!bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = 0;
    bSharedFillData = false;
    eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
    pkBasisData = 0;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::InitChannelScratchPadData(
    unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*,
    bool, NiAVObjectPalette*,
    NiPoseBufferHandle kPBHandle) const
{
    EE_UNUSED_ARG(uiChannel);
    EE_ASSERT(uiChannel == POINT3);
    EE_ASSERT(pkEvalSPData);
    EE_ASSERT(pkEvalSPData->GetEvaluator() == this);
    EE_ASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    EE_ASSERT(!IsEvalChannelInvalid(uiChannel));

    // Mark the segment data as valid for all sequence times.
    pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
    EE_ASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

    // Initialize the scratch pad segment data.
    NiScratchPad::CubicPoint3SegmentData* pkCubicSeg =
        (NiScratchPad::CubicPoint3SegmentData*)
        pkEvalSPData->GetSPSegmentData();
    EE_ASSERT(pkCubicSeg);
    pkCubicSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
    if (IsReferencedEvaluator())
    {
        EE_ASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
        pkCubicSeg->m_kHeader.m_usOutputIndex =
            NiScratchPad::INVALIDOUTPUTINDEX;
    }
    else
    {
        EE_ASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
        pkCubicSeg->m_kHeader.m_usOutputIndex =
            kPBHandle.GetChannelIndex();
    }
    pkCubicSeg->m_fStartTime = 0.0f;
    pkCubicSeg->m_fInvDeltaTime = 0.0f;
    pkCubicSeg->m_kValue0 = m_kPoint3Value;
    pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
    pkCubicSeg->m_kA0 = NiPoint3::ZERO;
    pkCubicSeg->m_kB0 = NiPoint3::ZERO;

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::SetEvalChannelTypes()
{
    if (m_kPoint3Value != INVALID_POINT3)
    {
        m_aiEvalChannelTypes[POINT3] = EVALPOINT3CHANNEL | EVALPOSEDFLAG;
    }
    else
    {
        m_aiEvalChannelTypes[POINT3] = EVALINVALIDCHANNEL;
    }
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Cloning
//--------------------------------------------------------------------------------------------------
NiImplementCreateClone(NiConstPoint3Evaluator);

//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::CopyMembers(NiConstPoint3Evaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kPoint3Value = m_kPoint3Value;
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiConstPoint3Evaluator);

//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    m_kPoint3Value.LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    m_kPoint3Value.SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiConstPoint3Evaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiConstPoint3Evaluator* pkDest = (NiConstPoint3Evaluator*)pkObject;

    if (m_kPoint3Value != pkDest->m_kPoint3Value)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Viewer strings
//--------------------------------------------------------------------------------------------------
void NiConstPoint3Evaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiConstPoint3Evaluator::ms_RTTI.GetName()));

    pkStrings->Add(m_kPoint3Value.GetViewerString("m_kPoint3Value"));
}

//--------------------------------------------------------------------------------------------------
