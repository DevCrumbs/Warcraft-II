#include "Mage.h"

Mage::Mage(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const MageInfo& mageInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), mageInfo(mageInfo)
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