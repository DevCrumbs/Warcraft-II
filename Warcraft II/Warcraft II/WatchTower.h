#ifndef __WatchTower_H__
#define __WatchTower_H__

#include "StaticEntity.h"
#include <list>

struct WatchTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;


	uint sightRadius = 0;
	uint damage = 0;
	uint attackWaitTime = 0;
	uint arrowSpeed = 0;
};

class WatchTower :public StaticEntity
{
public:

	WatchTower(fPoint pos, iPoint size, int currLife, uint maxLife, const WatchTowerInfo& watchTowerInfo, j1Module* listener);
	~WatchTower();

	void Move(float dt);
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void TowerStateMachine(float dt);

	//Arrows
	void CreateArrow();

private:

	WatchTowerInfo watchTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	j1Timer attackTimer;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* arrowParticle = nullptr;

	// Reconstruction
	Particle* peon = nullptr;

	float secondsReconstruction = 0.0f;

	float startReconstructionTimer = 0.0f;
	float inProgressReconstructionTimer = 0.0f;

	bool isRestartReconstructionTimer = false;
	bool isStartReconstructionTimer = false;
	bool isInProgressReconstructionTimer = false;

	BuildingState buildingStateBeforeReconstruction = BuildingState_NoState;
};

#endif //__WatchTower_H__