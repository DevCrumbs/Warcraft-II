#ifndef __PigFarm_H__
#define __PigFarm_H__

#include "StaticEntity.h"

struct PigFarmInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};


class PigFarm : public StaticEntity
{
public:
	PigFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const PigFarmInfo& pigFarmInfo, j1Module* listener);
	virtual ~PigFarm() {}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PigFarmInfo pigFarmInfo;
};

#endif
