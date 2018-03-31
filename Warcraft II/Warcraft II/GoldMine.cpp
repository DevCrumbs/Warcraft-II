#include "GoldMine.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), goldMineInfo(goldMineInfo)
{
	texArea = &goldMineInfo.completeTexArea;
	isBuilt = true;
}

void GoldMine::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(entityEvent);
}

// Animations
void GoldMine::LoadAnimationsSpeed()
{

}
void GoldMine::UpdateAnimations(float dt)
{

}