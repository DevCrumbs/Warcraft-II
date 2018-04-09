#ifndef __EnemyGuardTower_H__
#define __EnemyGuardTower_H__

#include "StaticEntity.h"

struct EnemyGuardTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};


class EnemyGuardTower :public StaticEntity
{
public:

	EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener);
	~EnemyGuardTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyGuardTowerInfo enemyGuardTowerInfo;
};

#endif //__EnemyGuardTower_H__