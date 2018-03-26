#ifndef __ElvenArcher_H__
#define __ElvenArcher_H__

#include "DynamicEntity.h"

struct ElvenArcherInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class ElvenArcher :public DynamicEntity
{
public:

	ElvenArcher(fPoint pos, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener);
	~ElvenArcher() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ElvenArcherInfo elvenArcherInfo;
};

#endif //__ElvenArcher_H__