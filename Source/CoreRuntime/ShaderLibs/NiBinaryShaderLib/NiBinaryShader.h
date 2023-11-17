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
#ifndef NIBINARYSHADER_H
#define NIBINARYSHADER_H

#include "NiBinaryShaderLibLibType.h"
#include "NSBUserDefinedDataSet.h"

#include <NiD3DShader.h>
#include <NiShaderDeclaration.h>
#include <NiShaderRequirementDesc.h>

// For reporting errors...
#include <NiShaderFactory.h>

class NSBImplementation;

class NIBINARYSHADERLIB_ENTRY NiBinaryShader : public NiD3DShader
{
    NiDeclareRTTI;
public:
    NiBinaryShader();
    virtual ~NiBinaryShader();

    virtual bool Initialize();

    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);

    inline NSBUserDefinedDataSet* GetShaderUserDefinedDataSet();
    inline NSBUserDefinedDataSet* GetImplementationUserDefinedDataSet();
    inline NSBUserDefinedDataSet* GetPassUserDefinedDataSet(
        unsigned int uiPass);

    // *** begin Emergent internal use only
    // These functions are used for the creation of the shader
    inline void SetUserDefinedDataSet(NSBUserDefinedDataSet* pkUDDSet);
    inline void SetImplementationUserDefinedDataSet(
        NSBUserDefinedDataSet* pkUDDSet);
    inline void SetPassUserDefinedDataSet(unsigned int uiPass,
        NSBUserDefinedDataSet* pkUDDSet);

    // Render State Group
    inline NiD3DRenderStateGroup* GetRenderStateGroup();
    inline void SetRenderStateGroup(NiD3DRenderStateGroup* pkRenderStateGroup);

    // Vertex shader mapping
    inline NiD3DShaderConstantMap* GetVertexConstantMap() const;
    inline void SetVertexConstantMap(NiD3DShaderConstantMap* pkSCMVertex);

    // Pixel shader mapping
    inline NiD3DShaderConstantMap* GetPixelConstantMap() const;
    inline void SetPixelConstantMap(NiD3DShaderConstantMap* pkSCMPixel);

    inline D3DDevicePtr GetD3DDevice();
    inline NiD3DRenderer* GetD3DRenderer();
    inline NiD3DRenderState* GetD3DRenderState();

    // Inserts the pass at the given position.
    inline bool InsertPass(unsigned int uiPass, NiD3DPass* pkPass);

    // For skinning shaders
    enum BoneMatrixCalcMethod
    {
        BONECALC_SKIN       = 0x00000000,
        BONECALC_NO_SKIN    = 0x00000001
    };

    inline unsigned int GetBonesPerPartition() const;
    inline void SetBonesPerPartition(unsigned int uiBones);
    inline unsigned int GetBoneMatrixRegisters() const;
    inline void SetBoneMatrixRegisters(unsigned int uiRegisters);
    BoneMatrixCalcMethod GetBoneCalcMethod() const;
    inline void SetBoneCalcMethod(BoneMatrixCalcMethod eMethod);

    inline NiShaderRequirementDesc::NBTFlags GetBinormalTangentMethod() const;
    inline void SetBinormalTangentMethod(
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    inline unsigned int GetBinormalTangentUVSource() const;
    inline void SetBinormalTangentUVSource(unsigned int uiSource);

    virtual bool SetupGeometry(NiRenderObject* pkGeometry,
        NiMaterialInstance* pkMaterialInstance);

    // *** end Emergent internal use only

protected:
    bool LoadVertexShaderProgram(NiD3DPass& kPass);
    bool LoadGeometryShaderProgram(NiD3DPass& kPass);
    bool LoadPixelShaderProgram(NiD3DPass& kPass);
    bool ResolveVertexShaderFileName(const char* pcOriginalName,
        char* pcTrueName, unsigned int uiNameLength);
    bool ResolveGeometryShaderFileName(const char* pcOriginalName,
        char* pcTrueName, unsigned int uiNameLength);
    bool ResolvePixelShaderFileName(const char* pcOriginalName,
        char* pcTrueName, unsigned int uiNameLength);

    void PrepareTextureStage(NiD3DTextureStage* pkStage,
        const NiRenderCallContext& kRCC);

    unsigned int m_uiBonesPerPartition;
    unsigned int m_uiBoneMatrixRegisters;
    BoneMatrixCalcMethod m_eBoneCalcMethod;
    NiShaderRequirementDesc::NBTFlags m_eBinormalTangentMethod;
    unsigned int m_uiBinormalTangentUVSource;

    // User defined data set
    NSBUserDefinedDataSetPtr m_spUDDSet_Shader;
    NSBUserDefinedDataSetPtr m_spUDDSet_Implementation;
    NiTObjectArray<NSBUserDefinedDataSetPtr> m_kUDDSet_PassArray;
};

#include "NiBinaryShader.inl"

#endif  //NIBINARYSHADER_H
