#include "Defs.h"
#include "p2Log.h"

#include "TempleOfTheDamned.h"

#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

TempleOfTheDamned::TempleOfTheDamned(fPoint pos, iPoint size, int currLife, uint maxLife, const TempleOfTheDamnedInfo& templeOfTheDamnedInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), templeOfTheDamnedInfo(templeOfTheDamnedInfo)
{
	buildingSize = Medium;

	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	walkability.push_back({ buildingTile.x, buildingTile.y });
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

	texArea = &templeOfTheDamnedInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void TempleOfTheDamned::Move(float dt) {


}

// Animations
void TempleOfTheDamned::LoadAnimationsSpeed()
{

}
void TempleOfTheDamned::UpdateAnimations(float dt)
{

}



