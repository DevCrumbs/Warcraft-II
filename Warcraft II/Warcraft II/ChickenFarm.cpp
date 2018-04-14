#include "ChickenFarm.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"

ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
	entitySide = EntitySide_Enemy;
	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;

	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	//uint i = App->scene->data[App->scene->w * buildingTile.y + buildingTile.x];
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	isBuilt = chickenFarmInfo.isBuilt;

	if (isBuilt)
		texArea = &chickenFarmInfo.completeTexArea;
	else if (!isBuilt) {
		texArea = &chickenFarmInfo.constructionPlanks1;
		this->constructionTimer.Start();
		App->audio->PlayFx(2, 0); //Construction sound
	}
}

ChickenFarm::~ChickenFarm() {
	App->player->currentFood -= 4;
	App->scene->hasFoodChanged = true;
};

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	if(!isBuilt)
		UpdateAnimations(dt);
	
	if (constructionTimer.Read() >= (constructionTime * 1000) && isBuilt == false) {
		isBuilt = true;
		App->player->currentFood += 4;
		App->scene->hasFoodChanged = true;

	}
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{
		if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
			texArea = &chickenFarmInfo.constructionPlanks2;

		if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
			texArea = &chickenFarmInfo.inProgressTexArea;

		if (constructionTimer.Read() >= constructionTime * 1000)
			texArea = &chickenFarmInfo.completeTexArea;

}
