#include "Defs.h"
#include "p2Log.h"

#include "Barracks.h"

#include "j1Player.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Movement.h"

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

	texArea = &barracksInfo.barracksCompleteTexArea;
	currLife = maxLife;
	isBuilt = true;
	buildingState = BuildingState_Normal;
	//this->constructionTimer.Start();
}

void Barracks::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (App->player->barracksUpgrade) {
		if (startTimer) {
			this->constructionTimer.Start();
			App->player->HideEntitySelectedInfo();
			startTimer = false;
		}
		UpdateAnimations(dt);
		barracksInfo.barracksType = BarracksType_Barracks2;
	}

	//if (constructionTimer.Read() >= (constructionTime * 1000))
	//	isBuilt = true;
	
}

// Animations
void Barracks::LoadAnimationsSpeed()
{

}
void Barracks::UpdateAnimations(float dt)
{
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
}