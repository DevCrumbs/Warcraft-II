#ifndef __Paladin_H__
#define __Paladin_H__

#include "DynamicEntity.h"

struct PaladinInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Paladin :public DynamicEntity
{
public:

	Paladin(fPoint pos, const PaladinInfo& paladinInfo, j1Module* listener);
	~Paladin() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PaladinInfo paladinInfo;
};

#endif //__Paladin_H__