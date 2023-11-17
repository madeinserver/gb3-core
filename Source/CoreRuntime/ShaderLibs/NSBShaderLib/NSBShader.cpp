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

#include <NiShaderDesc.h>
#include <NiShaderFactory.h>
#include <NiTextureStage.h>

#include "NSBShader.h"
#include "NSBUtility.h"
#include "NSBTexture.h"
#include "NSBPass.h"
#include "NSBTextureStage.h"
#include "NSBAttributeDesc.h"

//--------------------------------------------------------------------------------------------------
unsigned int NSBShader::ms_uiReadVersion = NSBShader::NSB_VERSION;
//--------------------------------------------------------------------------------------------------
NSBShader::NSBShader() :
    m_pcName(0),
    m_pcDescription(0),
    m_uiMinVertexShaderVersionRequest(0),
    m_uiMaxVertexShaderVersionRequest(0),
    m_uiMinGeometryShaderVersionRequest(0),
    m_uiMaxGeometryShaderVersionRequest(0),
    m_uiMinPixelShaderVersionRequest(0),
    m_uiMaxPixelShaderVersionRequest(0),
    m_uiMinComputeShaderVersionRequest(0),
    m_uiMaxComputeShaderVersionRequest(0),
    m_uiMinUserVersionRequest(0),
    m_uiMaxUserVersionRequest(0),
    m_uiMinFeatureLevelRequest(0),
    m_uiMaxFeatureLevelRequest(0),
    m_uiPlatformRequest(0),
    m_uiMinVertexShaderVersion(0xffffffff),
    m_uiMaxVertexShaderVersion(0),
    m_uiMinGeometryShaderVersion(0xffffffff),
    m_uiMaxGeometryShaderVersion(0),
    m_uiMinPixelShaderVersion(0xffffffff),
    m_uiMaxPixelShaderVersion(0),
    m_uiMinComputeShaderVersion(0xffffffff),
    m_uiMaxComputeShaderVersion(0),
    m_uiMinUserVersion(0xffffffff),
    m_uiMaxUserVersion(0),
    m_uiMinFeatureLevel(0xffffffff),
    m_uiMaxFeatureLevel(0),
    m_uiPlatform(0),
    m_spShaderDesc(0)
{
    m_kPackingDefMap.RemoveAll();
    m_kImplementationArray.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSBShader::~NSBShader()
{
    NILOG(NIMESSAGE_GENERAL_1, "Killing NSBShader %s\n", m_pcName);

    m_spShaderDesc = 0;
    NiFree(m_pcName);
    NiFree(m_pcDescription);

    const char* pcName;
    NiTMapIterator kIter;

    NSBPackingDef* pkPackingDef;
    kIter = m_kPackingDefMap.GetFirstPos();
    while (kIter)
    {
        m_kPackingDefMap.GetNext(kIter, pcName, pkPackingDef);
        if (pkPackingDef)
        {
            NiDelete pkPackingDef;
        }
    }
    m_kPackingDefMap.RemoveAll();

    NSBImplementation* pkImplementation;
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            NiDelete pkImplementation;
            m_kImplementationArray.SetAt(ui, 0);
        }
    }
    m_kImplementationArray.RemoveAll();

    m_kOutputStreamDescriptors.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBShader::GetPackingDefCount()
{
    return m_kPackingDefMap.GetCount();
}

//--------------------------------------------------------------------------------------------------
NSBPackingDef* NSBShader::GetPackingDef(const char* pcName,
    bool bCreate)
{
    NSBPackingDef* pkPackingDef = 0;

    if (!m_kPackingDefMap.GetAt(pcName, pkPackingDef))
    {
        if (bCreate)
        {
            pkPackingDef = NiNew NSBPackingDef();
            EE_ASSERT(pkPackingDef);

            pkPackingDef->SetName(pcName);
            m_kPackingDefMap.SetAt(pcName, pkPackingDef);
        }
    }

    return pkPackingDef;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBShader::GetImplementationCount()
{
    return m_kImplementationArray.GetEffectiveSize();
}

//--------------------------------------------------------------------------------------------------
NSBImplementation* NSBShader::GetImplementation(const char* pcName,
    bool bCreate, unsigned int uiNextIndex)
{
    NSBImplementation* pkImplementation = 0;

    pkImplementation = GetImplementationByName(pcName);
    if (!pkImplementation)
    {
        if (bCreate)
        {
            pkImplementation = NiNew NSBImplementation();
            EE_ASSERT(pkImplementation);

            pkImplementation->SetName(pcName);
            pkImplementation->SetIndex(uiNextIndex);
            m_kImplementationArray.SetAtGrow(uiNextIndex, pkImplementation);
        }
    }

    return pkImplementation;
}

//--------------------------------------------------------------------------------------------------
NSBImplementation* NSBShader::GetImplementationByName(const char* pcName)
{
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NSBImplementation* pkImplementation;
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            if (strcmp(pkImplementation->GetName(), pcName) == 0)
                return pkImplementation;
        }
    }

    return 0;
}

//--------------------------------------------------------------------------------------------------
NSBImplementation* NSBShader::GetImplementationByIndex(unsigned int uiIndex)
{
    return m_kImplementationArray.GetAt(uiIndex);
}

//--------------------------------------------------------------------------------------------------
const NiShaderAttributeDesc* NSBShader::GetTextureAttribute(
    unsigned int uiIndex, const char* pcMap) const
{
    const NiShaderAttributeDesc* pkDesc = m_spShaderDesc->GetFirstAttribute();
    while (pkDesc)
    {
        unsigned int uiCurrentIndex;
        const char* pcCurrentMap;
        pkDesc->GetValue_Texture(uiCurrentIndex, pcCurrentMap);
        if (pcCurrentMap && uiCurrentIndex == uiIndex &&
            strcmp(pcCurrentMap, pcMap) == 0)
        {
            return pkDesc;
        }

        pkDesc = m_spShaderDesc->GetNextAttribute();
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::AddTextureAttributeDesc(const unsigned int uiTextureFlags) const
{
    NiShaderAttributeDesc* pkNewDesc = NULL;

    if (uiTextureFlags != NiTextureStage::TSTF_IGNORE)
    {
        unsigned int uiTextureType = uiTextureFlags & NiTextureStage::TSTF_NDL_TYPEMASK;
        if (uiTextureType == 0 && uiTextureFlags & NiTextureStage::TSTF_MAP_DECAL)
        {
            uiTextureType = NiTextureStage::TSTF_MAP_DECAL;
        }

        if (uiTextureType != 0)
        {
            switch (uiTextureType)
            {
            case NiTextureStage::TSTF_NONE:
                break;
            case NiTextureStage::TSTF_NDL_BASE:
                if (!GetTextureAttribute(0, "base"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("BaseMap");
                    pkNewDesc->SetValue_Texture(0, "base");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_DARK:
                if (!GetTextureAttribute(0, "dark"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("DarkMap");
                    pkNewDesc->SetValue_Texture(0, "dark");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_DETAIL:
                if (!GetTextureAttribute(0, "detail"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("DetailMap");
                    pkNewDesc->SetValue_Texture(0, "detail");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_GLOSS:
                if (!GetTextureAttribute(0, "gloss"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("GlossMap");
                    pkNewDesc->SetValue_Texture(0, "gloss");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_GLOW:
                if (!GetTextureAttribute(0, "glow"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("GlowMap");
                    pkNewDesc->SetValue_Texture(0, "glow");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_BUMP:
                if (!GetTextureAttribute(0, "bump"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("BumpMap");
                    pkNewDesc->SetValue_Texture(0, "bump");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_NORMAL:
                if (!GetTextureAttribute(0, "normal"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("NormalMap");
                    pkNewDesc->SetValue_Texture(0, "normal");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_NDL_PARALLAX:
                if (!GetTextureAttribute(0, "parallax"))
                {
                    pkNewDesc = NiNew NiShaderAttributeDesc;
                    pkNewDesc->SetName("ParallaxMap");
                    pkNewDesc->SetValue_Texture(0, "parallax");
                    pkNewDesc->SetHidden(true);
                    m_spShaderDesc->AddAttribute(pkNewDesc);
                }
                break;
            case NiTextureStage::TSTF_MAP_DECAL:
                {
                    unsigned int uiIndex = uiTextureFlags &
                        NiTextureStage::TSTF_INDEX_MASK;
                    if (!GetTextureAttribute(uiIndex, "decal"))
                    {
                        pkNewDesc = NiNew NiShaderAttributeDesc;
                        pkNewDesc->SetName("DecalMap");
                        pkNewDesc->SetValue_Texture(uiIndex, "decal");
                        pkNewDesc->SetHidden(true);
                        m_spShaderDesc->AddAttribute(pkNewDesc);
                    }
                }
                break;
            }
        }
    }

    return (pkNewDesc != NULL);
}

//--------------------------------------------------------------------------------------------------
NiShaderDesc* NSBShader::GetShaderDesc()
{
    if (m_spShaderDesc)
        return m_spShaderDesc;

    m_spShaderDesc = NiNew NiShaderDesc();
    EE_ASSERT(m_spShaderDesc);

    m_spShaderDesc->SetName(m_pcName);
    m_spShaderDesc->SetDescription(m_pcDescription);

    m_spShaderDesc->AddVertexShaderVersion(m_uiMinVertexShaderVersion);
    m_spShaderDesc->AddVertexShaderVersion(m_uiMaxVertexShaderVersion);
    m_spShaderDesc->AddGeometryShaderVersion(m_uiMinGeometryShaderVersion);
    m_spShaderDesc->AddGeometryShaderVersion(m_uiMaxGeometryShaderVersion);
    m_spShaderDesc->AddPixelShaderVersion(m_uiMinPixelShaderVersion);
    m_spShaderDesc->AddPixelShaderVersion(m_uiMaxPixelShaderVersion);
    m_spShaderDesc->AddComputeShaderVersion(m_uiMinComputeShaderVersion);
    m_spShaderDesc->AddComputeShaderVersion(m_uiMaxComputeShaderVersion);
    m_spShaderDesc->AddUserDefinedVersion(m_uiMinUserVersion);
    m_spShaderDesc->AddUserDefinedVersion(m_uiMaxUserVersion);
    m_spShaderDesc->AddFeatureLevel(m_uiMinFeatureLevel);
    m_spShaderDesc->AddFeatureLevel(m_uiMaxFeatureLevel);
    m_spShaderDesc->AddPlatformFlags(m_uiPlatform);

    NSBImplementation* pkImplementation;
    m_spShaderDesc->SetNumberOfImplementations(GetImplementationCount());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            // Add the standard texture attributes so they can be exposed in the UI
            unsigned int uiNumPasses = pkImplementation->GetPassCount();
            for (unsigned int uj = 0; uj < uiNumPasses; uj++)
            {
                NSBPass* pkPass = pkImplementation->GetPass(uj, false);
                if (pkPass)
                {
                    unsigned int uiNumStages = pkPass->GetStageCount();
                    for (unsigned int uk = 0; uk < uiNumStages; uk++)
                    {
                        NSBTextureStage* pkStage = pkPass->GetStage(uk, false);
                        if (pkStage)
                        {
                            unsigned int uiTextureFlags = pkStage->GetTextureFlags();
                            AddTextureAttributeDesc(uiTextureFlags);
                        }
                    }

                    unsigned int uiNumTextures = pkPass->GetTextureCount();
                    for (unsigned int uk = 0; uk < uiNumTextures; uk++)
                    {
                        NSBTexture* pkTexture = pkPass->GetTexture(uk, false);
                        if (pkTexture)
                        {
                            unsigned int uiTextureFlags = pkTexture->GetTextureFlags();
                            AddTextureAttributeDesc(uiTextureFlags);
                        }
                    }
                }
            }

            NSBRequirements* pkReqs = pkImplementation->GetRequirements();
            EE_ASSERT(pkReqs);
            // Currently, implementation 0 defines the bones per partition
            // as well as the Binormal Tangent method.
            if (ui == 0)
            {
                m_spShaderDesc->SetBonesPerPartition(
                    pkReqs->GetBonesPerPartition());
                m_spShaderDesc->SetBinormalTangentMethod(
                    pkReqs->GetBinormalTangentMethod());
                m_spShaderDesc->SetBinormalTangentUVSource(
                    (unsigned short)pkReqs->GetBinormalTangentUVSource());
            }

            NiShaderRequirementDesc* pkReqDesc =
                NiNew NiShaderRequirementDesc();
            EE_ASSERT(pkReqDesc);

            pkReqDesc->SetName(pkImplementation->GetName());
            pkReqDesc->SetDescription(pkImplementation->GetDesc());
            pkReqDesc->AddVertexShaderVersion(pkReqs->GetVSVersion());
            pkReqDesc->AddGeometryShaderVersion(pkReqs->GetGSVersion());
            pkReqDesc->AddPixelShaderVersion(pkReqs->GetPSVersion());
            pkReqDesc->AddComputeShaderVersion(pkReqs->GetCSVersion());
            pkReqDesc->AddUserDefinedVersion(pkReqs->GetUserVersion());
            pkReqDesc->AddFeatureLevel(pkReqs->GetFeatureLevel());
            pkReqDesc->AddPlatformFlags(pkReqs->GetPlatformFlags());
            pkReqDesc->SetBonesPerPartition(pkReqs->GetBonesPerPartition());
            pkReqDesc->SetBinormalTangentMethod(
                pkReqs->GetBinormalTangentMethod());
            pkReqDesc->SetBinormalTangentUVSource(
                (unsigned short)pkReqs->GetBinormalTangentUVSource());
            pkReqDesc->SetSoftwareVPAcceptable(
                pkReqs->GetSoftwareVPAcceptable());
            pkReqDesc->SetSoftwareVPRequired(
                pkReqs->GetSoftwareVPRequired());

            m_spShaderDesc->SetImplementationDescription(ui, pkReqDesc);
        }
    }

    // Add the remanding attributes
    NiTListIterator kIter;
    NSBAttributeDesc* pkAttribDesc = m_kAttribTable.GetFirstAttribute(kIter);
    while (pkAttribDesc)
    {
        NiShaderAttributeDesc* pkNewDesc =
            pkAttribDesc->GetShaderAttributeDesc();
        EE_ASSERT(pkNewDesc);
        m_spShaderDesc->AddAttribute(pkNewDesc);
        pkAttribDesc = m_kAttribTable.GetNextAttribute(kIter);
    }

    return m_spShaderDesc;
}

//--------------------------------------------------------------------------------------------------
void NSBShader::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//--------------------------------------------------------------------------------------------------
void NSBShader::SetDescription(const char* pcDescription)
{
    NSBUtility::SetString(m_pcDescription, 0, pcDescription);
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::Save(const char* pcFilename, bool bEndianSwap)
{
    // Open a binary stream
    efd::File* pkFile = efd::File::GetFile(pcFilename, efd::File::WRITE_ONLY);
    if (!pkFile)
        return false;

    bool bResult = SaveBinary(*pkFile, bEndianSwap);
    NiDelete pkFile;

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::SaveBinary(efd::BinaryStream& kStream, bool bEndianSwap)
{
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kStream.SetEndianSwap(!bPlatformLittle);

    // File header and version number must be streamed in/out little endian!
    unsigned int uiMagicNumber = MAGIC_NUMBER;
    char acMagicNumber[4];
    acMagicNumber[0] = (char)(uiMagicNumber & 0x000000ff);
    acMagicNumber[1] = (char)((uiMagicNumber & 0x0000ff00) >> 8);
    acMagicNumber[2] = (char)((uiMagicNumber & 0x00ff0000) >> 16);
    acMagicNumber[3] = (char)((uiMagicNumber & 0xff000000) >> 24);

    NiStreamSaveBinary(kStream, acMagicNumber, 4);

    unsigned int uiNSBVersion = NSB_VERSION;
    char acNSBVersion[4];
    acNSBVersion[0] = (char)(uiNSBVersion & 0x000000ff);
    acNSBVersion[1] = (char)((uiNSBVersion & 0x0000ff00) >> 8);
    acNSBVersion[2] = (char)((uiNSBVersion & 0x00ff0000) >> 16);
    acNSBVersion[3] = (char)((uiNSBVersion & 0xff000000) >> 24);

    NiStreamSaveBinary(kStream, acNSBVersion, 4);

    // Version 1.10 added endianness support.
    bool bFileLittle = (bPlatformLittle != bEndianSwap);
    NiStreamSaveBinary(kStream, bFileLittle);
    kStream.SetEndianSwap(bEndianSwap);

    kStream.WriteCString(m_pcName);
    kStream.WriteCString(m_pcDescription);

    // Version 1.6 added support for user-defined data...
    unsigned int uiUDDSetPresent = 0;
    if (m_spUserDefinedDataSet)
    {
        uiUDDSetPresent = 1;
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
        if (!m_spUserDefinedDataSet->SaveBinary(kStream))
            return false;
    }
    else
    {
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
    }

    if (!m_kGlobalAttribTable.SaveBinary(kStream))
        return false;

    if (!m_kAttribTable.SaveBinary(kStream))
        return false;

    if (!SaveBinaryPackingDefs(kStream))
        return false;

    if (!SaveBinaryImplementations(kStream))
        return false;

    if (!SaveBinaryOutputStreamDescriptors(kStream))
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::Load(const char* pcFilename)
{
    // Open a binary stream
    efd::File* pkFile = efd::File::GetFile(pcFilename, efd::File::READ_ONLY);
    if (!pkFile)
        return false;

    bool bResult = LoadBinary(*pkFile);
    NiDelete pkFile;

#if defined(_DEBUG_DUMP_LOADED_SHADER_)
    if (bResult)
        Dump();
#endif  //#if defined(_DEBUG_DUMP_LOADED_SHADER_)

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::LoadBinary(efd::BinaryStream& kStream)
{
    // Header is in little endian.
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kStream.SetEndianSwap(!bPlatformLittle);

    char acMagicNumber[4];
    NiStreamLoadBinary(kStream, acMagicNumber, 4);
    unsigned int uiMagicNumber =
        acMagicNumber[0] << 0 |
        acMagicNumber[1] << 8 |
        acMagicNumber[2] << 16 |
        acMagicNumber[3] << 24;

    if (uiMagicNumber != MAGIC_NUMBER)
        return false;

    char acReadVersion[4];
    NiStreamLoadBinary(kStream, acReadVersion, 4);
    ms_uiReadVersion =
        acReadVersion[0] << 0 |
        acReadVersion[1] << 8 |
        acReadVersion[2] << 16 |
        acReadVersion[3] << 24;

    if (ms_uiReadVersion > NSBShader::NSB_VERSION)
        return false;

    // Get endianness if version 1.10 or later.
    // Old files by default are little endian.
    bool bLittle = true;
    if (ms_uiReadVersion >= 0x00010010)
        NiStreamLoadBinary(kStream, bLittle);
    kStream.SetEndianSwap(bLittle != bPlatformLittle);

    m_pcName = kStream.ReadCString();
    m_pcDescription = kStream.ReadCString();

    // Version 1.6 added support for user-defined data...
    if (NSBShader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBUserDefinedDataSet();
            EE_ASSERT(m_spUserDefinedDataSet);
            if (!m_spUserDefinedDataSet->LoadBinary(kStream))
                return false;
        }
        else
        {
            m_spUserDefinedDataSet = 0;
        }
    }

    if (!m_kGlobalAttribTable.LoadBinary(kStream))
        return false;

    if (!m_kAttribTable.LoadBinary(kStream))
        return false;

    if (!LoadBinaryPackingDefs(kStream))
        return false;

    if (!LoadBinaryImplementations(kStream))
        return false;

    if (!LoadBinaryOutputStreamDescriptors(kStream))
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::SaveBinaryPackingDefs(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kPackingDefMap.GetCount();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    const char* pcName;
    NSBPackingDef* pkPackingDef;
    NiTMapIterator iter = m_kPackingDefMap.GetFirstPos();
    while (iter)
    {
        m_kPackingDefMap.GetNext(iter, pcName, pkPackingDef);
        if (pcName && pkPackingDef)
        {
            // Stream the packing def
            if (!pkPackingDef->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::LoadBinaryPackingDefs(efd::BinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBPackingDef* pkPackingDef;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPackingDef = NiNew NSBPackingDef();
        EE_ASSERT(pkPackingDef);

        if (!pkPackingDef->LoadBinary(kStream))
            return false;

        m_kPackingDefMap.SetAt(pkPackingDef->GetName(), pkPackingDef);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::SaveBinaryImplementations(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kImplementationArray.GetEffectiveSize();
    unsigned int uiSize = m_kImplementationArray.GetSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;

    NSBImplementation* pkImplementation;

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkImplementation = m_kImplementationArray.GetAt(ui);
        if (pkImplementation)
        {
            // Stream the packing def
            if (!pkImplementation->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::LoadBinaryImplementations(efd::BinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    NSBImplementation* pkImplementation;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkImplementation = NiNew NSBImplementation();
        EE_ASSERT(pkImplementation);

        if (!pkImplementation->LoadBinary(kStream))
            return false;

        NSBRequirements* pkReqs = pkImplementation->GetRequirements();
        if (pkReqs)
        {
            AddPixelShaderVersion(pkReqs->GetPSVersion());
            AddVertexShaderVersion(pkReqs->GetVSVersion());

            if (NSBShader::GetReadVersion() >= 0x00010013)
            {
                // Version 1.13 added D3D10 and geometry shader support.
                AddGeometryShaderVersion(pkReqs->GetGSVersion());
            }

            if (NSBShader::GetReadVersion() >= 0x00030001)
            {
                // Version 3.1 added D3D11 support.
                AddComputeShaderVersion(pkReqs->GetCSVersion());
                AddFeatureLevel(pkReqs->GetFeatureLevel());
            }

            AddUserVersion(pkReqs->GetUserVersion());
            AddPlatform(pkReqs->GetPlatformFlags());
        }

        EE_ASSERT(ui == pkImplementation->GetIndex());
        m_kImplementationArray.SetAtGrow(ui, pkImplementation);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::SaveBinaryOutputStreamDescriptors(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kOutputStreamDescriptors.GetSize();
    NiStreamSaveBinary(kStream, uiCount);  // number of screen polygons

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        m_kOutputStreamDescriptors[ui].SaveBinary(kStream);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBShader::LoadBinaryOutputStreamDescriptors(efd::BinaryStream& kStream)
{
    m_kOutputStreamDescriptors.RemoveAll();

    // Version 2.2 added support for stream output...
    if (NSBShader::GetReadVersion() >= 0x00020002)
    {
        unsigned int uiCount = 0;
        NiStreamLoadBinary(kStream, uiCount);
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiOutputStreamDescriptor kDesc;
            kDesc.LoadBinary(kStream);
            m_kOutputStreamDescriptors.Add(kDesc);
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
void NSBShader::Dump()
{
    char acName[NI_MAX_PATH];

    NiSprintf(acName, NI_MAX_PATH, "%s.log", m_pcName);
    FILE* pf;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    fopen_s(&pf, acName, "wt");
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pf = fopen(acName, "wt");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    NSBUtility::Dump(pf, true, "Dumping BinaryShader %s\n", m_pcName);

    NSBUtility::Dump(pf, true, "    Desc: %s", m_pcDescription);
    NSBUtility::Dump(pf, false, "\n");

    NSBUtility::Dump(pf, true, "    Min VS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMinVertexShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMinVertexShaderVersion));
    NSBUtility::Dump(pf, true, "    Max VS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMaxVertexShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMaxVertexShaderVersion));
    NSBUtility::Dump(pf, true, "    Min GS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMinGeometryShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMinGeometryShaderVersion));
    NSBUtility::Dump(pf, true, "    Max GS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMaxGeometryShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMaxGeometryShaderVersion));
    NSBUtility::Dump(pf, true, "    Min PS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMinPixelShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMinPixelShaderVersion));
    NSBUtility::Dump(pf, true, "    Max PS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMaxPixelShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMaxPixelShaderVersion));
    NSBUtility::Dump(pf, true, "    Min CS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMinComputeShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMinComputeShaderVersion));
    NSBUtility::Dump(pf, true, "    Max CS Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMaxComputeShaderVersion),
        NSBSHADER_VERSION_MINOR(m_uiMaxComputeShaderVersion));
    NSBUtility::Dump(pf, true, "    Min US Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMinUserVersion),
        NSBSHADER_VERSION_MINOR(m_uiMinUserVersion));
    NSBUtility::Dump(pf, true, "    Max US Version: %d.%d\n",
        NSBSHADER_VERSION_MAJOR(m_uiMaxUserVersion),
        NSBSHADER_VERSION_MINOR(m_uiMaxUserVersion));
    NSBUtility::Dump(pf, true, "    Min FeatureLevel: %d\n",
        m_uiMinFeatureLevel);
    NSBUtility::Dump(pf, true, "    Max FeatureLevel: %d\n",
        m_uiMaxFeatureLevel);

    NSBUtility::Dump(pf, true, "          Platform: 0x%08x\n", m_uiPlatform);
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "Global Attribute Table:\n");
    NSBUtility::IndentInsert();
    m_kGlobalAttribTable.Dump(pf);
    NSBUtility::IndentRemove();
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "Local Attribute Table:\n");
    NSBUtility::IndentInsert();
    m_kAttribTable.Dump(pf);
    NSBUtility::IndentRemove();
    NSBUtility::Dump(pf, true, "\n");

    NSBUtility::Dump(pf, true, "PackingDef Count: %d\n",
        GetPackingDefCount());
    NSBUtility::Dump(pf, true, "\n");
//    NiTStringPointerMap<NSBPackingDef*> m_kPackingDefMap;

    NSBUtility::Dump(pf, true, "%2d Implementations\n",
        m_kImplementationArray.GetEffectiveSize());
    for (unsigned int ui = 0; ui < m_kImplementationArray.GetSize(); ui++)
    {
        NSBImplementation* pkImpl = m_kImplementationArray.GetAt(ui);
        if (pkImpl)
        {
            NSBUtility::IndentInsert();
            pkImpl->Dump(pf);
            NSBUtility::IndentRemove();
            NSBUtility::Dump(pf, true, "\n");
        }
    }

    if (pf)
        fclose(pf);
}

//--------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
