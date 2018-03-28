#ifndef __Runestone_H__
#define __Runestone_H__

#include "StaticEntity.h"

struct RunestoneInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Runestone :public StaticEntity
{
public:

	Runestone(fPoint pos, iPoint size, int currLife, uint maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener);
	~Runestone() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	RunestoneInfo runestoneInfo;
};

#endif //__Runestone_H__