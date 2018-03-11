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