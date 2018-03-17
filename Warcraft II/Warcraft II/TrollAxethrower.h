#ifndef __TrollAxethrower_H__
#define __TrollAxethrower_H__

#include "DynamicEntity.h"

struct TrollAxethrowerInfo
{

};

class TrollAxethrower :public DynamicEntity
{
public:

	TrollAxethrower(fPoint pos, iPoint size, int maxLife, float speed, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener);
	~TrollAxethrower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TrollAxethrowerInfo trollAxethrowerInfo;
};

#endif //__TrollAxethrower_H__