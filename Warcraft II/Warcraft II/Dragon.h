#ifndef __Dragon_H__
#define __Dragon_H__

#include "DynamicEntity.h"

struct DragonInfo 
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Dragon :public DynamicEntity
{
public:

	Dragon(fPoint pos, const DragonInfo& dragonInfo, j1Module* listener);
	~Dragon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	DragonInfo dragonInfo;
};

#endif //__Dragon_H__