#ifndef __PlayerCannonTower_H__
#define __PlayerCannonTower_H__

#include "StaticEntity.h"

struct PlayerCannonTowerInfo
{

};

class PlayerCannonTower :public StaticEntity
{
public:

	PlayerCannonTower(fPoint pos, iPoint size, int life, const PlayerCannonTowerInfo& playerCannonTowerInfo);
	~PlayerCannonTower() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	PlayerCannonTowerInfo playerCannonTowerInfo;
};

#endif //__PlayerCannonTower_H__