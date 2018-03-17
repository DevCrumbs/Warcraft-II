#include "Footman.h"

Footman::Footman(fPoint pos, iPoint size, int maxLife, float speed, const FootmanInfo& footmanInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener),footmanInfo(footmanInfo)
{
	currentLife = maxLife;

}

void Footman::Move(float dt)
{

}

// Animations
void Footman::LoadAnimationsSpeed()
{

}
void Footman::UpdateAnimations(float dt)
{

}