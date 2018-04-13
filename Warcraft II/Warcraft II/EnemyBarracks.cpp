#include "EnemyBarracks.h"
#include "j1Collision.h"

EnemyBarracks::EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBarracksInfo(enemyBarracksInfo)
{
	texArea = &enemyBarracksInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void EnemyBarracks::Move(float dt) {


}

// Animations
void EnemyBarracks::LoadAnimationsSpeed()
{

}
void EnemyBarracks::UpdateAnimations(float dt)
{

}


