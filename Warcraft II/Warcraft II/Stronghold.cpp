#include "Defs.h"
#include "p2Log.h"

#include "Stronghold.h"

#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

Stronghold::Stronghold(fPoint pos, iPoint size, int currLife, uint maxLife, const StrongholdInfo& strongholdInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), strongholdInfo(strongholdInfo)
{
	buildingSize = Big;

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

	texArea = &strongholdInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void Stronghold::Move(float dt) {


}

// Animations
void Stronghold::LoadAnimationsSpeed()
{

}
void Stronghold::UpdateAnimations(float dt)
{

}
