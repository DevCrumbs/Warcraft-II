#ifndef __Footman_H__
#define __Footman_H__

#include "DynamicEntity.h"

struct FootmanInfo
{

};

class Footman :public DynamicEntity
{
public:

	Footman(fPoint pos, iPoint size, int maxLife, float speed, const FootmanInfo& footmanInfo, j1Module* listener);
	~Footman() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	FootmanInfo footmanInfo;
};

#endif //__Footman_H__