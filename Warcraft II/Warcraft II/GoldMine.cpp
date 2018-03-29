#include "GoldMine.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), goldMineInfo(goldMineInfo)
{
	texArea = &goldMineInfo.completeTexArea;
	currentLife = maxLife;

}

void GoldMine::Move(float dt)
{

}

// Animations
void GoldMine::LoadAnimationsSpeed()
{

}
void GoldMine::UpdateAnimations(float dt)
{

}