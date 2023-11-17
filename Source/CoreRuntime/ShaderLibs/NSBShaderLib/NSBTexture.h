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

#pragma once
#ifndef NSBTEXTURE_H
#define NSBTEXTURE_H

#include <NiShaderAttributeDesc.h>
#include <NiTextureStage.h>

#include "NSBShaderLibLibType.h"
#include "NSBStageAndSamplerStates.h"
#include "NSBStateGroup.h"
#include "NSBUtility.h"

class NSBTextureStage;

class NSBSHADERLIB_ENTRY NSBTexture : public NiMemObject
{
public:
    NSBTexture();
    ~NSBTexture();

    // Name
    inline const char* GetName();
    void SetName(const char* pcName);

    inline unsigned int GetStage();
    inline void SetStage(unsigned int uiStage);

    //
    NSBStateGroup* GetTextureStageGroup();
    NSBStateGroup* GetSamplerStageGroup();

    inline unsigned int GetTextureFlags();
    inline unsigned short GetObjTextureFlags();

    inline void SetSource(unsigned int uiNDLMap);
    inline void SetSourceDecal(unsigned int uiDecal);
    inline void SetSourceShader(unsigned int uiShaderMap);
    inline void SetSourceObject(
        NiShaderAttributeDesc::ObjectType eObjectType,
        unsigned int uiObjectIndex);

    // *** begin Emergent internal use only

    bool CreateFromTextureStage(
        NSBTextureStage* pkTextureStage, 
        const char* pcTextureName);

    bool SaveBinary(efd::BinaryStream& kStream);
    bool LoadBinary(efd::BinaryStream& kStream);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    char* m_pcName;

    unsigned int m_uiStage;
    unsigned int m_uiTextureFlags;
    unsigned short m_usObjTextureFlags;
};

#include "NSBTexture.inl"

#endif  //NSBTEXTURE_H
