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

#include "NSBUtility.h"
#include "NSBPackingDef.h"

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::NSBPDEntry::SaveBinary(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiStream);
    NiStreamSaveBinary(kStream, m_uiRegister);
    NiStreamSaveBinary(kStream, m_uiInput);

    unsigned int uiValue;
    uiValue = (unsigned int)m_eType;
    NiStreamSaveBinary(kStream, uiValue);
    uiValue = (unsigned int)m_eTesselator;
    NiStreamSaveBinary(kStream, uiValue);
    uiValue = (unsigned int)m_eUsage;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiUsageIndex);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::NSBPDEntry::LoadBinary(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiStream);
    NiStreamLoadBinary(kStream, m_uiRegister);
    NiStreamLoadBinary(kStream, m_uiInput);

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eType = (NSBPackingDefEnum)uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eTesselator =
        (NiShaderDeclaration::ShaderParameterTesselator)uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_eUsage = (NiShaderDeclaration::ShaderParameterUsage)uiValue;

    NiStreamLoadBinary(kStream, m_uiUsageIndex);

    return true;
}

//--------------------------------------------------------------------------------------------------
NSBPackingDef::NSBPackingDef() :
    m_pcName(0),
    m_bFixedFunction(false),
    m_bGenerated(false)
{
    m_kEntryList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSBPackingDef::~NSBPackingDef()
{
    NSBPDEntry* pkEntry;
    NiTListIterator kIter = m_kEntryList.GetHeadPos();
    while (kIter)
    {
        pkEntry = m_kEntryList.GetNext(kIter);
        if (pkEntry)
            NiDelete pkEntry;
    }
    m_kEntryList.RemoveAll();

    NiFree(m_pcName);
}

//--------------------------------------------------------------------------------------------------
const char* NSBPackingDef::GetName()
{
    return m_pcName;
}

//--------------------------------------------------------------------------------------------------
void NSBPackingDef::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::GetFixedFunction()
{
    return m_bFixedFunction;
}

//--------------------------------------------------------------------------------------------------
void NSBPackingDef::SetFixedFunction(bool bFixedFunction)
{
    m_bFixedFunction = bFixedFunction;
}

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::AddPackingEntry(unsigned int uiStream,
    unsigned int uiRegister, unsigned int uiInput, NSBPackingDefEnum eType,
    NiShaderDeclaration::ShaderParameterTesselator eTesselator,
    NiShaderDeclaration::ShaderParameterUsage eUsage,
    unsigned int uiUsageIndex)
{
    bool bCreatedEntry = false;

    // Check for the entry in the list...
    NiTListIterator kIter = 0;
    NSBPDEntry* pkEntry = GetFirstEntry(kIter);
    while (pkEntry)
    {
        if ((pkEntry->GetStream() == uiStream) &&
            (pkEntry->GetRegister() == uiRegister))
        {
            // Exists... just overwrite
            NILOG(NIMESSAGE_GENERAL_1,
                "WARNING> PackingDef %s - Entry stream %d "
                "register %d being written over!\n", m_pcName,
                uiStream, uiRegister);
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* WARNING: NSBPackingDef::AddPackingEntry\n"
                "    %s - overwriting stream %d, register %d\n",
                m_pcName, uiStream, uiRegister);
            break;
        }
        pkEntry = GetNextEntry(kIter);
    }

    if (pkEntry == 0)
    {
        pkEntry = NiNew NSBPDEntry();
        if (!pkEntry)
            return false;
        bCreatedEntry = true;
    }

    pkEntry->SetStream(uiStream);
    pkEntry->SetRegister(uiRegister);
    pkEntry->SetInput(uiInput);
    pkEntry->SetType(eType);
    pkEntry->SetTesselator(eTesselator);
    pkEntry->SetUsage(eUsage);
    pkEntry->SetUsageIndex(uiUsageIndex);

    if (bCreatedEntry)
        m_kEntryList.AddTail(pkEntry);

    return true;
}

//--------------------------------------------------------------------------------------------------
const char* NSBPackingDef::GetTypeName(
    NSBPackingDef::NSBPackingDefEnum eType)
{
    switch (eType)
    {
    case NSB_PD_FLOAT1:       return "Float1";
    case NSB_PD_FLOAT2:       return "Float2";
    case NSB_PD_FLOAT3:       return "Float3";
    case NSB_PD_FLOAT4:       return "Float4";
    case NSB_PD_UBYTECOLOR:   return "UByteColor";
    case NSB_PD_UBYTE4:       return "UByte4";
    case NSB_PD_SHORT1:       return "Short1";
    case NSB_PD_SHORT2:       return "Short2";
    case NSB_PD_SHORT3:       return "Short3";
    case NSB_PD_SHORT4:       return "Short4";
    case NSB_PD_NORMSHORT1:   return "NormShort1";
    case NSB_PD_NORMSHORT2:   return "NormShort2";
    case NSB_PD_NORMSHORT3:   return "NormShort3";
    case NSB_PD_NORMSHORT4:   return "NormShort4";
    case NSB_PD_NORMPACKED3:  return "NormPacked3";
    case NSB_PD_PBYTE1:       return "PByte1";
    case NSB_PD_PBYTE2:       return "PByte2";
    case NSB_PD_PBYTE3:       return "PByte3";
    case NSB_PD_PBYTE4:       return "PByte4";
    case NSB_PD_FLOAT2H:      return "Float2H";
    default:                    return "UNKNOWN";
    }
}

//--------------------------------------------------------------------------------------------------
const char* NSBPackingDef::GetParameterName(
    NiShaderDeclaration::ShaderParameter eParam)
{
    if ((eParam & NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK) != 0)
    {
        return "ExtraData";
    }

    switch (eParam)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
        return "NiPosition";
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
        return "NiBlendWeight";
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
        return "NiBlendIndices";
    case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
        return "NiNormal";
    case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
        return "NiColor";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        return "NiTexCoord0";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
        return "NiTexCoord1";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
        return "NiTexCoord2";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
        return "NiTexCoord3";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
        return "NiTexCoord4";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
        return "NiTexCoord5";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
        return "NiTexCoord6";
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
        return "NiTexCoord7";
    case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
        return "NiTangent";
    case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
        return "NiBinormal";
    default:
        return "UNKNOWN";
    }
}

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);

    unsigned int uiValue = m_bFixedFunction ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    unsigned int uiCount = m_kEntryList.GetSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NiTListIterator kIter = 0;
    NSBPDEntry* pkEntry = GetFirstEntry(kIter);
    while (pkEntry)
    {
        if (pkEntry->SaveBinary(kStream))
            uiTestCount++;
        pkEntry = GetNextEntry(kIter);
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBPackingDef::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_bFixedFunction = (uiValue != 0);

    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    NSBPDEntry* pkEntry;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkEntry = NiNew NSBPDEntry();
        EE_ASSERT(pkEntry);

        if (!pkEntry->LoadBinary(kStream))
            return false;

        m_kEntryList.AddTail(pkEntry);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NSBPackingDef::NSBPDEntry* NSBPackingDef::GetFirstEntry(NiTListIterator& kIter)
{
    NSBPDEntry* pkEntry = 0;

    kIter = m_kEntryList.GetHeadPos();
    if (kIter)
        pkEntry = m_kEntryList.GetNext(kIter);

    return pkEntry;
}

//--------------------------------------------------------------------------------------------------
NSBPackingDef::NSBPDEntry* NSBPackingDef::GetNextEntry(NiTListIterator& kIter)
{
    NSBPDEntry* pkEntry = 0;

    if (kIter)
        pkEntry = m_kEntryList.GetNext(kIter);

    return pkEntry;
}

//--------------------------------------------------------------------------------------------------
void NSBPackingDef::GetStreamInfo(unsigned int& uiStreamCount,
    unsigned int& uiMaxStreamEntryCount)
{
    // Should use the MAX_STREAM here, but this will suffice for Xenon
    // and DX9
    const unsigned int LOCAL_MAX_STREAM = 64;
    unsigned int auiStreamEntryCount[LOCAL_MAX_STREAM];

    uiStreamCount = 0;
    uiMaxStreamEntryCount = 0;

    unsigned int ui;

    // Initialize the 'seen' array
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
        auiStreamEntryCount[ui] = 0;

    // Now cycle through and count the streams
    NiTListIterator kIter = 0;
    NSBPDEntry* pkEntry = GetFirstEntry(kIter);
    while (pkEntry)
    {
        unsigned int uiStream = pkEntry->GetStream();
        // Safety catch!
        EE_ASSERT(uiStream < LOCAL_MAX_STREAM);

        if (auiStreamEntryCount[uiStream] == 0)
            uiStreamCount++;
        auiStreamEntryCount[uiStream] += 1;

        pkEntry = GetNextEntry(kIter);
    }

    // Now, determine the max entry count
    for (ui = 0; ui < LOCAL_MAX_STREAM; ui++)
    {
        if (auiStreamEntryCount[ui] > uiMaxStreamEntryCount)
            uiMaxStreamEntryCount = auiStreamEntryCount[ui];
    }
}

//--------------------------------------------------------------------------------------------------
