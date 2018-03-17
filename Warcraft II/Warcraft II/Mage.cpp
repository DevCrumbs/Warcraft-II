#include "Mage.h"

Mage::Mage(fPoint pos, iPoint size, int maxLife, float speed, const MageInfo& mageInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), mageInfo(mageInfo)
{
	currentLife = maxLife;

}

void Mage::Move(float dt)
{

}

// Animations
void Mage::LoadAnimationsSpeed()
{

}
void Mage::UpdateAnimations(float dt)
{

}