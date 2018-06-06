#include "Defs.h"
#include "p2Log.h"

#include "GryphonAviary.h"

#include "j1Player.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Movement.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1FadeToBlack.h"

GryphonAviary::GryphonAviary(fPoint pos, iPoint size, int currLife, uint maxLife, const GryphonAviaryInfo& gryphonAviaryInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), gryphonAviaryInfo(gryphonAviaryInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_HumanBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_GRYPHON_AVIARY;
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

	texArea = &gryphonAviaryInfo.constructionPlanks1;
	buildingState = BuildingState_Building;
	
	
	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;
}

GryphonAviary::~GryphonAviary()
{
	if (peasants != nullptr) {
		peasants->isRemove = true;
		peasants = nullptr;
	}
}

void GryphonAviary::Move(float dt)
{
	if (!isCheckedBuildingState && !App->fade->IsFading()) {

		CheckBuildingState();
		isCheckedBuildingState = true;

		if (!isBuilt) {
			//Construction peasants
			App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
			peasants = App->particles->AddParticle(App->particles->peasantMediumBuild, { (int)pos.x - 30,(int)pos.y - 30 });
		}
		else if(isBuilt)
			texArea = &gryphonAviaryInfo.completeTexArea;
	}

	if (listener != nullptr)
		HandleInput(EntityEvent);

	if (!isBuilt) {
		constructionTimer += dt;
		UpdateAnimations(dt);
	}

	if (constructionTimer >= constructionTime && !isBuilt) {
		isBuilt = true;

		if (peasants != nullptr) {
			peasants->isRemove = true;
			peasants = nullptr;
		}
	}
}

// Animations
void GryphonAviary::LoadAnimationsSpeed()
{

}
void GryphonAviary::UpdateAnimations(float dt)
{
	if (constructionTimer >= (constructionTime / 3))
		texArea = &gryphonAviaryInfo.constructionPlanks2;

	if (constructionTimer >= (constructionTime / 3 * 2))
		texArea = &gryphonAviaryInfo.inProgressTexArea;

	if (constructionTimer >= constructionTime || isBuilt) {
		texArea = &gryphonAviaryInfo.completeTexArea;
		buildingState = BuildingState_Normal;
	}
}