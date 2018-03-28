#include "WatchTower.h"

WatchTower::WatchTower(fPoint pos, iPoint size, int currLife, uint maxLife, const WatchTowerInfo& watchTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), watchTowerInfo(watchTowerInfo)
{
	texArea = &watchTowerInfo.completeTexArea;
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