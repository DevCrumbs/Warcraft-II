#ifndef __Mage_H__
#define __Mage_H__

#include "DynamicEntity.h"

struct MageInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Mage :public DynamicEntity
{
public:

	Mage(fPoint pos, const MageInfo& mageInfo, j1Module* listener);
	~Mage() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	MageInfo mageInfo;
};

#endif //__Mage_H__