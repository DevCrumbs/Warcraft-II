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
		HandleInput(entityEvent);

	if (App->player->townHallUpgrade) {
		if (startTimer) {
			this->constructionTimer.Start();
			startTimer = false;
		}
		UpdateAnimations(dt);
		if (App->player->keepUpgrade)
			townHallInfo.townHallType = TownHallType_Castle;
		else
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
			SetMaxLife(1400);
			SetCurrLife(1400);
			//entityEvent = EntitiesEvent_CREATED;
		}
		else if (townHallInfo.townHallType == TownHallType_Castle) {
			texArea = &townHallInfo.castleCompleteTexArea;
			buildingState = BuildingState_Normal;
			SetMaxLife(1400);
			SetCurrLife(1400);
			//entityEvent = EntitiesEvent_CREATED;
		}
	}
	else {
		texArea = &townHallInfo.keepInProgressTexArea;
		buildingState = BuildingState_Building;
	}

}