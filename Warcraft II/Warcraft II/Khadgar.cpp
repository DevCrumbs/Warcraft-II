#include "Khadgar.h"

Khadgar::Khadgar(fPoint pos, iPoint size, int maxLife, float speed, const KhadgarInfo& khadgarInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), khadgarInfo(khadgarInfo)
{
	currentLife = maxLife;

}

void Khadgar::Move(float dt)
{

}

// Animations
void Khadgar::LoadAnimationsSpeed()
{

}
void Khadgar::UpdateAnimations(float dt)
{

}