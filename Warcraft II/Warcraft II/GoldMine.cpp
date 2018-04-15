#include "GoldMine.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), goldMineInfo(goldMineInfo)
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

	texArea = &goldMineInfo.completeTexArea;
	isBuilt = true;
}

void GoldMine::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);

	if (buildingState == BuildingState_Destroyed)
		texArea = &goldMineInfo.inProgressTexArea;

		/*if (startTimer) {
			constructionTimer.Start();
			startTimer = false;
		}
		else 
			UpdateAnimations(dt);
	}*/
}

// Animations
void GoldMine::LoadAnimationsSpeed()
{

}
void GoldMine::UpdateAnimations(float dt)
{
	/*if (constructionTimer.Read() >= constructionTime * 1000) {
		buildingState = BuildingState_Building;
		texArea = &goldMineInfo.inProgressTexArea;
		startTimer = true;
		int random = rand() % 4;
		switch (random) {
		case 0:
			App->player->AddGold(1500);
			break;
		case 1:
			App->player->AddGold(2000);
			break;
		case 2:
			App->player->AddGold(2500);
			break;
		case 3:
			App->player->AddGold(3000);
			break;
		}
		App->scene->hasGoldChanged = true;
	}*/
}