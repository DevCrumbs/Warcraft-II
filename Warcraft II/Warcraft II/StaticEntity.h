#ifndef __StaticEntity_H__
#define __StaticEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;

enum StaticEntityType
{
	StaticEntityType_NoType,

	// Player buildings
	/// Production buildings
	StaticEntityType_TownHall,
	StaticEntityType_ChickenFarm,
	StaticEntityType_Barracks,
	StaticEntityType_MageTower,
	StaticEntityType_GryphonAviary,
	StaticEntityType_Stables,

	/// Defense buildings
	StaticEntityType_ScoutTower,
	StaticEntityType_PlayerGuardTower,
	StaticEntityType_PlayerCannonTower,

	// Neutral buildings
	StaticEntityType_GoldMine,
	StaticEntityType_Runestone,

	// Enemy buildings
	/// Defense buildings
	StaticEntityType_WatchTower,
	StaticEntityType_EnemyGuardTower,
	StaticEntityType_EnemyCannonTower,

	StaticEntityType_MaxTypes
};

enum StaticEntityCategory
{
	StaticEntityCategory_NoCategory,
	StaticEntityCategory_HumanBuilding,
	StaticEntityCategory_OrcishBuilding,
	StaticEntityCategory_NeutralBuilding,
	StaticEntityCategory_MaxCategories
};

class StaticEntity :public Entity
{
public:

	StaticEntity(fPoint pos, iPoint size, int life, j1Module* listener);
	virtual ~StaticEntity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void Move(float dt) {}

	//virtual void DebugDrawSelected();

public:

	StaticEntityType staticEntityType = StaticEntityType_NoType;
	StaticEntityCategory staticEntityCategory = StaticEntityCategory_NoCategory;

protected:

	const SDL_Rect* texArea = nullptr;
	//j1Module* listener = nullptr;
};

#endif //__StaticEntity_H__