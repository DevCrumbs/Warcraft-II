#ifndef __OrcShip_H__
#define __OrcShip_H__

#include "DynamicEntity.h"

struct Collider;
struct ColliderGroup;

enum CollisionState;

enum ShipType {
	ShipType_UP_LEFT,
	ShipType_BOTTOM,
	ShipType_BOTTOM_RIGHT,
	ShipType_UP_RIGHT,
	ShipType_NONE
};

struct OrcShipInfo
{
	UnitInfo unitInfo;

	Animation up, down, left, right;
	Animation upLeft, upRight, downLeft, downRight;

	ShipType orcShipType = ShipType_NONE;
};

class OrcShip :public DynamicEntity
{
public:

	OrcShip(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const OrcShipInfo& orcShipInfo, j1Module* listener);
	~OrcShip() {};
	void Move(float dt);
	void Draw(SDL_Texture* sprites);
	void DebugDrawSelected();

private:

	OrcShipInfo orcShipInfo;

	int speed = 0;

	iPoint spawnTile = { -1,-1 };

	j1Timer movementTimer;

	bool isSpawnedWave = false;
};

#endif //__Dragon_H__
