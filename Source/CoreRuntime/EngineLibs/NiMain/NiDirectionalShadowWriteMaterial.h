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
#ifndef NIDIRECTIONALSHADOWWRITEMATERIAL_H
#define NIDIRECTIONALSHADOWWRITEMATERIAL_H

#include "NiStandardMaterial.h"

class NiStandardVertexProgramDescriptor;

class NIMAIN_ENTRY NiDirectionalShadowWriteMaterial : public NiStandardMaterial
{
    NiDeclareRTTI;
public:

    // Create a default material using the specified material node library. If
    // bAutoCreateCaches is true, a cache is created using the static renderer
    // pointer.
    NiDirectionalShadowWriteMaterial(NiMaterialNodeLibrary* pkLibrary,
        bool bAutoCreateCaches);

    // This constructor creates a default material using the specified material
    // node library named kName. If bAutoCreateCaches is true, a cache is
    // created using the static renderer pointer. Unlike the other protected
    // constructor which takes explicit version numbers (for derived classes
    // that implement their own versioning scheme), this function just uses the
    // shader version numbers internal to this class.
    NiDirectionalShadowWriteMaterial(const NiFixedString& kName,
        NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches);


    // This function takes the same parameters as its parent class
    // NiFragmentMaterial. It creates a standard material using the given node
    // library. It names the material kName. If bAutoCreateCaches is true, then
    // a cache for this type of material will be created using the static
    // renderer pointer. The version numbers will be stored in the cache so
    // that when a material is updated to a newer version, older cached shaders
    // will be invalidated and then regenerated.
    NiDirectionalShadowWriteMaterial(const NiFixedString& kName,
        NiMaterialNodeLibrary* pkLibrary, unsigned int uiVertexVersion,
        unsigned int uiGeometryVersion, unsigned int uiPixelVersion,
        bool bAutoCreateCaches);

    // Inherited from NiFragmentMaterial. Creates the default fallback
    // functions.
    virtual void AddDefaultFallbacks();

    // Populates the shader cache with a set of fall back shaders. Replacement shaders 
    // are 'fail safe'shaders that can be used when shader compilation can not be performed.
    virtual void AddReplacementShaders();

protected:
    // Inherited from NiFragmentMaterial. Generate the descriptor object that
    // matches the state of this geometry. This descriptor will be a
    // NiStandardMaterialDescriptor object.
    virtual bool GenerateDescriptor(const NiRenderObject* pkGeometry,
        const NiPropertyState* pkState,
        const NiDynamicEffectState* pkEffects,
        NiMaterialDescriptor& kMaterialDesc);

    // This method controls the wiring of the vertex shader material nodes.
    virtual bool GenerateVertexShadeTree(Context& kContext,
        NiGPUProgramDescriptor* pkDesc);

    // This method controls the wiring of the pixel shader material nodes.
    virtual bool GeneratePixelShadeTree(Context& kContext,
        NiGPUProgramDescriptor* pkDesc);

    // This method applies the view projection matrix to the world position.
    // If requested, this method will generate resources for the computed view
    // position.
    virtual bool HandleViewProjectionFragment(Context& kContext,
        bool bForceViewPos, NiMaterialResource* pkVertWorldPos,
        NiMaterialResource*& pkVertOutProjectedPos,
        NiMaterialResource*& pkVertOutViewPos);

    // This method generates the view vector and if requested generates the
    // view vector in a specific tangent space.
    virtual bool HandleViewVectorFragment(Context& kContext,
        NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
        NiMaterialResource* pkWorldBinorm, NiMaterialResource* pkWorldTangent,
        bool bComputeTangent, NiMaterialResource*& pkWorldViewVector,
        NiMaterialResource*& pkTangentViewVector);

    // This method encapsulates the vertex/normal/binormal/tangent pipeline.
    // If requested, this method will generate the world view vector.
    virtual bool SetupTransformPipeline(Context& kContext,
        NiMaterialResource* pkVertOutProjPos,
        NiStandardVertexProgramDescriptor* pkVertDesc, bool bForceView,
        bool bForceViewPos, NiMaterialResource*& pkWorldPos,
        NiMaterialResource*& pkViewPos, NiMaterialResource*& pkProjectedPos,
        NiMaterialResource*& pkWorldNormal, NiMaterialResource*& pkWorldView);

    // Inherited from NiFragmentMaterial. By default this creates one render
    // pass with a NiStandardPixelProgramDescriptor and a
    // NiStandardVertexProgramDescriptor.
    virtual ReturnCode GenerateShaderDescArray(
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiMaxCount,
        unsigned int& uiCountAdded);

    // Inherited from NiFragmentMaterial. Creates the NiShader that will be
    // filled in by later methods.
    virtual NiShader* CreateShader(NiMaterialDescriptor* pkDesc);

    // Inherited from NiFragmentMaterial. Configures the shader's packing
    // structures for the requirements of this instance of the material. Note
    // that this method is considered deprecated, and GetVertexInputSemantics
    // should be used instead.
    virtual bool SetupPackingRequirements(NiShader* pkShader,
        NiMaterialDescriptor* pkMaterialDescriptor,
        RenderPassDescriptor* pkRenderPasses, unsigned int uiCount);

    // Shader fallback to be used when all other fallbacks fail. This fall back
    // will resort to using one of the replacement shaders that was inserted into
    // the shader cache at cache creation time. These represent bare bones shaders
    // that are exist to allow an object to be rendered.
    static bool UseDirShadowWriteReplacementShader(
        NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
        unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses,
        unsigned int uiMaxCount, unsigned int& uiCount,
        unsigned int& uiFunctionData);


    NiFixedString m_kDescriptorName;

};

NiSmartPointer(NiDirectionalShadowWriteMaterial);


#endif // NIDIRECTIONALSHADOWWRITEMATERIAL_H
