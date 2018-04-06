#ifndef __TrollLumberMill_H__
#define __TrollLumberMill_H__

#include "StaticEntity.h"

struct TrollLumberMillInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class TrollLumberMill :public StaticEntity
{
public:
	TrollLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const TrollLumberMillInfo& trollLumberMillInfo, j1Module* listener);
	virtual ~TrollLumberMill(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TrollLumberMillInfo trollLumberMillInfo;

};

#endif
