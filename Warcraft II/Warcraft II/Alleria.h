#ifndef __Alleria_H__
#define __Alleria_H__

#include "DynamicEntity.h"

struct AlleriaInfo
{

};

class Alleria :public DynamicEntity
{
public:

	Alleria(fPoint pos, iPoint size, int life, float speed, const AlleriaInfo& alleriaInfo);
	~Alleria() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	AlleriaInfo alleriaInfo;
};

#endif //__Alleria_H__