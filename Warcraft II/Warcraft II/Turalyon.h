#ifndef __Turalyon_H__
#define __Turalyon_H__

#include "DynamicEntity.h"

struct TuralyonInfo
{
	UnitInfo unitInfo;
	Animation idle;
	int currLife = 0;
	uint maxLife = 0;
};

class Turalyon :public DynamicEntity
{
public:

	Turalyon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TuralyonInfo& khadgarInfo, j1Module* listener);
	~Turalyon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:
	EntitiesEvent entityEvent = EntitiesEvent_NONE;

	TuralyonInfo turalyonInfo;
};

#endif //__Turalyon_H__