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

	uint sightRadius = 0;
};

class Runestone :public StaticEntity
{
public:

	Runestone(fPoint pos, iPoint size, int currLife, uint maxLife, const RunestoneInfo& runestoneInfo, j1Module* listener);
	~Runestone() {};

	void Move(float dt);

	// Heal area
	bool IsUnitHealingArea() const;
	void SetUnitHealingArea(bool isUnitHealingArea);

	// Tex area
	void SwapTexArea();

	// Blit alpha area
	void BlitSightArea(Uint8 alpha);

private:

	RunestoneInfo runestoneInfo;
	EntitiesEvent entityEvent = EntitiesEvent_NONE;

	// Heal area
	bool isUnitHealingArea = false;
};

#endif //__Runestone_H__