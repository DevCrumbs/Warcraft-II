#include "PigFarm.h"
#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

PigFarm::PigFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const PigFarmInfo& pigFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), pigFarmInfo(pigFarmInfo)
{
	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	texArea = &pigFarmInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
}

void PigFarm::Move(float dt) {


}

// Animations
void PigFarm::LoadAnimationsSpeed()
{

}
void PigFarm::UpdateAnimations(float dt)
{

}
