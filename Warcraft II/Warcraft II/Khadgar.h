#ifndef __Khadgar_H__
#define __Khadgar_H__

#include "DynamicEntity.h"

struct KhadgarInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class Khadgar :public DynamicEntity
{
public:

	Khadgar(fPoint pos, const KhadgarInfo& khadgarInfo, j1Module* listener);
	~Khadgar() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	KhadgarInfo khadgarInfo;
};

#endif //__Khadgar_H__