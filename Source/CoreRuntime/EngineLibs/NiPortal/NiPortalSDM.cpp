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
#include "NiPortalPCH.h"

#include "NiPortal.h"
#include "NiPortalSDM.h"
#include "NiRoom.h"
#include "NiRoomGroup.h"

NiImplementSDMConstructor(NiPortal, "NiMesh NiFloodgate NiMain");

#ifdef NIPORTAL_EXPORT
NiImplementDllMain(NiPortal);
#endif

//--------------------------------------------------------------------------------------------------
void NiPortalSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiPortal);
    NiRegisterStream(NiRoom);
    NiRegisterStream(NiRoomGroup);

    // NiWall has been deprecated.  NiRoom took its place.
    NiStream::RegisterLoader("NiWall", NiRoom::CreateOldWallObject);
}

//--------------------------------------------------------------------------------------------------
void NiPortalSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiPortal);
    NiUnregisterStream(NiRoom);
    NiUnregisterStream(NiRoomGroup);

    // NiWall has been deprecated.  NiRoom took its place.
    NiUnregisterStream(NiWall);
}

//--------------------------------------------------------------------------------------------------
