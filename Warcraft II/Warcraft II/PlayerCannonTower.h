#ifndef __PlayerCannonTower_H__
#define __PlayerCannonTower_H__

#include "StaticEntity.h"

struct PlayerCannonTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
	SDL_Rect constructionPlanks1 = { 0,0,0,0 };
	SDL_Rect constructionPlanks2 = { 0,0,0,0 };
	int maxLife = 0;	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class PlayerCannonTower :public StaticEntity
{
public:

	PlayerCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener);
	~PlayerCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PlayerCannonTowerInfo playerCannonTowerInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_CREATED;
};

#endif //__PlayerCannonTower_H__