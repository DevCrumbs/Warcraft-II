#include "Dragon.h"

Dragon::Dragon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const DragonInfo& dragonInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), dragonInfo(dragonInfo)
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