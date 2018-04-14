#include "Alleria.h"

Alleria::Alleria(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const AlleriaInfo& alleriaInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), alleriaInfo(alleriaInfo)
{
	animation = &this->alleriaInfo.idle;
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