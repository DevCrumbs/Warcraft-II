#ifndef __PlayerGuardTower_H__
#define __PlayerGuardTower_H__

#include "StaticEntity.h"
#include <list>

struct PlayerGuardTowerInfo
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

struct ColliderGroup;

class PlayerGuardTower :public StaticEntity
{
public:

	PlayerGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerGuardTowerInfo& playerGuardTowerInfo, j1Module* listener);
	~PlayerGuardTower() {};

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

	PlayerGuardTowerInfo playerGuardTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	j1Timer attackTimer;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* arrowParticle = nullptr;

	Particle* peasants = nullptr;

	bool isColliderCreated = false;
};

#endif //__PlayerGuardTower_H__