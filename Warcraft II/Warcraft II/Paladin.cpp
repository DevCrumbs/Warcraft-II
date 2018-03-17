#include "Paladin.h"

Paladin::Paladin(fPoint pos, iPoint size, int maxLife, float speed, const PaladinInfo& paladinInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), paladinInfo(paladinInfo)
{
	currentLife = maxLife;

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