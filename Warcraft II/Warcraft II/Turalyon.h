#ifndef __Turalyon_H__
#define __Turalyon_H__

#include "DynamicEntity.h"

struct TuralyonInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Turalyon :public DynamicEntity
{
public:

	Turalyon(fPoint pos, const TuralyonInfo& turalyonInfo, j1Module* listener);
	~Turalyon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TuralyonInfo turalyonInfo;
};

#endif //__Turalyon_H__