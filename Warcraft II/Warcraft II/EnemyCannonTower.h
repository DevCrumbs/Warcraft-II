#ifndef __EnemyCannonTower_H__
#define __EnemyCannonTower_H__

#include "StaticEntity.h"

struct EnemyCannonTowerInfo
{

};

class EnemyCannonTower :public StaticEntity
{
public:

	EnemyCannonTower(fPoint pos, iPoint size, int life, const EnemyCannonTowerInfo& enemyCannonTowerInfo);
	~EnemyCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	EnemyCannonTowerInfo enemyCannonTowerInfo;
};

#endif //__EnemyCannonTower_H__