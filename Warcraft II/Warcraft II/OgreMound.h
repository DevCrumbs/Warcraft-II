#ifndef __OgreMound_H__
#define __OgreMound_H__

#include "StaticEntity.h"

struct OgreMoundInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class OgreMound :public StaticEntity
{
public:
	OgreMound(fPoint pos, iPoint size, int currLife, uint maxLife, const OgreMoundInfo& ogreMoundInfo, j1Module* listener);
	virtual ~OgreMound(){}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	OgreMoundInfo ogreMoundInfo;

};

#endif

