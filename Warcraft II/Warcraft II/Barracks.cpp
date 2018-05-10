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
		this->constructionTimer.Start();
		buildingState = BuildingState_Building;
		App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound

		//Construction peasants
		peasants = App->particles->AddParticle(App->particles->peasantMediumBuild, { (int)pos.x - 30,(int)pos.y - 30 });

	}
	
	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;

}

void Barracks::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (!isBuilt) 
		UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000) && !isBuilt)
		isBuilt = true;

	//It isnt used anymore
	/*if (App->player->barracksUpgrade) {
		if (startTimer) {
			this->constructionTimer.Start();
			App->player->HideEntitySelectedInfo();
			startTimer = false;
		}
		UpdateAnimations(dt);
		barracksInfo.barracksType = BarracksType_Barracks2;
	}*/
	//-----------------------------
}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}

void Barracks::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &barracksInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &barracksInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000) {
		texArea = &barracksInfo.completeTexArea;
		buildingState = BuildingState_Normal;
		peasants->isRemove = true;
	}

	//It isnt used anymore
	/*
	if (constructionTimer.Read() >= constructionTime * 1000) {
		if (barracksInfo.barracksType == BarracksType_Barracks2) {
			texArea = &barracksInfo.barracks2CompleteTexArea;
			buildingState = BuildingState_Normal;
			SetMaxLife(1200);
			SetCurrLife(1200);
		}
	}
	else {
		texArea = &barracksInfo.constructionPlanks2;
		buildingState = BuildingState_Building;
	}
	*/
	//-----------------------------
}