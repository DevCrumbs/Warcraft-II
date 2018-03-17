#ifndef __GryphonAviary_H__
#define __GryphonAviary_H__

#include "StaticEntity.h"

struct GryphonAviaryInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
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

	EntitiesEvent EntityEvent = EntitiesEvent_Created;
};

#endif //__GryphonAviary_H__