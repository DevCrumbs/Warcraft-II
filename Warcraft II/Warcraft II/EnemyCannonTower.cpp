#include "EnemyCannonTower.h"

EnemyCannonTower::EnemyCannonTower(fPoint pos, iPoint size, int life, const EnemyCannonTowerInfo& enemyCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), enemyCannonTowerInfo(enemyCannonTowerInfo)
{
	texArea = &enemyCannonTowerInfo.completeTexArea;
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