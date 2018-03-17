#include "ElvenArcher.h"

ElvenArcher::ElvenArcher(fPoint pos, iPoint size, int maxLife, float speed, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), elvenArcherInfo(elvenArcherInfo)
{
	currentLife = maxLife;

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