#ifndef __EnemyCannonTower_H__
#define __EnemyCannonTower_H__

#include "StaticEntity.h"

struct EnemyCannonTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class EnemyCannonTower :public StaticEntity
{
public:

	EnemyCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyCannonTowerInfo& enemyCannonTowerInfo, j1Module* listener);
	~EnemyCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyCannonTowerInfo enemyCannonTowerInfo;
};

#endif //__EnemyCannonTower_H__