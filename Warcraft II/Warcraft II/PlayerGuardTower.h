#ifndef __PlayerGuardTower_H__
#define __PlayerGuardTower_H__

#include "StaticEntity.h"

struct PlayerGuardTowerInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class PlayerGuardTower :public StaticEntity
{
public:

	PlayerGuardTower(fPoint pos, iPoint size, int maxLife, const PlayerGuardTowerInfo& playerGuardTowerInfo, j1Module* listener);
	~PlayerGuardTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PlayerGuardTowerInfo playerGuardTowerInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_Created;

};

#endif //__PlayerGuardTower_H__