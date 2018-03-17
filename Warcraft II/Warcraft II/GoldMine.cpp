#include "GoldMine.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int life, const GoldMineInfo& goldMineInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), goldMineInfo(goldMineInfo)
{
	texArea = &goldMineInfo.completeTexArea;
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