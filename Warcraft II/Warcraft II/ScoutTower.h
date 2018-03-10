#ifndef __ScoutTower_H__
#define __ScoutTower_H__

#include "StaticEntity.h"

struct ScoutTowerInfo
{

};

class ScoutTower :public StaticEntity
{
public:

	ScoutTower(fPoint pos, iPoint size, int life, const ScoutTowerInfo& scoutTowerInfo);
	~ScoutTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ScoutTowerInfo scoutTowerInfo;
};

#endif //__ScoutTower_H__