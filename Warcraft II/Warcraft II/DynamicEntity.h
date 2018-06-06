#ifndef __DynamicEntity_H__
#define __DynamicEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1Timer.h"
#include "j1FogOfWar.h"

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
class Goal_Think;
class UILifeBar;

enum ColliderType;
enum CollisionState;
enum DistanceHeuristic;

struct Particle;
struct GoldMine;
struct Runestone;

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
	UnitState_GatherGold,
	UnitState_HealRunestone,
	UnitState_RescuePrisoner,

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
	UnitCommand_GatherGold,
	UnitCommand_HealRunestone,
	UnitCommand_RescuePrisoner,

	UnitCommand_MaxCommands
};

// Struct UnitInfo: contains all necessary information of the movement and attack of the unit
struct UnitInfo
{
	UnitInfo();
	UnitInfo(const UnitInfo& u);
	~UnitInfo();

	uint priority = 1;

	// Radius
	uint sightRadius = 0;
	uint attackRadius = 0;

	// Damage
	uint heavyDamage = 0;
	uint lightDamage = 0;
	uint airDamage = 0;
	uint towerDamage = 0;

	// Speed
	float maxSpeed = 0.0f;
	float currSpeed = 0.0f;

	// Life
	int currLife = 0;
	uint maxLife = 0;

	// Size
	iPoint size = { 0,0 };
	iPoint offsetSize = { 0,0 };

	// Wander (only enemies)
	bool isWanderSpawnTile = true;
};

struct TargetInfo
{
	TargetInfo();
	TargetInfo(const TargetInfo& t);
	~TargetInfo();

	bool isSightSatisfied = false; // if true, sight distance is satisfied
	bool isAttackSatisfied = false; // if true, attack distance is satisfied

	// Remove for targets that are in goals
	uint isInGoals = 0;
	bool isRemoveNeeded = false;

	Entity* target = nullptr;

	// Attacking tile for buildings
	iPoint attackingTile = { -1,-1 };

	// -----

	bool IsTargetDead() const;
	bool IsTargetValid() const;
};

class DynamicEntity :public Entity
{
public:

	DynamicEntity(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, j1Module* listener, bool isSingleUnit = true);
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
	uint GetDamage(Entity* target) const;
	UILifeBar* GetLifeBar() const;

	// State machine
	void UnitStateMachine(float dt);

	void SetUnitState(UnitState unitState);
	UnitState GetUnitState() const;

	// Movement
	SingleUnit* GetSingleUnit() const;
	PathPlanner* GetPathPlanner() const;

	void SetIsStill(bool isStill);
	bool IsStill() const;

	// Animations
	virtual void LoadAnimationsSpeed();
	virtual void UpdateAnimationsSpeed(float dt);
	virtual bool ChangeAnimation();

	// Direction
	void SetUnitDirection(UnitDirection unitDirection);
	UnitDirection GetUnitDirection() const;
	UnitDirection GetDirection(fPoint direction) const;

	void SetUnitDirectionByValue(fPoint unitDirection);
	fPoint GetUnitDirectionByValue() const;

	// Collision
	ColliderGroup* GetSightRadiusCollider() const;
	ColliderGroup* GetAttackRadiusCollider() const;

	ColliderGroup* CreateRhombusCollider(ColliderType colliderType, uint radius, DistanceHeuristic distanceHeuristic);
	void UpdateRhombusColliderPos(ColliderGroup* collider, uint radius, DistanceHeuristic distanceHeuristic);

	void HandleInput(EntitiesEvent &EntityEvent);
	bool MouseHover() const;

	// Attack
	/// Unit attacks a target
	list<TargetInfo*> GetTargets() const;
	list<TargetInfo*> GetTargetsToRemove() const;
	Entity* GetCurrTarget() const;

	bool SetCurrTarget(Entity* target);
	void InvalidateCurrTarget();

	bool UpdateTargetsToRemove();

	TargetInfo* GetBestTargetInfo(ENTITY_CATEGORY entityCategory = EntityCategory_NONE, ENTITY_TYPE entityType = EntityType_NONE, bool isCrittersCheck = true, bool isOnlyCritters = false) const; // TODO: add argument EntityType??? For critters vs enemies

	void SetHitting(bool isHitting);
	bool IsHitting() const;

	// Interact with the map
	void SetGoldMine(GoldMine* goldMine);
	GoldMine* GetGoldMine() const;
	void SetUnitGatheringGold(bool isGatheringGold);
	bool IsUnitGatheringGold() const;

	void SetRunestone(Runestone* runestone);
	Runestone* GetRunestone() const;
	void SetUnitHealingRunestone(bool isHealingRunestone);
	bool IsUnitHealingRunestone() const;

	void SetPrisoner(DynamicEntity* prisoner);
	DynamicEntity* GetPrisoner() const;
	void SetUnitRescuePrisoner(bool isRescuingPrisoner);
	bool IsUnitRescuingPrisoner() const;

	// Player commands
	bool SetUnitCommand(UnitCommand unitCommand);
	UnitCommand GetUnitCommand() const;

	// Blit
	void SetBlitState(bool isBlit);
	bool GetBlitState() const;

	// Minimap
	iPoint GetLastTile();

	// Fow
	void SetLastSeenTile(iPoint lastSeenTile);
	iPoint GetLastSeenTile() const;

	// Group selection
	void BlitSavedGroupSelection();
	void BlitSelectedGroupSelection();

public:

	const ENTITY_TYPE dynamicEntityType = EntityType_NONE;

	// Dead
	bool isDead = false; // if true, the unit is performing their dead animation

	// Spawn
	bool isSpawned = false;

	// Fow
	iPoint lastSeenTile = { -1,-1 };

	uint savedGroup = 0;

protected:

	UnitInfo unitInfo;
	UnitState unitState = UnitState_Idle;

	Animation* animation = nullptr;

	// Root of a bot's goal hierarchy
	Goal_Think* brain = nullptr;

	// Player commands
	UnitCommand unitCommand = UnitCommand_NoCommand;

	// Movement
	fPoint direction = { 0.0f,0.0f };

	SingleUnit* singleUnit = nullptr;
	PathPlanner* pathPlanner = nullptr;

	bool isStill = true; // if true, the unit is still. Else, the unit is moving

	// Attack
	list<TargetInfo*> targets;
	list<TargetInfo*> targetsToRemove;
	TargetInfo* currTarget = nullptr;
	TargetInfo* newTarget = nullptr;

	bool isHitting = false; // if true, the unit is hitting their target

	// Interact with the map
	GoldMine* goldMine = nullptr;
	bool isGatheringGold = false;

	Runestone* runestone = nullptr;
	bool isHealingRunestone = false;

	DynamicEntity* prisoner = nullptr;
	bool isRescuingPrisoner = false;

	// Collision
	ColliderGroup* sightRadiusCollider = nullptr;
	ColliderGroup* attackRadiusCollider = nullptr;
	iPoint lastColliderUpdateTile = { -1,-1 };

	// Death
	j1Timer deadTimer;

	// Blit
	bool isBlit = true;

	// LifeBar
	UILifeBar* lifeBar = nullptr;
	int lifeBarMarginX = 0;
	int lifeBarMarginY = 0;

	// Group selection
	bool isBlitSavedGroupSelection = false;
	bool isBlitSelectedGroupSelection = false;
	int alphaSavedGroupSelection = 0;
	int alphaSelectedGroupSelection = 0;
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