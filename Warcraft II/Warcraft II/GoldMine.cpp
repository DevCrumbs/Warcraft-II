#include "Defs.h"
#include "p2Log.h"

#include "GoldMine.h"

#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), goldMineInfo(goldMineInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_NeutralBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_GOLD_MINE;
	*(EntitySide*)&entitySide = EntitySide_Neutral;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Medium;

	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 1 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &goldMineInfo.completeTexArea;
	isBuilt = true;
}

void GoldMine::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (buildingState == BuildingState_Destroyed && !isUnitGatheringGold)
		texArea = &goldMineInfo.destroyedTexArea;
}

// Gather gold
bool GoldMine::IsUnitGatheringGold() const
{
	return isUnitGatheringGold;
}

void GoldMine::SetUnitGatheringGold(bool isUnitGatheringGold)
{
	this->isUnitGatheringGold = isUnitGatheringGold;
}

// Tex area
void GoldMine::SwapTexArea()
{
	if (texArea == &goldMineInfo.completeTexArea)
		texArea = &goldMineInfo.inProgressTexArea;
	else
		texArea = &goldMineInfo.completeTexArea;
}

// Gold Mine state
void GoldMine::SetGoldMineState(GoldMineState goldMineState)
{
	this->goldMineState = goldMineState;
}

GoldMineState GoldMine::GetGoldMineState() const 
{
	return goldMineState;
}