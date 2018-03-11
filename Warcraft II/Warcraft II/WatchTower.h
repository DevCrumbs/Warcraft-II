#ifndef __WatchTower_H__
#define __WatchTower_H__

#include "StaticEntity.h"

struct WatchTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
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