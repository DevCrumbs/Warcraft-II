#ifndef __ScoutTower_H__
#define __ScoutTower_H__

#include "StaticEntity.h"

struct ScoutTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect constructionPlanks1 = { 0,0,0,0 };
	SDL_Rect constructionPlanks2 = { 0,0,0,0 };
	int maxLife = 0;
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class ScoutTower :public StaticEntity
{
public:

	ScoutTower(fPoint pos, iPoint size, int currLife, uint maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener);
	~ScoutTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ScoutTowerInfo scoutTowerInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
};

#endif //__ScoutTower_H__