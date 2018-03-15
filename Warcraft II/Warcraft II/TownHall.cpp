// Upgrades to Keep
// Keep upgrades to Castle

#include "TownHall.h"

TownHall::TownHall(fPoint pos, iPoint size, int life, const TownHallInfo& townHallInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), townHallInfo(townHallInfo)
{
	texArea = &townHallInfo.townHallCompleteTexArea;
}

void TownHall::Move(float dt)
{

}

// Animations
void TownHall::LoadAnimationsSpeed()
{

}

void TownHall::UpdateAnimations(float dt)
{

}