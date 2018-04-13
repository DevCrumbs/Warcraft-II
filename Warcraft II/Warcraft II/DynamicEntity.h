#ifndef __DynamicEntity_H__
#define __DynamicEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1Timer.h"

#include "Entity.h"

#include <string>
#include <vector>
#include <list>
using namespace std;

struct SDL_Texture;
struct SDL_Color;

struct Collider;
struct ColliderGroup;
struct SingleUnit;

class PathPlanner;
class Navgraph;
class Goal_Think;
class UILifeBar;

enum ColliderType;
enum CollisionState;
enum DistanceHeuristic;

#define TIME_REMOVE_CORPSE 3.0

enum UnitState
{
	UnitState_NoState,

	UnitState_Idle,
	UnitState_Walk,
	UnitState_Die,

	// Composite goals only
	UnitState_AttackTarget,
	UnitState_Patrol,
	UnitState_Wander,

	UnitState_MaxStates
};

enum UnitDirection
{
	UnitDirection_NoDirection,

	UnitDirection_Up,
	UnitDirection_Down,
	UnitDirection_Left,
	UnitDirection_Right,
	UnitDirection_UpLeft,
	UnitDirection_UpRight,
	UnitDirection_DownLeft,
	UnitDirection_DownRight
};

enum UnitCommand
{
	UnitCommand_NoCommand,

	UnitCommand_Stop,
	UnitCommand_MoveToPosition,
	UnitCommand_AttackTarget,
	UnitCommand_Patrol,

	UnitCommand_MaxCommands
};

// Struct UnitInfo: contains all necessary information of the movement and attack of the unit
struct UnitInfo
{
	uint priority = 1;

	uint sightRadius = 0;
	uint attackRadius = 0;
	uint damage = 0;

	float maxSpeed = 0.0f;
	float currSpeed = 0.0f;
};

class DynamicEntity :public Entity
{
public:

	DynamicEntity(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, j1Module* listener);
	virtual ~DynamicEntity();
	virtual void Move(float dt);
	virtual void Draw(SDL_Texture* sprites);
	virtual void DebugDrawSelected();
	virtual void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);
	
	Animation* GetAnimation() const;
	Goal_Think* GetBrain() const;

	// UnitInfo
	float GetSpeed() const;
	uint GetPriority() const;
	uint GetDamage() const;

	// State machine
	void UnitStateMachine(float dt);

	void SetUnitState(UnitState unitState);
	UnitState GetUnitState() const;

	// Movement
	SingleUnit* GetSingleUnit() const;
	PathPlanner* GetPathPlanner() const;
	Navgraph* GetNavgraph() const;

	void SetIsStill(bool isStill);
	bool IsStill() const;

	// Blit
	void SetBlitState(bool isBlitting) const;
	bool GetBlitState() const;

	// Animations
	virtual void LoadAnimationsSpeed();
	virtual void UpdateAnimationsSpeed(float dt);
	virtual bool ChangeAnimation();

	// Direction
	void SetUnitDirection(UnitDirection unitDirection);
	UnitDirection GetUnitDirection() const;

	void SetUnitDirectionByValue(fPoint unitDirection);
	fPoint GetUnitDirectionByValue() const;

	// Selection color
	void SetColor(SDL_Color color, string colorName);
	SDL_Color GetColor() const;
	string GetColorName() const;

	// Collision
	ColliderGroup* GetSightRadiusCollider() const;
	ColliderGroup* GetAttackRadiusCollider() const;

	ColliderGroup* CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic);
	void UpdateRhombusColliderPos(ColliderGroup* collider, uint radius, DistanceHeuristic distanceHeuristic);

	// Attack
	/// Unit attacks a target
	Entity* GetCurrTarget() const;
	bool SetCurrTarget(Entity* target);

	bool IsEntityInTargetsList(Entity* entity) const;
	bool InvalidateTarget(Entity* entity); // sets isRemove to true
	bool RemoveTargetInfo(TargetInfo* targetInfo);

	TargetInfo* GetBestTargetInfo() const; // TODO: add argument EntityType??? For critters vs enemies

	void SetHitting(bool isHitting);
	bool IsHitting() const;

	// Player commands
	bool SetUnitCommand(UnitCommand unitCommand);
	UnitCommand GetUnitCommand() const;

public:

	ENTITY_TYPE dynamicEntityType = EntityType_NONE;

	// Dead
	bool isDead = false; // if true, the unit is performing their dead animation

	// Spawn
	bool isSpawned = false;

protected:

	UnitInfo unitInfo;
	UnitState unitState = UnitState_Idle;

	Animation* animation = nullptr;

	bool isFlying = false; /// Dragon and Gryphon Rider fly

	// Root of a bot's goal hierarchy
	Goal_Think* brain = nullptr;

	// Player commands
	UnitCommand unitCommand = UnitCommand_NoCommand;

	// Movement
	fPoint direction = { 0.0f,0.0f };

	SingleUnit* singleUnit = nullptr;
	PathPlanner* pathPlanner = nullptr;
	Navgraph* navgraph = nullptr;

	bool isStill = true; // if true, the unit is still. Else, the unit is moving

	// Attack
	list<TargetInfo*> targets;
	TargetInfo* currTarget = nullptr;
	TargetInfo* newTarget = nullptr;

	bool isHitting = false; // if true, the unit is hitting their target

	// Collision
	ColliderGroup* sightRadiusCollider = nullptr;
	ColliderGroup* attackRadiusCollider = nullptr;
	iPoint lastColliderUpdateTile = { -1,-1 };

	// Death
	j1Timer deadTimer;

	//Blit
	bool isBlitting = false;

	// Selection color
	SDL_Color color = ColorWhite;
	string colorName = "White";

	//LifeBar
	UILifeBar* lifeBar = nullptr;
	int lifeBarMarginX = 0;
	int lifeBarMarginY = 0;
};

// ---------------------------------------------------------------------
// Helper class to establish a priority to a TargetInfo
// ---------------------------------------------------------------------
class TargetInfoPriority
{
public:
	TargetInfoPriority() {}
	TargetInfoPriority(TargetInfo* targetInfo, int priority) :targetInfo(targetInfo), priority(priority) {}
	TargetInfoPriority(const TargetInfoPriority& t)
	{
		targetInfo = t.targetInfo;
		priority = t.priority;
	}

	TargetInfo* targetInfo = nullptr;
	int priority = 0;
};

// ---------------------------------------------------------------------
// Helper class to compare two TargetInfo by its priority values
// ---------------------------------------------------------------------
class TargetInfoPriorityComparator
{
public:
	int operator() (const TargetInfoPriority a, const TargetInfoPriority b)
	{
		return a.priority > b.priority;
	}
};

#endif //__DynamicEntity_H__