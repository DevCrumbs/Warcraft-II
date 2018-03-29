#include "Mage.h"

Mage::Mage(fPoint pos, const MageInfo& mageInfo, j1Module* listener) :DynamicEntity(pos, mageInfo.size, mageInfo.life, mageInfo.speed, listener), mageInfo(mageInfo)
{
	currentLife = mageInfo.life;

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