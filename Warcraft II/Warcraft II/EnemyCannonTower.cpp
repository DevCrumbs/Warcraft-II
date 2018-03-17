#include "EnemyCannonTower.h"

EnemyCannonTower::EnemyCannonTower(fPoint pos, iPoint size, int maxLife, const EnemyCannonTowerInfo& enemyCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), enemyCannonTowerInfo(enemyCannonTowerInfo)
{
	texArea = &enemyCannonTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void EnemyCannonTower::Move(float dt)
{

}

// Animations
void EnemyCannonTower::LoadAnimationsSpeed()
{

}
void EnemyCannonTower::UpdateAnimations(float dt)
{

}