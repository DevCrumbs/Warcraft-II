#include "PlayerGuardTower.h"

PlayerGuardTower::PlayerGuardTower(fPoint pos, iPoint size, int life, const PlayerGuardTowerInfo& playerGuardTowerInfo) :StaticEntity(pos, size, life), playerGuardTowerInfo(playerGuardTowerInfo) 
{
	texArea = &playerGuardTowerInfo.completeTexArea;
}

void PlayerGuardTower::Move(float dt)
{

}

// Animations
void PlayerGuardTower::LoadAnimationsSpeed()
{

}
void PlayerGuardTower::UpdateAnimations(float dt)
{

}