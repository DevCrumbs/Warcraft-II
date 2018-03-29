#ifndef __TrollAxethrower_H__
#define __TrollAxethrower_H__

#include "DynamicEntity.h"

struct TrollAxethrowerInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class TrollAxethrower :public DynamicEntity
{
public:

	TrollAxethrower(fPoint pos, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener);
	~TrollAxethrower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TrollAxethrowerInfo trollAxethrowerInfo;
};

#endif //__TrollAxethrower_H__