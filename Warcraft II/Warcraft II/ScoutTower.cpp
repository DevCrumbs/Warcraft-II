#include "ScoutTower.h"

ScoutTower::ScoutTower(fPoint pos, iPoint size, int maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), scoutTowerInfo(scoutTowerInfo)
{
	texArea = &constructionPlanks.constructionPlanksSmall1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void ScoutTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void ScoutTower::LoadAnimationsSpeed()
{

}
void ScoutTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksSmall2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &scoutTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &scoutTowerInfo.completeTexArea;
}