#include "ElvenArcher.h"

ElvenArcher::ElvenArcher(fPoint pos, iPoint size, int life, float speed, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), elvenArcherInfo(elvenArcherInfo) 
{

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