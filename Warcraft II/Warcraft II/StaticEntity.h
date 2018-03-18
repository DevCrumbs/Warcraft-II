#ifndef __StaticEntity_H__
#define __StaticEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"
#include "j1Input.h"
#include "j1Window.h"

struct SDL_Texture;

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

struct ConstructionBuildingInfo
{
	SDL_Rect constructionPlanksBig1 = { 376,529,128,128 };
	SDL_Rect constructionPlanksBig2 = { 565,485,128,128 };
	SDL_Rect constructionPlanksSmall1 = { 405,561,64,64 };
	SDL_Rect constructionPlanksSmall2 = { 597,517,64,64 };
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

	StaticEntity(fPoint pos, iPoint size, int maxLife, j1Module* listener);
	virtual ~StaticEntity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void Move(float dt) {}

	//virtual void DebugDrawSelected();
	
	void HandleInput(EntitiesEvent &EntityEvent);
	bool MouseHover() const;

public:

	StaticEntityType staticEntityType = StaticEntityType_NoType;
	StaticEntityCategory staticEntityCategory = StaticEntityCategory_NoCategory;

protected:

	const SDL_Rect* texArea = nullptr;
	j1Timer constructionTimer;
	uint constructionTime = 0;

};

#endif //__StaticEntity_H__