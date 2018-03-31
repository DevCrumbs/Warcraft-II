#ifndef __GoldMine_H__
#define __GoldMine_H__

#include "StaticEntity.h"

struct GoldMineInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	int maxLife = 0;
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class GoldMine :public StaticEntity
{
public:

	GoldMine(fPoint pos, iPoint size, int currLife, uint maxLife, const GoldMineInfo& goldMineInfo, j1Module* listener);
	~GoldMine() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GoldMineInfo goldMineInfo;

	EntitiesEvent entityEvent = EntitiesEvent_NONE;
};

#endif //__GoldMine_H__