#include "ChickenFarm.h"


ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
	texArea = &chickenFarmInfo.completeTexArea;
	currentLife = maxLife;

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
