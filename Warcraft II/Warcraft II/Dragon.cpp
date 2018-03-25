#include "Dragon.h"

Dragon::Dragon(fPoint pos, const DragonInfo& dragonInfo, j1Module* listener) :DynamicEntity(pos, dragonInfo.size, dragonInfo.life, dragonInfo.speed, listener), dragonInfo(dragonInfo)
{
	currentLife = dragonInfo.life;

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