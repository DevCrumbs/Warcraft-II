#include "PlayerGuardTower.h"

PlayerGuardTower::PlayerGuardTower(fPoint pos, iPoint size, int maxLife, const PlayerGuardTowerInfo& playerGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), playerGuardTowerInfo(playerGuardTowerInfo)
{
	texArea = &playerGuardTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void PlayerGuardTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void PlayerGuardTower::LoadAnimationsSpeed()
{

}
void PlayerGuardTower::UpdateAnimations(float dt)
{

}