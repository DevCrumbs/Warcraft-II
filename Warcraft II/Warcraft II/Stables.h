#ifndef __Stables_H__
#define __Stables_H__

#include "StaticEntity.h"

struct StablesInfo
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

class Stables :public StaticEntity
{
public:

	Stables(fPoint pos, iPoint size, int maxLife, const StablesInfo& stablesInfo, j1Module* listener);
	~Stables() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	StablesInfo stablesInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
};

#endif //__Stables_H__