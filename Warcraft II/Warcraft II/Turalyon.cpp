#include "Turalyon.h"

Turalyon::Turalyon(fPoint pos, iPoint size, int maxLife, float speed, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), turalyonInfo(turalyonInfo)
{
	currentLife = maxLife;

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