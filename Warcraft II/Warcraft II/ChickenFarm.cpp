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
#include "j1Particles.h"
#include "j1FadeToBlack.h"

ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_HumanBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_CHICKEN_FARM;
	*(EntitySide*)&entitySide = EntitySide_Player;
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

	isBuilt = chickenFarmInfo.isBuilt;

	if (isBuilt) {
		texArea = &chickenFarmInfo.completeTexArea;
		App->player->currentFood += 3;
		App->scene->hasFoodChanged = true;
		buildingState = BuildingState_Normal;
	}
	else if (!isBuilt) {
		texArea = &chickenFarmInfo.constructionPlanks1;
		buildingState = BuildingState_Building;
		App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
	}

	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;
}

ChickenFarm::~ChickenFarm() 
{
	App->player->currentFood -= 3;
	App->scene->hasFoodChanged = true;

	if (peasants != nullptr) {
		peasants->isRemove = true;
		peasants = nullptr;
	}

	LOG("Chicken farm destroyed");
}

void ChickenFarm::Move(float dt)
{
	if (!isCheckedBuildingState && !App->fade->IsFading()) {

		CheckBuildingState();
		isCheckedBuildingState = true;

		if (!isBuilt)
			//Construction peasants
			peasants = App->particles->AddParticle(App->particles->peasantSmallBuild, { (int)pos.x - 20,(int)pos.y - 20 });
	}
	
	if (listener != nullptr)
		HandleInput(EntityEvent);

	if (!isBuilt) {
		constructionTimer += dt;
		UpdateAnimations(dt);
	}

	if (constructionTimer >= constructionTime && !isBuilt) {
		isBuilt = true;
		App->player->currentFood += 3;
		App->scene->hasFoodChanged = true;

		if (peasants != nullptr) {
			peasants->isRemove = true;
			peasants = nullptr;
		}
	}
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{
}

void ChickenFarm::UpdateAnimations(float dt)
{
	if (constructionTimer >= (constructionTime / 3))
		texArea = &chickenFarmInfo.constructionPlanks2;

	if (constructionTimer >= (constructionTime / 3 * 2))
		texArea = &chickenFarmInfo.inProgressTexArea;

	if (constructionTimer >= constructionTime || isBuilt) {
		texArea = &chickenFarmInfo.completeTexArea;
		buildingState = BuildingState_Normal;
	}
}
