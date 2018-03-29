#ifndef __MageTower_H__
#define __MageTower_H__

#include "StaticEntity.h"

struct MageTowerInfo
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

class MageTower :public StaticEntity
{
public:

	MageTower(fPoint pos, iPoint size, int maxLife, const MageTowerInfo& mageTowerInfo, j1Module* listener);
	~MageTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	MageTowerInfo mageTowerInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
};

#endif //__MageTower_H__