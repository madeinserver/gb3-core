module("EseGriddedConsumer", package.seeall)

-- Behavior
function OnCreate(self, params)
  local gridCategoryToConsume = 0 -- an arbitrary value
  local realCat = BehaviorAPI.CreateApplicationCategory(gridCategoryToConsume)
  -- Initial consumer subscription for any Replication Group
  bapiOnline.SubscribeReplicationChannel(realCat)

  -- Keep track of the currently consumed categories (one for each nearby grid cell)
  -- The key of the map holds the category (for easy lookup), like a set.
  self.ConsumingCategories[gridCategoryToConsume] = "yes"

  -- Recompute subscription every so often
  self:SendEvent("OnUpdateConsumerSubscription", nil, "", 0.1)

  local thing = 8
end

-- Note: this must match the computation in GriddedProducer
function ComputeGriddedCategorySet(position, gridSize, range)
  local xmin = math.floor((2000+position[1]-range)/gridSize)
  local xmax = math.floor((2000+position[1]+range)/gridSize)
  local ymin = math.floor((2000+position[2]-range)/gridSize)
  local ymax = math.floor((2000+position[2]+range)/gridSize)

  -- OK, so it is square
  local set = {}
  for x = xmin,xmax do
    for y = ymin,ymax do
      -- assume 1000x1000 grid cells max
      local cat =  x * 1000 + y + 0
      set[tonumber(cat)] = "yes"
    end
  end

  return set
end

-- Behavior
function OnUpdateConsumerSubscription(self, params)
  local updateDelta = self.ConsumerUpdateDelta
  -- Recompute subscription every so often
  self:SendEvent("OnUpdateConsumerSubscription", nil, "", updateDelta)

  -- Recompute grids to consume.
  local position = self.Position
  local range = self.Range
  local gridSize = self.GridSize

  local categorySet = ComputeGriddedCategorySet(position, gridSize, range)

  -- get the list of categories we are consuming (stored in the keys)

  local key = BehaviorAPI.GetNextPropertyKey(self, "ConsumingCategories", nil)

  while key ~= "" do
    local realCat = BehaviorAPI.CreateApplicationCategory(key)

    -- trick the iterator, in case I delete with the key
    local nextkey = BehaviorAPI.GetNextPropertyKey(self, "ConsumingCategories", key)

    -- Is this old category still in the compute set? Leave it, or delete it.
    local what = categorySet[tonumber(key)]
    if categorySet[tonumber(key)] == nil then
        -- not in new set, unsubscribe
        bapiOnline.UnsubscribeReplicationChannel(realCat)

        -- Now forget that we were subscribed to this one
        BehaviorAPI.RemovePropertyValue(self, "ConsumingCategories", key)
    else
        -- already subscribed, so don't redo it
        categorySet[tonumber(key)] = nil
    end

    -- Advance to the next key
    key = nextkey
  end

  for cat, v in pairs(categorySet) do
    local realCat = BehaviorAPI.CreateApplicationCategory(cat)
    if v ~= nil then
      -- this is a new category we need to add
      bapiOnline.SubscribeReplicationChannel(realCat)

      -- now remember that we are subscribe to this one...
      self.ConsumingCategories[cat] = "yes"
    end
  end

end
