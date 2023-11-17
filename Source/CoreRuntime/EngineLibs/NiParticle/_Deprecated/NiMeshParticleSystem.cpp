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
#include <NiParticlePCH.h>

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING

#include "NiMeshParticleSystem.h"

NiImplementRTTI(NiMeshParticleSystem, NiParticleSystem);

//--------------------------------------------------------------------------------------------------
NiMeshParticleSystem::NiMeshParticleSystem()
{
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiMeshParticleSystem);

//--------------------------------------------------------------------------------------------------
void NiMeshParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiMeshParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiMeshParticleSystem::RegisterStreamables(NiStream& kStream)
{
    return NiParticleSystem::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiMeshParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiMeshParticleSystem::IsEqual(NiObject* pkObject)
{
    return NiParticleSystem::IsEqual(pkObject);
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
