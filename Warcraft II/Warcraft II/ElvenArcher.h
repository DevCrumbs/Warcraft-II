#ifndef __ElvenArcher_H__
#define __ElvenArcher_H__

#include "DynamicEntity.h"

struct ElvenArcherInfo
{

};

class ElvenArcher :public DynamicEntity
{
public:

	ElvenArcher(fPoint pos, iPoint size, int life, float speed, const ElvenArcherInfo& elvenArcherInfo);
	~ElvenArcher() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ElvenArcherInfo elvenArcherInfo;
};

#endif //__ElvenArcher_H__