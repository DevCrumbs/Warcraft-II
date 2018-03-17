// Allows the creation of the Paladin

#include "Stables.h"

Stables::Stables(fPoint pos, iPoint size, int maxLife, const StablesInfo& stablesInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), stablesInfo(stablesInfo)
{
	texArea = &stablesInfo.completeTexArea;
	                                                                    
	currentLife = maxLife;
}

void Stables::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void Stables::LoadAnimationsSpeed()
{

}
void Stables::UpdateAnimations(float dt)
{

}