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

#include "NiFloatInterpController.h"
#include "NiBlendFloatInterpolator.h"
#include "NiFloatInterpolator.h"
#include "NiConstFloatEvaluator.h"
#include "NiFloatData.h"

NiImplementRTTI(NiFloatInterpController, NiSingleInterpController);

//--------------------------------------------------------------------------------------------------
NiFloatInterpController::NiFloatInterpController()
{
}

//--------------------------------------------------------------------------------------------------
NiEvaluator* NiFloatInterpController::CreatePoseEvaluator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiConstFloatEvaluator(fValue);
}

//--------------------------------------------------------------------------------------------------
NiInterpolator* NiFloatInterpController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiFloatInterpolator(fValue);
}

//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::SynchronizePoseInterpolator(NiInterpolator*
    pkInterp,
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
NiBlendInterpolator* NiFloatInterpController::CreateBlendInterpolator(
    unsigned short usIndex,
    bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return NiNew NiBlendFloatInterpolator(bManagerControlled,
        fWeightThreshold, ucArraySize);
}

//--------------------------------------------------------------------------------------------------
bool NiFloatInterpController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator,
    unsigned short usIndex) const
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return pkInterpolator->IsFloatValueSupported();
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Cloning
//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::CopyMembers(NiFloatInterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiFloatInterpController::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiFloatInterpController::IsEqual(NiObject* pkObject)
{
    return NiSingleInterpController::IsEqual(pkObject);
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Viewer strings
//--------------------------------------------------------------------------------------------------
void NiFloatInterpController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(
        NiGetViewerString(NiFloatInterpController::ms_RTTI.GetName()));
}

//--------------------------------------------------------------------------------------------------
