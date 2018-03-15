#include "GoldMine.h"

GoldMine::GoldMine(fPoint pos, iPoint size, int life, const GoldMineInfo& goldMineInfo) :StaticEntity(pos, size, life), goldMineInfo(goldMineInfo) 
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