#include "Alleria.h"

Alleria::Alleria(fPoint pos, iPoint size, int maxLife, float speed, const AlleriaInfo& alleriaInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), alleriaInfo(alleriaInfo)
{
	currentLife = maxLife;

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