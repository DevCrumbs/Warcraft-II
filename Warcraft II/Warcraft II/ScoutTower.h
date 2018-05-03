#ifndef __ScoutTower_H__
#define __ScoutTower_H__

#include "StaticEntity.h"
#include <list>

struct ColliderGroup;

struct ScoutTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect constructionPlanks1 = { 0,0,0,0 };
	SDL_Rect constructionPlanks2 = { 0,0,0,0 };
	int maxLife = 0;
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;

	uint sightRadius = 0;
	uint damage = 0;
	uint attackWaitTime = 0;
	uint arrowSpeed = 0;
};

class ScoutTower :public StaticEntity
{
public:

	ScoutTower(fPoint pos, iPoint size, int currLife, uint maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener);
	~ScoutTower() {};

	void Move(float dt);
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void TowerStateMachine(float dt);

	//Arrows
	void CreateArrow();
	

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ScoutTowerInfo scoutTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	j1Timer attackTimer;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* arrowParticle = nullptr;

	bool isColliderCreated = false;
};

#endif //__ScoutTower_H__