#ifndef __PlayerGuardTower_H__
#define __PlayerGuardTower_H__

#include "StaticEntity.h"

struct PlayerGuardTowerInfo
{

};

class PlayerGuardTower :public StaticEntity
{
public:

	PlayerGuardTower(fPoint pos, iPoint size, int life, const PlayerGuardTowerInfo& playerGuardTowerInfo);
	~PlayerGuardTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PlayerGuardTowerInfo playerGuardTowerInfo;
};

#endif //__PlayerGuardTower_H__