#ifndef __EnemyGuardTower_H__
#define __EnemyGuardTower_H__

#include "StaticEntity.h"

struct EnemyGuardTowerInfo
{

};

class EnemyGuardTower :public StaticEntity
{
public:

	EnemyGuardTower(fPoint pos, iPoint size, int life, const EnemyGuardTowerInfo& enemyGuardTowerInfo);
	~EnemyGuardTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyGuardTowerInfo enemyGuardTowerInfo;
};

#endif //__EnemyGuardTower_H__