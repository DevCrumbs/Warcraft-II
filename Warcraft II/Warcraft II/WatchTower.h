#ifndef __WatchTower_H__
#define __WatchTower_H__

#include "StaticEntity.h"

struct WatchTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class WatchTower :public StaticEntity
{
public:

	WatchTower(fPoint pos, iPoint size, int currLife, uint maxLife, const WatchTowerInfo& watchTowerInfo, j1Module* listener);
	~WatchTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	WatchTowerInfo watchTowerInfo;
};

#endif //__WatchTower_H__