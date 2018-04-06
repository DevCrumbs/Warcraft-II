#include "EnemyBlacksmith.h"

EnemyBlacksmith::EnemyBlacksmith(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBlacksmithInfo& enemyBlacksmithInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBlacksmithInfo(enemyBlacksmithInfo)
{
	texArea = &enemyBlacksmithInfo.completeTexArea;
}

void EnemyBlacksmith::Move(float dt) {


}

// Animations
void EnemyBlacksmith::LoadAnimationsSpeed()
{

}
void EnemyBlacksmith::UpdateAnimations(float dt)
{

}