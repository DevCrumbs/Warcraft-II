#include "Turalyon.h"

Turalyon::Turalyon(fPoint pos, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, turalyonInfo.size, turalyonInfo.life, turalyonInfo.speed, listener), turalyonInfo(turalyonInfo)
{
	currentLife = turalyonInfo.life;

}

void Turalyon::Move(float dt)
{

}

// Animations
void Turalyon::LoadAnimationsSpeed()
{

}
void Turalyon::UpdateAnimations(float dt)
{

}