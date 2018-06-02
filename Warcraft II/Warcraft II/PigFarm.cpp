#include "Defs.h"
#include "p2Log.h"

#include "PigFarm.h"

#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

PigFarm::PigFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const PigFarmInfo& pigFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), pigFarmInfo(pigFarmInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_OrcishBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_PIG_FARM;
	*(EntitySide*)&entitySide = EntitySide_Enemy;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Small;

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
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &pigFarmInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
}

void PigFarm::Move(float dt) 
{
}

// Animations
void PigFarm::LoadAnimationsSpeed()
{

}
void PigFarm::UpdateAnimations(float dt)
{

}
