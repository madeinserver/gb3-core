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

#include "NiPSysVolumeEmitter.h"
#include <NiAVObject.h>
#include <NiStream.h>

NiImplementRTTI(NiPSysVolumeEmitter, NiPSysEmitter);

//--------------------------------------------------------------------------------------------------
NiPSysVolumeEmitter::NiPSysVolumeEmitter() : m_pkEmitterObj(NULL)
{
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
void NiPSysVolumeEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkEmitterObj
}

//--------------------------------------------------------------------------------------------------
void NiPSysVolumeEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);

    m_pkEmitterObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}

//--------------------------------------------------------------------------------------------------
bool NiPSysVolumeEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSysVolumeEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkEmitterObj);
}

//--------------------------------------------------------------------------------------------------
bool NiPSysVolumeEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysVolumeEmitter* pkDest = (NiPSysVolumeEmitter*) pkObject;

    if ((m_pkEmitterObj && !pkDest->m_pkEmitterObj) ||
        (!m_pkEmitterObj && pkDest->m_pkEmitterObj))
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
