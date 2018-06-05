#ifndef __StaticEntity_H__
#define __StaticEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"
#include "j1Input.h"
#include "j1Window.h"

/// TODO Balancing (seconds until the enemies reconstruct a building)
#define SECONDS_START_RECONSTRUCTION 20.0f

struct SDL_Texture;
struct Particle;

enum ENTITY_TYPE;
enum ColliderType;
enum DistanceHeuristic;
enum ROOM_TYPE;

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
	BuildingState_NoState,
	BuildingState_Normal,
	BuildingState_LowFire,	
	BuildingState_HardFire,
	BuildingState_Destroyed,
	BuildingState_Building,
	BuildingState_MaxStates
};

enum StaticEntitySize
{
	StaticEntitySize_None,
	StaticEntitySize_Small,
	StaticEntitySize_Medium,
	StaticEntitySize_Big,
	StaticEntitySize_MaxSize
};

enum TowerState
{
	TowerState_NoState,
	TowerState_Idle,
	TowerState_Attack,
	TowerState_Die,
	TowerState_MaxStates
};

class StaticEntity :public Entity
{
public:

	StaticEntity(fPoint pos, iPoint size, int currLife, uint maxLife, j1Module* listener);
	virtual ~StaticEntity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void DebugDrawSelected();
	virtual void Move(float dt) {}

	//virtual void DebugDrawSelected();

	void HandleInput(EntitiesEvent &EntityEvent);
	bool MouseHover() const;
	bool CheckBuildingState();
	float GetConstructionTimer() const;
	float GetConstructionTime() const;
	bool GetIsFinishedBuilt() const;

	BuildingState GetBuildingState() const;

	void SetBuildingState(BuildingState state);

	// Collision
	ColliderGroup* CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic);
	ColliderGroup* GetSightRadiusCollider() const;

	// Reconstruction
	float GetSecondsReconstruction(StaticEntitySize buildingSize) const;

	// Respawn
	float GetRandomSecondsRespawn() const;
	uint GetMaxEnemiesPerRoom(ROOM_TYPE roomType) const;

public:

	const ENTITY_TYPE staticEntityType = EntityType_NONE;
	const StaticEntityCategory staticEntityCategory = StaticEntityCategory_NoCategory;
	BuildingState buildingState = BuildingState_Normal;
	const StaticEntitySize buildingSize = StaticEntitySize_None;

	bool isBuilt = false;

protected:

	Particle* fire = nullptr;
	const SDL_Rect* texArea = nullptr;

	float constructionTimer = 0.0f;
	float constructionTime = 0.0f;

	// Collision
	ColliderGroup* sightRadiusCollider = nullptr;
};

#endif //__StaticEntity_H__