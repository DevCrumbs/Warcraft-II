#include "Paladin.h"

Paladin::Paladin(fPoint pos, const PaladinInfo& paladinInfo, j1Module* listener) :DynamicEntity(pos, paladinInfo.size, paladinInfo.life, paladinInfo.speed, listener), paladinInfo(paladinInfo)
{
	currentLife = paladinInfo.life;

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