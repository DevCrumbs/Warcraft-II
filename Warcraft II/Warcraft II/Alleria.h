#ifndef __Alleria_H__
#define __Alleria_H__

#include "DynamicEntity.h"

struct AlleriaInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class Alleria :public DynamicEntity
{
public:

	Alleria(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const AlleriaInfo& alleriaInfo, j1Module* listener);
	~Alleria() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	AlleriaInfo alleriaInfo;
};

#endif //__Alleria_H__