#ifndef __TownHall_H__
#define __TownHall_H__

#include "StaticEntity.h"

enum TownHallType
{
	TownHallType_TownHall,
	TownHallType_Keep,
	TownHallType_Castle
};

struct TownHallInfo
{
	TownHallType townHallType = TownHallType_TownHall;
};

class TownHall :public StaticEntity
{
public:

	TownHall(fPoint pos, iPoint size, int life, const TownHallInfo& townHallInfo);
	~TownHall() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TownHallInfo townHallInfo;
};

#endif //__TownHall_H__