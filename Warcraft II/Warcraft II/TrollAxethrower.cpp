#include "TrollAxethrower.h"

TrollAxethrower::TrollAxethrower(fPoint pos, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener) :DynamicEntity(pos, trollAxethrowerInfo.size, trollAxethrowerInfo.life, trollAxethrowerInfo.speed, listener), trollAxethrowerInfo(trollAxethrowerInfo)
{
	currentLife = trollAxethrowerInfo.life;

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