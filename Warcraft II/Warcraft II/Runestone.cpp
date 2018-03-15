#include "Runestone.h"

Runestone::Runestone(fPoint pos, iPoint size, int life, const RunestoneInfo& runestoneInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), runestoneInfo(runestoneInfo)
{
	texArea = &runestoneInfo.completeTexArea;
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