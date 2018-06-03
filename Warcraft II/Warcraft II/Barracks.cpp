#include "Defs.h"
#include "p2Log.h"

#include "Barracks.h"

#include "j1Player.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Movement.h"
#include "j1Collision.h"
#include "j1Particles.h"

Barracks::Barracks(fPoint pos, iPoint size, int currLife, uint maxLife, const BarracksInfo& barracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), barracksInfo(barracksInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_HumanBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_BARRACKS;
	*(EntitySide*)&entitySide = EntitySide_Player;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Medium;

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

	isBuilt = barracksInfo.isBuilt;

	if (isBuilt) {
		texArea = &barracksInfo.completeTexArea;
		buildingState = BuildingState_Normal;
	}

	else if (!isBuilt) {
		texArea = &barracksInfo.constructionPlanks1;
		buildingState = BuildingState_Building;
		App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound

		//Construction peasants
		peasants = App->particles->AddParticle(App->particles->peasantMediumBuild, { (int)pos.x - 30,(int)pos.y - 30 });

	}
	
	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;

}

Barracks::~Barracks()
{
	if (peasants != nullptr) {
		peasants->isRemove = true;
		peasants = nullptr;
	}
}

void Barracks::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (!isBuilt) {
		constructionTimer += dt;
		UpdateAnimations(dt);
	}

	if (constructionTimer >= (constructionTime) && !isBuilt) {
		isBuilt = true;

		if (peasants != nullptr) {
			peasants->isRemove = true;
			peasants = nullptr;
		}
	}
}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}

void Barracks::UpdateAnimations(float dt)
{
	if (constructionTimer >= (constructionTime / 3))
		texArea = &barracksInfo.constructionPlanks2;

	if (constructionTimer >= (constructionTime / 3 * 2))
		texArea = &barracksInfo.inProgressTexArea;

	if (constructionTimer >= constructionTime) {
		texArea = &barracksInfo.completeTexArea;
		buildingState = BuildingState_Normal;

	}

}