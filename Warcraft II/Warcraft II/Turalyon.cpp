#include "Turalyon.h"

Turalyon::Turalyon(fPoint pos, iPoint size, int life, float speed, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), turalyonInfo(turalyonInfo)
{

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