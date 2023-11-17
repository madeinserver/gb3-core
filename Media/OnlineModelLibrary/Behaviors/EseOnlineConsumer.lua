module("EseOnlineConsumer", package.seeall)

-- Behavior
function OnCreate(self, params)
    -- Setup application to observe the BasicReplication messages
    baseID = BehaviorAPI.GetEnumValue("eonDemoSystemServiceIDs", "BasicReplication")
    categoryToConsume = BehaviorAPI.GetServicePublicCategory(baseID)
    bapiOnline.SubscribeReplicationChannel(categoryToConsume)
end
