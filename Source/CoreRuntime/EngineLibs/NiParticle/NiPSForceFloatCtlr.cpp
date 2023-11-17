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
#include "NiParticlePCH.h"

#include "NiPSForceFloatCtlr.h"
#include <NiFloatInterpolator.h>
#include <NiConstFloatEvaluator.h>
#include <NiBlendFloatInterpolator.h>

NiImplementRTTI(NiPSForceFloatCtlr, NiPSForceCtlr);

//--------------------------------------------------------------------------------------------------
NiPSForceFloatCtlr::NiPSForceFloatCtlr(const NiFixedString& kForceName) :
    NiPSForceCtlr(kForceName)
{
}

//--------------------------------------------------------------------------------------------------
NiPSForceFloatCtlr::NiPSForceFloatCtlr()
{
}

//--------------------------------------------------------------------------------------------------
NiEvaluator* NiPSForceFloatCtlr::CreatePoseEvaluator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiConstFloatEvaluator(fValue);
}

//--------------------------------------------------------------------------------------------------
NiInterpolator* NiPSForceFloatCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiFloatInterpolator(fValue);
}

//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp,
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    NiFloatInterpolator* pkFloatInterp =
        NiDynamicCast(NiFloatInterpolator, pkInterp);

    EE_ASSERT(usIndex == 0);
    EE_ASSERT(pkFloatInterp);
    float fValue;
    GetTargetFloatValue(fValue);
    pkFloatInterp->SetPoseValue(fValue);
}

//--------------------------------------------------------------------------------------------------
NiBlendInterpolator* NiPSForceFloatCtlr::CreateBlendInterpolator(
    unsigned short usIndex,
    bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return NiNew NiBlendFloatInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}

//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::Update(float fTime)
{
    if (GetManagerControlled())
    {
        return;
    }

    if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        float fValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, fValue))
        {
            SetTargetFloatValue(fValue);
        }
    }
}

//--------------------------------------------------------------------------------------------------
bool NiPSForceFloatCtlr::UpdateValue(float fTime, float fFloat,
    unsigned short usIndex)
{
    EE_UNUSED_ARG(fTime);
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(GetManagerControlled());
    SetTargetFloatValue(fFloat);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiPSForceFloatCtlr::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator,
    unsigned short usIndex) const
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return pkInterpolator->IsFloatValueSupported();
}

//--------------------------------------------------------------------------------------------------
// Cloning
//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::CopyMembers(NiPSForceFloatCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceCtlr::CopyMembers(pkDest, kCloning);
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceCtlr::LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceCtlr::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSForceFloatCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceCtlr::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceCtlr::SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSForceFloatCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceCtlr::IsEqual(pkObject);
}

//--------------------------------------------------------------------------------------------------
// Viewer strings
//--------------------------------------------------------------------------------------------------
void NiPSForceFloatCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForceCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSForceFloatCtlr::ms_RTTI.GetName()));
}

//--------------------------------------------------------------------------------------------------
