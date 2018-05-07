#ifndef __OrcShip_H__
#define __OrcShip_H__

#include "DynamicEntity.h"

struct Collider;
struct ColliderGroup;

enum CollisionState;

struct OrcShipInfo
{
	UnitInfo unitInfo;

	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;
};

class OrcShip :public DynamicEntity
{
public:

	OrcShip(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const OrcShipInfo& orcShipInfo, j1Module* listener);
	~OrcShip() {};
	void Move(float dt);
	void Draw(SDL_Texture* sprites);
	void DebugDrawSelected();

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimationsSpeed(float dt);
	bool ChangeAnimation();


private:

	OrcShipInfo orcShipInfo;

	// Animations speed
	float upSpeed = 0.0f, downSpeed = 0.0f, leftSpeed = 0.0f, rightSpeed = 0.0f;
	float upLeftSpeed = 0.0f, upRightSpeed = 0.0f, downLeftSpeed = 0.0f, downRightSpeed = 0.0f;

	iPoint spawnTile = { -1,-1 };
};

#endif //__Dragon_H__
