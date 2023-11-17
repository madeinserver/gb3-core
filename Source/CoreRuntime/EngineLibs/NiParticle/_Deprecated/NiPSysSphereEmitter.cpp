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

#include "NiPSysSphereEmitter.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysSphereEmitter, NiPSysVolumeEmitter);

//--------------------------------------------------------------------------------------------------
NiPSysSphereEmitter::NiPSysSphereEmitter() :
    m_fEmitterRadius(0.0f)
{
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiPSysSphereEmitter);

//--------------------------------------------------------------------------------------------------
void NiPSysSphereEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
}

//--------------------------------------------------------------------------------------------------
void NiPSysSphereEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSysSphereEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSysSphereEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
}

//--------------------------------------------------------------------------------------------------
bool NiPSysSphereEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSphereEmitter* pkDest = (NiPSysSphereEmitter*) pkObject;

    if (m_fEmitterRadius != pkDest->m_fEmitterRadius)
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
