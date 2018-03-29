#ifndef __Alleria_H__
#define __Alleria_H__

#include "DynamicEntity.h"

struct AlleriaInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Alleria :public DynamicEntity
{
public:

	Alleria(fPoint pos, const AlleriaInfo& alleriaInfo, j1Module* listener);
	~Alleria() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	AlleriaInfo alleriaInfo;
};

#endif //__Alleria_H__