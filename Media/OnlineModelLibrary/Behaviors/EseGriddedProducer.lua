module("EseGriddedProducer", package.seeall)

-- Behavior
function OnCreate(self, params)
  -- Initial producer category for Replication Group 0
  local gridCategoryToProduce = BehaviorAPI.GetGloballyUniqueCategory() -- an arbitrary value
  bapiOnline.SetReplicationCategory(self, 0, gridCategoryToProduce)

  -- Periodically update where we are producing
  self:SendEvent("OnUpdateProducerCategory", nil, "", 0.1)

  local thing = 7

end

function ComputeGriddedCategory(position, gridSize)

  -- 100 unit grids. make it positive
  local xgrid = math.floor((2000+position[1])/gridSize)
  local ygrid = math.floor((2000+position[2])/gridSize)

  -- assume 1000x1000 grid cells max
  local cat = xgrid * 1000 + ygrid + 0 -- we use first 1M app categories

  return cat
end

-- Behavior
function OnUpdateProducerCategory(self, params)

   -- Resend the update request
  local updateDelta = self.ProducerUpdateDelta
  -- Recompute subscription every so often
  self:SendEvent("OnUpdateProducerCategory", nil, "", updateDelta)

  local position = self.Position
  local gridSize = self.GridSize

  local cat = ComputeGriddedCategory(position, gridSize)

  -- set the category for replication group 0
  local realCat = BehaviorAPI.CreateApplicationCategory(cat)
  bapiOnline.SetReplicationCategory(self, 0, realCat)

end

