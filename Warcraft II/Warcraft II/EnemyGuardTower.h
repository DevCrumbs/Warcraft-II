#ifndef __EnemyGuardTower_H__
#define __EnemyGuardTower_H__

#include "StaticEntity.h"
#include <list>

struct EnemyGuardTowerInfo
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


class EnemyGuardTower :public StaticEntity
{
public:

	EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener);
	~EnemyGuardTower();

	void Move(float dt);
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void TowerStateMachine(float dt);

	//Arrows
	void CreateArrow();

private:

	EnemyGuardTowerInfo enemyGuardTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	float attackTimer = 0.0f;
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

#endif //__EnemyGuardTower_H__