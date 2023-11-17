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

#include "NiFloatsExtraDataController.h"
#include <NiFloatsExtraData.h>
#include "NiBlendFloatInterpolator.h"
#include "NiFloatData.h"
#include "NiFloatInterpolator.h"
#include "NiConstFloatEvaluator.h"

NiImplementRTTI(NiFloatsExtraDataController, NiExtraDataController);

//------------------------------------------------------------------------------------------------
NiFloatsExtraDataController::NiFloatsExtraDataController(
    const NiFixedString& kFloatsExtraDataName, int iFloatsExtraDataIndex) :
    NiExtraDataController(kFloatsExtraDataName),
    m_iFloatsExtraDataIndex(iFloatsExtraDataIndex), m_kCtlrID(NULL)
{
}

//------------------------------------------------------------------------------------------------
NiFloatsExtraDataController::NiFloatsExtraDataController() :
    m_iFloatsExtraDataIndex(-1), m_kCtlrID(NULL)
{
}

//------------------------------------------------------------------------------------------------
NiFloatsExtraDataController::~NiFloatsExtraDataController()
{
}

//------------------------------------------------------------------------------------------------
NiEvaluator* NiFloatsExtraDataController::CreatePoseEvaluator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return NiNew NiConstFloatEvaluator(((NiFloatsExtraData*)(m_spExtraData.data()))->GetValue(
        m_iFloatsExtraDataIndex));
}

//------------------------------------------------------------------------------------------------
NiInterpolator* NiFloatsExtraDataController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return NiNew NiFloatInterpolator(((NiFloatsExtraData*)(m_spExtraData.data()))->GetValue(
        m_iFloatsExtraDataIndex));
}

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::SynchronizePoseInterpolator(NiInterpolator*
    pkInterp,
    unsigned short usIndex)
{
    EE_UNUSED_ARG(usIndex);
    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(NiFloatInterpolator, pkInterp);

    EE_ASSERT(usIndex == 0);
    EE_ASSERT(pkFloatInterp);
    pkFloatInterp->SetPoseValue(((NiFloatsExtraData*)(m_spExtraData.data()))->GetValue(
        m_iFloatsExtraDataIndex));
}

//------------------------------------------------------------------------------------------------
NiBlendInterpolator* NiFloatsExtraDataController::CreateBlendInterpolator(
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

//------------------------------------------------------------------------------------------------
const char* NiFloatsExtraDataController::GetCtlrID()
{
    if (m_kCtlrID.Exists())
        return m_kCtlrID;

    m_kCtlrID = NULL;
    const char* pcExtraDataName = GetExtraDataName();
    if (pcExtraDataName)
    {
        size_t iLength = strlen(pcExtraDataName) + 15;
        char* acString = NiAlloc(char,iLength);
        NiSprintf(acString, iLength, "%s[%d]", pcExtraDataName,
            m_iFloatsExtraDataIndex);
        m_kCtlrID = acString;
        NiFree(acString);
    }
    return m_kCtlrID;
}

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::Update(float fTime)
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
            if (m_spExtraData)
            {
                EE_ASSERT(NiIsKindOf(NiFloatsExtraData, m_spExtraData));
                ((NiFloatsExtraData*)(m_spExtraData.data()))->SetValue(
                    m_iFloatsExtraDataIndex,
                    fValue);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
bool NiFloatsExtraDataController::UpdateValue(float, float fFloat,
    unsigned short)
{
    EE_ASSERT(GetManagerControlled());
    if (m_spExtraData)
    {
        EE_ASSERT(NiIsKindOf(NiFloatsExtraData, m_spExtraData));
        ((NiFloatsExtraData*)(m_spExtraData.data()))->SetValue(m_iFloatsExtraDataIndex, fFloat);
    }
    return true;
}

//------------------------------------------------------------------------------------------------
bool NiFloatsExtraDataController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator,
    unsigned short usIndex) const
{
    EE_UNUSED_ARG(usIndex);
    EE_ASSERT(usIndex == 0);
    return pkInterpolator->IsFloatValueSupported();
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Cloning
//------------------------------------------------------------------------------------------------
NiImplementCreateClone(NiFloatsExtraDataController);

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::CopyMembers(
    NiFloatsExtraDataController* pkDest, NiCloningProcess& kCloning)
{
    NiExtraDataController::CopyMembers(pkDest, kCloning);

    pkDest->m_iFloatsExtraDataIndex = m_iFloatsExtraDataIndex;
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Streaming
//------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiFloatsExtraDataController);

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::LoadBinary(NiStream& kStream)
{
    NiExtraDataController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_iFloatsExtraDataIndex);
}

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::LinkObject(NiStream& kStream)
{
    NiExtraDataController::LinkObject(kStream);
}

//------------------------------------------------------------------------------------------------
bool NiFloatsExtraDataController::RegisterStreamables(NiStream& kStream)
{
    return NiExtraDataController::RegisterStreamables(kStream);
}

//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::SaveBinary(NiStream& kStream)
{
    NiExtraDataController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_iFloatsExtraDataIndex);
}

//------------------------------------------------------------------------------------------------
bool NiFloatsExtraDataController::IsEqual(NiObject* pkObject)
{
    if (!NiExtraDataController::IsEqual(pkObject))
    {
        return false;
    }

    NiFloatsExtraDataController* pkDest = (NiFloatsExtraDataController*)
        pkObject;

    if (m_iFloatsExtraDataIndex != pkDest->m_iFloatsExtraDataIndex)
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Viewer Strings
//------------------------------------------------------------------------------------------------
void NiFloatsExtraDataController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiExtraDataController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiFloatsExtraDataController::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_iFloatsExtraDataIndex",
        m_iFloatsExtraDataIndex));
}

//------------------------------------------------------------------------------------------------
