#ifndef __Paladin_H__
#define __Paladin_H__

#include "DynamicEntity.h"

struct PaladinInfo
{

};

class Paladin :public DynamicEntity
{
public:

	Paladin(fPoint pos, iPoint size, int life, float speed, const PaladinInfo& paladinInfo, j1Module* listener);
	~Paladin() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PaladinInfo paladinInfo;
};

#endif //__Paladin_H__