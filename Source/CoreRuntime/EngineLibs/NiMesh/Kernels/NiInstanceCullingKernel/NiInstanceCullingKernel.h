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
#ifndef NIINSTANCECULLINGKERNEL_H
#define NIINSTANCECULLINGKERNEL_H

#include EE_PLATFORM_SPECIFIC_INCLUDE(NiFloodgate,NiSPKernelMacros,h)


/**
    The NiInstanceCullingKernel is a NiFloodgate kernel class used
    to perform per-instance culling.

    It is used by the NiInstancingMeshModifier class and should never
    need to be used directly by an application.
*/
NiSPDeclareKernel(NiInstanceCullingKernel)

#endif // NIINSTANCECULLINGKERNEL_H

