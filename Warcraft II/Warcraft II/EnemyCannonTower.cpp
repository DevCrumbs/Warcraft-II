#include "EnemyCannonTower.h"

EnemyCannonTower::EnemyCannonTower(fPoint pos, iPoint size, int life, const EnemyCannonTowerInfo& enemyCannonTowerInfo) :StaticEntity(pos, size, life), enemyCannonTowerInfo(enemyCannonTowerInfo) 
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