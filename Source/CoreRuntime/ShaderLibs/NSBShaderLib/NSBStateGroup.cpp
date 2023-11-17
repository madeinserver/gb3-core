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
// Precompiled Header
#include "NSBShaderLibPCH.h"

#include <NiShaderFactory.h>

#include "NSBRenderStates.h"
#include "NSBStateGroup.h"
#include "NSBUtility.h"

#if defined(NIDEBUG)
#include "NSBStageAndSamplerStates.h"
#endif  //#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::NSBSGEntry::SetAttribute(const char* pcAttribute)
{
    NSBUtility::SetString(m_pcAttribute, 0, pcAttribute);
}

//--------------------------------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::SaveBinary(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiState);
    NiStreamSaveBinary(kStream, m_uiValue);
    kStream.WriteCString(m_pcAttribute);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBStateGroup::NSBSGEntry::LoadBinary(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiState);
    NiStreamLoadBinary(kStream, m_uiValue);
    m_pcAttribute = kStream.ReadCString();

    return true;
}

//--------------------------------------------------------------------------------------------------
NSBStateGroup::NSBStateGroup()
{
    m_kStateList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSBStateGroup::~NSBStateGroup()
{
    // Clean up the list
    NSBSGEntry* pkEntry;

    NiTListIterator kIter = m_kStateList.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kStateList.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kStateList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::SetState(unsigned int uiState, unsigned int uiValue,
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        NILOG(NIMESSAGE_GENERAL_1,
            "Warning: NSBStateGroup::SetState> Overwriting "
            " State 0x%08x - original value 0x%08x - NiNew value 0x%08x\n",
            uiState, pkEntry->GetValue(), uiValue);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
            true, "* WARNING: NSBStateGroup::SetState\n"
            "    Overwriting State 0x%08x\n"
            "    Original Value = 0x%08x\n"
            "    New Value      = 0x%08x\n",
            uiState, pkEntry->GetValue(), uiValue);

        pkEntry->SetValue(uiValue);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(false);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBSGEntry();
    EE_ASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetValue(uiValue);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(false);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::SetState(unsigned int uiState, const char* pcAttribute,
    bool bSave, bool bUseMapValue)
{
    NSBSGEntry* pkEntry = FindStateInList(uiState);
    if (pkEntry)
    {
        // We may want to generate a warning about this.
        pkEntry->SetAttribute(pcAttribute);
        pkEntry->SetSaved(bSave);
        pkEntry->SetUseAttribute(true);
        return;
    }

    // The entry didn't exist, so create one and add it
    pkEntry = NiNew NSBSGEntry();
    EE_ASSERT(pkEntry);

    pkEntry->SetState(uiState);
    pkEntry->SetAttribute(pcAttribute);
    pkEntry->SetSaved(bSave);
    pkEntry->SetUseAttribute(true);
    pkEntry->SetUseMapValue(bUseMapValue);

    m_kStateList.AddTail(pkEntry);
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBStateGroup::GetStateCount()
{
    return m_kStateList.GetSize();
}

//--------------------------------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetFirstState(NiTListIterator& kIter)
{
    NSBSGEntry* pkEntry = 0;

    kIter = m_kStateList.GetHeadPos();
    if (kIter)
        pkEntry = m_kStateList.GetNext(kIter);

    return pkEntry;
}

//--------------------------------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::GetNextState(NiTListIterator& kIter)
{
    NSBSGEntry* pkEntry = 0;

    if (kIter)
        pkEntry = m_kStateList.GetNext(kIter);

    return pkEntry;
}

//--------------------------------------------------------------------------------------------------
bool NSBStateGroup::SaveBinary(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kStateList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NiTListIterator kIter = 0;
    NSBSGEntry* pkEntry = GetFirstState(kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextState(kIter);
    }

    if (uiTestCount != uiCount)
        return false;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBStateGroup::LoadBinary(efd::BinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBSGEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBSGEntry();
        EE_ASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kStateList.AddTail(pkEntry);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NSBStateGroup::NSBSGEntry* NSBStateGroup::FindStateInList(
    unsigned int uiState)
{
    NiTListIterator kIter = 0;
    NSBSGEntry* pkEntry = GetFirstState(kIter);
    while (pkEntry)
    {
        if (pkEntry->GetState() == uiState)
            return pkEntry;

        pkEntry = GetNextState(kIter);
    }

    return 0;
}

//--------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::Dump(FILE* pf, DumpMode eMode)
{
    NSBUtility::Dump(pf, true, "Entry Count = %d\n", GetStateCount());

    NSBUtility::IndentInsert();

    NiTListIterator kIter = 0;
    NSBSGEntry* pkEntry = GetFirstState(kIter);
    while (pkEntry)
    {
        if (eMode == DUMP_RENDERSTATES)
            DumpEntryAsRenderState(pf, pkEntry);
        else
        if (eMode == DUMP_STAGESTATES)
            DumpEntryAsStageState(pf, pkEntry);
        else
        if (eMode == DUMP_SAMPLERSTATES)
            DumpEntryAsSamplerState(pf, pkEntry);
        else
            DumpEntryAsUnknown(pf, pkEntry);

        pkEntry = GetNextState(kIter);
    }

    NSBUtility::IndentRemove();
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsUnknown(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    NSBUtility::Dump(pf, true, "0x%08x = 0x%08x %s\n",
        pkEntry->GetState(), pkEntry->GetValue(),
        pkEntry->IsSaved() ? "SAVED" : "");
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsRenderState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState = NSBRenderStates::LookupRenderStateString(
        (NSBRenderStates::NSBRenderStateEnum)pkEntry->GetState());
    NSBUtility::Dump(pf, true, "%32s = 0x%08x %s\n", pcState,
        pkEntry->GetValue(), pkEntry->IsSaved() ? "SAVED" : "");
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsStageState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState =
        NSBStageAndSamplerStates::LookupTextureStageString(
            (NSBStageAndSamplerStates::NSBTextureStageState)
            pkEntry->GetState());
    const char* pcValue =
        NSBStageAndSamplerStates::LookupTextureStageValueString(
            (NSBStageAndSamplerStates::NSBTextureStageState)pkEntry->
            GetState(), pkEntry->GetValue());

    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue,
        pkEntry->IsSaved() ? "SAVED" : "");
}

//--------------------------------------------------------------------------------------------------
void NSBStateGroup::DumpEntryAsSamplerState(FILE* pf, NSBSGEntry* pkEntry)
{
    if (!pkEntry)
        return;

    const char* pcState =
        NSBStageAndSamplerStates::LookupTextureSamplerString(
        (NSBStageAndSamplerStates::NSBTextureSamplerState)
            pkEntry->GetState());
    const char* pcValue =
        NSBStageAndSamplerStates::LookupTextureSamplerValueString(
            (NSBStageAndSamplerStates::NSBTextureSamplerState)pkEntry->
            GetState(), pkEntry->GetValue());

    NSBUtility::Dump(pf, true, "%32s = %32s %s\n", pcState, pcValue,
        pkEntry->IsSaved() ? "SAVED" : "");
}

//--------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
