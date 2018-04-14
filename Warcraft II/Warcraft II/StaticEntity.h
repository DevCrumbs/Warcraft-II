#ifndef __StaticEntity_H__
#define __StaticEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"
#include "j1Input.h"
#include "j1Window.h"

struct SDL_Texture;
struct Particle;

enum ENTITY_TYPE;
enum ColliderType;
enum DistanceHeuristic;

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
	BuildingState_Destroyed,
	BuildingState_Building

};

enum StaticEntitySize
{
	None,
	Small,
	Medium,
	Big
};

enum TowerState
{
	TowerState_Idle,
	TowerState_Attack,
	TowerState_Die
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

	StaticEntity(fPoint pos, iPoint size, int currLife, uint maxLife, j1Module* listener);
	virtual ~StaticEntity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void Move(float dt) {}

	//virtual void DebugDrawSelected();
	
	void HandleInput(EntitiesEvent &EntityEvent);
	bool MouseHover() const;
	bool CheckBuildingState();
	uint GetConstructionTimer() const;
	uint GetConstructionTime() const;
	bool GetIsFinishedBuilt() const;

	//Colliders
	ColliderGroup* CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic);
	ColliderGroup* GetSightRadiusCollider() const;

public:

	ENTITY_TYPE staticEntityType = EntityType_NONE;
	StaticEntityCategory staticEntityCategory = StaticEntityCategory_NoCategory;
	BuildingState buildingState = BuildingState_Normal;
	StaticEntitySize buildingSize = None;

protected:

	Particle* fire;
	const SDL_Rect* texArea = nullptr;
	j1Timer constructionTimer;
	uint constructionTime = 0;
	bool isBuilt = false;

	ColliderGroup* sightRadiusCollider = nullptr;

};

#endif //__StaticEntity_H__