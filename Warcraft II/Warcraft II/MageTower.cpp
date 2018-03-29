// Creates the Mage

#include "MageTower.h"

MageTower::MageTower(fPoint pos, iPoint size, int maxLife, const MageTowerInfo& mageTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), mageTowerInfo(mageTowerInfo)
{
	texArea = &mageTowerInfo.constructionPlanks1;
	currentLife = maxLife;
	this->constructionTimer.Start();
	buildingState = BuildingState_Building;
}

void MageTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;
}

// Animations
void MageTower::LoadAnimationsSpeed()
{

}
void MageTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000) {
		texArea = &mageTowerInfo.constructionPlanks2;
	}

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &mageTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000) {
		texArea = &mageTowerInfo.completeTexArea;
		buildingState = BuildingState_Normal;
	}
}