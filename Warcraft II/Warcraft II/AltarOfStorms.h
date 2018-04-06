#ifndef __AltarOfStorms_H__
#define __AltarOfStorms_H__

#include "StaticEntity.h"

struct AltarOfStormsInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class AltarOfStorms :public StaticEntity
{
public:
	AltarOfStorms(fPoint pos, iPoint size, int currLife, uint maxLife, const AltarOfStormsInfo& altarOfStormsInfo, j1Module* listener);
	virtual ~AltarOfStorms(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	AltarOfStormsInfo altarOfStormsInfo;

};

#endif

