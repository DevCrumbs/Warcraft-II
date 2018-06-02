#include "Defs.h"
#include "p2Log.h"

#include "GreatHall.h"

#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

GreatHall::GreatHall(fPoint pos, iPoint size, int currLife, uint maxLife, const GreatHallInfo& greatHallInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), greatHallInfo(greatHallInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_OrcishBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_GREAT_HALL;
	*(EntitySide*)&entitySide = EntitySide_Enemy;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Big;

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
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 3 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &greatHallInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
}

void GreatHall::Move(float dt) {


}

// Animations
void GreatHall::LoadAnimationsSpeed()
{

}
void GreatHall::UpdateAnimations(float dt)
{

}

