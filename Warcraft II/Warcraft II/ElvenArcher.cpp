#include "ElvenArcher.h"

ElvenArcher::ElvenArcher(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), elvenArcherInfo(elvenArcherInfo)
{

}

void ElvenArcher::Move(float dt)
{

}

// Animations
void ElvenArcher::LoadAnimationsSpeed()
{

}
void ElvenArcher::UpdateAnimations(float dt)
{

}