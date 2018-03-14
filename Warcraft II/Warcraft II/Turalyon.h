#ifndef __Turalyon_H__
#define __Turalyon_H__

#include "DynamicEntity.h"

struct TuralyonInfo
{

};

class Turalyon :public DynamicEntity
{
public:

	Turalyon(fPoint pos, iPoint size, int life, float speed, const TuralyonInfo& turalyonInfo);
	~Turalyon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TuralyonInfo turalyonInfo;
};

#endif //__Turalyon_H__