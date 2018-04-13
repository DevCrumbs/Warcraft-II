#ifndef __PlayerCannonTower_H__
#define __PlayerCannonTower_H__

#include "StaticEntity.h"
#include "ScoutTower.h"
#include <list>

struct PlayerCannonTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect constructionPlanks1 = { 0,0,0,0 };
	SDL_Rect constructionPlanks2 = { 0,0,0,0 };
	int maxLife = 0;	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;

	uint sightRadius = 0;
	uint damage = 0;
	uint attackWaitTime = 0;
	uint arrowSpeed = 0;
};

class PlayerCannonTower :public StaticEntity
{
public:

	PlayerCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener);
	~PlayerCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void TowerStateMachine(float dt);

	//Cannon bullet
	void DetermineCannonBulletDirection();
	void CreateCannonBullet();
	void CheckCannonBulletMovement(float dt);
	void MoveCannonTowardsTarget(float dt);
	void InflictDamageAndDestroyCannonBullet();

private:

	PlayerCannonTowerInfo playerCannonTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	j1Timer attackTimer;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* cannonParticle = nullptr;
	ArrowDirection cannonDirection = NO_DIRECTION;
	bool isColliderCreated = false;
};

#endif //__PlayerCannonTower_H__