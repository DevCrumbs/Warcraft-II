#include "Dragon.h"

Dragon::Dragon(fPoint pos, iPoint size, int maxLife, float speed, const DragonInfo& dragonInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), dragonInfo(dragonInfo)
{
	currentLife = maxLife;

}

void Dragon::Move(float dt) 
{

}

// Animations
void Dragon::LoadAnimationsSpeed() 
{

}
void Dragon::UpdateAnimations(float dt) 
{

}