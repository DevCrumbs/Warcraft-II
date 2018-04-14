#include "TrollLumberMill.h"
#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

TrollLumberMill::TrollLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const TrollLumberMillInfo& trollLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), trollLumberMillInfo(trollLumberMillInfo)
{
	buildingSize = Medium;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	texArea = &trollLumberMillInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void TrollLumberMill::Move(float dt) {


}

// Animations
void TrollLumberMill::LoadAnimationsSpeed()
{

}
void TrollLumberMill::UpdateAnimations(float dt)
{

}


