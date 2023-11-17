module("EseOnlineProducer", package.seeall)

-- Behavior
function OnCreate(self, params)
    -- set this entity to replicate Group 0 using the system-defined BasicReplication category
    baseID = BehaviorAPI.GetEnumValue("eonDemoSystemServiceIDs", "BasicReplication")
    replicationCategory = BehaviorAPI.GetServicePublicCategory(baseID)
    bapiOnline.SetReplicationCategory(self, 0, replicationCategory)
end
