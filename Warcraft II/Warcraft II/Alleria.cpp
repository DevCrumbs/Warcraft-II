#include "Alleria.h"

Alleria::Alleria(fPoint pos, const AlleriaInfo& alleriaInfo, j1Module* listener) :DynamicEntity(pos, alleriaInfo.size, alleriaInfo.life, alleriaInfo.speed, listener), alleriaInfo(alleriaInfo)
{
	currentLife = alleriaInfo.life;

}

void Alleria::Move(float dt)
{

}

// Animations
void Alleria::LoadAnimationsSpeed()
{

}
void Alleria::UpdateAnimations(float dt)
{

}