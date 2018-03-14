#ifndef __ChickenFarm_H__
#define __ChickenFarm_H__

#include "StaticEntity.h"

struct ChickenFarmInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class ChickenFarm :public StaticEntity
{
public:

	ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo);
	~ChickenFarm() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ChickenFarmInfo chickenFarmInfo;
};

#endif //__ChickenFarm_H__