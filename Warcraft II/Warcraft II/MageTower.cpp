// Creates the Mage

#include "MageTower.h"

MageTower::MageTower(fPoint pos, iPoint size, int maxLife, const MageTowerInfo& mageTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), mageTowerInfo(mageTowerInfo)
{
	texArea = &mageTowerInfo.completeTexArea;
	currentLife = maxLife;

}

void MageTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void MageTower::LoadAnimationsSpeed()
{

}
void MageTower::UpdateAnimations(float dt)
{

}