#include "WatchTower.h"

WatchTower::WatchTower(fPoint pos, iPoint size, int maxLife, const WatchTowerInfo& watchTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), watchTowerInfo(watchTowerInfo)
{
	texArea = &watchTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void WatchTower::Move(float dt)
{

}

// Animations
void WatchTower::LoadAnimationsSpeed()
{

}
void WatchTower::UpdateAnimations(float dt)
{

}