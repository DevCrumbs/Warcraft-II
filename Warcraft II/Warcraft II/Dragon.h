#ifndef __Dragon_H__
#define __Dragon_H__

#include "DynamicEntity.h"

struct DragonInfo 
{

};

class Dragon :public DynamicEntity
{
public:

	Dragon(fPoint pos, iPoint size, int life, float speed, const DragonInfo& dragonInfo);
	~Dragon() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	DragonInfo dragonInfo;
};

#endif //__Dragon_H__