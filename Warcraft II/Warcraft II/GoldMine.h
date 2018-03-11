#ifndef __GoldMine_H__
#define __GoldMine_H__

#include "StaticEntity.h"

struct GoldMineInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class GoldMine :public StaticEntity
{
public:

	GoldMine(fPoint pos, iPoint size, int life, const GoldMineInfo& goldMineInfo);
	~GoldMine() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GoldMineInfo goldMineInfo;
};

#endif //__GoldMine_H__