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

#include "NSBShader.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"

//------------------------------------------------------------------------------------------------
NSBTextureStage::NSBTextureStage() :
    m_pcName(0),
    m_uiStage(0),
    m_uiTextureFlags(0),
    m_pkTextureStageGroup(0),
    m_pkSamplerStageGroup(0),
    m_usObjTextureFlags(0),
    m_bTextureTransform(false),
    m_uiTextureTransformFlags(NiTextureStage::TSTTF_IGNORE),
    m_pcGlobalEntry(0)
{
    m_afTextureTransform[ 0] = 1.0f;
    m_afTextureTransform[ 1] = 0.0f;
    m_afTextureTransform[ 2] = 0.0f;
    m_afTextureTransform[ 3] = 0.0f;
    m_afTextureTransform[ 4] = 0.0f;
    m_afTextureTransform[ 5] = 1.0f;
    m_afTextureTransform[ 6] = 0.0f;
    m_afTextureTransform[ 7] = 0.0f;
    m_afTextureTransform[ 8] = 0.0f;
    m_afTextureTransform[ 9] = 0.0f;
    m_afTextureTransform[10] = 1.0f;
    m_afTextureTransform[11] = 0.0f;
    m_afTextureTransform[12] = 0.0f;
    m_afTextureTransform[13] = 0.0f;
    m_afTextureTransform[14] = 0.0f;
    m_afTextureTransform[15] = 1.0f;
}

//------------------------------------------------------------------------------------------------
NSBTextureStage::~NSBTextureStage()
{
    NiFree(m_pcGlobalEntry);
    NiFree(m_pcName);
    NiDelete m_pkTextureStageGroup;
    NiDelete m_pkSamplerStageGroup;
}

//------------------------------------------------------------------------------------------------
void NSBTextureStage::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//------------------------------------------------------------------------------------------------
NSBStateGroup* NSBTextureStage::GetTextureStageGroup()
{
    if (m_pkTextureStageGroup == 0)
        m_pkTextureStageGroup = NiNew NSBStateGroup();

    return m_pkTextureStageGroup;
}

//------------------------------------------------------------------------------------------------
NSBStateGroup* NSBTextureStage::GetSamplerStageGroup()
{
    if (m_pkSamplerStageGroup == 0)
        m_pkSamplerStageGroup = NiNew NSBStateGroup();

    return m_pkSamplerStageGroup;
}

//------------------------------------------------------------------------------------------------
bool NSBTextureStage::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkTextureStageGroup))
        return false;
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkSamplerStageGroup))
        return false;
    if (!SaveBinaryTextureTransform(kStream))
        return false;
    // use unsigned int for backwards compatibility
    unsigned int uiTemp = m_usObjTextureFlags;
    NiStreamSaveBinary(kStream, uiTemp);
    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBTextureStage::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkTextureStageGroup))
        return false;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkSamplerStageGroup))
        return false;
    if (!LoadBinaryTextureTransform(kStream))
        return false;
    if (NSBShader::GetReadVersion() >= 0x00010009)
    {
        // Use unsigned int for backwards compatibility.
        unsigned int uiObjTextureFlags;
        NiStreamLoadBinary(kStream, uiObjTextureFlags);
        m_usObjTextureFlags = (unsigned short) uiObjTextureFlags;
    }
    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBTextureStage::SaveBinaryTextureTransform(efd::BinaryStream& kStream)
{
    unsigned int uiValue = m_bTextureTransform ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_afTextureTransform[ 0]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 1]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 2]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 3]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 4]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 5]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 6]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 7]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 8]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[ 9]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[10]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[11]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[12]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[13]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[14]);
    NiStreamSaveBinary(kStream, m_afTextureTransform[15]);

    NiStreamSaveBinary(kStream, m_uiTextureTransformFlags);
    kStream.WriteCString(m_pcGlobalEntry);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBTextureStage::LoadBinaryTextureTransform(efd::BinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_bTextureTransform = (uiValue != 0);

    NiStreamLoadBinary(kStream, m_afTextureTransform[ 0]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 1]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 2]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 3]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 4]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 5]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 6]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 7]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 8]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[ 9]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[10]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[11]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[12]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[13]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[14]);
    NiStreamLoadBinary(kStream, m_afTextureTransform[15]);

    NiStreamLoadBinary(kStream, m_uiTextureTransformFlags);
    m_pcGlobalEntry = kStream.ReadCString();

    return true;
}

//------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
void NSBTextureStage::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "TextureStage %s\n", m_pcName);
    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBUtility::Dump(pf, true, "Texture Flags = 0x%08x\n", m_uiTextureFlags);

    if (m_pkTextureStageGroup)
    {
        NSBUtility::Dump(pf, true, "TextureStageGroup\n");
        NSBUtility::IndentInsert();
        m_pkTextureStageGroup->Dump(pf, NSBStateGroup::DUMP_STAGESTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***  NO STAGE GROUP ***\n");
    }

    if (m_pkSamplerStageGroup)
    {
        NSBUtility::Dump(pf, true, "SamplerStageGroup\n");
        NSBUtility::IndentInsert();
        m_pkSamplerStageGroup->Dump(pf, NSBStateGroup::DUMP_SAMPLERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "**  NO SAMPLER GROUP **\n");
    }

    NSBUtility::Dump(pf, true, "TextureTransform ");
    if (m_bTextureTransform)
        NSBUtility::Dump(pf, false, "ENABLE\n");
    else
        NSBUtility::Dump(pf, false, "DISABLED\n");

    NSBUtility::IndentRemove();
}

//------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
