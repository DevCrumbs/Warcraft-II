#include "ChickenFarm.h"

ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo) :StaticEntity(pos, size, life), chickenFarmInfo(chickenFarmInfo) 
{
	texArea = &chickenFarmInfo.completeTexArea;
}

void ChickenFarm::Move(float dt)
{

}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{

}