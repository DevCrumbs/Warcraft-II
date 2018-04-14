// Creates the Footman and the Elven Archer
// Upgrades to Barracks 2: creates the Paladin (needed Stables created) and the Ballista

#include "Barracks.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"

Barracks::Barracks(fPoint pos, iPoint size, int currLife, uint maxLife, const BarracksInfo& barracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), barracksInfo(barracksInfo)
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
			App->player->DeleteEntitiesMenu();
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