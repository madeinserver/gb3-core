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

#include "VisualizerConnectionTracker.h"
#include <efd/IDs.h>
#include <egf/Entity.h>

using namespace egmVisualizers;
using namespace egf;
using namespace efd;

//--------------------------------------------------------------------------------------------------
VisualizerConnectionTracker::VisualizerConnectionTracker()
: m_removing(egf::kENTITY_INVALID)
{
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::GetConnections(
    egf::EntityID entityID,
    efd::list<IPropertyVisualizerPtr>& connections)
{
    connections = m_connectionMap[entityID];
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::GetUnresolvedConnections(
    efd::list<IPropertyVisualizerPtr>& connections)
{
    connections = m_connectionMap[EntityID::kENTITY_INVALID];
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::AddConnection(
    egf::EntityID entityID,
    IPropertyVisualizerPtr spVisualizer)
{
    m_connectionMap[entityID].push_back(spVisualizer);
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::RemoveConnection(
    egf::EntityID entityID,
    IPropertyVisualizerPtr spVisualizer)
{
    list<IPropertyVisualizerPtr> propertyVisualizers;
    if (m_connectionMap.find(entityID, propertyVisualizers))
    {
        list<IPropertyVisualizerPtr>::iterator itr = m_connectionMap[entityID].find(spVisualizer);

        if (itr != m_connectionMap[entityID].end())
        {
            m_connectionMap[entityID].erase(itr);
        }

        if (m_connectionMap[entityID].empty())
        {
            m_connectionMap.erase(entityID);
        }
    }
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::SetupEntityForRemoval(egf::EntityID entityID)
{
    m_removing = entityID;
}
//--------------------------------------------------------------------------------------------------
bool VisualizerConnectionTracker::IsEntityBeingRemoved(egf::EntityID entityID)
{
    return (m_removing == entityID);
}
//--------------------------------------------------------------------------------------------------
void VisualizerConnectionTracker::RemoveEntity()
{
    m_connectionMap.erase(m_removing);
    m_removing = egf::kENTITY_INVALID;
}
//--------------------------------------------------------------------------------------------------
