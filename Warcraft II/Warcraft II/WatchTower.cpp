#include "WatchTower.h"

WatchTower::WatchTower(fPoint pos, iPoint size, int life, const WatchTowerInfo& watchTowerInfo) :StaticEntity(pos, size, life), watchTowerInfo(watchTowerInfo) 
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