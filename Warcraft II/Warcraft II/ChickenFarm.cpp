#include "ChickenFarm.h"


ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), chickenFarmInfo(chickenFarmInfo)
{
	texArea = &chickenFarmInfo.completeTexArea;
}

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{

}
