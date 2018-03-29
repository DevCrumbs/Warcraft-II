#include "Khadgar.h"

Khadgar::Khadgar(fPoint pos, const KhadgarInfo& khadgarInfo, j1Module* listener) :DynamicEntity(pos, khadgarInfo.size, khadgarInfo.life, khadgarInfo.speed, listener), khadgarInfo(khadgarInfo)
{
	currentLife = khadgarInfo.life;

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