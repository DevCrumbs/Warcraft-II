#include "Defs.h"
#include "p2Log.h"

#include "TownHall.h"

#include "j1Player.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Movement.h"
#include "j1Collision.h"
#include "j1Particles.h"

TownHall::TownHall(fPoint pos, iPoint size, int currLife, uint maxLife, const TownHallInfo& townHallInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), townHallInfo(townHallInfo)
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

	texArea = &townHallInfo.townHallCompleteTexArea;
	isBuilt = true;

	// Collision
	CreateEntityCollider(EntitySide_Player, true);
	entityCollider->isTrigger = true;
}

void TownHall::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (App->player->townHallUpgrade) {
		
		if (startTimer) {
			this->constructionTimer.Start();
			isBuilt = false;
			startTimer = false;

			//Construction peasants
			peasants = App->particles->AddParticle(App->particles->peasantBigBuild, { (int)pos.x - 40,(int)pos.y - 40 });
		}
		townHallInfo.townHallType = TownHallType_Keep;
		UpdateAnimations(dt);
	}
}

// Animations
void TownHall::LoadAnimationsSpeed()
{

}

void TownHall::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= constructionTime * 1000) {

		if (townHallInfo.townHallType == TownHallType_Keep && buildingState == BuildingState_Building) {
			texArea = &townHallInfo.keepCompleteTexArea;
			buildingState = BuildingState_Normal;
			SetMaxLife(1400);
			SetCurrLife(1400);
			isBuilt = true;
			peasants->isRemove = true;
		}
	}
	else {
		texArea = &townHallInfo.keepInProgressTexArea;
		buildingState = BuildingState_Building;
	}
}