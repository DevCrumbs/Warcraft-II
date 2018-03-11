// Creates the Mage

#include "MageTower.h"

MageTower::MageTower(fPoint pos, iPoint size, int life, const MageTowerInfo& mageTowerInfo) :StaticEntity(pos, size, life), mageTowerInfo(mageTowerInfo) 
{
	texArea = &mageTowerInfo.completeTexArea;
}

void MageTower::Move(float dt)
{

}

// Animations
void MageTower::LoadAnimationsSpeed()
{

}
void MageTower::UpdateAnimations(float dt)
{

}