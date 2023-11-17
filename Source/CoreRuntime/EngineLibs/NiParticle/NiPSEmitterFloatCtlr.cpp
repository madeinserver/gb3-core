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

#include "NiPSEmitterFloatCtlr.h"
#include <NiFloatInterpolator.h>
#include <NiConstFloatEvaluator.h>
#include <NiBlendFloatInterpolator.h>

NiImplementRTTI(NiPSEmitterFloatCtlr, NiPSEmitterCtlr);

//--------------------------------------------------------------------------------------------------
NiPSEmitterFloatCtlr::NiPSEmitterFloatCtlr(const NiFixedString& kEmitterName) :
    NiPSEmitterCtlr(kEmitterName)
{
}

//--------------------------------------------------------------------------------------------------
NiPSEmitterFloatCtlr::NiPSEmitterFloatCtlr()
{
}

//--------------------------------------------------------------------------------------------------
NiEvaluator* NiPSEmitterFloatCtlr::CreatePoseEvaluator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiConstFloatEvaluator(fValue);
}

//--------------------------------------------------------------------------------------------------
NiInterpolator* NiPSEmitterFloatCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiFloatInterpolator(fValue);
}

//--------------------------------------------------------------------------------------------------
void NiPSEmitterFloatCtlr::SynchronizePoseInterpolator(
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
NiBlendInterpolator* NiPSEmitterFloatCtlr::CreateBlendInterpolator(
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
void NiPSEmitterFloatCtlr::Update(float fTime)
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
bool NiPSEmitterFloatCtlr::UpdateValue(float fTime, float fFloat,
    unsigned short usIndex)
{
    EE_UNUSED_ARG(fTime);
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(GetManagerControlled());
    SetTargetFloatValue(fFloat);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiPSEmitterFloatCtlr::InterpolatorIsCorrectType(
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
void NiPSEmitterFloatCtlr::CopyMembers(NiPSEmitterFloatCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterCtlr::CopyMembers(pkDest, kCloning);
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
void NiPSEmitterFloatCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterCtlr::LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSEmitterFloatCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterCtlr::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSEmitterFloatCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterCtlr::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSEmitterFloatCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterCtlr::SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSEmitterFloatCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterCtlr::IsEqual(pkObject);
}

//--------------------------------------------------------------------------------------------------
// Viewer strings
//--------------------------------------------------------------------------------------------------
void NiPSEmitterFloatCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSEmitterCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterFloatCtlr::ms_RTTI.GetName()));
}

//--------------------------------------------------------------------------------------------------
