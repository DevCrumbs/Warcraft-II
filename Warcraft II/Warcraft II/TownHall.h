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
	SDL_Rect townHallCompleteTexArea = { 0,0,0,0 };
	SDL_Rect townHallInProgressTexArea = { 0,0,0,0 };
	SDL_Rect keepCompleteTexArea = { 0,0,0,0 };
	SDL_Rect keepInProgressTexArea = { 0,0,0,0 };
	SDL_Rect castleCompleteTexArea = { 0,0,0,0 };
	SDL_Rect castleInProgressTexArea = { 0,0,0,0 };
	int townHallMaxLife = 0;
	int keepMaxLife = 0;
	int castleMaxLife = 0;
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class TownHall :public StaticEntity
{
public:

	TownHall(fPoint pos, iPoint size, int currLife, uint maxLife, const TownHallInfo& townHallInfo, j1Module* listener);
	~TownHall() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	TownHallInfo townHallInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_NONE;

};

#endif //__TownHall_H__