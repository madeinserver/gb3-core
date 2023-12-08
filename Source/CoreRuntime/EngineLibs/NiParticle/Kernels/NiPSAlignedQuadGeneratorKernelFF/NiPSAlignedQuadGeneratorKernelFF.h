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

#ifndef NIPSALIGNEDQUADGENERATORKERNELFF_H
#define NIPSALIGNEDQUADGENERATORKERNELFF_H

#include <NiParticleLibType.h>
#include EE_PLATFORM_SPECIFIC_INCLUDE(NiFloodgate,NiSPKernelMacros,h)
#include <NiPSKernelDefinitions.h>
#include <NiPSAlignedQuadGeneratorKernelStruct.h>

/**
    Generates aligned quads for a particle system where the alignment is the
    same for every particle.

    Each quad is composed of two triangles. Each vertex has a normal and
    optionally a color. Rotation angles, if available, are applied on top
    of the alignment.
*/
NiSPDeclareKernelLib(NiPSAlignedQuadGeneratorKernelFF, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSALIGNEDQUADGENERATORKERNELFF_H
