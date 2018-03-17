#ifndef __Khadgar_H__
#define __Khadgar_H__

#include "DynamicEntity.h"

struct KhadgarInfo
{

};

class Khadgar :public DynamicEntity
{
public:

	Khadgar(fPoint pos, iPoint size, int maxLife, float speed, const KhadgarInfo& khadgarInfo, j1Module* listener);
	~Khadgar() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	KhadgarInfo khadgarInfo;
};

#endif //__Khadgar_H__