#include "PlayerCannonTower.h"

PlayerCannonTower::PlayerCannonTower(fPoint pos, iPoint size, int maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), playerCannonTowerInfo(playerCannonTowerInfo)
{
	texArea = &constructionPlanks.constructionPlanksSmall2;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void PlayerCannonTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void PlayerCannonTower::LoadAnimationsSpeed()
{

}
void PlayerCannonTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksSmall2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &playerCannonTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &playerCannonTowerInfo.completeTexArea;
}