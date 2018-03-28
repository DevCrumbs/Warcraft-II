#ifndef __Mage_H__
#define __Mage_H__

#include "DynamicEntity.h"

struct MageInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class Mage :public DynamicEntity
{
public:

	Mage(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const MageInfo& mageInfo, j1Module* listener);
	~Mage() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	MageInfo mageInfo;
};

#endif //__Mage_H__