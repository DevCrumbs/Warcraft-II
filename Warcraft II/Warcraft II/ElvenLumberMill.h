#ifndef __ElvenLumberMill_H__
#define __ElvenLumberMill_H__

#include "StaticEntity.h"

struct ElvenLumberMillInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	int maxLife = 0;
};

class ElvenLumberMill : public StaticEntity
{
public:
	ElvenLumberMill(fPoint pos, iPoint size, int life, const ElvenLumberMillInfo& elvenLumberMillInfo, j1Module* listener);
	~ElvenLumberMill() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ElvenLumberMillInfo elvenLumberMillInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_Created;
	ConstructionBuildingInfo constructionPlanks;
};

#endif