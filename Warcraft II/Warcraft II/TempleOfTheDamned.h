#ifndef __TempleOfTheDamned_H__
#define __TempleOfTheDamned_H__

#include "StaticEntity.h"

struct TempleOfTheDamnedInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class TempleOfTheDamned :
	public StaticEntity
{
public:
	TempleOfTheDamned(fPoint pos, iPoint size, int currLife, uint maxLife, const TempleOfTheDamnedInfo& templeOfTheDamnedInfo, j1Module* listener);
	virtual ~TempleOfTheDamned(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:
	TempleOfTheDamnedInfo templeOfTheDamnedInfo;
};

#endif

