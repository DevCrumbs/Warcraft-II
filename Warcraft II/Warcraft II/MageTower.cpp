// Creates the Mage

#include "MageTower.h"

MageTower::MageTower(fPoint pos, iPoint size, int maxLife, const MageTowerInfo& mageTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), mageTowerInfo(mageTowerInfo)
{
	texArea = &constructionPlanks.constructionPlanksBig1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void MageTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void MageTower::LoadAnimationsSpeed()
{

}
void MageTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksBig2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &mageTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &mageTowerInfo.completeTexArea;
}