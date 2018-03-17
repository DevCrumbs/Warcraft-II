#include "WatchTower.h"

WatchTower::WatchTower(fPoint pos, iPoint size, int life, const WatchTowerInfo& watchTowerInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), watchTowerInfo(watchTowerInfo)
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