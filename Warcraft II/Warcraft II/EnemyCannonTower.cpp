#include "EnemyCannonTower.h"
#include "j1Collision.h"

EnemyCannonTower::EnemyCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyCannonTowerInfo& enemyCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyCannonTowerInfo(enemyCannonTowerInfo)
{
	texArea = &enemyCannonTowerInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
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