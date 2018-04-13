#ifndef __ScoutTower_H__
#define __ScoutTower_H__

#include "StaticEntity.h"
#include <queue>
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

enum ArrowDirection {
	NO_DIRECTION,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UP_LEFT,
	UP_RIGHT,
	DOWN_LEFT,
	DOWN_RIGHT
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
	void DetermineArrowDirection();
	void CreateArrow();
	void CheckArrowMovement(float dt);
	void MoveArrowTowardsTarget(float dt);
	void InflictDamageAndDestroyArrow();
	

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
	std::queue<Entity*> enemyAttackQueue;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* arrowParticle = nullptr;
	ArrowDirection arrowDirection = NO_DIRECTION;

	bool isColliderCreated = false;
};

#endif //__ScoutTower_H__