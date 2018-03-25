// Upgrades to Keep
// Keep upgrades to Castle

#include "TownHall.h"

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
}

// Animations
void TownHall::LoadAnimationsSpeed()
{

}

void TownHall::UpdateAnimations(float dt)
{

}