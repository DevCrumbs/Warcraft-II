#include "ScoutTower.h"

ScoutTower::ScoutTower(fPoint pos, iPoint size, int maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), scoutTowerInfo(scoutTowerInfo)
{
	texArea = &scoutTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void ScoutTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void ScoutTower::LoadAnimationsSpeed()
{

}
void ScoutTower::UpdateAnimations(float dt)
{

}