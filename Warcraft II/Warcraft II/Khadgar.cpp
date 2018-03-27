#include "Khadgar.h"

Khadgar::Khadgar(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const KhadgarInfo& khadgarInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), khadgarInfo(khadgarInfo)
{
}

void Khadgar::Move(float dt)
{

}

// Animations
void Khadgar::LoadAnimationsSpeed()
{

}
void Khadgar::UpdateAnimations(float dt)
{

}