#ifndef __ChickenFarm_H__
#define __ChickenFarm_H__

#include "SDL\include\SDL.h"

#include "Defs.h"
#include "StaticEntity.h"



struct ChickenFarmInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect constructionPlanks1 = { 0,0,0,0 };
	SDL_Rect constructionPlanks2 = { 0,0,0,0 };
	int maxLife = 0;
};

class ChickenFarm :public StaticEntity
{
public:

	ChickenFarm(fPoint pos, iPoint size, int maxLife, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener = nullptr);
	~ChickenFarm() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

	bool nextEvent = false;

private:

	ChickenFarmInfo chickenFarmInfo;
	StaticEntityType type = StaticEntityType_ChickenFarm;

	EntitiesEvent EntityEvent = EntitiesEvent_Created;
};

#endif //__ChickenFarm_H__