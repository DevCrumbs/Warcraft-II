#include "TrollAxethrower.h"

TrollAxethrower::TrollAxethrower(fPoint pos, iPoint size, int maxLife, float speed, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), trollAxethrowerInfo(trollAxethrowerInfo)
{
	currentLife = maxLife;

}

void TrollAxethrower::Move(float dt)
{

}

// Animations
void TrollAxethrower::LoadAnimationsSpeed()
{

}
void TrollAxethrower::UpdateAnimations(float dt)
{

}