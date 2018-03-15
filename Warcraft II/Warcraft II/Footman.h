#ifndef __Footman_H__
#define __Footman_H__

#include "DynamicEntity.h"

struct FootmanInfo
{

};

class Footman :public DynamicEntity
{
public:

	Footman(fPoint pos, iPoint size, int life, float speed, const FootmanInfo& footmanInfo);
	~Footman() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	FootmanInfo footmanInfo;
};

#endif //__Footman_H__