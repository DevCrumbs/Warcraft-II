#include "EnemyGuardTower.h"

EnemyGuardTower::EnemyGuardTower(fPoint pos, iPoint size, int maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), enemyGuardTowerInfo(enemyGuardTowerInfo)
{
	texArea = &enemyGuardTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void EnemyGuardTower::Move(float dt)
{

}

// Animations
void EnemyGuardTower::LoadAnimationsSpeed()
{

}
void EnemyGuardTower::UpdateAnimations(float dt)
{

}