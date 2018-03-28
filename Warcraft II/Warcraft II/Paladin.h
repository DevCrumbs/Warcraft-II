#ifndef __Paladin_H__
#define __Paladin_H__

#include "DynamicEntity.h"

struct PaladinInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class Paladin :public DynamicEntity
{
public:

	Paladin(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const PaladinInfo& paladinInfo, j1Module* listener);
	~Paladin() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PaladinInfo paladinInfo;
};

#endif //__Paladin_H__