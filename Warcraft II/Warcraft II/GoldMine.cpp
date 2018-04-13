#include "GoldMine.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), goldMineInfo(goldMineInfo)
{
	texArea = &goldMineInfo.completeTexArea;
}

void GoldMine::Move(float dt)
{

}

// Animations
void GoldMine::LoadAnimationsSpeed()
{

}
void GoldMine::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= constructionTime * 1000) {
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
	}
}