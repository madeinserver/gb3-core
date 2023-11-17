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

#include "egmVisualizersPCH.h"

#include "VisualizerVisibilityTracker.h"
#include <efd/IDs.h>
#include <egf/Entity.h>

using namespace egmVisualizers;
using namespace egf;
using namespace efd;

//-------------------------------------------------------------------------------------------------
void VisualizerVisibilityTracker::SetEntityVisibility(
    egf::EntityID entityID,
    const efd::utf8string& source,
    efd::UInt32 category,
    bool isVisible)
{
    if (!isVisible)
        m_entityVisibility[entityID][source] |= (1 << category);
    else
        m_entityVisibility[entityID][source] &= ~(1 << category);

    // Remove the source from the map if it indicates the entity is visible.
    // This way, when determining visibility, we can simply check if any
    // sources are in the map - if not, the entity is visible.
    if (m_entityVisibility[entityID][source] == 0)
        m_entityVisibility[entityID].erase(source);
}
//-------------------------------------------------------------------------------------------------
bool VisualizerVisibilityTracker::IsEntityVisible(egf::EntityID entityID)
{
    return m_entityVisibility[entityID].size() == 0;
}
//-------------------------------------------------------------------------------------------------
