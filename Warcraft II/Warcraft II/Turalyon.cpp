#include "Turalyon.h"

Turalyon::Turalyon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TuralyonInfo& turalyonInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), turalyonInfo(turalyonInfo)
{
	animation = &this->turalyonInfo.idle;
}

void Turalyon::Move(float dt)
{
	HandleInput(entityEvent);
}

// Animations
void Turalyon::LoadAnimationsSpeed()
{

}
void Turalyon::UpdateAnimations(float dt)
{

}