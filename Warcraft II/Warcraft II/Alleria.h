#ifndef __Alleria_H__
#define __Alleria_H__

#include "DynamicEntity.h"

struct AlleriaInfo
{

};

class Alleria :public DynamicEntity
{
public:

	Alleria(fPoint pos, iPoint size, int maxLife, float speed, const AlleriaInfo& alleriaInfo, j1Module* listener);
	~Alleria() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	AlleriaInfo alleriaInfo;
};

#endif //__Alleria_H__