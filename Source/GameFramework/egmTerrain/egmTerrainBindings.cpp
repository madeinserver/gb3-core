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

// Pre-compiled header
#include "egmTerrainPCH.h"


#include <efd/ecrLogIDs.h>

#include "egmTerrainBindings.h"
#include "TerrainService.h"

#include <egf/ScriptContext.h>

using namespace egf;
using namespace egmTerrain;

//--------------------------------------------------------------------------------------------------
efd::Bool egmTerrainBindings::FindTerrainIntersection(
    const efd::Point3& i_pt,
    efd::Float32* o_height,
    efd::Point3* o_normal)
{
    TerrainService* pTerrainService = g_bapiContext.GetSystemServiceAs<TerrainService>();
    EE_ASSERT(pTerrainService);
    EE_ASSERT(o_height);
    EE_ASSERT(o_normal);

    return pTerrainService->GetTerrainHeightAt(i_pt, *o_height, *o_normal);
}
//--------------------------------------------------------------------------------------------------