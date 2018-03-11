#include "ScoutTower.h"

ScoutTower::ScoutTower(fPoint pos, iPoint size, int life, const ScoutTowerInfo& scoutTowerInfo) :StaticEntity(pos, size, life), scoutTowerInfo(scoutTowerInfo) 
{
	texArea = &scoutTowerInfo.completeTexArea;
}

void ScoutTower::Move(float dt)
{

}

// Animations
void ScoutTower::LoadAnimationsSpeed()
{

}
void ScoutTower::UpdateAnimations(float dt)
{

}