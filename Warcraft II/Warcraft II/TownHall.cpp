// Upgrades to Keep
// Keep upgrades to Castle

#include "TownHall.h"
#include "j1Player.h"

TownHall::TownHall(fPoint pos, iPoint size, int maxLife, const TownHallInfo& townHallInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), townHallInfo(townHallInfo)
{
	texArea = &townHallInfo.townHallCompleteTexArea;
	currentLife = maxLife;
	isBuilt = true;
}

void TownHall::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	if (App->player->townHallUpgrade) {
		if (startTimer) {
			this->constructionTimer.Start();
			startTimer = false;
		}
		UpdateAnimations(dt);
		townHallInfo.townHallType = TownHallType_Keep;
	}
}

// Animations
void TownHall::LoadAnimationsSpeed()
{

}

void TownHall::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= constructionTime * 1000) {
		if (townHallInfo.townHallType == TownHallType_Keep) {
			texArea = &townHallInfo.keepCompleteTexArea;
			buildingState = BuildingState_Normal;
		}
	}
	else {
		texArea = &townHallInfo.keepInProgressTexArea;
		buildingState = BuildingState_Building;
	}

}