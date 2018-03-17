#include "Runestone.h"

Runestone::Runestone(fPoint pos, iPoint size, int maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), runestoneInfo(runestoneInfo)
{
	texArea = &runestoneInfo.completeTexArea;
	currentLife = maxLife;

}

void Runestone::Move(float dt)
{

}

// Animations
void Runestone::LoadAnimationsSpeed()
{

}
void Runestone::UpdateAnimations(float dt)
{

}