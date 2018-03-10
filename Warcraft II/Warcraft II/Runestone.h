#ifndef __Runestone_H__
#define __Runestone_H__

#include "StaticEntity.h"

struct RunestoneInfo
{

};

class Runestone :public StaticEntity
{
public:

	Runestone(fPoint pos, iPoint size, int life, const RunestoneInfo& runestoneInfo);
	~Runestone() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	RunestoneInfo runestoneInfo;
};

#endif //__Runestone_H__