#ifndef __Footman_H__
#define __Footman_H__

#include "DynamicEntity.h"

struct FootmanInfo
{
	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;
	Animation attackUp, attackDown, attackLeft, attackRight;
	Animation attackUpLeft, attackUpRight, attackDownLeft, attackDownRight;
	Animation deathUp, deathDown;

	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Footman :public DynamicEntity
{
public:

	Footman(fPoint pos, const FootmanInfo& footmanInfo, j1Module* listener);
	~Footman() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	FootmanInfo footmanInfo;
};

#endif //__Footman_H__