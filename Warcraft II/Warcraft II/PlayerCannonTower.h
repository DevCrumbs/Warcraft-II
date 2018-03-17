#ifndef __PlayerCannonTower_H__
#define __PlayerCannonTower_H__

#include "StaticEntity.h"

struct PlayerCannonTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class PlayerCannonTower :public StaticEntity
{
public:

	PlayerCannonTower(fPoint pos, iPoint size, int maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener);
	~PlayerCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PlayerCannonTowerInfo playerCannonTowerInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_Created;

};

#endif //__PlayerCannonTower_H__