#include "EnemyBarracks.h"

EnemyBarracks::EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBarracksInfo(enemyBarracksInfo)
{
	texArea = &enemyBarracksInfo.completeTexArea;
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


