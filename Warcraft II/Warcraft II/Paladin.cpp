#include "Paladin.h"

Paladin::Paladin(fPoint pos, iPoint size, int life, float speed, const PaladinInfo& paladinInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), paladinInfo(paladinInfo)
{

}

void Paladin::Move(float dt)
{

}

// Animations
void Paladin::LoadAnimationsSpeed()
{

}
void Paladin::UpdateAnimations(float dt)
{

}