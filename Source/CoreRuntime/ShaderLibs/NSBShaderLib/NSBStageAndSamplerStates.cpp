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

#include "NSBStageAndSamplerStates.h"
#if defined(NIDEBUG)
#include "NSBRenderStates.h"
#include "NSBUtility.h"

#include <NiSystem.h>

//--------------------------------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureStageString(
    NSBStageAndSamplerStates::NSBTextureStageState eState)
{
    switch (eState)
    {
    STATE_CASE_STRING(NSB_TSS_COLOROP)
    STATE_CASE_STRING(NSB_TSS_COLORARG0)
    STATE_CASE_STRING(NSB_TSS_COLORARG1)
    STATE_CASE_STRING(NSB_TSS_COLORARG2)
    STATE_CASE_STRING(NSB_TSS_ALPHAOP)
    STATE_CASE_STRING(NSB_TSS_ALPHAARG0)
    STATE_CASE_STRING(NSB_TSS_ALPHAARG1)
    STATE_CASE_STRING(NSB_TSS_ALPHAARG2)
    STATE_CASE_STRING(NSB_TSS_RESULTARG)
    STATE_CASE_STRING(NSB_TSS_BUMPENVMAT00)
    STATE_CASE_STRING(NSB_TSS_BUMPENVMAT01)
    STATE_CASE_STRING(NSB_TSS_BUMPENVMAT10)
    STATE_CASE_STRING(NSB_TSS_BUMPENVMAT11)
    STATE_CASE_STRING(NSB_TSS_BUMPENVLSCALE)
    STATE_CASE_STRING(NSB_TSS_BUMPENVLOFFSET)
    STATE_CASE_STRING(NSB_TSS_TEXCOORDINDEX)
    STATE_CASE_STRING(NSB_TSS_TEXTURETRANSFORMFLAGS)
    default:
        return "***** UNKNOWN STAGE STATE *****";
    }
}

//--------------------------------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureStageValueString(
    NSBStageAndSamplerStates::NSBTextureStageState eState, unsigned int uiValue)
{
    static char s_acTemp[256];

    NiStrcpy(s_acTemp, 256, "*** USS ***");

    switch (eState)
    {
    case NSB_TSS_COLOROP:
    case NSB_TSS_ALPHAOP:
        {
            switch (uiValue)
            {
            STATE_CASE_STRING(NSB_TOP_DISABLE)
            STATE_CASE_STRING(NSB_TOP_SELECTARG1)
            STATE_CASE_STRING(NSB_TOP_SELECTARG2)
            STATE_CASE_STRING(NSB_TOP_MODULATE)
            STATE_CASE_STRING(NSB_TOP_MODULATE2X)
            STATE_CASE_STRING(NSB_TOP_MODULATE4X)
            STATE_CASE_STRING(NSB_TOP_ADD)
            STATE_CASE_STRING(NSB_TOP_ADDSIGNED)
            STATE_CASE_STRING(NSB_TOP_ADDSIGNED2X)
            STATE_CASE_STRING(NSB_TOP_SUBTRACT)
            STATE_CASE_STRING(NSB_TOP_ADDSMOOTH)
            STATE_CASE_STRING(NSB_TOP_BLENDDIFFUSEALPHA)
            STATE_CASE_STRING(NSB_TOP_BLENDTEXTUREALPHA)
            STATE_CASE_STRING(NSB_TOP_BLENDFACTORALPHA)
            STATE_CASE_STRING(NSB_TOP_BLENDTEXTUREALPHAPM)
            STATE_CASE_STRING(NSB_TOP_BLENDCURRENTALPHA)
            STATE_CASE_STRING(NSB_TOP_PREMODULATE)
            STATE_CASE_STRING(NSB_TOP_MODULATEALPHA_ADDCOLOR)
            STATE_CASE_STRING(NSB_TOP_MODULATECOLOR_ADDALPHA)
            STATE_CASE_STRING(NSB_TOP_MODULATEINVALPHA_ADDCOLOR)
            STATE_CASE_STRING(NSB_TOP_MODULATEINVCOLOR_ADDALPHA)
            STATE_CASE_STRING(NSB_TOP_BUMPENVMAP)
            STATE_CASE_STRING(NSB_TOP_BUMPENVMAPLUMINANCE)
            STATE_CASE_STRING(NSB_TOP_DOTPRODUCT3)
            STATE_CASE_STRING(NSB_TOP_MULTIPLYADD)
            STATE_CASE_STRING(NSB_TOP_LERP)
            default:
                return "*** USS ***";
            }
        }
        break;
    case NSB_TSS_COLORARG0:
    case NSB_TSS_COLORARG1:
    case NSB_TSS_COLORARG2:
    case NSB_TSS_ALPHAARG0:
    case NSB_TSS_ALPHAARG1:
    case NSB_TSS_ALPHAARG2:
    case NSB_TSS_RESULTARG:
        {
            unsigned int uiArg = uiValue & 0x0fffffff;
            switch (uiArg)
            {
            case NSB_TA_CURRENT:
                NiSprintf(s_acTemp, 256, "CURRENT");
                break;
            case NSB_TA_DIFFUSE:
                NiSprintf(s_acTemp, 256, "DIFFUSE");
                break;
            case NSB_TA_SELECTMASK:
                NiSprintf(s_acTemp, 256, "SELECTMASK");
                break;
            case NSB_TA_SPECULAR:
                NiSprintf(s_acTemp, 256, "SPECULAR");
                break;
            case NSB_TA_TEMP:
                NiSprintf(s_acTemp, 256, "TEMP");
                break;
            case NSB_TA_TEXTURE:
                NiSprintf(s_acTemp, 256, "TEXTURE");
                break;
            case NSB_TA_TFACTOR:
                NiSprintf(s_acTemp, 256, "TFACTOR");
                break;
            default:
                NiSprintf(s_acTemp, 256, "*** USS ***");
                break;
            }

            if (uiValue & NSB_TA_ALPHAREPLICATE)
                NiStrcat(s_acTemp, 256, "   ALPHAREPLICATE");
            if (uiValue & NSB_TA_COMPLEMENT)
                NiStrcat(s_acTemp, 256, "   COMPLEMENT");
        }
        break;

    case NSB_TSS_BUMPENVMAT00:
    case NSB_TSS_BUMPENVMAT01:
    case NSB_TSS_BUMPENVMAT10:
    case NSB_TSS_BUMPENVMAT11:
    case NSB_TSS_BUMPENVLSCALE:
    case NSB_TSS_BUMPENVLOFFSET:
        {
            float fValue = *((float*)&uiValue);
            NiSprintf(s_acTemp, 256, "%8.5f", fValue);
        }
        break;
    case NSB_TSS_TEXCOORDINDEX:
        {
            NiSprintf(s_acTemp, 256, "%2d - ", uiValue & 0x0fffffff);
            switch (uiValue & 0xf0000000)
            {
            case NSB_TSI_PASSTHRU:
                NiStrcat(s_acTemp, 256, "PASSTHRU");
                break;
            case NSB_TSI_CAMERASPACENORMAL:
                NiStrcat(s_acTemp, 256, "CAMERASPACENORMAL");
                break;
            case NSB_TSI_CAMERASPACEPOSITION:
                NiStrcat(s_acTemp, 256, "CAMERASPACEPOSITION");
                break;
            case NSB_TSI_CAMERASPACEREFLECTIONVECTOR:
                NiStrcat(s_acTemp, 256, "CAMERASPACEREFLECTIONVECTOR");
                break;
            case NSB_TSI_SPHEREMAP:
                NiStrcat(s_acTemp, 256, "SPHEREMAP");
                break;
            default:
                NiStrcat(s_acTemp, 256, "*** USS ***");
                break;
            }
        }
        break;
    case NSB_TSS_TEXTURETRANSFORMFLAGS:
        {
            switch (uiValue)
            {
            case NSB_TTFF_DISABLE:     return "DISABLE";
            case NSB_TTFF_COUNT1:      return "COUNT1";
            case NSB_TTFF_COUNT2:      return "COUNT2";
            case NSB_TTFF_COUNT3:      return "COUNT3";
            case NSB_TTFF_COUNT4:      return "COUNT4";
            case NSB_TTFF_PROJECTED:   return "PROJECTED";
            default:                   return "*** USS ***";
            }
        }
        break;
    default:
        break;
    }
    return s_acTemp;
}

//--------------------------------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureSamplerString(
    NSBStageAndSamplerStates::NSBTextureSamplerState eState)
{
    switch (eState)
    {
    STATE_CASE_STRING(NSB_SAMP_ADDRESSU)
    STATE_CASE_STRING(NSB_SAMP_ADDRESSV)
    STATE_CASE_STRING(NSB_SAMP_ADDRESSW)
    STATE_CASE_STRING(NSB_SAMP_BORDERCOLOR)
    STATE_CASE_STRING(NSB_SAMP_MAGFILTER)
    STATE_CASE_STRING(NSB_SAMP_MINFILTER)
    STATE_CASE_STRING(NSB_SAMP_MIPFILTER)
    STATE_CASE_STRING(NSB_SAMP_MIPMAPLODBIAS)
    STATE_CASE_STRING(NSB_SAMP_MAXMIPLEVEL)
    STATE_CASE_STRING(NSB_SAMP_MINMIPLEVEL)
    STATE_CASE_STRING(NSB_SAMP_MAXANISOTROPY)
    STATE_CASE_STRING(NSB_SAMP_SRGBTEXTURE)
    STATE_CASE_STRING(NSB_SAMP_ELEMENTINDEX)
    STATE_CASE_STRING(NSB_SAMP_DMAPOFFSET)
    STATE_CASE_STRING(NSB_SAMP_COMPARISONFUNC)
    default:
        return "**** UNKNOWN SAMPLER STATE ****";
    }
}

//--------------------------------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureSamplerValueString(
    NSBStageAndSamplerStates::NSBTextureSamplerState eState, unsigned int uiValue)
{
    static char s_acTemp[256];

    NiStrcpy(s_acTemp, 256, "*** USS ***");

    switch (eState)
    {
    case NSB_SAMP_ADDRESSU:
    case NSB_SAMP_ADDRESSV:
    case NSB_SAMP_ADDRESSW:
        switch (uiValue)
        {
        case NSB_TADDRESS_WRAP:            return "WRAP";
        case NSB_TADDRESS_MIRROR:          return "MIRROR";
        case NSB_TADDRESS_CLAMP:           return "CLAMP";
        case NSB_TADDRESS_BORDER:          return "BORDER";
        case NSB_TADDRESS_MIRRORONCE:      return "MIRRORONCE";
        default:                           return "*** USS ***";
        }
        break;
    case NSB_SAMP_MAGFILTER:
    case NSB_SAMP_MINFILTER:
    case NSB_SAMP_MIPFILTER:
        switch (uiValue)
        {
        case NSB_TEXF_NONE:                return "NONE";
        case NSB_TEXF_POINT:               return "POINT";
        case NSB_TEXF_LINEAR:              return "LINEAR";
        case NSB_TEXF_ANISOTROPIC:         return "ANISOTROPIC";
        case NSB_TEXF_PYRAMIDALQUAD:       return "PYRAMIDALQUAD";
        case NSB_TEXF_GAUSSIANQUAD:        return "GAUSSIANQUAD";
        default:                           return "*** USS ***";
        }
        break;
    case NSB_SAMP_BORDERCOLOR:
    case NSB_SAMP_MIPMAPLODBIAS:
    case NSB_SAMP_MAXMIPLEVEL:
    case NSB_SAMP_MINMIPLEVEL: // D3D10-specific
    case NSB_SAMP_MAXANISOTROPY:
    case NSB_SAMP_SRGBTEXTURE:
    case NSB_SAMP_ELEMENTINDEX:
    case NSB_SAMP_DMAPOFFSET:
        NiSprintf(s_acTemp, 256, "0x%08x", uiValue);
        break;
    case NSB_SAMP_COMPARISONFUNC:
        switch (uiValue)
        {
        case NSBRenderStates::NSB_CMP_NEVER:
            return "NEVER";
        case NSBRenderStates::NSB_CMP_LESS:
            return "LESS";
        case NSBRenderStates::NSB_CMP_EQUAL:
            return "EQUAL";
        case NSBRenderStates::NSB_CMP_LESSEQUAL:
            return "LESSEQUAL";
        case NSBRenderStates::NSB_CMP_GREATER:
            return "GREATER";
        case NSBRenderStates::NSB_CMP_NOTEQUAL:
            return "NOTEQUAL";
        case NSBRenderStates::NSB_CMP_GREATEREQUAL:
            return "GREATEREQUAL";
        case NSBRenderStates::NSB_CMP_ALWAYS:
            return "ALWAYS";
        default:
            return "*** USS ***";
        }
        break;
    default:
        break;
    }

    return s_acTemp;
}

//--------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
