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

#include "eonPCH.h"

#include <eon/eonSDM.h>
#include <eon/IReplicationGroupPolicy.h>

// The second argument is dependancies, but efd is always implicitly included
EE_IMPLEMENT_SDM_CONSTRUCTOR(eon, "egf");

#if defined(EE_EON_EXPORT) && !defined(EE_PLATFORM_LINUX)
EE_IMPLEMENT_DLLMAIN(eon);
#endif

//--------------------------------------------------------------------------------------------------
void eonSDM::Init()
{
    EE_IMPLEMENT_SDM_INIT_CHECK();

}
//--------------------------------------------------------------------------------------------------
void eonSDM::Shutdown()
{
    EE_IMPLEMENT_SDM_SHUTDOWN_CHECK();

    eon::IReplicationGroupPolicy::_SDMShutdown();
}
//--------------------------------------------------------------------------------------------------
