#ifndef __ChickenFarm_H__
#define __ChickenFarm_H__

#include "StaticEntity.h"

struct ChickenFarmInfo
{

};

class ChickenFarm :public StaticEntity
{
public:

	ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo);
	~ChickenFarm() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ChickenFarmInfo chickenFarmInfo;
};

#endif //__ChickenFarm_H__