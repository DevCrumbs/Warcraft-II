#ifndef __Stronghold_H__
#define __Stronghold_H__

#include "StaticEntity.h"

struct StrongholdInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class Stronghold :public StaticEntity
{
public:
	Stronghold(fPoint pos, iPoint size, int currLife, uint maxLife, const StrongholdInfo& strongholdInfo, j1Module* listener);
	virtual ~Stronghold(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	StrongholdInfo strongholdInfo;

};

#endif
