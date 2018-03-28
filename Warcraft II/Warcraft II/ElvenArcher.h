#ifndef __ElvenArcher_H__
#define __ElvenArcher_H__

#include "DynamicEntity.h"

struct ElvenArcherInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class ElvenArcher :public DynamicEntity
{
public:

	ElvenArcher(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener);
	~ElvenArcher() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ElvenArcherInfo elvenArcherInfo;
};

#endif //__ElvenArcher_H__