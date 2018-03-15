#include "Dragon.h"

Dragon::Dragon(fPoint pos, iPoint size, int life, float speed, const DragonInfo& dragonInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), dragonInfo(dragonInfo)
{

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