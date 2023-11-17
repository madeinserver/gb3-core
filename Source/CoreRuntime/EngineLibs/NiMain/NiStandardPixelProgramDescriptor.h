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

#ifndef NISTANDARDPIXELPROGRAMDESCRIPTOR_H
#define NISTANDARDPIXELPROGRAMDESCRIPTOR_H

#include "NiLight.h"
#include "NiGPUProgramDescriptor.h"
#include "NiBitfield.h"

class NIMAIN_ENTRY NiStandardPixelProgramDescriptor :
    public NiGPUProgramDescriptor
{
public:
    NiStandardPixelProgramDescriptor();

    unsigned int GetUVSetForMap(unsigned int uiWhichMap);
    void SetUVSetForMap(unsigned int uiWhichMap, unsigned int uiUVSet);

    unsigned int GetStandardTextureCount();
    unsigned int GetInputUVCount();

    NiString ToString();

    NiBeginDefaultBitfieldEnum()

    // First Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(APPLYMODE,    1, BITFIELDSTART, 0)
    NiDeclareDefaultIndexedBitfieldEntry(WORLDPOSITION,   1, APPLYMODE, 0)
    NiDeclareDefaultIndexedBitfieldEntry(WORLDNORMAL,   1, WORLDPOSITION, 0)
    NiDeclareDefaultIndexedBitfieldEntry(WORLDNBT,  1, WORLDNORMAL, 0)
    NiDeclareDefaultIndexedBitfieldEntry(WORLDVIEW, 1, WORLDNBT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPTYPE,   2, WORLDVIEW, 0)
    NiDeclareDefaultIndexedBitfieldEntry(PARALLAXMAPCOUNT,1, NORMALMAPTYPE, 0)

    // Second Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(BASEMAPCOUNT,     1, PARALLAXMAPCOUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(NORMALMAPCOUNT,   1, BASEMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(DARKMAPCOUNT,     1, NORMALMAPCOUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(DETAILMAPCOUNT,   1, DARKMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(BUMPMAPCOUNT,     1, DETAILMAPCOUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(GLOSSMAPCOUNT,    1, BUMPMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(GLOWMAPCOUNT,     1, GLOSSMAPCOUNT, 0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP00COUNT, 1, GLOWMAPCOUNT, 0)

    // Third Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP01COUNT, 1, CUSTOMMAP00COUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP02COUNT, 1, CUSTOMMAP01COUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP03COUNT, 1, CUSTOMMAP02COUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(CUSTOMMAP04COUNT, 1, CUSTOMMAP03COUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(DECALMAPCOUNT,    3, CUSTOMMAP04COUNT,
        0)
    NiDeclareDefaultIndexedBitfieldEntry(FOGENABLED,       1, DECALMAPCOUNT, 0)

    // Fourth Byte, Index 0
    NiDeclareDefaultIndexedBitfieldEntry(ENVMAPTYPE,   3,  FOGENABLED, 0)
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPCOUNT, 2, ENVMAPTYPE, 0)
    NiDeclareDefaultLastIndexedBitfieldEntry(PROJLIGHTMAPTYPES, 3,
        PROJLIGHTMAPCOUNT, 0)

    // First Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(PROJLIGHTMAPCLIPPED, 3, BITFIELDSTART,
        1)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,  2,
        PROJLIGHTMAPCLIPPED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPTYPES,  3,
        PROJSHADOWMAPCOUNT, 1)

    // Second Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(PROJSHADOWMAPCLIPPED, 3,
        PROJSHADOWMAPTYPES, 1)
    NiDeclareDefaultIndexedBitfieldEntry(PERVERTEXLIGHTING, 1,
        PROJSHADOWMAPCLIPPED, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP00,  4,
        PERVERTEXLIGHTING, 1)

    // Third Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP01, 4, UVSETFORMAP00, 1)
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP02, 4, UVSETFORMAP01, 1)

    // Fourth Byte, Index 1
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP03, 4, UVSETFORMAP02, 1)
    NiDeclareDefaultLastIndexedBitfieldEntry(UVSETFORMAP04, 4, UVSETFORMAP03,
        1)

    // First Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP05, 4, BITFIELDSTART, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP06, 4, UVSETFORMAP05, 2)

    // Second Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP07, 4, UVSETFORMAP06, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP08, 4, UVSETFORMAP07, 2)

    // Third Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP09, 4, UVSETFORMAP08, 2)
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP10, 4, UVSETFORMAP09, 2)

    // Fourth Byte, Index 2
    NiDeclareDefaultIndexedBitfieldEntry(UVSETFORMAP11, 4, UVSETFORMAP10, 2)
    NiDeclareDefaultLastIndexedBitfieldEntry(POINTLIGHTCOUNT, 4, UVSETFORMAP11,
        2)

    // First Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(SPOTLIGHTCOUNT, 4, BITFIELDSTART, 3)
    NiDeclareDefaultIndexedBitfieldEntry(DIRLIGHTCOUNT, 4, SPOTLIGHTCOUNT, 3)

    // Second Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWMAPFORLIGHT, 8, DIRLIGHTCOUNT,
        3)

    // Third Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(SPECULAR, 1, SHADOWMAPFORLIGHT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2, SPECULAR, 3)
    NiDeclareDefaultIndexedBitfieldEntry(LIGHTINGMODE, 2, AMBDIFFEMISSIVE, 3)
    NiDeclareDefaultIndexedBitfieldEntry(APPLYAMBIENT, 1, LIGHTINGMODE, 3)
    NiDeclareDefaultIndexedBitfieldEntry(BASEMAPALPHAONLY, 1, APPLYAMBIENT, 3)
    NiDeclareDefaultIndexedBitfieldEntry(APPLYEMISSIVE, 1, BASEMAPALPHAONLY, 3)

    // Forth Byte, Index 3
    NiDeclareDefaultIndexedBitfieldEntry(SHADOWTECHNIQUE,  4, APPLYEMISSIVE, 3)
    NiDeclareDefaultIndexedBitfieldEntry(PSSMSLICETRANSITIONSENABLED, 1,
        SHADOWTECHNIQUE, 3)
    NiDeclareDefaultLastIndexedBitfieldEntry(PSSMSLICECOUNT, 3,
        PSSMSLICETRANSITIONSENABLED, 3)

    // First Byte, Index 4
    NiDeclareDefaultIndexedBitfieldEntry(PSSMWHICHLIGHT, 4, BITFIELDSTART, 4)
    NiDeclareDefaultIndexedBitfieldEntry(ALPHATEST,  1, PSSMWHICHLIGHT, 4)       
    // 5 bits free for future use

    NiEndDefaultBitfieldEnum();
};

#endif  //#ifndef NISTANDARDPIXELPROGRAMDESCRIPTOR_H
