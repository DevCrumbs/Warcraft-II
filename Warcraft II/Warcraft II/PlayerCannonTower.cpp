#include "PlayerCannonTower.h"

PlayerCannonTower::PlayerCannonTower(fPoint pos, iPoint size, int life, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), playerCannonTowerInfo(playerCannonTowerInfo)
{
	texArea = &playerCannonTowerInfo.completeTexArea;
}

void PlayerCannonTower::Move(float dt)
{

}

// Animations
void PlayerCannonTower::LoadAnimationsSpeed()
{

}
void PlayerCannonTower::UpdateAnimations(float dt)
{

}