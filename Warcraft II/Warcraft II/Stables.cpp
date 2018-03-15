// Allows the creation of the Paladin

#include "Stables.h"

Stables::Stables(fPoint pos, iPoint size, int life, const StablesInfo& stablesInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), stablesInfo(stablesInfo)
{
	texArea = &stablesInfo.completeTexArea;
}

void Stables::Move(float dt)
{

}

// Animations
void Stables::LoadAnimationsSpeed()
{

}
void Stables::UpdateAnimations(float dt)
{

}