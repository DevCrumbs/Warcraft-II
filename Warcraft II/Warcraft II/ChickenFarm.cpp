#include "Defs.h"
#include "p2Log.h"

#include "ChickenFarm.h"

#include "j1Player.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "j1Movement.h"

ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
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

	isBuilt = chickenFarmInfo.isBuilt;

	if (isBuilt) {
		texArea = &chickenFarmInfo.completeTexArea;
		App->player->currentFood += 3;
		App->scene->hasFoodChanged = true;
		buildingState = BuildingState_Normal;
	}
	else if (!isBuilt) {
		texArea = &chickenFarmInfo.constructionPlanks1;
		this->constructionTimer.Start();
		buildingState = BuildingState_Building;
		App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
	}

	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;
}

ChickenFarm::~ChickenFarm() {
	App->player->currentFood -= 3;
	App->scene->hasFoodChanged = true;
};

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	if(!isBuilt)
		UpdateAnimations(dt);
	
	if (constructionTimer.Read() >= (constructionTime * 1000) && !isBuilt) {
		isBuilt = true;
		App->player->currentFood += 3;
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

		if (constructionTimer.Read() >= constructionTime * 1000) {
			texArea = &chickenFarmInfo.completeTexArea;
			buildingState = BuildingState_Normal;
		}

}
