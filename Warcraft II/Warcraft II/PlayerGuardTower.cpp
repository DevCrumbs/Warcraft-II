#include "PlayerGuardTower.h"

PlayerGuardTower::PlayerGuardTower(fPoint pos, iPoint size, int maxLife, const PlayerGuardTowerInfo& playerGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), playerGuardTowerInfo(playerGuardTowerInfo)
{
	texArea = &constructionPlanks.constructionPlanksSmall1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void PlayerGuardTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void PlayerGuardTower::LoadAnimationsSpeed()
{

}
void PlayerGuardTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksSmall2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &playerGuardTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &playerGuardTowerInfo.completeTexArea;
}