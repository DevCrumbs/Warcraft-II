#include "Paladin.h"

Paladin::Paladin(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const PaladinInfo& paladinInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), paladinInfo(paladinInfo)
{
}

void Paladin::Move(float dt)
{

}

// Animations
void Paladin::LoadAnimationsSpeed()
{

}
void Paladin::UpdateAnimations(float dt)
{

}