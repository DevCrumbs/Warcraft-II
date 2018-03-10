#ifndef __Mage_H__
#define __Mage_H__

#include "DynamicEntity.h"

struct MageInfo
{

};

class Mage :public DynamicEntity
{
public:

	Mage(fPoint pos, iPoint size, int life, float speed, const MageInfo& mageInfo);
	~Mage() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	MageInfo mageInfo;
};

#endif //__Mage_H__