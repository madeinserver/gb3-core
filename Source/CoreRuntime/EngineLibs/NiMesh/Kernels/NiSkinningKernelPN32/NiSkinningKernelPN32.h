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
#ifndef NISKINNINGKERNELPN32_H
#define NISKINNINGKERNELPN32_H

#include EE_PLATFORM_SPECIFIC_INCLUDE(NiFloodgate,NiSPKernelMacros,h)

/**
    The NiSkinningKernels is a Floodgate kernel class used
    to perform software skinning deformation.

    It is used by the NiSkinningMeshModifier class and should never
    need to be used directly by an application.
*/
NiSPDeclareKernel(NiSkinningKernelPN32)

#endif

