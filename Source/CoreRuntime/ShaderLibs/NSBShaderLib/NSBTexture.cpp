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
#include "NSBShaderLibPCH.h"

#include "NSBTexture.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"

#include <NiStream.h>

//--------------------------------------------------------------------------------------------------
NSBTexture::NSBTexture() :
    m_pcName(NULL),
    m_uiStage(0),
    m_uiTextureFlags(0),
    m_usObjTextureFlags(0)
{
    /* */
}

//--------------------------------------------------------------------------------------------------
NSBTexture::~NSBTexture()
{
    NiFree(m_pcName);
}

//--------------------------------------------------------------------------------------------------
void NSBTexture::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//--------------------------------------------------------------------------------------------------
bool NSBTexture::CreateFromTextureStage(
    NSBTextureStage* pkTextureStage, const char* pcTextureName)
{
    NSBUtility::SetString(m_pcName, 0, pcTextureName);
    m_uiStage = pkTextureStage->GetStage();
    m_uiTextureFlags = pkTextureStage->GetTextureFlags();
    m_usObjTextureFlags = pkTextureStage->GetObjTextureFlags();

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NSBTexture::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    NiStreamSaveBinary(kStream, m_usObjTextureFlags);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBTexture::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    NiStreamLoadBinary(kStream, m_usObjTextureFlags);

    return true;
}

//--------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
void NSBTexture::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Texture %s\n", m_pcName);
    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBUtility::Dump(pf, true, "Texture Flags = 0x%08x\n", m_uiTextureFlags);
    NSBUtility::Dump(pf, true, "Object Flags = 0x%04x\n", m_usObjTextureFlags);

    NSBUtility::IndentRemove();
}

//--------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
