#ifndef __Grunt_H__
#define __Grunt_H__

#include "DynamicEntity.h"

struct GruntInfo
{

};

class Grunt :public DynamicEntity
{
public:

	Grunt(fPoint pos, iPoint size, int maxLife, float speed, const GruntInfo& gruntInfo, j1Module* listener);
	~Grunt() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GruntInfo gruntInfo;
};

#endif //__Grunt_H__