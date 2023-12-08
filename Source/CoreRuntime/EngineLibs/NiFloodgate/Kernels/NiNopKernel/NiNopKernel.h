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
#ifndef NINOPKERNEL_H
#define NINOPKERNEL_H

#include EE_PLATFORM_SPECIFIC_INCLUDE(NiFloodgate,NiSPKernelMacros,h)

/**
    A Floodgate kernel with an empty Execute function for use with Signal
    tasks.
*/
NiSPDeclareKernelLib(NiNopKernel, NIFLOODGATE_ENTRY)

#endif

