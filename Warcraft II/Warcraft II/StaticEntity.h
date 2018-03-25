#ifndef __StaticEntity_H__
#define __StaticEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"
#include "j1Input.h"
#include "j1Window.h"

struct SDL_Texture;
struct Particle;

enum StaticEntityType
{
	StaticEntityType_NoType,

	// Player buildings
	/// Production buildings
	StaticEntityType_TownHall,
	StaticEntityType_ChickenFarm,
	StaticEntityType_Barracks,
	StaticEntityType_ElvenLumberMill,
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

enum BuildingState
{
	BuildingState_Normal,
	BuildingState_LowFire,
	BuildingState_HardFire,
	BuildingState_Destroyed
};

enum StaticEntitySize
{
	None,
	Small,
	Medium,
	Big
};

struct BuildingPreviewTiles 
{
	SDL_Rect greenTile = { 0,0,0,0 };
	SDL_Rect redTile = { 0,0,0,0 };
	uint opacity = 0;
};

class StaticEntity :public Entity
{
public:

	StaticEntity(fPoint pos, iPoint size, int maxLife, j1Module* listener);
	virtual ~StaticEntity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void Move(float dt) {}

	//virtual void DebugDrawSelected();
	
	void HandleInput(EntitiesEvent &EntityEvent);
	bool MouseHover() const;
	bool CheckBuildingState();

public:

	StaticEntityType staticEntityType = StaticEntityType_NoType;
	StaticEntityCategory staticEntityCategory = StaticEntityCategory_NoCategory;
	BuildingState buildingState = BuildingState_Normal;

protected:

	Particle* fire;
	const SDL_Rect* texArea = nullptr;
	j1Timer constructionTimer;
	uint constructionTime = 0;
	bool isBuilt = false;

};

#endif //__StaticEntity_H__