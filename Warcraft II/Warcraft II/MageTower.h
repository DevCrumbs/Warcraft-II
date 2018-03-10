#ifndef __MageTower_H__
#define __MageTower_H__

#include "StaticEntity.h"

struct MageTowerInfo
{

};

class MageTower :public StaticEntity
{
public:

	MageTower(fPoint pos, iPoint size, int life, const MageTowerInfo& mageTowerInfo);
	~MageTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	MageTowerInfo mageTowerInfo;
};

#endif //__MageTower_H__