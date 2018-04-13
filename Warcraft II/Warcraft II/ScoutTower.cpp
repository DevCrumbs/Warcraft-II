#include <utility> 

#include "ScoutTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"


ScoutTower::ScoutTower(fPoint pos, iPoint size, int currLife, uint maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), scoutTowerInfo(scoutTowerInfo)
{
	texArea = &scoutTowerInfo.constructionPlanks1;
	this->constructionTimer.Start();
}

void ScoutTower::Move(float dt)
{
	if (!isColliderCreated) {

		CreateEntityCollider(EntitySide_Player);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, scoutTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
		sightRadiusCollider->isTrigger = true;
		isColliderCreated = true;
	}

	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;
}

// Animations
void ScoutTower::LoadAnimationsSpeed()
{

}
void ScoutTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &scoutTowerInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &scoutTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &scoutTowerInfo.completeTexArea;
}