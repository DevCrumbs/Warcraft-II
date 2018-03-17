#include "PlayerCannonTower.h"

PlayerCannonTower::PlayerCannonTower(fPoint pos, iPoint size, int maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), playerCannonTowerInfo(playerCannonTowerInfo)
{
	texArea = &playerCannonTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void PlayerCannonTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void PlayerCannonTower::LoadAnimationsSpeed()
{

}
void PlayerCannonTower::UpdateAnimations(float dt)
{

}