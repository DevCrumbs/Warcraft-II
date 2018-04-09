#ifndef __DragonRoost_H__
#define __DragonRoost_H__

#include "StaticEntity.h"

struct DragonRoostInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class DragonRoost :public StaticEntity
{
public:
	DragonRoost(fPoint pos, iPoint size, int currLife, uint maxLife, const DragonRoostInfo& dragonRoostInfo, j1Module* listener);
	virtual ~DragonRoost() {}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	DragonRoostInfo dragonRoostInfo;
};

#endif

