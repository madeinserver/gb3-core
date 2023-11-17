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

#include "NSBRenderStates.h"
#include "NSBUtility.h"

#include <NiSystem.h>

//------------------------------------------------------------------------------------------------
NSBRenderStates::NSBRenderStateEnum NSBRenderStates::LookupRenderState(
    const char* pcRenderState)
{
    if (!pcRenderState || pcRenderState[0] == '\0')
        return NSBRenderStates::NSB_RS_INVALID;

    if (NiStricmp(pcRenderState, "ZENABLE") == 0)
        return NSB_RS_ZENABLE;
    if (NiStricmp(pcRenderState, "FILLMODE") == 0)
        return NSB_RS_FILLMODE;
    if (NiStricmp(pcRenderState, "SHADEMODE") == 0)
        return NSB_RS_SHADEMODE;
    if (NiStricmp(pcRenderState, "ZWRITEENABLE") == 0)
        return NSB_RS_ZWRITEENABLE;
    if (NiStricmp(pcRenderState, "ALPHATESTENABLE") == 0)
        return NSB_RS_ALPHATESTENABLE;
    if (NiStricmp(pcRenderState, "LASTPIXEL") == 0)
        return NSB_RS_LASTPIXEL;
    if (NiStricmp(pcRenderState, "SRCBLEND") == 0 ||
        NiStricmp(pcRenderState, "SRCBLEND0") == 0)
    {
        return NSB_RS_SRCBLEND;
    }
    if (NiStricmp(pcRenderState, "DESTBLEND") == 0 ||
        NiStricmp(pcRenderState, "DESTBLEND0") == 0)
    {
        return NSB_RS_DESTBLEND;
    }
    if (NiStricmp(pcRenderState, "CULLMODE") == 0)
        return NSB_RS_CULLMODE;
    if (NiStricmp(pcRenderState, "ZFUNC") == 0)
        return NSB_RS_ZFUNC;
    if (NiStricmp(pcRenderState, "ALPHAREF") == 0)
        return NSB_RS_ALPHAREF;
    if (NiStricmp(pcRenderState, "ALPHAFUNC") == 0)
        return NSB_RS_ALPHAFUNC;
    if (NiStricmp(pcRenderState, "DITHERENABLE") == 0)
        return NSB_RS_DITHERENABLE;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE") == 0 ||
        NiStricmp(pcRenderState, "ALPHABLENDENABLE0") == 0)
    {
        return NSB_RS_ALPHABLENDENABLE;
    }
    if (NiStricmp(pcRenderState, "FOGENABLE") == 0)
        return NSB_RS_FOGENABLE;
    if (NiStricmp(pcRenderState, "SPECULARENABLE") == 0)
        return NSB_RS_SPECULARENABLE;
    if (NiStricmp(pcRenderState, "FOGCOLOR") == 0)
        return NSB_RS_FOGCOLOR;
    if (NiStricmp(pcRenderState, "FOGTABLEMODE") == 0)
        return NSB_RS_FOGTABLEMODE;
    if (NiStricmp(pcRenderState, "FOGSTART") == 0)
        return NSB_RS_FOGSTART;
    if (NiStricmp(pcRenderState, "FOGEND") == 0)
        return NSB_RS_FOGEND;
    if (NiStricmp(pcRenderState, "FOGDENSITY") == 0)
        return NSB_RS_FOGDENSITY;
    if (NiStricmp(pcRenderState, "RANGEFOGENABLE") == 0)
        return NSB_RS_RANGEFOGENABLE;
    if (NiStricmp(pcRenderState, "STENCILENABLE") == 0)
        return NSB_RS_STENCILENABLE;
    if (NiStricmp(pcRenderState, "STENCILFAIL") == 0)
        return NSB_RS_STENCILFAIL;
    if (NiStricmp(pcRenderState, "STENCILZFAIL") == 0)
        return NSB_RS_STENCILZFAIL;
    if (NiStricmp(pcRenderState, "STENCILPASS") == 0)
        return NSB_RS_STENCILPASS;
    if (NiStricmp(pcRenderState, "STENCILFUNC") == 0)
        return NSB_RS_STENCILFUNC;
    if (NiStricmp(pcRenderState, "STENCILREF") == 0)
        return NSB_RS_STENCILREF;
    if (NiStricmp(pcRenderState, "STENCILMASK") == 0)
        return NSB_RS_STENCILMASK;
    if (NiStricmp(pcRenderState, "STENCILWRITEMASK") == 0)
        return NSB_RS_STENCILWRITEMASK;
    if (NiStricmp(pcRenderState, "TEXTUREFACTOR") == 0)
        return NSB_RS_TEXTUREFACTOR;
    if (NiStricmp(pcRenderState, "WRAP0") == 0)
        return NSB_RS_WRAP0;
    if (NiStricmp(pcRenderState, "WRAP1") == 0)
        return NSB_RS_WRAP1;
    if (NiStricmp(pcRenderState, "WRAP2") == 0)
        return NSB_RS_WRAP2;
    if (NiStricmp(pcRenderState, "WRAP3") == 0)
        return NSB_RS_WRAP3;
    if (NiStricmp(pcRenderState, "WRAP4") == 0)
        return NSB_RS_WRAP4;
    if (NiStricmp(pcRenderState, "WRAP5") == 0)
        return NSB_RS_WRAP5;
    if (NiStricmp(pcRenderState, "WRAP6") == 0)
        return NSB_RS_WRAP6;
    if (NiStricmp(pcRenderState, "WRAP7") == 0)
        return NSB_RS_WRAP7;
    if (NiStricmp(pcRenderState, "CLIPPING") == 0)
        return NSB_RS_CLIPPING;
    if (NiStricmp(pcRenderState, "LIGHTING") == 0)
        return NSB_RS_LIGHTING;
    if (NiStricmp(pcRenderState, "AMBIENT") == 0)
        return NSB_RS_AMBIENT;
    if (NiStricmp(pcRenderState, "FOGVERTEXMODE") == 0)
        return NSB_RS_FOGVERTEXMODE;
    if (NiStricmp(pcRenderState, "COLORVERTEX") == 0)
        return NSB_RS_COLORVERTEX;
    if (NiStricmp(pcRenderState, "LOCALVIEWER") == 0)
        return NSB_RS_LOCALVIEWER;
    if (NiStricmp(pcRenderState, "NORMALIZENORMALS") == 0)
        return NSB_RS_NORMALIZENORMALS;
    if (NiStricmp(pcRenderState, "DIFFUSEMATERIALSOURCE ") == 0)
        return NSB_RS_DIFFUSEMATERIALSOURCE ;
    if (NiStricmp(pcRenderState, "SPECULARMATERIALSOURCE") == 0)
        return NSB_RS_SPECULARMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "AMBIENTMATERIALSOURCE") == 0)
        return NSB_RS_AMBIENTMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "EMISSIVEMATERIALSOURCE") == 0)
        return NSB_RS_EMISSIVEMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "VERTEXBLEND") == 0)
        return NSB_RS_VERTEXBLEND;
    if (NiStricmp(pcRenderState, "CLIPPLANEENABLE") == 0)
        return NSB_RS_CLIPPLANEENABLE;
    if (NiStricmp(pcRenderState, "POINTSIZE") == 0)
        return NSB_RS_POINTSIZE;
    if (NiStricmp(pcRenderState, "POINTSIZE_MIN") == 0)
        return NSB_RS_POINTSIZE_MIN;
    if (NiStricmp(pcRenderState, "POINTSPRITEENABLE") == 0)
        return NSB_RS_POINTSPRITEENABLE;
    if (NiStricmp(pcRenderState, "POINTSCALEENABLE") == 0)
        return NSB_RS_POINTSCALEENABLE;
    if (NiStricmp(pcRenderState, "POINTSCALE_A") == 0)
        return NSB_RS_POINTSCALE_A;
    if (NiStricmp(pcRenderState, "POINTSCALE_B") == 0)
        return NSB_RS_POINTSCALE_B;
    if (NiStricmp(pcRenderState, "POINTSCALE_C") == 0)
        return NSB_RS_POINTSCALE_C;
    if (NiStricmp(pcRenderState, "MULTISAMPLEANTIALIAS") == 0)
        return NSB_RS_MULTISAMPLEANTIALIAS;
    if (NiStricmp(pcRenderState, "MULTISAMPLEMASK") == 0)
        return NSB_RS_MULTISAMPLEMASK;
    if (NiStricmp(pcRenderState, "PATCHEDGESTYLE") == 0)
        return NSB_RS_PATCHEDGESTYLE;
    if (NiStricmp(pcRenderState, "DEBUGMONITORTOKEN") == 0)
        return NSB_RS_DEBUGMONITORTOKEN;
    if (NiStricmp(pcRenderState, "POINTSIZE_MAX") == 0)
        return NSB_RS_POINTSIZE_MAX;
    if (NiStricmp(pcRenderState, "INDEXEDVERTEXBLENDENABLE") == 0)
        return NSB_RS_INDEXEDVERTEXBLENDENABLE;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE") == 0 ||
        NiStricmp(pcRenderState, "COLORWRITEENABLE0") == 0 )
        return NSB_RS_COLORWRITEENABLE;
    if (NiStricmp(pcRenderState, "TWEENFACTOR") == 0)
        return NSB_RS_TWEENFACTOR;
    if (NiStricmp(pcRenderState, "BLENDOP") == 0 || 
        NiStricmp(pcRenderState, "BLENDOP0") == 0)
    {
        return NSB_RS_BLENDOP;
    }
    if (NiStricmp(pcRenderState, "POSITIONDEGREE") == 0)
        return NSB_RS_POSITIONDEGREE;
    if (NiStricmp(pcRenderState, "NORMALDEGREE") == 0)
        return NSB_RS_NORMALDEGREE;
    if (NiStricmp(pcRenderState, "SCISSORTESTENABLE") == 0)
        return NSB_RS_SCISSORTESTENABLE;
    if (NiStricmp(pcRenderState, "SLOPESCALEDEPTHBIAS") == 0)
        return NSB_RS_SLOPESCALEDEPTHBIAS;
    if (NiStricmp(pcRenderState, "ANTIALIASEDLINEENABLE") == 0)
        return NSB_RS_ANTIALIASEDLINEENABLE;
    if (NiStricmp(pcRenderState, "MINTESSELLATIONLEVEL") == 0)
        return NSB_RS_MINTESSELLATIONLEVEL;
    if (NiStricmp(pcRenderState, "MAXTESSELLATIONLEVEL") == 0)
        return NSB_RS_MAXTESSELLATIONLEVEL;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_X") == 0)
        return NSB_RS_ADAPTIVETESS_X;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_Y") == 0)
        return NSB_RS_ADAPTIVETESS_Y;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_Z") == 0)
        return NSB_RS_ADAPTIVETESS_Z;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_W") == 0)
        return NSB_RS_ADAPTIVETESS_W;
    if (NiStricmp(pcRenderState, "ENABLEADAPTIVETESSELATION") == 0 ||
        NiStricmp(pcRenderState, "ENABLEADAPTIVETESSELLATION") == 0)
    {
        return NSB_RS_ENABLEADAPTIVETESSELLATION;
    }
    if (NiStricmp(pcRenderState, "TWOSIDEDSTENCILMODE") == 0)
        return NSB_RS_TWOSIDEDSTENCILMODE;
    if (NiStricmp(pcRenderState, "CCW_STENCILFAIL") == 0)
        return NSB_RS_CCW_STENCILFAIL;
    if (NiStricmp(pcRenderState, "CCW_STENCILZFAIL") == 0)
        return NSB_RS_CCW_STENCILZFAIL;
    if (NiStricmp(pcRenderState, "CCW_STENCILPASS") == 0)
        return NSB_RS_CCW_STENCILPASS;
    if (NiStricmp(pcRenderState, "CCW_STENCILFUNC") == 0)
        return NSB_RS_CCW_STENCILFUNC;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE1") == 0)
        return NSB_RS_COLORWRITEENABLE1;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE2") == 0)
        return NSB_RS_COLORWRITEENABLE2;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE3") == 0)
        return NSB_RS_COLORWRITEENABLE3;
    if (NiStricmp(pcRenderState, "BLENDFACTOR") == 0)
        return NSB_RS_BLENDFACTOR;
    if (NiStricmp(pcRenderState, "SRGBWRITEENABLE") == 0)
        return NSB_RS_SRGBWRITEENABLE;
    if (NiStricmp(pcRenderState, "DEPTHBIAS") == 0)
        return NSB_RS_DEPTHBIAS;
    if (NiStricmp(pcRenderState, "WRAP8") == 0)
        return NSB_RS_WRAP8;
    if (NiStricmp(pcRenderState, "WRAP9") == 0)
        return NSB_RS_WRAP9;
    if (NiStricmp(pcRenderState, "WRAP10") == 0)
        return NSB_RS_WRAP10;
    if (NiStricmp(pcRenderState, "WRAP11") == 0)
        return NSB_RS_WRAP11;
    if (NiStricmp(pcRenderState, "WRAP12") == 0)
        return NSB_RS_WRAP12;
    if (NiStricmp(pcRenderState, "WRAP13") == 0)
        return NSB_RS_WRAP13;
    if (NiStricmp(pcRenderState, "WRAP14") == 0)
        return NSB_RS_WRAP14;
    if (NiStricmp(pcRenderState, "WRAP15") == 0)
        return NSB_RS_WRAP15;
    if (NiStricmp(pcRenderState, "SEPARATEALPHABLENDENABLE") == 0)
        return NSB_RS_SEPARATEALPHABLENDENABLE;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA") == 0 ||
        NiStricmp(pcRenderState, "SRCBLENDALPHA0") == 0 ||
        NiStricmp(pcRenderState, "SRCBLENDSEPARATEA") == 0)
    {
        return NSB_RS_SRCBLENDALPHA;
    }
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA") == 0 ||
        NiStricmp(pcRenderState, "DESTBLENDALPHA0") == 0 ||
        NiStricmp(pcRenderState, "DSTBLENDSEPARATEA") == 0)
    {
        return NSB_RS_DESTBLENDALPHA;
    }
    if (NiStricmp(pcRenderState, "BLENDOPALPHA") == 0 ||
        NiStricmp(pcRenderState, "BLENDOPALPHA0") == 0 ||
        NiStricmp(pcRenderState, "BLENDEQUATIONSEPARATEA") == 0)
    {
        return NSB_RS_BLENDOPALPHA;
    }
    if (NiStricmp(pcRenderState, "VIEWPORTENABLE") == 0)
        return NSB_RS_VIEWPORTENABLE;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE") == 0)
        return NSB_RS_HIGHPRECISIONBLENDENABLE;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE1") == 0)
        return NSB_RS_HIGHPRECISIONBLENDENABLE1;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE2") == 0)
        return NSB_RS_HIGHPRECISIONBLENDENABLE2;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE3") == 0)
        return NSB_RS_HIGHPRECISIONBLENDENABLE3;
    if (NiStricmp(pcRenderState, "TESSELLATIONMODE") == 0)
        return NSB_RS_TESSELLATIONMODE;

    // Begin PS3-only states.
    if (NiStricmp(pcRenderState, "COLORLOGICOPENABLE") == 0)
        return NSB_RS_COLORLOGICOPENABLE;
    if (NiStricmp(pcRenderState, "CULLFACEENABLE") == 0)
        return NSB_RS_CULLFACEENABLE;
    if (NiStricmp(pcRenderState, "MULTISAMPLEENABLE") == 0)
        return NSB_RS_MULTISAMPLEENABLE;
    if (NiStricmp(pcRenderState, "POINTSMOOTHENABLE") == 0)
        return NSB_RS_POINTSMOOTHENABLE;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETFILLENABLE") == 0)
        return NSB_RS_POLYGONOFFSETFILLENABLE;
    // End PS3-only states.

    // PS3 and D3D10/11 state.
    if (NiStricmp(pcRenderState, "SAMPLEALPHATOCOVERAGEENABLE") == 0)
        return NSB_RS_SAMPLEALPHATOCOVERAGEENABLE;

    // Begin PS3-only states.
    if (NiStricmp(pcRenderState, "SAMPLEALPHATOONEENABLE") == 0)
        return NSB_RS_SAMPLEALPHATOONEENABLE;
    if (NiStricmp(pcRenderState, "SAMPLECOVERAGEENABLE") == 0)
        return NSB_RS_SAMPLECOVERAGEENABLE;
    if (NiStricmp(pcRenderState, "VERTEXPROGRAMPOINTSIZEENABLE") == 0)
        return NSB_RS_VERTEXPROGRAMPOINTSIZEENABLE;
    if (NiStricmp(pcRenderState, "BLENDCOLORR") == 0)
        return NSB_RS_BLENDCOLORR;
    if (NiStricmp(pcRenderState, "BLENDCOLORG") == 0)
        return NSB_RS_BLENDCOLORG;
    if (NiStricmp(pcRenderState, "BLENDCOLORB") == 0)
        return NSB_RS_BLENDCOLORB;
    if (NiStricmp(pcRenderState, "BLENDCOLORA") == 0)
        return NSB_RS_BLENDCOLORA;
    if (NiStricmp(pcRenderState, "SRCBLENDSEPARATERGB") == 0)
        return NSB_RS_SRCBLENDSEPARATERGB;
    if (NiStricmp(pcRenderState, "DSTBLENDSEPARATERGB") == 0)
        return NSB_RS_DSTBLENDSEPARATERGB;
    if (NiStricmp(pcRenderState, "BLENDEQUATIONSEPARATERGB") == 0)
        return NSB_RS_BLENDEQUATIONSEPARATERGB;
    if (NiStricmp(pcRenderState, "CULLFACE") == 0)
        return NSB_RS_CULLFACE;
    if (NiStricmp(pcRenderState, "COLORMASKR") == 0)
        return NSB_RS_COLORMASKR;
    if (NiStricmp(pcRenderState, "COLORMASKG") == 0)
        return NSB_RS_COLORMASKG;
    if (NiStricmp(pcRenderState, "COLORMASKB") == 0)
        return NSB_RS_COLORMASKB;
    if (NiStricmp(pcRenderState, "COLORMASKA") == 0)
        return NSB_RS_COLORMASKA;
    if (NiStricmp(pcRenderState, "DEPTHRANGENEAR") == 0)
        return NSB_RS_DEPTHRANGENEAR;
    if (NiStricmp(pcRenderState, "DEPTHRANGEFAR") == 0)
        return NSB_RS_DEPTHRANGEFAR;
    if (NiStricmp(pcRenderState, "FRONTFACE") == 0)
        return NSB_RS_FRONTFACE;
    if (NiStricmp(pcRenderState, "LINEWIDTH") == 0)
        return NSB_RS_LINEWIDTH;
    if (NiStricmp(pcRenderState, "POINTSPRITECOORDREPLACE") == 0)
        return NSB_RS_POINTSPRITECOORDREPLACE;
    if (NiStricmp(pcRenderState, "POLYGONMODEFACE") == 0)
        return NSB_RS_POLYGONMODEFACE;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETFACTOR") == 0)
        return NSB_RS_POLYGONOFFSETFACTOR;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETUNITS") == 0)
        return NSB_RS_POLYGONOFFSETUNITS;
    if (NiStricmp(pcRenderState, "SCISSORX") == 0)
        return NSB_RS_SCISSORX;
    if (NiStricmp(pcRenderState, "SCISSORY") == 0)
        return NSB_RS_SCISSORY;
    if (NiStricmp(pcRenderState, "SCISSORWIDTH") == 0)
        return NSB_RS_SCISSORWIDTH;
    if (NiStricmp(pcRenderState, "SCISSORHEIGHT") == 0)
        return NSB_RS_SCISSORHEIGHT;
    // End PS3-only states.

    // Begin D3D10/11-only states.
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE1") == 0)
        return NSB_RS_ALPHABLENDENABLE1;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE2") == 0)
        return NSB_RS_ALPHABLENDENABLE2;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE3") == 0)
        return NSB_RS_ALPHABLENDENABLE3;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE4") == 0)
        return NSB_RS_ALPHABLENDENABLE4;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE5") == 0)
        return NSB_RS_ALPHABLENDENABLE5;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE6") == 0)
        return NSB_RS_ALPHABLENDENABLE6;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE7") == 0)
        return NSB_RS_ALPHABLENDENABLE7;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE4") == 0)
        return NSB_RS_COLORWRITEENABLE4;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE5") == 0)
        return NSB_RS_COLORWRITEENABLE5;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE6") == 0)
        return NSB_RS_COLORWRITEENABLE6;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE7") == 0)
        return NSB_RS_COLORWRITEENABLE7;
    if (NiStricmp(pcRenderState, "FRONTCCW") == 0)
        return NSB_RS_FRONTCCW;
    if (NiStricmp(pcRenderState, "DEPTHBIASCLAMP") == 0)
        return NSB_RS_DEPTHBIASCLAMP;
    // End D3D10/11-only states.

    // Begin D3D11-only states.
    if (NiStricmp(pcRenderState, "NSB_RS_INDEPENDENTBLENDENABLE") == 0)
        return NSB_RS_INDEPENDENTBLENDENABLE;
    if (NiStricmp(pcRenderState, "SRCBLEND1") == 0)
        return NSB_RS_SRCBLEND1;
    if (NiStricmp(pcRenderState, "SRCBLEND2") == 0)
        return NSB_RS_SRCBLEND2;
    if (NiStricmp(pcRenderState, "SRCBLEND3") == 0)
        return NSB_RS_SRCBLEND3;
    if (NiStricmp(pcRenderState, "SRCBLEND4") == 0)
        return NSB_RS_SRCBLEND4;
    if (NiStricmp(pcRenderState, "SRCBLEND5") == 0)
        return NSB_RS_SRCBLEND5;
    if (NiStricmp(pcRenderState, "SRCBLEND6") == 0)
        return NSB_RS_SRCBLEND6;
    if (NiStricmp(pcRenderState, "SRCBLEND7") == 0)
        return NSB_RS_SRCBLEND7;
    if (NiStricmp(pcRenderState, "DESTBLEND1") == 0)
        return NSB_RS_DESTBLEND1;
    if (NiStricmp(pcRenderState, "DESTBLEND2") == 0)
        return NSB_RS_DESTBLEND2;
    if (NiStricmp(pcRenderState, "DESTBLEND3") == 0)
        return NSB_RS_DESTBLEND3;
    if (NiStricmp(pcRenderState, "DESTBLEND4") == 0)
        return NSB_RS_DESTBLEND4;
    if (NiStricmp(pcRenderState, "DESTBLEND5") == 0)
        return NSB_RS_DESTBLEND5;
    if (NiStricmp(pcRenderState, "DESTBLEND6") == 0)
        return NSB_RS_DESTBLEND6;
    if (NiStricmp(pcRenderState, "DESTBLEND7") == 0)
        return NSB_RS_DESTBLEND7;
    if (NiStricmp(pcRenderState, "BLENDOP1") == 0)
        return NSB_RS_BLENDOP1;
    if (NiStricmp(pcRenderState, "BLENDOP2") == 0)
        return NSB_RS_BLENDOP2;
    if (NiStricmp(pcRenderState, "BLENDOP3") == 0)
        return NSB_RS_BLENDOP3;
    if (NiStricmp(pcRenderState, "BLENDOP4") == 0)
        return NSB_RS_BLENDOP4;
    if (NiStricmp(pcRenderState, "BLENDOP5") == 0)
        return NSB_RS_BLENDOP5;
    if (NiStricmp(pcRenderState, "BLENDOP6") == 0)
        return NSB_RS_BLENDOP6;
    if (NiStricmp(pcRenderState, "BLENDOP7") == 0)
        return NSB_RS_BLENDOP7;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA1") == 0)
        return NSB_RS_SRCBLENDALPHA1;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA2") == 0)
        return NSB_RS_SRCBLENDALPHA2;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA3") == 0)
        return NSB_RS_SRCBLENDALPHA3;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA4") == 0)
        return NSB_RS_SRCBLENDALPHA4;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA5") == 0)
        return NSB_RS_SRCBLENDALPHA5;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA6") == 0)
        return NSB_RS_SRCBLENDALPHA6;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA7") == 0)
        return NSB_RS_SRCBLENDALPHA7;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA1") == 0)
        return NSB_RS_DESTBLENDALPHA1;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA2") == 0)
        return NSB_RS_DESTBLENDALPHA2;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA3") == 0)
        return NSB_RS_DESTBLENDALPHA3;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA4") == 0)
        return NSB_RS_DESTBLENDALPHA4;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA5") == 0)
        return NSB_RS_DESTBLENDALPHA5;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA6") == 0)
        return NSB_RS_DESTBLENDALPHA6;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA7") == 0)
        return NSB_RS_DESTBLENDALPHA7;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA1") == 0)
        return NSB_RS_BLENDOPALPHA1;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA2") == 0)
        return NSB_RS_BLENDOPALPHA2;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA3") == 0)
        return NSB_RS_BLENDOPALPHA3;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA4") == 0)
        return NSB_RS_BLENDOPALPHA4;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA5") == 0)
        return NSB_RS_BLENDOPALPHA5;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA6") == 0)
        return NSB_RS_BLENDOPALPHA6;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA7") == 0)
        return NSB_RS_BLENDOPALPHA7;
    // End D3D11-only states.

    // Deprecated values
    if (NiStricmp(pcRenderState, "LINEPATTERN") == 0 ||
        NiStricmp(pcRenderState, "EDGEANTIALIAS") == 0 ||
        NiStricmp(pcRenderState, "ZBIAS") == 0 ||
        NiStricmp(pcRenderState, "SOFTWAREVERTEXPROCESSING") == 0 ||
        NiStricmp(pcRenderState, "PATCHSEGMENTS") == 0 ||
        NiStricmp(pcRenderState, "POSITIONORDER") == 0 ||
        NiStricmp(pcRenderState, "NORMALORDER") == 0 ||
        NiStricmp(pcRenderState, "BLENDCOLOR") == 0 ||
        NiStricmp(pcRenderState, "SWATHWIDTH") == 0 ||
        NiStricmp(pcRenderState, "POLYGONOFFSETZSLOPESCALE") == 0 ||
        NiStricmp(pcRenderState, "POLYGONOFFSETZOFFSET") == 0 ||
        NiStricmp(pcRenderState, "POINTOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "WIREFRAMEOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "SOLIDOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "DEPTHCLIPCONTROL") == 0 ||
        NiStricmp(pcRenderState, "STIPPLEENABLE") == 0 ||
        NiStricmp(pcRenderState, "BACKSPECULARMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKDIFFUSEMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKAMBIENTMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKEMISSIVEMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKAMBIENT") == 0 ||
        NiStricmp(pcRenderState, "SWAPFILTER") == 0 ||
        NiStricmp(pcRenderState, "PRESENTATIONINTERVAL") == 0 ||
        NiStricmp(pcRenderState, "BACKFILLMODE") == 0 ||
        NiStricmp(pcRenderState, "TWOSIDEDLIGHTING") == 0 ||
        //        NiStricmp(pcRenderState, "FRONTFACE") == 0 ||    // now an ogl state
        NiStricmp(pcRenderState, "LOGICOP") == 0 ||
        NiStricmp(pcRenderState, "MULTISAMPLEMODE") == 0 ||
        NiStricmp(pcRenderState, "MULTISAMPLERENDERTARGETMODE") == 0 ||
        NiStricmp(pcRenderState, "SHADOWFUNC") == 0 ||
        //        NiStricmp(pcRenderState, "LINEWIDTH") == 0 ||    // now an ogl state
        NiStricmp(pcRenderState, "SAMPLEALPHA") == 0 ||
        NiStricmp(pcRenderState, "DXT1NOISEENABLE") == 0 ||
        NiStricmp(pcRenderState, "YUVENABLE") == 0 ||
        NiStricmp(pcRenderState, "OCCLUSIONCULLENABLE") == 0 ||
        NiStricmp(pcRenderState, "STENCILCULLENABLE") == 0 ||
        NiStricmp(pcRenderState, "ROPZCMPALWAYSREAD") == 0 ||
        NiStricmp(pcRenderState, "ROPZREAD") == 0 ||
        NiStricmp(pcRenderState, "DONOTCULLUNCOMPRESSED") == 0 ||
        NiStricmp(pcRenderState, "PSTEXTUREMODES") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERINPUTSABCD") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERINPUTSEFG") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_0") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_1") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_2") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_3") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_4") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_5") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_6") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_7") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_0") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_1") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_2") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_3") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_4") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_5") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_6") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_7") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSCOMPAREMODE") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERCONSTANT0") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERCONSTANT1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSCOMBINERCOUNT") == 0 ||
        NiStricmp(pcRenderState, "PSDOTMAPPING") == 0 ||
        NiStricmp(pcRenderState, "PSINPUTTEXTURE") == 0)
    {
        return NSB_RS_DEPRECATED;
    }

    return NSB_RS_INVALID;
}

//------------------------------------------------------------------------------------------------
bool NSBRenderStates::LookupRenderStateValue(
    NSBRenderStates::NSBRenderStateEnum eRenderState,
    const char* pcValue, 
    unsigned int& uiValue)
{
    // We will default to a value of 0, as this will most likely not cause any
    // assertions on devices; it will simply cause incorrect results.  (This
    // case assumes that the value string was not valid for the renderstate.)
    uiValue = 0;

    if (!pcValue || pcValue[0] == '\0')
        return false;

    switch (eRenderState)
    {
        // Simple states
    case NSB_RS_ZFUNC:
    case NSB_RS_ALPHAFUNC:
    case NSB_RS_STENCILFUNC:
    case NSB_RS_CCW_STENCILFUNC:
        if (NiStricmp(pcValue, "NEVER") == 0)
        {
            uiValue = NSB_CMP_NEVER;
            return true;
        }
        else if (NiStricmp(pcValue, "LESS") == 0)
        {
            uiValue = NSB_CMP_LESS;
            return true;
        }
        else if (NiStricmp(pcValue, "EQUAL") == 0)
        {
            uiValue = NSB_CMP_EQUAL;
            return true;
        }
        else if (NiStricmp(pcValue, "LESSEQUAL") == 0)
        {
            uiValue = NSB_CMP_LESSEQUAL;
            return true;
        }
        else if (NiStricmp(pcValue, "GREATER") == 0)
        {
            uiValue = NSB_CMP_GREATER;
            return true;
        }
        else if (NiStricmp(pcValue, "NOTEQUAL") == 0)
        {
            uiValue = NSB_CMP_NOTEQUAL;
            return true;
        }
        else if (NiStricmp(pcValue, "GREATEREQUAL") == 0)
        {
            uiValue = NSB_CMP_GREATEREQUAL;
            return true;
        }
        else if (NiStricmp(pcValue, "ALWAYS") == 0)
        {
            uiValue = NSB_CMP_ALWAYS;
            return true;
        }
        break;
    case NSB_RS_SRCBLEND:
    case NSB_RS_SRCBLEND1:
    case NSB_RS_SRCBLEND2:
    case NSB_RS_SRCBLEND3:
    case NSB_RS_SRCBLEND4:
    case NSB_RS_SRCBLEND5:
    case NSB_RS_SRCBLEND6:
    case NSB_RS_SRCBLEND7:
    case NSB_RS_DESTBLEND:
    case NSB_RS_DESTBLEND1:
    case NSB_RS_DESTBLEND2:
    case NSB_RS_DESTBLEND3:
    case NSB_RS_DESTBLEND4:
    case NSB_RS_DESTBLEND5:
    case NSB_RS_DESTBLEND6:
    case NSB_RS_DESTBLEND7:
    case NSB_RS_SRCBLENDALPHA:
    case NSB_RS_SRCBLENDALPHA1:
    case NSB_RS_SRCBLENDALPHA2:
    case NSB_RS_SRCBLENDALPHA3:
    case NSB_RS_SRCBLENDALPHA4:
    case NSB_RS_SRCBLENDALPHA5:
    case NSB_RS_SRCBLENDALPHA6:
    case NSB_RS_SRCBLENDALPHA7:
    case NSB_RS_DESTBLENDALPHA:
    case NSB_RS_DESTBLENDALPHA1:
    case NSB_RS_DESTBLENDALPHA2:
    case NSB_RS_DESTBLENDALPHA3:
    case NSB_RS_DESTBLENDALPHA4:
    case NSB_RS_DESTBLENDALPHA5:
    case NSB_RS_DESTBLENDALPHA6:
    case NSB_RS_DESTBLENDALPHA7:
    case NSB_RS_SRCBLENDSEPARATERGB:
    case NSB_RS_DSTBLENDSEPARATERGB:
        if (NiStricmp(pcValue, "ZERO") == 0)
        {
            uiValue = NSB_BLEND_ZERO;
            return true;
        }
        else if (NiStricmp(pcValue, "ONE") == 0)
        {
            uiValue = NSB_BLEND_ONE;
            return true;
        }
        else if (NiStricmp(pcValue, "SRCCOLOR") == 0)
        {
            uiValue = NSB_BLEND_SRCCOLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "INVSRCCOLOR") == 0)
        {
            uiValue = NSB_BLEND_INVSRCCOLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "SRCALPHA") == 0)
        {
            uiValue = NSB_BLEND_SRCALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "INVSRCALPHA") == 0)
        {
            uiValue = NSB_BLEND_INVSRCALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "DESTALPHA") == 0)
        {
            uiValue = NSB_BLEND_DESTALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "INVDESTALPHA") == 0)
        {
            uiValue = NSB_BLEND_INVDESTALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "DESTCOLOR") == 0)
        {
            uiValue = NSB_BLEND_DESTCOLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "INVDESTCOLOR") == 0)
        {
            uiValue = NSB_BLEND_INVDESTCOLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "SRCALPHASAT") == 0)
        {
            uiValue = NSB_BLEND_SRCALPHASAT;
            return true;
        }
        else if (NiStricmp(pcValue, "BOTHSRCALPHA") == 0)
        {
            uiValue = NSB_BLEND_BOTHSRCALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "BOTHINVSRCALPHA") == 0)
        {
            uiValue = NSB_BLEND_BOTHINVSRCALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "CONSTANTCOLOR") == 0 ||
            NiStricmp(pcValue, "BLENDFACTOR") == 0)
        {
            uiValue = NSB_BLEND_BLENDFACTOR;
            return true;
        }
        else if (NiStricmp(pcValue, "INVCONSTANTCOLOR") == 0 ||
            NiStricmp(pcValue, "INVBLENDFACTOR") == 0)
        {
            uiValue = NSB_BLEND_INVBLENDFACTOR;
            return true;
        }
        else if (NiStricmp(pcValue, "CONSTANTALPHA") == 0)
        {
            uiValue = NSB_BLEND_CONSTANTALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "INVCONSTANTALPHA") == 0)
        {
            uiValue = NSB_BLEND_INVCONSTANTALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "BLEND_SRC1_COLOR") == 0)
        {
            uiValue = NSB_BLEND_SRC1_COLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "BLEND_INV_SRC1_COLOR") == 0)
        {
            uiValue = NSB_BLEND_INV_SRC1_COLOR;
            return true;
        }
        else if (NiStricmp(pcValue, "BLEND_SRC1_ALPHA") == 0)
        {
            uiValue = NSB_BLEND_SRC1_ALPHA;
            return true;
        }
        else if (NiStricmp(pcValue, "BLEND_INV_SRC1_ALPHA") == 0)
        {
            uiValue = NSB_BLEND_INV_SRC1_ALPHA;
            return true;
        }
        break;
    case NSB_RS_SHADEMODE:
        if (NiStricmp(pcValue, "FLAT") == 0)
        {
            uiValue = NSB_SHADE_FLAT;
            return true;
        }
        else if (NiStricmp(pcValue, "GOURAUD") == 0)
        {
            uiValue = NSB_SHADE_GOURAUD;
            return true;
        }
        else if (NiStricmp(pcValue, "PHONG") == 0)
        {
            uiValue = NSB_SHADE_PHONG;
            return true;
        }
        break;
    case NSB_RS_STENCILZFAIL:
    case NSB_RS_STENCILPASS:
    case NSB_RS_STENCILFAIL:
    case NSB_RS_CCW_STENCILFAIL:
    case NSB_RS_CCW_STENCILZFAIL:
    case NSB_RS_CCW_STENCILPASS:
        if (NiStricmp(pcValue, "KEEP") == 0)
        {
            uiValue = NSB_STENCTIL_OP_KEEP;
            return true;
        }
        else if (NiStricmp(pcValue, "ZERO") == 0)
        {
            uiValue = NSB_STENCTIL_OP_ZERO;
            return true;
        }
        else if (NiStricmp(pcValue, "REPLACE") == 0)
        {
            uiValue = NSB_STENCTIL_OP_REPLACE;
            return true;
        }
        else if (NiStricmp(pcValue, "INCRSAT") == 0)
        {
            uiValue = NSB_STENCTIL_OP_INCRSAT;
            return true;
        }
        else if (NiStricmp(pcValue, "DECRSAT") == 0)
        {
            uiValue = NSB_STENCTIL_OP_DECRSAT;
            return true;
        }
        else if (NiStricmp(pcValue, "INVERT") == 0)
        {
            uiValue = NSB_STENCTIL_OP_INVERT;
            return true;
        }
        else if (NiStricmp(pcValue, "INCR") == 0)
        {
            uiValue = NSB_STENCTIL_OP_INCR;
            return true;
        }
        else if (NiStricmp(pcValue, "DECR") == 0)
        {
            uiValue = NSB_STENCTIL_OP_DECR;
            return true;
        }
        break;
    case NSB_RS_BLENDOP:
    case NSB_RS_BLENDOP1:
    case NSB_RS_BLENDOP2:
    case NSB_RS_BLENDOP3:
    case NSB_RS_BLENDOP4:
    case NSB_RS_BLENDOP5:
    case NSB_RS_BLENDOP6:
    case NSB_RS_BLENDOP7:
    case NSB_RS_BLENDOPALPHA:
    case NSB_RS_BLENDOPALPHA1:
    case NSB_RS_BLENDOPALPHA2:
    case NSB_RS_BLENDOPALPHA3:
    case NSB_RS_BLENDOPALPHA4:
    case NSB_RS_BLENDOPALPHA5:
    case NSB_RS_BLENDOPALPHA6:
    case NSB_RS_BLENDOPALPHA7:
    case NSB_RS_BLENDEQUATIONSEPARATERGB:
        if (NiStricmp(pcValue, "ADD") == 0)
        {
            uiValue = NSB_BLENDOP_ADD;
            return true;
        }
        else if (NiStricmp(pcValue, "SUBTRACT") == 0)
        {
            uiValue = NSB_BLENDOP_SUBTRACT;
            return true;
        }
        else if (NiStricmp(pcValue, "REVSUBTRACT") == 0)
        {
            uiValue = NSB_BLENDOP_REVSUBTRACT;
            return true;
        }
        else if (NiStricmp(pcValue, "MIN") == 0)
        {
            uiValue = NSB_BLENDOP_MIN;
            return true;
        }
        else if (NiStricmp(pcValue, "MAX") == 0)
        {
            uiValue = NSB_BLENDOP_MAX;
            return true;
        }
        // Deprecated values
        else if (NiStricmp(pcValue, "ADDSIGNED") == 0 ||
            NiStricmp(pcValue, "REVSUBTRACTSIGNED") == 0)
        {
            uiValue = NSB_BLENDOP_DEPRECATED;
            return true;
        }
        break;
    case NSB_RS_FOGTABLEMODE:
    case NSB_RS_FOGVERTEXMODE:
        if (NiStricmp(pcValue, "NONE") == 0)
        {
            uiValue = NSB_FOG_NONE;
            return true;
        }
        else if (NiStricmp(pcValue, "EXP") == 0)
        {
            uiValue = NSB_FOG_EXP;
            return true;
        }
        else if (NiStricmp(pcValue, "EXP2") == 0)
        {
            uiValue = NSB_FOG_EXP2;
            return true;
        }
        else if (NiStricmp(pcValue, "LINEAR") == 0)
        {
            uiValue = NSB_FOG_LINEAR;
            return true;
        }
        break;
    case NSB_RS_WRAP0:
    case NSB_RS_WRAP1:
    case NSB_RS_WRAP2:
    case NSB_RS_WRAP3:
    case NSB_RS_WRAP4:
    case NSB_RS_WRAP5:
    case NSB_RS_WRAP6:
    case NSB_RS_WRAP7:
    case NSB_RS_WRAP8:
    case NSB_RS_WRAP9:
    case NSB_RS_WRAP10:
    case NSB_RS_WRAP11:
    case NSB_RS_WRAP12:
    case NSB_RS_WRAP13:
    case NSB_RS_WRAP14:
    case NSB_RS_WRAP15:
        if (NiStricmp(pcValue, "DISABLED") == 0)
        {
            uiValue = NSB_WRAP_DISABLED;
            return true;
        }
        else if (NiStricmp(pcValue, "U") == 0)
        {
            uiValue = NSB_WRAP_U;
            return true;
        }
        else if (NiStricmp(pcValue, "V") == 0)
        {
            uiValue = NSB_WRAP_V;
            return true;
        }
        else if (NiStricmp(pcValue, "W") == 0)
        {
            uiValue = NSB_WRAP_W;
            return true;
        }
        else if (NiStricmp(pcValue, "UV") == 0)
        {
            uiValue = NSB_WRAP_UV;
            return true;
        }
        else if (NiStricmp(pcValue, "UW") == 0)
        {
            uiValue = NSB_WRAP_UW;
            return true;
        }
        else if (NiStricmp(pcValue, "VW") == 0)
        {
            uiValue = NSB_WRAP_VW;
            return true;
        }
        else if (NiStricmp(pcValue, "UVW") == 0)
        {
            uiValue = NSB_WRAP_UVW;
            return true;
        }
        break;
    case NSB_RS_SPECULARMATERIALSOURCE:
    case NSB_RS_DIFFUSEMATERIALSOURCE:
    case NSB_RS_AMBIENTMATERIALSOURCE:
    case NSB_RS_EMISSIVEMATERIALSOURCE:
        if (NiStricmp(pcValue, "MATERIAL") == 0)
        {
            uiValue = NSB_MCS_MATERIAL;
            return true;
        }
        else if (NiStricmp(pcValue, "COLOR1") == 0)
        {
            uiValue = NSB_MCS_COLOR1;
            return true;
        }
        else if (NiStricmp(pcValue, "COLOR2") == 0)
        {
            uiValue = NSB_MCS_COLOR2;
            return true;
        }
        break;
    case NSB_RS_PATCHEDGESTYLE:
        if (NiStricmp(pcValue, "DISCRETE") == 0)
        {
            uiValue = NSB_PATCH_EDGE_DISCRETE;
            return true;
        }
        else if (NiStricmp(pcValue, "CONTINUOUS") == 0)
        {
            uiValue = NSB_PATCH_EDGE_CONTINUOUS;
            return true;
        }
        break;
    case NSB_RS_VERTEXBLEND:
        if (NiStricmp(pcValue, "DISABLE") == 0)
        {
            uiValue = NSB_VBF_DISABLE;
            return true;
        }
        else if (NiStricmp(pcValue, "1WEIGHTS") == 0)
        {
            uiValue = NSB_VBF_1WEIGHTS;
            return true;
        }
        else if (NiStricmp(pcValue, "2WEIGHTS") == 0)
        {
            uiValue = NSB_VBF_2WEIGHTS;
            return true;
        }
        else if (NiStricmp(pcValue, "3WEIGHTS") == 0)
        {
            uiValue = NSB_VBF_3WEIGHTS;
            return true;
        }
        else if (NiStricmp(pcValue, "TWEENING") == 0)
        {
            uiValue = NSB_VBF_TWEENING;
            return true;
        }
        else if (NiStricmp(pcValue, "0WEIGHTS") == 0)
        {
            uiValue = NSB_VBF_0WEIGHTS;
            return true;
        }
        // Deprecated values
        else if (NiStricmp(pcValue, "2WEIGHTS2MATRICES") == 0 ||
            NiStricmp(pcValue, "3WEIGHTS3MATRICES") == 0 ||
            NiStricmp(pcValue, "4WEIGHTS4MATRICES") == 0)
        {
            uiValue = NSB_VBF_DEPRECATED;
            return true;
        }
        break;
    case NSB_RS_FILLMODE:
        if (NiStricmp(pcValue, "POINT") == 0)
        {
            uiValue = NSB_FILL_POINT;
            return true;
        }
        else if (NiStricmp(pcValue, "WIREFRAME") == 0)
        {
            uiValue = NSB_FILL_WIREFRAME;
            return true;
        }
        else if (NiStricmp(pcValue, "SOLID") == 0)
        {
            uiValue = NSB_FILL_SOLID;
            return true;
        }
        break;
    case NSB_RS_ZENABLE:
        if (NiStricmp(pcValue, "ZB_FALSE") == 0)
        {
            uiValue = NSB_ZB_FALSE;
            return true;
        }
        else if (NiStricmp(pcValue, "ZB_TRUE") == 0)
        {
            uiValue = NSB_ZB_TRUE;
            return true;
        }
        else if (NiStricmp(pcValue, "ZB_USEW") == 0)
        {
            uiValue = NSB_ZB_USEW;
            return true;
        }
        break;
    case NSB_RS_CULLMODE:
    case NSB_RS_FRONTFACE:
        if (NiStricmp(pcValue, "NONE") == 0)
        {
            uiValue = NSB_CULL_NONE;
            return true;
        }
        else if (NiStricmp(pcValue, "CW") == 0)
        {
            uiValue = NSB_CULL_CW;
            return true;
        }
        else if (NiStricmp(pcValue, "CCW") == 0)
        {
            uiValue = NSB_CULL_CCW;
            return true;
        }
        break;
    case NSB_RS_DEBUGMONITORTOKEN:
        if (NiStricmp(pcValue, "ENABLE") == 0)
        {
            uiValue = NSB_DMT_ENABLE;
            return true;
        }
        else if (NiStricmp(pcValue, "DISABLE") == 0)
        {
            uiValue = NSB_DMT_DISABLE;
            return true;
        }
        break;
    case NSB_RS_POSITIONDEGREE:
    case NSB_RS_NORMALDEGREE:
        if (NiStricmp(pcValue, "LINEAR") == 0)
        {
            uiValue = NSB_DEGREE_LINEAR;
            return true;
        }
        else if (NiStricmp(pcValue, "QUADRATIC") == 0)
        {
            if (eRenderState == NSB_RS_NORMALDEGREE)
            {
                uiValue = NSB_DEGREE_QUADRATIC;
                return true;
            }
        }
        else if (NiStricmp(pcValue, "CUBIC") == 0)
        {
            if (eRenderState == NSB_RS_POSITIONDEGREE)
            {
                uiValue = NSB_DEGREE_CUBIC;
                return true;
            }
        }
        else if (NiStricmp(pcValue, "QUINTIC") == 0)
        {
            // NSB_DEGREE_QUINTIC is not a valid value;
        }
        break;
    case NSB_RS_TESSELLATIONMODE:
        if (NiStricmp(pcValue, "DISCRETE") == 0)
        {
            uiValue = NSB_TM_DISCRETE;
            return true;
        }
        else if (NiStricmp(pcValue, "CONTINUOUS") == 0)
        {
            uiValue = NSB_TM_CONTINUOUS;
            return true;
        }
        else if (NiStricmp(pcValue, "PEREDGE") == 0)
        {
            uiValue = NSB_TM_PEREDGE;
            return true;
        }
        break;
    case NSB_RS_CULLFACE:
    case NSB_RS_POLYGONMODEFACE:
        if (NiStricmp(pcValue, "FRONT") == 0)
        {
            uiValue = NSB_FACE_FRONT;
            return true;
        }
        else if (NiStricmp(pcValue, "BACK") == 0)
        {
            uiValue = NSB_FACE_BACK;
            return true;
        }
        else if (NiStricmp(pcValue, "FRONT_AND_BACK") == 0)
        {
            uiValue = NSB_FACE_FRONT_AND_BACK;
            return true;
        }
        break;
    default:
#ifdef NIDEBUG
        NILOG(NIMESSAGE_GENERAL_0, "LookupRenderStateValue(%s): "
            "Render state not found.\n",
            LookupRenderStateString(eRenderState));
#endif
        break;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
const char* NSBRenderStates::LookupRenderStateString(NSBRenderStateEnum eRS)
{
    switch (eRS)
    {
    STATE_CASE_STRING(NSB_RS_ZENABLE)
    STATE_CASE_STRING(NSB_RS_FILLMODE)
    STATE_CASE_STRING(NSB_RS_SHADEMODE)
    STATE_CASE_STRING(NSB_RS_ZWRITEENABLE)
    STATE_CASE_STRING(NSB_RS_ALPHATESTENABLE)
    STATE_CASE_STRING(NSB_RS_LASTPIXEL)
    STATE_CASE_STRING(NSB_RS_SRCBLEND)
    STATE_CASE_STRING(NSB_RS_DESTBLEND)
    STATE_CASE_STRING(NSB_RS_CULLMODE)
    STATE_CASE_STRING(NSB_RS_ZFUNC)
    STATE_CASE_STRING(NSB_RS_ALPHAREF)
    STATE_CASE_STRING(NSB_RS_ALPHAFUNC)
    STATE_CASE_STRING(NSB_RS_DITHERENABLE)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE)
    STATE_CASE_STRING(NSB_RS_FOGENABLE)
    STATE_CASE_STRING(NSB_RS_SPECULARENABLE)
    STATE_CASE_STRING(NSB_RS_FOGCOLOR)
    STATE_CASE_STRING(NSB_RS_FOGTABLEMODE)
    STATE_CASE_STRING(NSB_RS_FOGSTART)
    STATE_CASE_STRING(NSB_RS_FOGEND)
    STATE_CASE_STRING(NSB_RS_FOGDENSITY)
    STATE_CASE_STRING(NSB_RS_RANGEFOGENABLE)
    STATE_CASE_STRING(NSB_RS_STENCILENABLE)
    STATE_CASE_STRING(NSB_RS_STENCILFAIL)
    STATE_CASE_STRING(NSB_RS_STENCILZFAIL)
    STATE_CASE_STRING(NSB_RS_STENCILPASS)
    STATE_CASE_STRING(NSB_RS_STENCILFUNC)
    STATE_CASE_STRING(NSB_RS_STENCILREF)
    STATE_CASE_STRING(NSB_RS_STENCILMASK)
    STATE_CASE_STRING(NSB_RS_STENCILWRITEMASK)
    STATE_CASE_STRING(NSB_RS_TEXTUREFACTOR)
    STATE_CASE_STRING(NSB_RS_WRAP0)
    STATE_CASE_STRING(NSB_RS_WRAP1)
    STATE_CASE_STRING(NSB_RS_WRAP2)
    STATE_CASE_STRING(NSB_RS_WRAP3)
    STATE_CASE_STRING(NSB_RS_WRAP4)
    STATE_CASE_STRING(NSB_RS_WRAP5)
    STATE_CASE_STRING(NSB_RS_WRAP6)
    STATE_CASE_STRING(NSB_RS_WRAP7)
    STATE_CASE_STRING(NSB_RS_CLIPPING)
    STATE_CASE_STRING(NSB_RS_LIGHTING)
    STATE_CASE_STRING(NSB_RS_AMBIENT)
    STATE_CASE_STRING(NSB_RS_FOGVERTEXMODE)
    STATE_CASE_STRING(NSB_RS_COLORVERTEX)
    STATE_CASE_STRING(NSB_RS_LOCALVIEWER)
    STATE_CASE_STRING(NSB_RS_NORMALIZENORMALS)
    STATE_CASE_STRING(NSB_RS_DIFFUSEMATERIALSOURCE)
    STATE_CASE_STRING(NSB_RS_SPECULARMATERIALSOURCE)
    STATE_CASE_STRING(NSB_RS_AMBIENTMATERIALSOURCE)
    STATE_CASE_STRING(NSB_RS_EMISSIVEMATERIALSOURCE)
    STATE_CASE_STRING(NSB_RS_VERTEXBLEND)
    STATE_CASE_STRING(NSB_RS_CLIPPLANEENABLE)
    STATE_CASE_STRING(NSB_RS_POINTSIZE)
    STATE_CASE_STRING(NSB_RS_POINTSIZE_MIN)
    STATE_CASE_STRING(NSB_RS_POINTSPRITEENABLE)
    STATE_CASE_STRING(NSB_RS_POINTSCALEENABLE)
    STATE_CASE_STRING(NSB_RS_POINTSCALE_A)
    STATE_CASE_STRING(NSB_RS_POINTSCALE_B)
    STATE_CASE_STRING(NSB_RS_POINTSCALE_C)
    STATE_CASE_STRING(NSB_RS_MULTISAMPLEANTIALIAS)
    STATE_CASE_STRING(NSB_RS_MULTISAMPLEMASK)
    STATE_CASE_STRING(NSB_RS_PATCHEDGESTYLE)
    STATE_CASE_STRING(NSB_RS_DEBUGMONITORTOKEN)
    STATE_CASE_STRING(NSB_RS_POINTSIZE_MAX)
    STATE_CASE_STRING(NSB_RS_INDEXEDVERTEXBLENDENABLE)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE)
    STATE_CASE_STRING(NSB_RS_TWEENFACTOR)
    STATE_CASE_STRING(NSB_RS_BLENDOP)
    STATE_CASE_STRING(NSB_RS_POSITIONDEGREE)
    STATE_CASE_STRING(NSB_RS_NORMALDEGREE)
    STATE_CASE_STRING(NSB_RS_SCISSORTESTENABLE)
    STATE_CASE_STRING(NSB_RS_SLOPESCALEDEPTHBIAS)
    STATE_CASE_STRING(NSB_RS_ANTIALIASEDLINEENABLE)
    STATE_CASE_STRING(NSB_RS_MINTESSELLATIONLEVEL)
    STATE_CASE_STRING(NSB_RS_MAXTESSELLATIONLEVEL)
    STATE_CASE_STRING(NSB_RS_ADAPTIVETESS_X)
    STATE_CASE_STRING(NSB_RS_ADAPTIVETESS_Y)
    STATE_CASE_STRING(NSB_RS_ADAPTIVETESS_Z)
    STATE_CASE_STRING(NSB_RS_ADAPTIVETESS_W)
    STATE_CASE_STRING(NSB_RS_ENABLEADAPTIVETESSELLATION)
    STATE_CASE_STRING(NSB_RS_TWOSIDEDSTENCILMODE)
    STATE_CASE_STRING(NSB_RS_CCW_STENCILFAIL)
    STATE_CASE_STRING(NSB_RS_CCW_STENCILZFAIL)
    STATE_CASE_STRING(NSB_RS_CCW_STENCILPASS)
    STATE_CASE_STRING(NSB_RS_CCW_STENCILFUNC)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE1)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE2)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE3)
    STATE_CASE_STRING(NSB_RS_BLENDFACTOR)
    STATE_CASE_STRING(NSB_RS_SRGBWRITEENABLE)
    STATE_CASE_STRING(NSB_RS_DEPTHBIAS)
    STATE_CASE_STRING(NSB_RS_WRAP8)
    STATE_CASE_STRING(NSB_RS_WRAP9)
    STATE_CASE_STRING(NSB_RS_WRAP10)
    STATE_CASE_STRING(NSB_RS_WRAP11)
    STATE_CASE_STRING(NSB_RS_WRAP12)
    STATE_CASE_STRING(NSB_RS_WRAP13)
    STATE_CASE_STRING(NSB_RS_WRAP14)
    STATE_CASE_STRING(NSB_RS_WRAP15)
    STATE_CASE_STRING(NSB_RS_SEPARATEALPHABLENDENABLE)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA)
    STATE_CASE_STRING(NSB_RS_VIEWPORTENABLE)
    STATE_CASE_STRING(NSB_RS_HIGHPRECISIONBLENDENABLE)
    STATE_CASE_STRING(NSB_RS_HIGHPRECISIONBLENDENABLE1)
    STATE_CASE_STRING(NSB_RS_HIGHPRECISIONBLENDENABLE2)
    STATE_CASE_STRING(NSB_RS_HIGHPRECISIONBLENDENABLE3)
    STATE_CASE_STRING(NSB_RS_TESSELLATIONMODE)
    STATE_CASE_STRING(NSB_RS_COLORLOGICOPENABLE)
    STATE_CASE_STRING(NSB_RS_CULLFACEENABLE)
    STATE_CASE_STRING(NSB_RS_MULTISAMPLEENABLE)
    STATE_CASE_STRING(NSB_RS_POINTSMOOTHENABLE)
    STATE_CASE_STRING(NSB_RS_POLYGONOFFSETFILLENABLE)
    STATE_CASE_STRING(NSB_RS_SAMPLEALPHATOCOVERAGEENABLE)
    STATE_CASE_STRING(NSB_RS_SAMPLEALPHATOONEENABLE)
    STATE_CASE_STRING(NSB_RS_SAMPLECOVERAGEENABLE)
    STATE_CASE_STRING(NSB_RS_VERTEXPROGRAMPOINTSIZEENABLE)
    STATE_CASE_STRING(NSB_RS_SRCBLENDSEPARATERGB)
    STATE_CASE_STRING(NSB_RS_DSTBLENDSEPARATERGB)
    STATE_CASE_STRING(NSB_RS_BLENDCOLORR)
    STATE_CASE_STRING(NSB_RS_BLENDCOLORG)
    STATE_CASE_STRING(NSB_RS_BLENDCOLORB)
    STATE_CASE_STRING(NSB_RS_BLENDCOLORA)
    STATE_CASE_STRING(NSB_RS_BLENDEQUATIONSEPARATERGB)
    STATE_CASE_STRING(NSB_RS_CULLFACE)
    STATE_CASE_STRING(NSB_RS_COLORMASKR)
    STATE_CASE_STRING(NSB_RS_COLORMASKG)
    STATE_CASE_STRING(NSB_RS_COLORMASKB)
    STATE_CASE_STRING(NSB_RS_COLORMASKA)
    STATE_CASE_STRING(NSB_RS_DEPTHRANGENEAR)
    STATE_CASE_STRING(NSB_RS_DEPTHRANGEFAR)
    STATE_CASE_STRING(NSB_RS_FRONTFACE)
    STATE_CASE_STRING(NSB_RS_LINEWIDTH)
    STATE_CASE_STRING(NSB_RS_POINTSPRITECOORDREPLACE)
    STATE_CASE_STRING(NSB_RS_POLYGONMODEFACE)
    STATE_CASE_STRING(NSB_RS_POLYGONOFFSETFACTOR)
    STATE_CASE_STRING(NSB_RS_POLYGONOFFSETUNITS)
    STATE_CASE_STRING(NSB_RS_SCISSORX)
    STATE_CASE_STRING(NSB_RS_SCISSORY)
    STATE_CASE_STRING(NSB_RS_SCISSORWIDTH)
    STATE_CASE_STRING(NSB_RS_SCISSORHEIGHT)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE1)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE2)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE3)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE4)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE5)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE6)
    STATE_CASE_STRING(NSB_RS_ALPHABLENDENABLE7)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE4)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE5)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE6)
    STATE_CASE_STRING(NSB_RS_COLORWRITEENABLE7)
    STATE_CASE_STRING(NSB_RS_FRONTCCW)
    STATE_CASE_STRING(NSB_RS_DEPTHBIASCLAMP)
    STATE_CASE_STRING(NSB_RS_INDEPENDENTBLENDENABLE)
    STATE_CASE_STRING(NSB_RS_SRCBLEND1)
    STATE_CASE_STRING(NSB_RS_SRCBLEND2)
    STATE_CASE_STRING(NSB_RS_SRCBLEND3)
    STATE_CASE_STRING(NSB_RS_SRCBLEND4)
    STATE_CASE_STRING(NSB_RS_SRCBLEND5)
    STATE_CASE_STRING(NSB_RS_SRCBLEND6)
    STATE_CASE_STRING(NSB_RS_SRCBLEND7)
    STATE_CASE_STRING(NSB_RS_DESTBLEND1)
    STATE_CASE_STRING(NSB_RS_DESTBLEND2)
    STATE_CASE_STRING(NSB_RS_DESTBLEND3)
    STATE_CASE_STRING(NSB_RS_DESTBLEND4)
    STATE_CASE_STRING(NSB_RS_DESTBLEND5)
    STATE_CASE_STRING(NSB_RS_DESTBLEND6)
    STATE_CASE_STRING(NSB_RS_DESTBLEND7)
    STATE_CASE_STRING(NSB_RS_BLENDOP1)
    STATE_CASE_STRING(NSB_RS_BLENDOP2)
    STATE_CASE_STRING(NSB_RS_BLENDOP3)
    STATE_CASE_STRING(NSB_RS_BLENDOP4)
    STATE_CASE_STRING(NSB_RS_BLENDOP5)
    STATE_CASE_STRING(NSB_RS_BLENDOP6)
    STATE_CASE_STRING(NSB_RS_BLENDOP7)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA1)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA2)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA3)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA4)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA5)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA6)
    STATE_CASE_STRING(NSB_RS_SRCBLENDALPHA7)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA1)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA2)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA3)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA4)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA5)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA6)
    STATE_CASE_STRING(NSB_RS_DESTBLENDALPHA7)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA1)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA2)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA3)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA4)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA5)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA6)
    STATE_CASE_STRING(NSB_RS_BLENDOPALPHA7)

    default:
        return "***** UNKNOWN RENDER STATE ****";
    }
}

//------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
