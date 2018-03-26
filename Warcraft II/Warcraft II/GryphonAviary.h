#ifndef __GryphonAviary_H__
#define __GryphonAviary_H__

#include "StaticEntity.h"

struct GryphonAviaryInfo
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

class GryphonAviary :public StaticEntity
{
public:

	GryphonAviary(fPoint pos, iPoint size, int maxLife, const GryphonAviaryInfo& gryphonAviaryInfo, j1Module* listener);
	~GryphonAviary() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GryphonAviaryInfo gryphonAviaryInfo;
	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
};

#endif //__GryphonAviary_H__