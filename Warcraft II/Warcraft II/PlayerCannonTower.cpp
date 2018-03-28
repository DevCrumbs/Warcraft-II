#include "PlayerCannonTower.h"

PlayerCannonTower::PlayerCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), playerCannonTowerInfo(playerCannonTowerInfo)
{
	texArea = &playerCannonTowerInfo.constructionPlanks1;
	this->constructionTimer.Start();
}

void PlayerCannonTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;
}

// Animations
void PlayerCannonTower::LoadAnimationsSpeed()
{

}
void PlayerCannonTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &playerCannonTowerInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &playerCannonTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &playerCannonTowerInfo.completeTexArea;
}