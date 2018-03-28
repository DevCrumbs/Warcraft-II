#ifndef __Khadgar_H__
#define __Khadgar_H__

#include "DynamicEntity.h"

struct KhadgarInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class Khadgar :public DynamicEntity
{
public:

	Khadgar(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const KhadgarInfo& khadgarInfo, j1Module* listener);
	~Khadgar() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	KhadgarInfo khadgarInfo;
};

#endif //__Khadgar_H__