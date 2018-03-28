#include "TrollAxethrower.h"

TrollAxethrower::TrollAxethrower(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), trollAxethrowerInfo(trollAxethrowerInfo)
{
}

void TrollAxethrower::Move(float dt)
{

}

// Animations
void TrollAxethrower::LoadAnimationsSpeed()
{

}
void TrollAxethrower::UpdateAnimations(float dt)
{

}