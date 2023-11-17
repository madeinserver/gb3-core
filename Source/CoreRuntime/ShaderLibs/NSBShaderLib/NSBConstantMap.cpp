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
//------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NSBShaderLibPCH.h"

#include "NSBConstantMap.h"
#include "NSBShader.h"
#include "NSBUtility.h"

#include <NiShaderDesc.h>
#include <NiGlobalConstantEntry.h>
#include <NiShaderFactory.h>

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry::~NSBCM_Entry()
{
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::NSBCM_Entry::SaveBinary(efd::BinaryStream& kStream)
{
    NiFixedString::SaveFixedStringAsCString(kStream, m_kKey);
    NiStreamSaveBinary(kStream, m_uiFlags);
    NiStreamSaveBinary(kStream, m_uiExtra);
    NiStreamSaveBinary(kStream, m_uiShaderRegister);
    NiStreamSaveBinary(kStream, m_uiRegisterCount);
    NiFixedString::SaveFixedStringAsCString(kStream, m_kVariableName);
    NiStreamSaveBinary(kStream, m_uiDataSize);
    NiStreamSaveBinary(kStream, m_uiDataStride);

    if (!NSBUtility::WriteData(kStream, m_pvDataSource, m_uiDataSize,
        GetComponentSize()))
        return false;
    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::NSBCM_Entry::LoadBinary(efd::BinaryStream& kStream)
{
    NiFixedString::LoadCStringAsFixedString(kStream, m_kKey);

    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiExtra);
    NiStreamLoadBinary(kStream, m_uiShaderRegister);
    NiStreamLoadBinary(kStream, m_uiRegisterCount);

    if (NSBShader::GetReadVersion() > 0x00010001)
    {
        // Version 1.2 added variable names
        NiFixedString::LoadCStringAsFixedString(kStream, m_kVariableName);
    }

    // Read in the actual data (used for CM_Constant and CM_Attribute)
    NiStreamLoadBinary(kStream, m_uiDataSize);
    NiStreamLoadBinary(kStream, m_uiDataStride);

    // Read in the data
    unsigned int uiDataSize = 0;
    if (!NSBUtility::AllocateAndReadData(kStream, m_pvDataSource,
        uiDataSize, GetComponentSize()))
    {
        return false;
    }
    EE_ASSERT(uiDataSize == m_uiDataSize);
    m_uiDataSize = uiDataSize;

    m_bOwnData = true;
    return true;
}

//------------------------------------------------------------------------------------------------
// Compute the fundamental size of an component in the entry
// (i.e., sizeof(float) for matrix4x4, not 16 or 16*sizeof(float))
unsigned int NSBConstantMap::NSBCM_Entry::GetComponentSize() const
{
    switch (GetAttributeType())
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        {
            if (m_uiDataSize > 0)
            {
                // Only warn about undefined types if there is actual data,
                // i.e., only when we cannot properly endian convert without
                // knowing type
                NILOG("Warning: NSB constant map contains entries with "
                    "undefined types.  Re-generating the NSB from the NSF may "
                    "resolve the problem.\n");
            }

            // CM_Constant entries are constrained by the NSF parser to be
            // floats, so we can still return a reasonable value for them
            if (IsConstant())
                return sizeof(float);
            else
                return 1;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        // unsigned ints
        return sizeof(unsigned int);
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        // floats
        return sizeof(float);
    default:
        // bytes or other data that makes no sense to swap
        return 1;
    }
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBConstantMap() :
    m_pcName(0),
    m_uiProgramType(NiGPUProgram::PROGRAM_MAX)
{
    m_kEntryList.RemoveAll();
    m_kEntryList_DX9.RemoveAll();
    m_kEntryList_Xenon.RemoveAll();
    m_kEntryList_PS3.RemoveAll();
    m_kEntryList_D3D10.RemoveAll();
    m_kEntryList_D3D11.RemoveAll();
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::~NSBConstantMap()
{
    NiFree(m_pcName);
    NSBCM_Entry* pkEntry;

    NiTListIterator kIter = m_kEntryList.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList.RemoveAll();

    kIter = m_kEntryList_DX9.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList_DX9.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_DX9.RemoveAll();

    kIter = m_kEntryList_Xenon.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList_Xenon.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_Xenon.RemoveAll();

    kIter = m_kEntryList_PS3.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList_PS3.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_PS3.RemoveAll();

    kIter = m_kEntryList_D3D10.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList_D3D10.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_D3D10.RemoveAll();

    kIter = m_kEntryList_D3D11.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList_D3D11.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList_D3D11.RemoveAll();
}

//------------------------------------------------------------------------------------------------
void NSBConstantMap::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//------------------------------------------------------------------------------------------------
unsigned int NSBConstantMap::GetGlobalEntryCount()
{
    return m_kEntryList.GetSize();
}

//------------------------------------------------------------------------------------------------
unsigned int NSBConstantMap::GetPlatformEntryCount(
    NiShader::Platform ePlatform)
{
    switch (ePlatform)
    {
    case NiShader::NISHADER_DX9:
        return m_kEntryList_DX9.GetSize();
    case NiShader::NISHADER_XENON:
        return m_kEntryList_Xenon.GetSize();
    case NiShader::NISHADER_PS3:
        return m_kEntryList_PS3.GetSize();
    case NiShader::NISHADER_D3D10:
        return m_kEntryList_D3D10.GetSize();
    case NiShader::NISHADER_D3D11:
        return m_kEntryList_D3D11.GetSize();
    default:
        return 0;
    }
}

//------------------------------------------------------------------------------------------------
unsigned int NSBConstantMap::GetTotalEntryCount()
{
    return GetGlobalEntryCount()
        + GetPlatformEntryCount(NiShader::NISHADER_PS3)
        + GetPlatformEntryCount(NiShader::NISHADER_DX9)
        + GetPlatformEntryCount(NiShader::NISHADER_XENON)
        + GetPlatformEntryCount(NiShader::NISHADER_D3D10)
        + GetPlatformEntryCount(NiShader::NISHADER_D3D11);
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddEntry(const char* pcKey, unsigned int uiFlags,
    unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
    const char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
    void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetEntryByKey(pcKey);
    if (pkEntry && pkEntry->GetFlags() == uiFlags
        && pkEntry->GetExtra() == uiExtra)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
            true, "* ERROR: NSBConstantMap::AddEntry\n"
            "    Failed to add entry %s\n"
            "    It already exists!\n",
            (const char*)pkEntry->GetKey());
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry(unsigned int uiPlatformFlags,
    const char* pcKey, unsigned int uiFlags, unsigned int uiExtra,
    unsigned int uiReg, unsigned int uiCount, const char* pcVariableName,
    unsigned int uiSize, unsigned int uiStride, void* pvSource,
    bool bCopyData)
{
    bool bResult = true;

    if (uiPlatformFlags & NiShader::NISHADER_DX9)
    {
        if (!AddPlatformSpecificEntry_DX9(pcKey, uiFlags, uiExtra, uiReg,
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_XENON)
    {
        if (!AddPlatformSpecificEntry_Xenon(pcKey, uiFlags, uiExtra, uiReg,
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_PS3)
    {
        if (!AddPlatformSpecificEntry_PS3(pcKey, uiFlags, uiExtra, uiReg,
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_D3D10)
    {
        if (!AddPlatformSpecificEntry_D3D10(pcKey, uiFlags, uiExtra, uiReg,
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }
    if (uiPlatformFlags & NiShader::NISHADER_D3D11)
    {
        if (!AddPlatformSpecificEntry_D3D11(pcKey, uiFlags, uiExtra, uiReg,
            uiCount, pcVariableName, uiSize, uiStride, pvSource, bCopyData))
        {
            bResult = false;
        }
    }

    return bResult;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetFirstEntry(NiTListIterator& kIter)
{
    NSBCM_Entry* pkEntry = 0;

    kIter = m_kEntryList.GetHeadPos();
    if (kIter)
        pkEntry = m_kEntryList.GetNext(kIter);
    return pkEntry;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetNextEntry(NiTListIterator& kIter)
{
    NSBCM_Entry* pkEntry = 0;

    if (kIter)
        pkEntry = m_kEntryList.GetNext(kIter);
    return pkEntry;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetFirstPlatformEntry(
    NiShader::Platform ePlatform, 
    NiTListIterator& kIter)
{
    NSBCM_Entry* pkEntry = 0;

    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkEntryList))
        return 0;

    kIter = pkEntryList->GetHeadPos();
    if (kIter)
        pkEntry = pkEntryList->GetNext(kIter);
    return pkEntry;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetNextPlatformEntry(
    NiShader::Platform ePlatform,
    NiTListIterator& kIter)
{
    NSBCM_Entry* pkEntry = 0;

    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkEntryList))
        return 0;

    if (kIter)
        pkEntry = pkEntryList->GetNext(kIter);
    return pkEntry;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetEntryByKey(const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;

    NiTListIterator kListIter = m_kEntryList.GetHeadPos();
    while (kListIter)
    {
        pkEntry = m_kEntryList.GetNext(kListIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey()== pcKey)
                return pkEntry;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetPlatformEntryByKey(
    NiShader::Platform ePlatform, const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;

    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkEntryList))
        return false;

    NiTListIterator kIter = pkEntryList->GetHeadPos();
    while (kIter)
    {
        pkEntry = pkEntryList->GetNext(kIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return pkEntry;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::GetEntryByIndex(unsigned int uiTargetIndex)
{
    NSBCM_Entry* pkEntry = 0;
    unsigned int uiIndex = 0;

    NiTListIterator kListIter = m_kEntryList.GetHeadPos();
    while (kListIter)
    {
        pkEntry = m_kEntryList.GetNext(kListIter);
        if (pkEntry && (uiIndex == uiTargetIndex))
        {
            return pkEntry;
        }
        uiIndex++;
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------
unsigned int NSBConstantMap::GetEntryIndexByKey(const char* pcKey)
{
    NSBCM_Entry* pkEntry = 0;
    unsigned int uiIndex = 0;

    NiTListIterator kListIter = m_kEntryList.GetHeadPos();
    while (kListIter)
    {
        pkEntry = m_kEntryList.GetNext(kListIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return uiIndex;
        }
        uiIndex++;
    }

    return 0xffffffff;
}

//------------------------------------------------------------------------------------------------
unsigned int NSBConstantMap::GetPlatformEntryIndexByKey(
    NiShader::Platform ePlatform, const char* pcKey)
{
    NiTPointerList<NSBCM_Entry*>* pkEntryList = 0;

    if (!GetPlatformListPointers(ePlatform, pkEntryList))
        return 0;

    NSBCM_Entry* pkEntry = 0;
    unsigned int uiIndex = 0;

    NiTListIterator kIter = pkEntryList->GetHeadPos();
    while (kIter)
    {
        pkEntry = pkEntryList->GetNext(kIter);
        if (pkEntry)
        {
            if (pkEntry->GetKey() == pcKey)
                return uiIndex;
        }
        uiIndex++;
    }

    return 0xffffffff;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiProgramType);
    if (!SaveBinaryEntries(kStream))
        return false;
    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::LoadBinary(efd::BinaryStream& kStream)
{
    // Version 3.1 adds names to constant maps
    if (NSBShader::GetReadVersion() >= 0x00030001)
    {
        m_pcName = kStream.ReadCString();
    }

    NiStreamLoadBinary(kStream, m_uiProgramType);
    if (!LoadBinaryEntries(kStream))
        return false;
    return true;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap::NSBCM_Entry* NSBConstantMap::CreateEntry(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
    if (pkEntry)
    {
        pkEntry->SetKey(pcKey);
        pkEntry->SetFlags(uiFlags);
        pkEntry->SetExtra(uiExtra);
        pkEntry->SetShaderRegister(uiReg);
        pkEntry->SetRegisterCount(uiCount);
        pkEntry->SetVariableName(pcVariableName);
        pkEntry->SetData(uiSize, uiStride, pvSource, bCopyData);
    }

    return pkEntry;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_DX9(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_DX9,
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_DX9.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_Xenon(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_XENON,
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_Xenon.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_PS3(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry = GetPlatformEntryByKey(NiShader::NISHADER_PS3,
        pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_PS3.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_D3D10(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry
        = GetPlatformEntryByKey(NiShader::NISHADER_D3D10, pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_D3D10.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::AddPlatformSpecificEntry_D3D11(const char* pcKey,
    unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
    unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
    unsigned int uiStride, void* pvSource, bool bCopyData)
{
    NSBCM_Entry* pkEntry
        = GetPlatformEntryByKey(NiShader::NISHADER_D3D11, pcKey);
    if (pkEntry)
    {
        // Once error tracking is complete, store this error
        return false;
    }

    pkEntry = CreateEntry(pcKey, uiFlags, uiExtra, uiReg, uiCount,
        pcVariableName, uiSize, uiStride, pvSource, bCopyData);
    if (!pkEntry)
    {
        return false;
    }

    m_kEntryList_D3D11.AddTail(pkEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::GetPlatformListPointers(NiShader::Platform ePlatform,
    NiTPointerList<NSBCM_Entry*>*& pkEntryList)
{
    pkEntryList = 0;

    switch (ePlatform)
    {
    case NiShader::NISHADER_DX9:
        pkEntryList = &m_kEntryList_DX9;
        return true;
    case NiShader::NISHADER_XENON:
        pkEntryList = &m_kEntryList_Xenon;
        return true;
    case NiShader::NISHADER_PS3:
        pkEntryList = &m_kEntryList_PS3;
        return true;
    case NiShader::NISHADER_D3D10:
        pkEntryList = &m_kEntryList_D3D10;
        return true;
    case NiShader::NISHADER_D3D11:
        pkEntryList = &m_kEntryList_D3D11;
        return true;
    default:
        return false;   // Invalid platform.
    }
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::SaveBinaryEntries(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kEntryList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NiTListIterator kIter = 0;
    NSBCM_Entry* pkEntry = GetFirstEntry(kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextEntry(kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    // Version 1.4 added support for platform-specific constant map entries.
    // DX9 comes first
    uiCount = m_kEntryList_DX9.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    kIter = 0;
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_DX9, kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_DX9, kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then Xenon
    uiCount = m_kEntryList_Xenon.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    kIter = 0;
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_XENON, kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_XENON, kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then PS3
    uiCount = m_kEntryList_PS3.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    kIter = 0;
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_PS3, kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_PS3, kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then D3D10.
    uiCount = m_kEntryList_D3D10.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    kIter = 0;
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_D3D10, kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_D3D10, kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    // Then D3D11.
    uiCount = m_kEntryList_D3D11.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    uiTestCount = 0;

    kIter = 0;
    pkEntry = GetFirstPlatformEntry(NiShader::NISHADER_D3D11, kIter);
    while (pkEntry)
    {
        if (!pkEntry->SaveBinary(kStream))
            return false;
        uiTestCount++;
        pkEntry = GetNextPlatformEntry(NiShader::NISHADER_D3D11, kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBConstantMap::LoadBinaryEntries(efd::BinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    unsigned int ui = 0;
    for (; ui < uiCount; ui++)
    {
        NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
        EE_ASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kEntryList.AddTail(pkEntry);
    }

    if (NSBShader::GetReadVersion() >= 0x00010004)
    {
        // Version 1.4 added support for platform-specific constant map
        // entries

        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            // Version 1.8 removed the DX8 support

            NiStreamLoadBinary(kStream, uiCount);

            NSBCM_Entry kEntry;
            for (ui = 0; ui < uiCount; ui++)
            {
                if (!kEntry.LoadBinary(kStream))
                    return false;
            }
        }

        // DX9 comes next
        NiStreamLoadBinary(kStream, uiCount);

        for (ui = 0; ui < uiCount; ui++)
        {
            NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
            EE_ASSERT(pkEntry);

            if (!pkEntry->LoadBinary(kStream))
                return false;

            m_kEntryList_DX9.AddTail(pkEntry);
        }

        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            // Version 1.8 removed the Xbox support

            NiStreamLoadBinary(kStream, uiCount);

            NSBCM_Entry kEntry;
            for (ui = 0; ui < uiCount; ui++)
            {
                if (!kEntry.LoadBinary(kStream))
                    return false;
            }
        }
        else //NSBShader::GetReadVersion() >= 0x00010008
        {
            // Version 1.8 added the Xenon support

            // Then Xenon
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                EE_ASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_Xenon.AddTail(pkEntry);
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Version 1.12 added PS3 support
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                EE_ASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_PS3.AddTail(pkEntry);
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Version 1.13 added D3D10 and geometry shader support.
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                EE_ASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_D3D10.AddTail(pkEntry);
            }
        }
        if (NSBShader::GetReadVersion() >= 0x00030000)
        {
            // Version 3.0 added D3D11 support.
            NiStreamLoadBinary(kStream, uiCount);

            for (ui = 0; ui < uiCount; ui++)
            {
                NSBCM_Entry* pkEntry = NiNew NSBCM_Entry();
                EE_ASSERT(pkEntry);

                if (!pkEntry->LoadBinary(kStream))
                    return false;

                m_kEntryList_D3D11.AddTail(pkEntry);
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
void NSBConstantMap::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Name %s\n", m_pcName);
    NSBUtility::Dump(pf, true, "ProgamType = %d\n", m_uiProgramType);
    NSBUtility::Dump(pf, true, "Entry Count = %d\n", m_kEntryList.GetSize());

    NSBUtility::IndentInsert();

    NiTListIterator kIter = 0;
    NSBCM_Entry* pkEntry = GetFirstEntry(kIter);
    while (pkEntry)
    {
        NSBUtility::Dump(pf, true, "%16s - ", (const char*)pkEntry->GetKey());
        if (pkEntry->IsConstant())
            NSBUtility::Dump(pf, false, "Constant  ");
        if (pkEntry->IsDefined())
            NSBUtility::Dump(pf, false, "Defined   ");
        if (pkEntry->IsAttribute())
            NSBUtility::Dump(pf, false, "Attribute ");

        if (pkEntry->IsBool())
            NSBUtility::Dump(pf, false, "BOOL    ");
        if (pkEntry->IsString())
            NSBUtility::Dump(pf, false, "STRING  ");
        if (pkEntry->IsUnsignedInt())
            NSBUtility::Dump(pf, false, "UINT    ");
        if (pkEntry->IsFloat())
            NSBUtility::Dump(pf, false, "FLOAT   ");
        if (pkEntry->IsPoint2())
            NSBUtility::Dump(pf, false, "POINT2  ");
        if (pkEntry->IsPoint3())
            NSBUtility::Dump(pf, false, "POINT3  ");
        if (pkEntry->IsPoint4())
            NSBUtility::Dump(pf, false, "POINT4  ");
        if (pkEntry->IsMatrix3())
            NSBUtility::Dump(pf, false, "MATRIX3 ");
        if (pkEntry->IsMatrix4())
            NSBUtility::Dump(pf, false, "MATRIX4 ");
        if (pkEntry->IsColor())
            NSBUtility::Dump(pf, false, "COLOR   ");
        if (pkEntry->IsTexture())
            NSBUtility::Dump(pf, false, "TEXTURE ");

        NSBUtility::Dump(pf, false, "Ex    = %3d ",
            pkEntry->GetExtra());
        NSBUtility::Dump(pf, false, "Reg   = %3d ",
            pkEntry->GetShaderRegister());
        NSBUtility::Dump(pf, false, "Count = %3d ",
            pkEntry->GetRegisterCount());
        NSBUtility::Dump(pf, false, "\n");

        pkEntry = GetNextEntry(kIter);
    }

    NSBUtility::IndentRemove();
}

//------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
