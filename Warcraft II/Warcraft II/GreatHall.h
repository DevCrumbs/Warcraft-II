#ifndef __GreatHall_H__
#define __GreatHall_H__

#include "StaticEntity.h"

struct GreatHallInfo {
	SDL_Rect completeTexArea = { 0,0,0,0 };
	uint life = 0u;
};

class GreatHall :public StaticEntity
{
public:
	GreatHall(fPoint pos, iPoint size, int currLife, uint maxLife, const GreatHallInfo& greatHallInfo, j1Module* listener);
	virtual ~GreatHall() {}

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GreatHallInfo greatHallInfo;

};

#endif
