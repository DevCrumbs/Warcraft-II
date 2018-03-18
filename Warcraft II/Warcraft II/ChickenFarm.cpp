#include "ChickenFarm.h"


ChickenFarm::ChickenFarm(fPoint pos, iPoint size, int maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), chickenFarmInfo(chickenFarmInfo)
{
	texArea = &constructionPlanks.constructionPlanksSmall1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void ChickenFarm::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void ChickenFarm::LoadAnimationsSpeed()
{

}
void ChickenFarm::UpdateAnimations(float dt)
{

	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksSmall2;

	if (constructionTimer.Read() >= (constructionTime/3 * 2) * 1000)
		texArea = &chickenFarmInfo.inProgressTexArea;

	if(constructionTimer.Read() >= constructionTime * 1000)
		texArea = &chickenFarmInfo.completeTexArea;

}
