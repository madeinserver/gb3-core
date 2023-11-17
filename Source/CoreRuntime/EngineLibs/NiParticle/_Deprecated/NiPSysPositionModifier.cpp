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

#include "NiPSysPositionModifier.h"

NiImplementRTTI(NiPSysPositionModifier, NiPSysModifier);

//--------------------------------------------------------------------------------------------------
NiPSysPositionModifier::NiPSysPositionModifier()
{
}

//--------------------------------------------------------------------------------------------------
// Streaming
//--------------------------------------------------------------------------------------------------
NiImplementCreateObject(NiPSysPositionModifier);

//--------------------------------------------------------------------------------------------------
void NiPSysPositionModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSysPositionModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSysPositionModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}

//--------------------------------------------------------------------------------------------------
void NiPSysPositionModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}

//--------------------------------------------------------------------------------------------------
bool NiPSysPositionModifier::IsEqual(NiObject* pkObject)
{
    return NiPSysModifier::IsEqual(pkObject);
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
