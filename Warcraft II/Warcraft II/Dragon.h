#ifndef __Dragon_H__
#define __Dragon_H__

#include "DynamicEntity.h"

struct DragonInfo 
{
	int currLife = 0;
	uint maxLife = 0;
};

class Dragon :public DynamicEntity
{
public:

	Dragon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const DragonInfo& dragonInfo, j1Module* listener);
	~Dragon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	DragonInfo dragonInfo;
};

#endif //__Dragon_H__