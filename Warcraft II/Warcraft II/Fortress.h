#ifndef __Fortress_H__
#define __Fortress_H__

#include "StaticEntity.h"

struct FortressInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class Fortress :public StaticEntity
{
public:
	Fortress(fPoint pos, iPoint size, int currLife, uint maxLife, const FortressInfo& fortressInfo, j1Module* listener);
	virtual ~Fortress(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	FortressInfo fortressInfo;
};

#endif

