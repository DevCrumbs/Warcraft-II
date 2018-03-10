#ifndef __WatchTower_H__
#define __WatchTower_H__

#include "StaticEntity.h"

struct WatchTowerInfo
{

};

class WatchTower :public StaticEntity
{
public:

	WatchTower(fPoint pos, iPoint size, int life, const WatchTowerInfo& watchTowerInfo);
	~WatchTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	WatchTowerInfo watchTowerInfo;
};

#endif //__WatchTower_H__