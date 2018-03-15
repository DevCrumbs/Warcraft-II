#include "Mage.h"

Mage::Mage(fPoint pos, iPoint size, int life, float speed, const MageInfo& mageInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), mageInfo(mageInfo)
{

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