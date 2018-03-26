#include "ElvenArcher.h"

ElvenArcher::ElvenArcher(fPoint pos, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener) :DynamicEntity(pos, elvenArcherInfo.size, elvenArcherInfo.life, elvenArcherInfo.speed, listener), elvenArcherInfo(elvenArcherInfo)
{
	currentLife = elvenArcherInfo.life;

}

void ElvenArcher::Move(float dt)
{

}

// Animations
void ElvenArcher::LoadAnimationsSpeed()
{

}
void ElvenArcher::UpdateAnimations(float dt)
{

}