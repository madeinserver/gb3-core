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
#include "NiBinaryShaderLibPCH.h"

//--------------------------------------------------------------------------------------------------
#include "NiBinaryShaderLibrary.h"
#include "NSBRenderStates.h"
#include "NSBPackingDef.h"
#include "NSBStageAndSamplerStates.h"
#include "NSBStateGroup.h"
#include "NSBTextureStage.h"
#include "NSBTexture.h"
#include "NSBUtility.h"

#include <NiD3DUtility.h>
#include <NiD3DRenderStateGroup.h>
#include <NiD3DTextureStageGroup.h>
#include <NiD3DTextureStage.h>
#include <NiDX9Renderer.h>

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupRenderStateGroupFromNSBStateGroup(
    NSBStateGroup* pkGroup,
    NiD3DRenderStateGroup& kRSGroup)
{
    if (pkGroup->GetStateCount() == 0)
        return false;

    NiTListIterator kIter = 0;
    NSBStateGroup::NSBSGEntry* pkEntry = pkGroup->GetFirstState(kIter);
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the RenderStateGroup
            D3DRENDERSTATETYPE eRS;

            if (GetD3DRenderStateType(
                (NSBRenderStates::NSBRenderStateEnum)pkEntry->GetState(), eRS))
            {
                unsigned int uiValue;
                if (ConvertNSBRenderStateValue(
                    (NSBRenderStates::NSBRenderStateEnum)pkEntry->GetState(),
                    pkEntry->GetValue(), uiValue))
                {
                    if (eRS != 0x7fffffff)
                    {
                        kRSGroup.SetRenderState(eRS, uiValue,
                            pkEntry->IsSaved());
                    }
                }
            }
        }
        else
        {
            // Not supported yet!
            EE_FAIL("NiBinaryShaderLibrary::SetupRenderStateGroupFromNSBStateGroup>"
                " Attribute State support not yet implemented!");
        }

        pkEntry = pkGroup->GetNextState(kIter);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupTextureStageGroupFromNSBStateGroup(
    NSBStateGroup* pkGroup,
    NiD3DTextureStageGroup& kTSGroup)
{
    if (pkGroup->GetStateCount() == 0)
        return false;

    NiTListIterator kIter = 0;
    NSBStateGroup::NSBSGEntry* pkEntry = pkGroup->GetFirstState(kIter);
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the TextureStateGroup
            unsigned int uiTSST;

            if (GetD3DTextureStageState(
                (NSBStageAndSamplerStates::NSBTextureStageState)
                    pkEntry->GetState(),
                uiTSST))
            {
                unsigned int uiValue;
                if (ConvertNSBTextureStageStateValue(
                    (NSBStageAndSamplerStates::NSBTextureStageState)
                        pkEntry->GetState(), pkEntry->GetValue(), uiValue))
                {
                    if (uiTSST != 0x7fffffff)
                    {
                        kTSGroup.SetStageState(uiTSST, uiValue,
                            pkEntry->IsSaved());
                    }
                }
            }
        }
        else
        {
            // Not support yet!
            EE_FAIL("NiBinaryShaderLibrary::SetupTextureStageGroupFromNSBStateGroup>"
                " Attribute State support not yet implemented!");
        }

        pkEntry = pkGroup->GetNextState(kIter);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupTextureSamplerGroupFromNSBStateGroup(NSBStateGroup* pkGroup,
    NiD3DTextureStageGroup& kTSGroup)
{
    if (pkGroup->GetStateCount() == 0)
        return false;

    NiTListIterator kIter = 0;
    NSBStateGroup::NSBSGEntry* pkEntry = pkGroup->GetFirstState(kIter);
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the TextureStateGroup
            unsigned int uiValue = 0x7fffffff;
            if (pkEntry->UsesMapValue() ||
                ConvertNSBTextureSamplerStateValue(
                (NSBStageAndSamplerStates::NSBTextureSamplerState)
                    pkEntry->GetState(), pkEntry->GetValue(), uiValue))
            {
                if (pkEntry->GetState() != 0x7fffffff)
                {
                    kTSGroup.SetSamplerState(pkEntry->GetState(),
                        uiValue, pkEntry->IsSaved(), pkEntry->UsesMapValue());
                }
            }
        }
        else
        {
            // Not support yet!
            EE_FAIL("NiBinaryShaderLibrary::SetupTextureSamplerGroupFromNSBStateGroup>"
                "Attribute State support not yet implemented!");
        }

        pkEntry = pkGroup->GetNextState(kIter);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupTextureStageFromNSBTextureStage(
    NSBTextureStage* pkNSBStage, 
    NiD3DTextureStage& kStage)
{
    kStage.SetStage(pkNSBStage->GetStage());
    kStage.SetTextureFlags(pkNSBStage->GetTextureFlags());
    kStage.SetTextureTransformFlags(pkNSBStage->GetTextureTransformFlags(),
        pkNSBStage->GetGlobalName());
    kStage.SetObjTextureFlags(pkNSBStage->GetObjTextureFlags());

    NiD3DTextureStageGroup* pkStageGroup = kStage.GetTextureStageGroup();
    NSBStateGroup* pkTextureStageGroup = pkNSBStage->GetTextureStageGroup();
    if (pkTextureStageGroup)
    {
        if (pkTextureStageGroup->GetStateCount())
        {
            EE_ASSERT(pkStageGroup);
            if (!SetupTextureStageGroupFromNSBStateGroup(pkTextureStageGroup,
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    NSBStateGroup* pkSamplerGroup = pkNSBStage->GetSamplerStageGroup();
    if (pkSamplerGroup)
    {
        if (pkSamplerGroup->GetStateCount())
        {
            EE_ASSERT(pkStageGroup);
            if (!SetupTextureSamplerGroupFromNSBStateGroup(pkSamplerGroup,
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    kStage.SetTextureTransformation(
        *((D3DMATRIX*)pkNSBStage->GetTextureTransformation()));

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ModifyTextureStageFromNSBTexture(
    NSBTexture* pkNSBTexture, 
    NiD3DTextureStage& kStage)
{
    EE_ASSERT(kStage.GetStage() == pkNSBTexture->GetStage());

    // Override all texture flags except for
    // NiTextureStage::TSTF_MAP_USE_INDEX
    unsigned int uiCurrentTextureFlags = kStage.GetTextureFlags();
    kStage.SetTextureFlags(
        (uiCurrentTextureFlags & NiTextureStage::TSTF_MAP_USE_INDEX) |
        (pkNSBTexture->GetTextureFlags() & ~NiTextureStage::TSTF_MAP_USE_INDEX));

    // Override object texture flags
    kStage.SetObjTextureFlags(pkNSBTexture->GetObjTextureFlags());

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureStageState(
    NSBStageAndSamplerStates::NSBTextureStageState eTSS, 
    unsigned int& uiD3DValue)
{
    switch (eTSS)
    {
    case NSBStageAndSamplerStates::NSB_TSS_COLOROP:
        uiD3DValue = (unsigned int)D3DTSS_COLOROP;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG0:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG0;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG1:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG1;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG2:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG2;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAOP:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAOP;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG0:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG0;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG1:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG1;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG2:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG2;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_RESULTARG:
        uiD3DValue = (unsigned int)D3DTSS_RESULTARG;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT00:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT00;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT01:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT01;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT10:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT10;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT11:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT11;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVLSCALE:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVLSCALE;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVLOFFSET:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVLOFFSET;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_TEXCOORDINDEX:
        uiD3DValue = (unsigned int)D3DTSS_TEXCOORDINDEX;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_TEXTURETRANSFORMFLAGS:
        uiD3DValue = (unsigned int)D3DTSS_TEXTURETRANSFORMFLAGS;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ConvertNSBTextureStageStateValue(
    NSBStageAndSamplerStates::NSBTextureStageState eTSS, unsigned int uiNSBValue,
    unsigned int& uiD3DValue)
{
    switch (eTSS)
    {
    case NSBStageAndSamplerStates::NSB_TSS_COLOROP:
        return GetD3DTextureOp((NSBStageAndSamplerStates::NSBTextureOp)uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG0:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG1:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_COLORARG2:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAOP:
        return GetD3DTextureOp((NSBStageAndSamplerStates::NSBTextureOp)uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG0:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG1:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_ALPHAARG2:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_RESULTARG:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT00:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT01:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT10:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT11:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVLSCALE:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_BUMPENVLOFFSET:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_TSS_TEXCOORDINDEX:
        return GetD3DTexCoordIndexFlags(uiNSBValue, uiD3DValue);
    case NSBStageAndSamplerStates::NSB_TSS_TEXTURETRANSFORMFLAGS:
        return GetD3DTextureTransformFlags(
            (NSBStageAndSamplerStates::NSBTextureTransformFlags)uiNSBValue, uiD3DValue);
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ConvertNSBTextureSamplerStateValue(
    NSBStageAndSamplerStates::NSBTextureSamplerState eTSS, unsigned int uiNSBValue,
    unsigned int& uiD3DValue)
{
    switch (eTSS)
    {
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSU:
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSV:
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSW:
        return GetD3DTextureAddress((NSBStageAndSamplerStates::NSBTextureAddress)uiNSBValue,
            uiD3DValue);
    case NSBStageAndSamplerStates::NSB_SAMP_BORDERCOLOR:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MAGFILTER:
    case NSBStageAndSamplerStates::NSB_SAMP_MINFILTER:
    case NSBStageAndSamplerStates::NSB_SAMP_MIPFILTER:
        return GetD3DTextureFilter((NSBStageAndSamplerStates::NSBTextureFilter)uiNSBValue,
            uiD3DValue);
    case NSBStageAndSamplerStates::NSB_SAMP_MIPMAPLODBIAS:
    case NSBStageAndSamplerStates::NSB_SAMP_MAXMIPLEVEL:
    case NSBStageAndSamplerStates::NSB_SAMP_MAXANISOTROPY:
        uiD3DValue = uiNSBValue;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_SRGBTEXTURE:
    case NSBStageAndSamplerStates::NSB_SAMP_ELEMENTINDEX:
    case NSBStageAndSamplerStates::NSB_SAMP_DMAPOFFSET:
        uiD3DValue = uiNSBValue;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureOp(
    NSBStageAndSamplerStates::NSBTextureOp eTOP,
    unsigned int& uiD3DValue)
{
    switch (eTOP)
    {
    case NSBStageAndSamplerStates::NSB_TOP_DISABLE:
        uiD3DValue = (unsigned int)D3DTOP_DISABLE;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_SELECTARG1:
        uiD3DValue = (unsigned int)D3DTOP_SELECTARG1;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_SELECTARG2:
        uiD3DValue = (unsigned int)D3DTOP_SELECTARG2;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATE:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATE2X:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE2X;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATE4X:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE4X;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_ADD:
        uiD3DValue = (unsigned int)D3DTOP_ADD;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_ADDSIGNED:
        uiD3DValue = (unsigned int)D3DTOP_ADDSIGNED;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_ADDSIGNED2X:
        uiD3DValue = (unsigned int)D3DTOP_ADDSIGNED2X;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_SUBTRACT:
        uiD3DValue = (unsigned int)D3DTOP_SUBTRACT;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_ADDSMOOTH:
        uiD3DValue = (unsigned int)D3DTOP_ADDSMOOTH;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BLENDDIFFUSEALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDDIFFUSEALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BLENDTEXTUREALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDTEXTUREALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BLENDFACTORALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDFACTORALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BLENDTEXTUREALPHAPM:
        uiD3DValue = (unsigned int)D3DTOP_BLENDTEXTUREALPHAPM;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BLENDCURRENTALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDCURRENTALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_PREMODULATE:
        uiD3DValue = (unsigned int)D3DTOP_PREMODULATE;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATEALPHA_ADDCOLOR:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEALPHA_ADDCOLOR;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATECOLOR_ADDALPHA:
        uiD3DValue = (unsigned int)D3DTOP_MODULATECOLOR_ADDALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATEINVALPHA_ADDCOLOR:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEINVALPHA_ADDCOLOR;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MODULATEINVCOLOR_ADDALPHA:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEINVCOLOR_ADDALPHA;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BUMPENVMAP:
        uiD3DValue = (unsigned int)D3DTOP_BUMPENVMAP;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_BUMPENVMAPLUMINANCE:
        uiD3DValue = (unsigned int)D3DTOP_BUMPENVMAPLUMINANCE;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_DOTPRODUCT3:
        uiD3DValue = (unsigned int)D3DTOP_DOTPRODUCT3;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_MULTIPLYADD:
        uiD3DValue = (unsigned int)D3DTOP_MULTIPLYADD;
        return true;
    case NSBStageAndSamplerStates::NSB_TOP_LERP:
        uiD3DValue = (unsigned int)D3DTOP_LERP;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureArg(
    unsigned int uiTA,
    unsigned int& uiD3DValue)
{
    if (uiTA == NSBStageAndSamplerStates::NSB_TA_INVALID)
        return false;

    unsigned int uiArg = uiTA & 0x0FFFFFFF;

    switch (uiArg)
    {
    case NSBStageAndSamplerStates::NSB_TA_CURRENT:
        uiD3DValue = (unsigned int)D3DTA_CURRENT;
        break;
    case NSBStageAndSamplerStates::NSB_TA_DIFFUSE:
        uiD3DValue = (unsigned int)D3DTA_DIFFUSE;
        break;
    case NSBStageAndSamplerStates::NSB_TA_SELECTMASK:
        uiD3DValue = (unsigned int)D3DTA_SELECTMASK;
        break;
    case NSBStageAndSamplerStates::NSB_TA_SPECULAR:
        uiD3DValue = (unsigned int)D3DTA_SPECULAR;
        break;
    case NSBStageAndSamplerStates::NSB_TA_TEMP:
        uiD3DValue = (unsigned int)D3DTA_TEMP;
        break;
    case NSBStageAndSamplerStates::NSB_TA_TEXTURE:
        uiD3DValue = (unsigned int)D3DTA_TEXTURE;
        break;
    case NSBStageAndSamplerStates::NSB_TA_TFACTOR:
        uiD3DValue = (unsigned int)D3DTA_TFACTOR;
        break;
    }

    if (uiTA & NSBStageAndSamplerStates::NSB_TA_ALPHAREPLICATE)
        uiD3DValue |= D3DTA_ALPHAREPLICATE;
    if (uiTA & NSBStageAndSamplerStates::NSB_TA_COMPLEMENT)
        uiD3DValue |= D3DTA_COMPLEMENT;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureTransformFlags(
    NSBStageAndSamplerStates::NSBTextureTransformFlags eTTF, 
    unsigned int& uiD3DValue)
{
    uiD3DValue = 0;

    switch (eTTF & ~NSBStageAndSamplerStates::NSB_TTFF_PROJECTED)
    {
    case NSBStageAndSamplerStates::NSB_TTFF_DISABLE:
        uiD3DValue = (unsigned int)D3DTTFF_DISABLE;
        break;
    case NSBStageAndSamplerStates::NSB_TTFF_COUNT1:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT1;
        break;
    case NSBStageAndSamplerStates::NSB_TTFF_COUNT2:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT2;
        break;
    case NSBStageAndSamplerStates::NSB_TTFF_COUNT3:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT3;
        break;
    case NSBStageAndSamplerStates::NSB_TTFF_COUNT4:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT4;
        break;
    default:
        return false;
    }

    if (eTTF & NSBStageAndSamplerStates::NSB_TTFF_PROJECTED)
        uiD3DValue |= (unsigned int)D3DTTFF_PROJECTED;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTexCoordIndexFlags(
    unsigned int uiTCI,
    unsigned int& uiD3DValue)
{
    unsigned int uiFlags = uiTCI & 0xF0000000;
    unsigned int uiIndex = uiTCI & 0x0FFFFFFF;

    switch (uiFlags)
    {
    case NSBStageAndSamplerStates::NSB_TSI_PASSTHRU:
        uiD3DValue = (unsigned int)D3DTSS_TCI_PASSTHRU;
        break;
    case NSBStageAndSamplerStates::NSB_TSI_CAMERASPACENORMAL:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACENORMAL;
        break;
    case NSBStageAndSamplerStates::NSB_TSI_CAMERASPACEPOSITION:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACEPOSITION;
        break;
    case NSBStageAndSamplerStates::NSB_TSI_CAMERASPACEREFLECTIONVECTOR:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        break;
    case NSBStageAndSamplerStates::NSB_TSI_SPHEREMAP:
        uiD3DValue = (unsigned int)D3DTSS_TCI_SPHEREMAP;
        break;
    default:
        uiD3DValue = 0;
        break;
    }

    uiD3DValue |= uiIndex;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureSamplerState(
    NSBStageAndSamplerStates::NSBTextureSamplerState eTSS, 
    unsigned int& uiD3DValue)
{
    switch (eTSS)
    {
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSU:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSU;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSV:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSV;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_ADDRESSW:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSW;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_BORDERCOLOR:
        uiD3DValue = (unsigned int)D3DSAMP_BORDERCOLOR;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MAGFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MAGFILTER;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MINFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MINFILTER;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MIPFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MIPFILTER;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MIPMAPLODBIAS:
        uiD3DValue = (unsigned int)D3DSAMP_MIPMAPLODBIAS;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MAXMIPLEVEL:
        uiD3DValue = (unsigned int)D3DSAMP_MAXMIPLEVEL;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_MAXANISOTROPY:
        uiD3DValue = (unsigned int)D3DSAMP_MAXANISOTROPY;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_SRGBTEXTURE:
        uiD3DValue = (unsigned int)D3DSAMP_SRGBTEXTURE;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_ELEMENTINDEX:
        uiD3DValue = (unsigned int)D3DSAMP_ELEMENTINDEX;
        return true;
    case NSBStageAndSamplerStates::NSB_SAMP_DMAPOFFSET:
        uiD3DValue = (unsigned int)D3DSAMP_DMAPOFFSET;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureAddress(
    NSBStageAndSamplerStates::NSBTextureAddress eTA,
    unsigned int& uiD3DValue)
{
    switch (eTA)
    {
    case NSBStageAndSamplerStates::NSB_TADDRESS_WRAP:
        uiD3DValue = (unsigned int)D3DTADDRESS_WRAP;
        return true;
    case NSBStageAndSamplerStates::NSB_TADDRESS_MIRROR:
        uiD3DValue = (unsigned int)D3DTADDRESS_MIRROR;
        return true;
    case NSBStageAndSamplerStates::NSB_TADDRESS_CLAMP:
        uiD3DValue = (unsigned int)D3DTADDRESS_CLAMP;
        return true;
    case NSBStageAndSamplerStates::NSB_TADDRESS_BORDER:
        uiD3DValue = (unsigned int)D3DTADDRESS_BORDER;
        return true;
    case NSBStageAndSamplerStates::NSB_TADDRESS_MIRRORONCE:
        uiD3DValue = (unsigned int)D3DTADDRESS_MIRRORONCE;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTextureFilter(
    NSBStageAndSamplerStates::NSBTextureFilter eTF,
    unsigned int& uiD3DValue)
{
    switch (eTF)
    {
    case NSBStageAndSamplerStates::NSB_TEXF_NONE:
        uiD3DValue = (unsigned int)D3DTEXF_NONE;
        return true;
    case NSBStageAndSamplerStates::NSB_TEXF_POINT:
        uiD3DValue = (unsigned int)D3DTEXF_POINT;
        return true;
    case NSBStageAndSamplerStates::NSB_TEXF_LINEAR:
        uiD3DValue = (unsigned int)D3DTEXF_LINEAR;
        return true;
    case NSBStageAndSamplerStates::NSB_TEXF_ANISOTROPIC:
        uiD3DValue = (unsigned int)D3DTEXF_ANISOTROPIC;
        return true;
    case NSBStageAndSamplerStates::NSB_TEXF_PYRAMIDALQUAD:
        uiD3DValue = (unsigned int)D3DTEXF_PYRAMIDALQUAD;
        return true;
    case NSBStageAndSamplerStates::NSB_TEXF_GAUSSIANQUAD:
        uiD3DValue = (unsigned int)D3DTEXF_GAUSSIANQUAD;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------
NiShaderDeclaration* NiBinaryShaderLibrary::GetShaderDeclaration(NSBPackingDef* pkNSBPackingDef)
{
    // Determine the stream count, and the max entry count
    unsigned int uiStreamCount = 0;
    unsigned int uiMaxStreamEntryCount = 0;

    pkNSBPackingDef->GetStreamInfo(uiStreamCount, uiMaxStreamEntryCount);

    NiShaderDeclaration* pkShaderDecl =
        NiShaderDeclaration::Create(uiMaxStreamEntryCount, uiStreamCount);

    if (pkShaderDecl)
    {
        unsigned int uiEntry = 0;
        unsigned int uiStream = 0xFFFFFFFF;

        NiTListIterator kIter = 0;
        NSBPackingDef::NSBPDEntry* pkEntry = pkNSBPackingDef->GetFirstEntry(kIter);
        while (pkEntry)
        {
            // Add the entry to the shader declaration
            NiShaderDeclaration::ShaderParameterType eType;

            eType = ConvertPackingDefType(pkEntry->GetType());

            EE_ASSERT((unsigned int)eType != 0xffffffff);

            if (uiStream != pkEntry->GetStream())
            {
                uiStream = pkEntry->GetStream();
                uiEntry = 0;
            }

            if (pkEntry->GetUsage() == NiShaderDeclaration::SPUSAGE_COUNT)
            {
                pkShaderDecl->SetEntry(uiEntry,
                    (NiShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType, pkEntry->GetStream());
            }
            else
            {
                pkShaderDecl->SetEntry(pkEntry->GetStream(), uiEntry,
                    (NiShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType,
                    NiShaderDeclaration::UsageToString(pkEntry->GetUsage()),
                    pkEntry->GetUsageIndex(), pkEntry->GetTesselator());
            }

            uiEntry++;
            pkEntry = pkNSBPackingDef->GetNextEntry(kIter);
        }
    }

    return pkShaderDecl;
}

//--------------------------------------------------------------------------------------------------
NiShaderDeclaration::ShaderParameterType
NiBinaryShaderLibrary::ConvertPackingDefType(NSBPackingDef::NSBPackingDefEnum eType)
{
    switch (eType)
    {
    case NSBPackingDef::NSB_PD_FLOAT1:
        return NiShaderDeclaration::SPTYPE_FLOAT1;
    case NSBPackingDef::NSB_PD_FLOAT2:
        return NiShaderDeclaration::SPTYPE_FLOAT2;
    case NSBPackingDef::NSB_PD_FLOAT3:
        return NiShaderDeclaration::SPTYPE_FLOAT3;
    case NSBPackingDef::NSB_PD_FLOAT4:
        return NiShaderDeclaration::SPTYPE_FLOAT4;
    case NSBPackingDef::NSB_PD_UBYTECOLOR:
        return NiShaderDeclaration::SPTYPE_UBYTECOLOR;
    case NSBPackingDef::NSB_PD_UBYTE4:
        return NiShaderDeclaration::SPTYPE_UBYTE4;
    case NSBPackingDef::NSB_PD_SHORT2:
        return NiShaderDeclaration::SPTYPE_SHORT2;
    case NSBPackingDef::NSB_PD_SHORT4:
        return NiShaderDeclaration::SPTYPE_SHORT4;
    case NSBPackingDef::NSB_PD_NORMUBYTE4:
        return NiShaderDeclaration::SPTYPE_NORMUBYTE4;
    case NSBPackingDef::NSB_PD_NORMSHORT2:
        return NiShaderDeclaration::SPTYPE_NORMSHORT2;
    case NSBPackingDef::NSB_PD_NORMSHORT4:
        return NiShaderDeclaration::SPTYPE_NORMSHORT4;
    case NSBPackingDef::NSB_PD_NORMUSHORT2:
        return NiShaderDeclaration::SPTYPE_NORMUSHORT2;
    case NSBPackingDef::NSB_PD_NORMUSHORT4:
        return NiShaderDeclaration::SPTYPE_NORMUSHORT4;
    case NSBPackingDef::NSB_PD_UDEC3:
        return NiShaderDeclaration::SPTYPE_UDEC3;
    case NSBPackingDef::NSB_PD_NORMDEC3:
        return NiShaderDeclaration::SPTYPE_NORMDEC3;
    case NSBPackingDef::NSB_PD_FLOAT16_2:
        return NiShaderDeclaration::SPTYPE_FLOAT16_2;
    case NSBPackingDef::NSB_PD_FLOAT16_4:
        return NiShaderDeclaration::SPTYPE_FLOAT16_4;
    default:
        return (NiShaderDeclaration::ShaderParameterType)0xffffffff;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ConvertNSBRenderStateValue(
    NSBRenderStates::NSBRenderStateEnum eNSBState, unsigned int uiNSBValue,
    unsigned int& uiD3DValue)
{
    bool bResult = true;

    switch (eNSBState)
    {
        // Simple states
    case NSBRenderStates::NSB_RS_ZFUNC:
    case NSBRenderStates::NSB_RS_ALPHAFUNC:
    case NSBRenderStates::NSB_RS_STENCILFUNC:
    case NSBRenderStates::NSB_RS_CCW_STENCILFUNC:
        bResult = GetD3DCmpFunc((NSBRenderStates::NSBCmpFunc)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_SRCBLEND:
    case NSBRenderStates::NSB_RS_DESTBLEND:
        bResult = GetD3DBlend((NSBRenderStates::NSBBlend)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_SHADEMODE:
        bResult = GetD3DShadeMode((NSBRenderStates::NSBShadeMode)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_STENCILZFAIL:
    case NSBRenderStates::NSB_RS_STENCILPASS:
    case NSBRenderStates::NSB_RS_STENCILFAIL:
    case NSBRenderStates::NSB_RS_CCW_STENCILFAIL:
    case NSBRenderStates::NSB_RS_CCW_STENCILZFAIL:
    case NSBRenderStates::NSB_RS_CCW_STENCILPASS:
        bResult = GetD3DStencilOp((NSBRenderStates::NSBStencilOp)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_BLENDOP:
        bResult = GetD3DBlendOp((NSBRenderStates::NSBBlendOp)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_FOGTABLEMODE:
    case NSBRenderStates::NSB_RS_FOGVERTEXMODE:
        bResult = GetD3DFogMode((NSBRenderStates::NSBFogMode)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_WRAP0:
    case NSBRenderStates::NSB_RS_WRAP1:
    case NSBRenderStates::NSB_RS_WRAP2:
    case NSBRenderStates::NSB_RS_WRAP3:
    case NSBRenderStates::NSB_RS_WRAP4:
    case NSBRenderStates::NSB_RS_WRAP5:
    case NSBRenderStates::NSB_RS_WRAP6:
    case NSBRenderStates::NSB_RS_WRAP7:
    case NSBRenderStates::NSB_RS_WRAP8:
    case NSBRenderStates::NSB_RS_WRAP9:
    case NSBRenderStates::NSB_RS_WRAP10:
    case NSBRenderStates::NSB_RS_WRAP11:
    case NSBRenderStates::NSB_RS_WRAP12:
    case NSBRenderStates::NSB_RS_WRAP13:
    case NSBRenderStates::NSB_RS_WRAP14:
    case NSBRenderStates::NSB_RS_WRAP15:
        bResult = GetD3DWrap((NSBRenderStates::NSBWrap)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_SPECULARMATERIALSOURCE:
    case NSBRenderStates::NSB_RS_DIFFUSEMATERIALSOURCE:
    case NSBRenderStates::NSB_RS_AMBIENTMATERIALSOURCE:
    case NSBRenderStates::NSB_RS_EMISSIVEMATERIALSOURCE:
        bResult = GetD3DMaterialColorSource(
            (NSBRenderStates::NSBMaterialColorSource)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_PATCHEDGESTYLE:
        bResult = GetD3DPatchEdgeStyle((NSBRenderStates::NSBPatchEdgeStyle)uiNSBValue,
            uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_VERTEXBLEND:
        bResult = GetD3DVertexBlendFlags((NSBRenderStates::NSBVertexBlendFlags)uiNSBValue,
            uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_FILLMODE:
        bResult = GetD3DFillMode((NSBRenderStates::NSBFillMode)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_ZENABLE:
        bResult = GetD3DZBufferType((NSBRenderStates::NSBZBufferType)uiNSBValue,
            uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_CULLMODE:
        bResult = GetD3DCull((NSBRenderStates::NSBCull)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_DEBUGMONITORTOKEN:
        bResult = GetD3DDebugMonitorTokens(
            (NSBRenderStates::NSBDebugMonitorTokens)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_POSITIONDEGREE:
    case NSBRenderStates::NSB_RS_NORMALDEGREE:
        bResult = GetD3DDegreeType((NSBRenderStates::NSBDegreeType)uiNSBValue, uiD3DValue);
        break;
    case NSBRenderStates::NSB_RS_TESSELLATIONMODE:
        bResult = GetD3DTessellationMode((NSBRenderStates::NSBTessellationMode)uiNSBValue,
            uiD3DValue);
        break;
    default:
        // Assume we are passing the value straight through!
        uiD3DValue = uiNSBValue;
        bResult = true;
        break;
    }

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DRenderStateType(
    NSBRenderStates::NSBRenderStateEnum eRenderState,
    D3DRENDERSTATETYPE& eD3DRS)
{
    switch (eRenderState)
    {
#if defined (WIN32) || defined (_XENON)
    case NSBRenderStates::NSB_RS_ZENABLE:
        eD3DRS = D3DRS_ZENABLE;
        return true;
    case NSBRenderStates::NSB_RS_FILLMODE:
        eD3DRS = D3DRS_FILLMODE;
        return true;
    case NSBRenderStates::NSB_RS_ZWRITEENABLE:
        eD3DRS = D3DRS_ZWRITEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_ALPHATESTENABLE:
        eD3DRS = D3DRS_ALPHATESTENABLE;
        return true;
    case NSBRenderStates::NSB_RS_SRCBLEND:
        eD3DRS = D3DRS_SRCBLEND;
        return true;
    case NSBRenderStates::NSB_RS_DESTBLEND:
        eD3DRS = D3DRS_DESTBLEND;
        return true;
    case NSBRenderStates::NSB_RS_CULLMODE:
        eD3DRS = D3DRS_CULLMODE;
        return true;
    case NSBRenderStates::NSB_RS_ZFUNC:
        eD3DRS = D3DRS_ZFUNC;
        return true;
    case NSBRenderStates::NSB_RS_ALPHAREF:
        eD3DRS = D3DRS_ALPHAREF;
        return true;
    case NSBRenderStates::NSB_RS_ALPHAFUNC:
        eD3DRS = D3DRS_ALPHAFUNC;
        return true;
    case NSBRenderStates::NSB_RS_ALPHABLENDENABLE:
        eD3DRS = D3DRS_ALPHABLENDENABLE;
        return true;
    case NSBRenderStates::NSB_RS_STENCILENABLE:
        eD3DRS = D3DRS_STENCILENABLE;
        return true;
    case NSBRenderStates::NSB_RS_STENCILFAIL:
        eD3DRS = D3DRS_STENCILFAIL;
        return true;
    case NSBRenderStates::NSB_RS_STENCILZFAIL:
        eD3DRS = D3DRS_STENCILZFAIL;
        return true;
    case NSBRenderStates::NSB_RS_STENCILPASS:
        eD3DRS = D3DRS_STENCILPASS;
        return true;
    case NSBRenderStates::NSB_RS_STENCILFUNC:
        eD3DRS = D3DRS_STENCILFUNC;
        return true;
    case NSBRenderStates::NSB_RS_STENCILREF:
        eD3DRS = D3DRS_STENCILREF;
        return true;
    case NSBRenderStates::NSB_RS_STENCILMASK:
        eD3DRS = D3DRS_STENCILMASK;
        return true;
    case NSBRenderStates::NSB_RS_STENCILWRITEMASK:
        eD3DRS = D3DRS_STENCILWRITEMASK;
        return true;
    case NSBRenderStates::NSB_RS_WRAP0:
        eD3DRS = D3DRS_WRAP0;
        return true;
    case NSBRenderStates::NSB_RS_WRAP1:
        eD3DRS = D3DRS_WRAP1;
        return true;
    case NSBRenderStates::NSB_RS_WRAP2:
        eD3DRS = D3DRS_WRAP2;
        return true;
    case NSBRenderStates::NSB_RS_WRAP3:
        eD3DRS = D3DRS_WRAP3;
        return true;
    case NSBRenderStates::NSB_RS_WRAP4:
        eD3DRS = D3DRS_WRAP4;
        return true;
    case NSBRenderStates::NSB_RS_WRAP5:
        eD3DRS = D3DRS_WRAP5;
        return true;
    case NSBRenderStates::NSB_RS_WRAP6:
        eD3DRS = D3DRS_WRAP6;
        return true;
    case NSBRenderStates::NSB_RS_WRAP7:
        eD3DRS = D3DRS_WRAP7;
        return true;
    case NSBRenderStates::NSB_RS_CLIPPLANEENABLE:
        eD3DRS = D3DRS_CLIPPLANEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_POINTSIZE:
        eD3DRS = D3DRS_POINTSIZE;
        return true;
    case NSBRenderStates::NSB_RS_POINTSIZE_MIN:
        eD3DRS = D3DRS_POINTSIZE_MIN;
        return true;
    case NSBRenderStates::NSB_RS_POINTSPRITEENABLE:
        eD3DRS = D3DRS_POINTSPRITEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_MULTISAMPLEANTIALIAS:
        eD3DRS = D3DRS_MULTISAMPLEANTIALIAS;
        return true;
    case NSBRenderStates::NSB_RS_MULTISAMPLEMASK:
        eD3DRS = D3DRS_MULTISAMPLEMASK;
        return true;
    case NSBRenderStates::NSB_RS_POINTSIZE_MAX:
        eD3DRS = D3DRS_POINTSIZE_MAX;
        return true;
    case NSBRenderStates::NSB_RS_COLORWRITEENABLE:
        eD3DRS = D3DRS_COLORWRITEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_BLENDOP:
        eD3DRS = D3DRS_BLENDOP;
        return true;
    case NSBRenderStates::NSB_RS_SCISSORTESTENABLE:
        eD3DRS = D3DRS_SCISSORTESTENABLE;
        return true;
    case NSBRenderStates::NSB_RS_SLOPESCALEDEPTHBIAS:
        eD3DRS = D3DRS_SLOPESCALEDEPTHBIAS;
        return true;
    case NSBRenderStates::NSB_RS_MINTESSELLATIONLEVEL:
        eD3DRS = D3DRS_MINTESSELLATIONLEVEL;
        return true;
    case NSBRenderStates::NSB_RS_MAXTESSELLATIONLEVEL:
        eD3DRS = D3DRS_MAXTESSELLATIONLEVEL;
        return true;
    case NSBRenderStates::NSB_RS_TWOSIDEDSTENCILMODE:
        eD3DRS = D3DRS_TWOSIDEDSTENCILMODE;
        return true;
    case NSBRenderStates::NSB_RS_CCW_STENCILFAIL:
        eD3DRS = D3DRS_CCW_STENCILFAIL;
        return true;
    case NSBRenderStates::NSB_RS_CCW_STENCILZFAIL:
        eD3DRS = D3DRS_CCW_STENCILZFAIL;
        return true;
    case NSBRenderStates::NSB_RS_CCW_STENCILPASS:
        eD3DRS = D3DRS_CCW_STENCILPASS;
        return true;
    case NSBRenderStates::NSB_RS_CCW_STENCILFUNC:
        eD3DRS = D3DRS_CCW_STENCILFUNC;
        return true;
    case NSBRenderStates::NSB_RS_COLORWRITEENABLE1:
        eD3DRS = D3DRS_COLORWRITEENABLE1;
        return true;
    case NSBRenderStates::NSB_RS_COLORWRITEENABLE2:
        eD3DRS = D3DRS_COLORWRITEENABLE2;
        return true;
    case NSBRenderStates::NSB_RS_COLORWRITEENABLE3:
        eD3DRS = D3DRS_COLORWRITEENABLE3;
        return true;
    case NSBRenderStates::NSB_RS_BLENDFACTOR:
        eD3DRS = D3DRS_BLENDFACTOR;
        return true;
    case NSBRenderStates::NSB_RS_DEPTHBIAS:
        eD3DRS = D3DRS_DEPTHBIAS;
        return true;
    case NSBRenderStates::NSB_RS_WRAP8:
        eD3DRS = D3DRS_WRAP8;
        return true;
    case NSBRenderStates::NSB_RS_WRAP9:
        eD3DRS = D3DRS_WRAP9;
        return true;
    case NSBRenderStates::NSB_RS_WRAP10:
        eD3DRS = D3DRS_WRAP10;
        return true;
    case NSBRenderStates::NSB_RS_WRAP11:
        eD3DRS = D3DRS_WRAP11;
        return true;
    case NSBRenderStates::NSB_RS_WRAP12:
        eD3DRS = D3DRS_WRAP12;
        return true;
    case NSBRenderStates::NSB_RS_WRAP13:
        eD3DRS = D3DRS_WRAP13;
        return true;
    case NSBRenderStates::NSB_RS_WRAP14:
        eD3DRS = D3DRS_WRAP14;
        return true;
    case NSBRenderStates::NSB_RS_WRAP15:
        eD3DRS = D3DRS_WRAP15;
        return true;
    case NSBRenderStates::NSB_RS_SEPARATEALPHABLENDENABLE:
        eD3DRS = D3DRS_SEPARATEALPHABLENDENABLE;
        return true;
    case NSBRenderStates::NSB_RS_SRCBLENDALPHA:
        eD3DRS = D3DRS_SRCBLENDALPHA;
        return true;
    case NSBRenderStates::NSB_RS_DESTBLENDALPHA:
        eD3DRS = D3DRS_DESTBLENDALPHA;
        return true;
    case NSBRenderStates::NSB_RS_BLENDOPALPHA:
        eD3DRS = D3DRS_BLENDOPALPHA;
        return true;
#endif //#if defined (WIN32) || defined (_XENON)

#if defined (WIN32)
    case NSBRenderStates::NSB_RS_SHADEMODE:
        eD3DRS = D3DRS_SHADEMODE;
        return true;
    case NSBRenderStates::NSB_RS_LASTPIXEL:
        eD3DRS = D3DRS_LASTPIXEL;
        return true;
    case NSBRenderStates::NSB_RS_DITHERENABLE:
        eD3DRS = D3DRS_DITHERENABLE;
        return true;
    case NSBRenderStates::NSB_RS_FOGENABLE:
        eD3DRS = D3DRS_FOGENABLE;
        return true;
    case NSBRenderStates::NSB_RS_SPECULARENABLE:
        eD3DRS = D3DRS_SPECULARENABLE;
        return true;
    case NSBRenderStates::NSB_RS_FOGCOLOR:
        eD3DRS = D3DRS_FOGCOLOR;
        return true;
    case NSBRenderStates::NSB_RS_FOGTABLEMODE:
        eD3DRS = D3DRS_FOGTABLEMODE;
        return true;
    case NSBRenderStates::NSB_RS_FOGSTART:
        eD3DRS = D3DRS_FOGSTART;
        return true;
    case NSBRenderStates::NSB_RS_FOGEND:
        eD3DRS = D3DRS_FOGEND;
        return true;
    case NSBRenderStates::NSB_RS_FOGDENSITY:
        eD3DRS = D3DRS_FOGDENSITY;
        return true;
    case NSBRenderStates::NSB_RS_RANGEFOGENABLE:
        eD3DRS = D3DRS_RANGEFOGENABLE;
        return true;
    case NSBRenderStates::NSB_RS_TEXTUREFACTOR:
        eD3DRS = D3DRS_TEXTUREFACTOR;
        return true;
    case NSBRenderStates::NSB_RS_CLIPPING:
        eD3DRS = D3DRS_CLIPPING;
        return true;
    case NSBRenderStates::NSB_RS_LIGHTING:
        eD3DRS = D3DRS_LIGHTING;
        return true;
    case NSBRenderStates::NSB_RS_AMBIENT:
        eD3DRS = D3DRS_AMBIENT;
        return true;
    case NSBRenderStates::NSB_RS_FOGVERTEXMODE:
        eD3DRS = D3DRS_FOGVERTEXMODE;
        return true;
    case NSBRenderStates::NSB_RS_COLORVERTEX:
        eD3DRS = D3DRS_COLORVERTEX;
        return true;
    case NSBRenderStates::NSB_RS_LOCALVIEWER:
        eD3DRS = D3DRS_LOCALVIEWER;
        return true;
    case NSBRenderStates::NSB_RS_NORMALIZENORMALS:
        eD3DRS = D3DRS_NORMALIZENORMALS;
        return true;
    case NSBRenderStates::NSB_RS_DIFFUSEMATERIALSOURCE :
        eD3DRS = D3DRS_DIFFUSEMATERIALSOURCE ;
        return true;
    case NSBRenderStates::NSB_RS_SPECULARMATERIALSOURCE:
        eD3DRS = D3DRS_SPECULARMATERIALSOURCE;
        return true;
    case NSBRenderStates::NSB_RS_AMBIENTMATERIALSOURCE:
        eD3DRS = D3DRS_AMBIENTMATERIALSOURCE;
        return true;
    case NSBRenderStates::NSB_RS_EMISSIVEMATERIALSOURCE:
        eD3DRS = D3DRS_EMISSIVEMATERIALSOURCE;
        return true;
    case NSBRenderStates::NSB_RS_VERTEXBLEND:
        eD3DRS = D3DRS_VERTEXBLEND;
        return true;
    case NSBRenderStates::NSB_RS_POINTSCALEENABLE:
        eD3DRS = D3DRS_POINTSCALEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_POINTSCALE_A:
        eD3DRS = D3DRS_POINTSCALE_A;
        return true;
    case NSBRenderStates::NSB_RS_POINTSCALE_B:
        eD3DRS = D3DRS_POINTSCALE_B;
        return true;
    case NSBRenderStates::NSB_RS_POINTSCALE_C:
        eD3DRS = D3DRS_POINTSCALE_C;
        return true;
    case NSBRenderStates::NSB_RS_PATCHEDGESTYLE:
        eD3DRS = D3DRS_PATCHEDGESTYLE;
        return true;
    case NSBRenderStates::NSB_RS_DEBUGMONITORTOKEN:
        eD3DRS = D3DRS_DEBUGMONITORTOKEN;
        return true;
    case NSBRenderStates::NSB_RS_INDEXEDVERTEXBLENDENABLE:
        eD3DRS = D3DRS_INDEXEDVERTEXBLENDENABLE;
        return true;
    case NSBRenderStates::NSB_RS_TWEENFACTOR:
        eD3DRS = D3DRS_TWEENFACTOR;
        return true;
    case NSBRenderStates::NSB_RS_POSITIONDEGREE:
        eD3DRS = D3DRS_POSITIONDEGREE;
        return true;
    case NSBRenderStates::NSB_RS_NORMALDEGREE:
        eD3DRS = D3DRS_NORMALDEGREE;
        return true;
    case NSBRenderStates::NSB_RS_ANTIALIASEDLINEENABLE:
        eD3DRS = D3DRS_ANTIALIASEDLINEENABLE;
        return true;
    case NSBRenderStates::NSB_RS_ADAPTIVETESS_X:
        eD3DRS = D3DRS_ADAPTIVETESS_X;
        return true;
    case NSBRenderStates::NSB_RS_ADAPTIVETESS_Y:
        eD3DRS = D3DRS_ADAPTIVETESS_Y;
        return true;
    case NSBRenderStates::NSB_RS_ADAPTIVETESS_Z:
        eD3DRS = D3DRS_ADAPTIVETESS_Z;
        return true;
    case NSBRenderStates::NSB_RS_ADAPTIVETESS_W:
        eD3DRS = D3DRS_ADAPTIVETESS_W;
        return true;
    case NSBRenderStates::NSB_RS_ENABLEADAPTIVETESSELLATION:
        eD3DRS = D3DRS_ENABLEADAPTIVETESSELLATION;
        return true;
    case NSBRenderStates::NSB_RS_SRGBWRITEENABLE:
        eD3DRS = D3DRS_SRGBWRITEENABLE;
        return true;
#endif //#if defined (WIN32)

#if defined (_XENON)
    case NSBRenderStates::NSB_RS_VIEWPORTENABLE:
        eD3DRS = D3DRS_VIEWPORTENABLE;
        return true;
    case NSBRenderStates::NSB_RS_HIGHPRECISIONBLENDENABLE:
        eD3DRS = D3DRS_HIGHPRECISIONBLENDENABLE;
        return true;
    case NSBRenderStates::NSB_RS_HIGHPRECISIONBLENDENABLE1:
        eD3DRS = D3DRS_HIGHPRECISIONBLENDENABLE1;
        return true;
    case NSBRenderStates::NSB_RS_HIGHPRECISIONBLENDENABLE2:
        eD3DRS = D3DRS_HIGHPRECISIONBLENDENABLE2;
        return true;
    case NSBRenderStates::NSB_RS_HIGHPRECISIONBLENDENABLE3:
        eD3DRS = D3DRS_HIGHPRECISIONBLENDENABLE3;
        return true;
    case NSBRenderStates::NSB_RS_TESSELLATIONMODE:
        eD3DRS = D3DRS_TESSELLATIONMODE;
        return true;
#endif //#if defined (_XENON)

    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DZBufferType(
    NSBRenderStates::NSBZBufferType eZBufferType,
    unsigned int& uiD3DValue)
{
    switch (eZBufferType)
    {
    case NSBRenderStates::NSB_ZB_FALSE:
        uiD3DValue = (unsigned int)D3DZB_FALSE;
        return true;
    case NSBRenderStates::NSB_ZB_TRUE:
        uiD3DValue = (unsigned int)D3DZB_TRUE;
        return true;
#if !defined(_XENON)
    case NSBRenderStates::NSB_ZB_USEW:
        uiD3DValue = (unsigned int)D3DZB_USEW;
        return true;
#endif  //#if !defined(_XENON)
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DFillMode(
    NSBRenderStates::NSBFillMode eFillMode,
    unsigned int& uiD3DValue)
{
    switch (eFillMode)
    {
    case NSBRenderStates::NSB_FILL_POINT:
        uiD3DValue = (unsigned int)D3DFILL_POINT;
        return true;
    case NSBRenderStates::NSB_FILL_WIREFRAME:
        uiD3DValue = (unsigned int)D3DFILL_WIREFRAME;
        return true;
    case NSBRenderStates::NSB_FILL_SOLID:
        uiD3DValue = (unsigned int)D3DFILL_SOLID;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DShadeMode(
    NSBRenderStates::NSBShadeMode eShadeMode,
    unsigned int& uiD3DValue)
{
#if !defined(_XENON)
    switch (eShadeMode)
    {
    case NSBRenderStates::NSB_SHADE_FLAT:
        uiD3DValue = (unsigned int)D3DSHADE_FLAT;
        return true;
    case NSBRenderStates::NSB_SHADE_GOURAUD:
        uiD3DValue = (unsigned int)D3DSHADE_GOURAUD;
        return true;
    case NSBRenderStates::NSB_SHADE_PHONG:
        uiD3DValue = (unsigned int)D3DSHADE_PHONG;
        return true;
    default:
        return false;
    }
#else
    return false;
#endif  //#if !defined(_XENON)
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DBlend(
    NSBRenderStates::NSBBlend eBlend,
    unsigned int& uiD3DValue)
{
    switch (eBlend)
    {
    case NSBRenderStates::NSB_BLEND_ZERO:
        uiD3DValue = (unsigned int)D3DBLEND_ZERO;
        return true;
    case NSBRenderStates::NSB_BLEND_ONE:
        uiD3DValue = (unsigned int)D3DBLEND_ONE;
        return true;
    case NSBRenderStates::NSB_BLEND_SRCCOLOR:
        uiD3DValue = (unsigned int)D3DBLEND_SRCCOLOR;
        return true;
    case NSBRenderStates::NSB_BLEND_INVSRCCOLOR:
        uiD3DValue = (unsigned int)D3DBLEND_INVSRCCOLOR;
        return true;
    case NSBRenderStates::NSB_BLEND_SRCALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_SRCALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_INVSRCALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_INVSRCALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_DESTALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_DESTALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_INVDESTALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_INVDESTALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_DESTCOLOR:
        uiD3DValue = (unsigned int)D3DBLEND_DESTCOLOR;
        return true;
    case NSBRenderStates::NSB_BLEND_INVDESTCOLOR:
        uiD3DValue = (unsigned int)D3DBLEND_INVDESTCOLOR;
        return true;
    case NSBRenderStates::NSB_BLEND_SRCALPHASAT:
        uiD3DValue = (unsigned int)D3DBLEND_SRCALPHASAT;
        return true;
#if defined(WIN32)
    case NSBRenderStates::NSB_BLEND_BOTHSRCALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_BOTHSRCALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_BOTHINVSRCALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_BOTHINVSRCALPHA;
        return true;
#endif  //#if defined(WIN32)
    case NSBRenderStates::NSB_BLEND_BLENDFACTOR:
        uiD3DValue = (unsigned int)D3DBLEND_BLENDFACTOR;
        return true;
    case NSBRenderStates::NSB_BLEND_INVBLENDFACTOR:
        uiD3DValue = (unsigned int)D3DBLEND_INVBLENDFACTOR;
        return true;
#if defined(_XENON)
    case NSBRenderStates::NSB_BLEND_CONSTANTALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_CONSTANTALPHA;
        return true;
    case NSBRenderStates::NSB_BLEND_INVCONSTANTALPHA:
        uiD3DValue = (unsigned int)D3DBLEND_INVCONSTANTALPHA;
        return true;
#endif  //#if defined(_XENON)
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DCull(
    NSBRenderStates::NSBCull eCull, 
    unsigned int& uiD3DValue)
{
    switch (eCull)
    {
    case NSBRenderStates::NSB_CULL_NONE:
        uiD3DValue = (unsigned int)D3DCULL_NONE;
        return true;
    case NSBRenderStates::NSB_CULL_CW:
        uiD3DValue = (unsigned int)D3DCULL_CW;
        return true;
    case NSBRenderStates::NSB_CULL_CCW:
        uiD3DValue = (unsigned int)D3DCULL_CCW;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DCmpFunc(
    NSBRenderStates::NSBCmpFunc eCmpFunc,
    unsigned int& uiD3DValue)
{
    switch (eCmpFunc)
    {
    case NSBRenderStates::NSB_CMP_NEVER:
        uiD3DValue = (unsigned int)D3DCMP_NEVER;
        return true;
    case NSBRenderStates::NSB_CMP_LESS:
        uiD3DValue = (unsigned int)D3DCMP_LESS;
        return true;
    case NSBRenderStates::NSB_CMP_EQUAL:
        uiD3DValue = (unsigned int)D3DCMP_EQUAL;
        return true;
    case NSBRenderStates::NSB_CMP_LESSEQUAL:
        uiD3DValue = (unsigned int)D3DCMP_LESSEQUAL;
        return true;
    case NSBRenderStates::NSB_CMP_GREATER:
        uiD3DValue = (unsigned int)D3DCMP_GREATER;
        return true;
    case NSBRenderStates::NSB_CMP_NOTEQUAL:
        uiD3DValue = (unsigned int)D3DCMP_NOTEQUAL;
        return true;
    case NSBRenderStates::NSB_CMP_GREATEREQUAL:
        uiD3DValue = (unsigned int)D3DCMP_GREATEREQUAL;
        return true;
    case NSBRenderStates::NSB_CMP_ALWAYS:
        uiD3DValue = (unsigned int)D3DCMP_ALWAYS;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DFogMode(
    NSBRenderStates::NSBFogMode eFogMode,
    unsigned int& uiD3DValue)
{
    switch (eFogMode)
    {
    case NSBRenderStates::NSB_FOG_NONE:
        uiD3DValue = (unsigned int)D3DFOG_NONE;
        return true;
    case NSBRenderStates::NSB_FOG_EXP:
        uiD3DValue = (unsigned int)D3DFOG_EXP;
        return true;
    case NSBRenderStates::NSB_FOG_EXP2:
        uiD3DValue = (unsigned int)D3DFOG_EXP2;
        return true;
    case NSBRenderStates::NSB_FOG_LINEAR:
        uiD3DValue = (unsigned int)D3DFOG_LINEAR;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DStencilOp(
    NSBRenderStates::NSBStencilOp eStencilOp,
    unsigned int& uiD3DValue)
{
    switch (eStencilOp)
    {
    case NSBRenderStates::NSB_STENCTIL_OP_KEEP:
        uiD3DValue = (unsigned int)D3DSTENCILOP_KEEP;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_ZERO:
        uiD3DValue = (unsigned int)D3DSTENCILOP_ZERO;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_REPLACE:
        uiD3DValue = (unsigned int)D3DSTENCILOP_REPLACE;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_INCRSAT:
        uiD3DValue = (unsigned int)D3DSTENCILOP_INCRSAT;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_DECRSAT:
        uiD3DValue = (unsigned int)D3DSTENCILOP_DECRSAT;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_INVERT:
        uiD3DValue = (unsigned int)D3DSTENCILOP_INVERT;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_INCR:
        uiD3DValue = (unsigned int)D3DSTENCILOP_INCR;
        return true;
    case NSBRenderStates::NSB_STENCTIL_OP_DECR:
        uiD3DValue = (unsigned int)D3DSTENCILOP_DECR;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DWrap(
    NSBRenderStates::NSBWrap eWrap, 
    unsigned int& uiD3DValue)
{
    switch (eWrap)
    {
    case NSBRenderStates::NSB_WRAP_DISABLED:
        uiD3DValue = 0;
        return true;
    case NSBRenderStates::NSB_WRAP_U:
        uiD3DValue = (unsigned int)D3DWRAP_U;
        return true;
    case NSBRenderStates::NSB_WRAP_V:
        uiD3DValue = (unsigned int)D3DWRAP_V;
        return true;
    case NSBRenderStates::NSB_WRAP_W:
        uiD3DValue = (unsigned int)D3DWRAP_W;
        return true;
    case NSBRenderStates::NSB_WRAP_UV:
        uiD3DValue = (unsigned int)(D3DWRAP_U | D3DWRAP_V);
        return true;
    case NSBRenderStates::NSB_WRAP_UW:
        uiD3DValue = (unsigned int)(D3DWRAP_U | D3DWRAP_W);
        return true;
    case NSBRenderStates::NSB_WRAP_VW:
        uiD3DValue = (unsigned int)(D3DWRAP_V | D3DWRAP_W);
        return true;
    case NSBRenderStates::NSB_WRAP_UVW:
        uiD3DValue = (unsigned int)(D3DWRAP_U | D3DWRAP_V | D3DWRAP_W);
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DMaterialColorSource(
    NSBRenderStates::NSBMaterialColorSource eMaterialColorSource, 
    unsigned int& uiD3DValue)
{
    switch (eMaterialColorSource)
    {
    case NSBRenderStates::NSB_MCS_MATERIAL:
        uiD3DValue = (unsigned int)D3DMCS_MATERIAL;
        return true;
    case NSBRenderStates::NSB_MCS_COLOR1:
        uiD3DValue = (unsigned int)D3DMCS_COLOR1;
        return true;
    case NSBRenderStates::NSB_MCS_COLOR2:
        uiD3DValue = (unsigned int)D3DMCS_COLOR2;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DVertexBlendFlags(
    NSBRenderStates::NSBVertexBlendFlags eVertexBlendFlags, 
    unsigned int& uiD3DValue)
{

#if defined (WIN32)
    EE_ASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
#endif

#if defined(WIN32)
    switch (eVertexBlendFlags)
    {
    case NSBRenderStates::NSB_VBF_DISABLE:
        uiD3DValue = (unsigned int)D3DVBF_DISABLE;
        return true;
    case NSBRenderStates::NSB_VBF_1WEIGHTS:
        uiD3DValue = (unsigned int)D3DVBF_1WEIGHTS;
        return true;
    case NSBRenderStates::NSB_VBF_2WEIGHTS:
        uiD3DValue = (unsigned int)D3DVBF_2WEIGHTS;
        return true;
    case NSBRenderStates::NSB_VBF_3WEIGHTS:
        uiD3DValue = (unsigned int)D3DVBF_3WEIGHTS;
        return true;
    case NSBRenderStates::NSB_VBF_TWEENING:
        uiD3DValue = (unsigned int)D3DVBF_TWEENING;
        return true;
    case NSBRenderStates::NSB_VBF_0WEIGHTS:
        uiD3DValue = (unsigned int)D3DVBF_0WEIGHTS;
        return true;
    default:
        return false;
    }
#else
    return false;
#endif  //#if defined(WIN32)
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DPatchEdgeStyle(
    NSBRenderStates::NSBPatchEdgeStyle ePatchEdgeStyle, 
    unsigned int& uiD3DValue)
{
    switch (ePatchEdgeStyle)
    {
    case NSBRenderStates::NSB_PATCH_EDGE_DISCRETE:
        uiD3DValue = (unsigned int)D3DPATCHEDGE_DISCRETE;
        return true;
    case NSBRenderStates::NSB_PATCH_EDGE_CONTINUOUS:
        uiD3DValue = (unsigned int)D3DPATCHEDGE_CONTINUOUS;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DDebugMonitorTokens(
    NSBRenderStates::NSBDebugMonitorTokens eDebugMonitorTokens, 
    unsigned int& uiD3DValue)
{

#if defined (WIN32)
    EE_ASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
#endif

#if defined(WIN32)
    switch (eDebugMonitorTokens)
    {
    case NSBRenderStates::NSB_DMT_ENABLE:
        uiD3DValue = (unsigned int)D3DDMT_ENABLE;
        return true;
    case NSBRenderStates::NSB_DMT_DISABLE:
        uiD3DValue = (unsigned int)D3DDMT_DISABLE;
        return true;
    default:
        return false;
    }
#else
    return false;
#endif  //#if defined(WIN32)
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DBlendOp(
    NSBRenderStates::NSBBlendOp eBlendOp,
    unsigned int& uiD3DValue)
{
    switch (eBlendOp)
    {
    case NSBRenderStates::NSB_BLENDOP_ADD:
        uiD3DValue = (unsigned int)D3DBLENDOP_ADD;
        return true;
    case NSBRenderStates::NSB_BLENDOP_SUBTRACT:
        uiD3DValue = (unsigned int)D3DBLENDOP_SUBTRACT;
        return true;
    case NSBRenderStates::NSB_BLENDOP_REVSUBTRACT:
        uiD3DValue = (unsigned int)D3DBLENDOP_REVSUBTRACT;
        return true;
    case NSBRenderStates::NSB_BLENDOP_MIN:
        uiD3DValue = (unsigned int)D3DBLENDOP_MIN;
        return true;
    case NSBRenderStates::NSB_BLENDOP_MAX:
        uiD3DValue = (unsigned int)D3DBLENDOP_MAX;
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DDegreeType(
    NSBRenderStates::NSBDegreeType eDegreeType,
    unsigned int& uiD3DValue)
{
    switch (eDegreeType)
    {
    case NSBRenderStates::NSB_DEGREE_LINEAR:
        uiD3DValue = (unsigned int)D3DDEGREE_LINEAR;
        return true;
    case NSBRenderStates::NSB_DEGREE_QUADRATIC:
        uiD3DValue = (unsigned int)D3DDEGREE_QUADRATIC;
        return true;
    case NSBRenderStates::NSB_DEGREE_CUBIC:
        uiD3DValue = (unsigned int)D3DDEGREE_CUBIC;
        return true;
    case NSBRenderStates::NSB_DEGREE_QUINTIC:
        uiD3DValue = (unsigned int)D3DDEGREE_QUINTIC;
        return true;
    default:
        return false;
    };
}

//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::GetD3DTessellationMode(
    NSBRenderStates::NSBTessellationMode eTessMode,
    unsigned int& uiD3DValue)
{
#if defined(_XENON)
    switch (eTessMode)
    {
    case NSBRenderStates::NSB_TM_DISCRETE:
        uiD3DValue = (unsigned int)D3DTM_DISCRETE;
        return true;
    case NSBRenderStates::NSB_TM_CONTINUOUS:
        uiD3DValue = (unsigned int)NSBRenderStates::NSB_TM_CONTINUOUS;
        return true;
    case NSBRenderStates::NSB_TM_PEREDGE:
        uiD3DValue = (unsigned int)NSBRenderStates::NSB_TM_PEREDGE;
        return true;
    default:
        return false;
    };
#else
    EE_UNUSED_ARG(eTessMode);
    EE_UNUSED_ARG(uiD3DValue); 
    return false;
#endif  //#if defined(_XENON)
}

//--------------------------------------------------------------------------------------------------
