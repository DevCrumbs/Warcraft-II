#include "Footman.h"

Footman::Footman(fPoint pos, const FootmanInfo& footmanInfo, j1Module* listener) :DynamicEntity(pos, footmanInfo.size, footmanInfo.life, footmanInfo.speed, listener),footmanInfo(footmanInfo)
{
	currentLife = footmanInfo.life;

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