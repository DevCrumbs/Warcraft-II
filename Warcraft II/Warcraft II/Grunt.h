#ifndef __Grunt_H__
#define __Grunt_H__

#include "DynamicEntity.h"

struct GruntInfo
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

class Grunt :public DynamicEntity
{
public:

	Grunt(fPoint pos, const GruntInfo& gruntInfo, j1Module* listener);
	~Grunt() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GruntInfo gruntInfo;
};

#endif //__Grunt_H__