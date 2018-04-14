#ifndef __EnemyCannonTower_H__
#define __EnemyCannonTower_H__

#include "StaticEntity.h"
#include "ScoutTower.h"
#include <list>

struct EnemyCannonTowerInfo
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

class EnemyCannonTower :public StaticEntity
{
public:

	EnemyCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyCannonTowerInfo& enemyCannonTowerInfo, j1Module* listener);
	~EnemyCannonTower() {};

	void Move(float dt);
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// State machine
	void TowerStateMachine(float dt);

	//Cannon bullet
	void DetermineCannonBulletDirection();
	void CreateCannonBullet();
	void CheckCannonBulletMovement(float dt);
	void MoveCannonTowardsTarget(float dt);
	void InflictDamageAndDestroyCannonBullet();

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyCannonTowerInfo enemyCannonTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
	TowerState towerState = TowerState_Idle;

	//Attack
	Entity* attackingTarget = nullptr;
	j1Timer attackTimer;
	std::list<Entity*> enemyAttackList;

	//Arrow
	Particle* cannonParticle = nullptr;
	ArrowDirection cannonDirection = NO_DIRECTION;
};

#endif //__EnemyCannonTower_H__