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

#include <eon/IReplicationGroupPolicy.h>
#include <efd/IConfigManager.h>
#include <efd/IConfigSection.h>
#include <eon/GroupUpdatePolicy.h>
#include <efd/QOSCompare.h>

using namespace eon;
using namespace efd;
using namespace egf;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(IReplicationGroupPolicyData);

efd::SmartPointer<IReplicationGroupPolicy>
    IReplicationGroupPolicy::m_groups[k_MAX_REPLICATION_GROUPS];

const static char* kConfigSection = "ReplicationGroup";
const static char* kQualityOfService = "QualityOfService";
const static char* kTimeout = "MinDelta";
const static char* kUpdateAll = "AlwaysUpdateAllProperties";
const static char* kUpdateAsDiscovery = "TreatUpdatesAsDiscovers";

//-------------------------------------------------------------------------------------------------
void IReplicationGroupPolicy::_SDMShutdown()
{
    for (unsigned int ui = 0; ui < k_MAX_REPLICATION_GROUPS; ++ui)
    {
        m_groups[ui] = NULL;
    }
}

//------------------------------------------------------------------------------------------------
IReplicationGroupPolicy::IReplicationGroupPolicy()
: m_messageQuality(QOS_RELIABLE)
, m_updateAll(false)
, m_treatUpdatesAsDiscovers(false)
, m_minUpdateDelta(0.0)
{
}

//-------------------------------------------------------------------------------------------------
IReplicationGroupPolicy::~IReplicationGroupPolicy()
{
}

//-------------------------------------------------------------------------------------------------
void IReplicationGroupPolicy::Init(efd::IConfigManager* pIConfigManager, EnumManager* pEnumManager)
{
    EE_ASSERT(pIConfigManager);

    for (UInt32 i = 0; i < k_MAX_REPLICATION_GROUPS; ++i)
    {
        efd::utf8string replicationGroupSection(
            Formatted, 
            "%s%d",
            kConfigSection,
            i);

        // setup defaults
        if (!m_groups[i])
        {
            m_groups[i] = EE_NEW GroupUpdatePolicy();
        }

        const ISection* pSection = pIConfigManager->GetConfiguration()->FindSection(
            replicationGroupSection.c_str());
        if (pSection)
        {
            // read QOS
            efd::utf8string temp = pSection->FindValue(kQualityOfService);
            if (!temp.empty())
            {
                QualityOfService foundQOS = QOSCompare::GetVirtualQOS(pEnumManager, temp);
                // if the lookup fails try Physical QOS
                if (foundQOS == QOS_INVALID)
                {
                    foundQOS = QOSCompare::GetPhysicalQOS(pEnumManager, temp);
                }
                // if the second lookup fails, use the default
                if (foundQOS != QOS_INVALID)
                {
                    m_groups[i]->SetQualityOfService(foundQOS);
                }
            }

            // read timeout
            temp = pSection->FindValue(kTimeout);
            if (!temp.empty())
            {
                m_groups[i]->SetMinUpdateDelta(atof(temp.c_str()));
            }

            // read send all
            efd::Bool updateAll = pSection->IsTrue(kUpdateAll);
            m_groups[i]->SetUpdateAll(updateAll);

            bool bUpdateAsDiscovery = pSection->IsTrue(kUpdateAsDiscovery);
            m_groups[i]->SetTreatUpdatesAsDiscovers(bUpdateAsDiscovery);

            m_groups[i]->ReadConfiguration(pSection);
        }
    }
}

//------------------------------------------------------------------------------------------------
void IReplicationGroupPolicy::ReadConfiguration(const efd::ISection*)
{
    /* do nothing */
}

//------------------------------------------------------------------------------------------------
IReplicationGroupPolicyData::~IReplicationGroupPolicyData()
{
}

//------------------------------------------------------------------------------------------------
