#ifndef __DynamicEntity_H__
#define __DynamicEntity_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1Timer.h"

#include "Entity.h"

#include <string>
#include <vector>
using namespace std;

struct SDL_Texture;
struct SDL_Color;

struct Collider;
struct ColliderGroup;
struct SingleUnit;

class PathPlanner;
class Navgraph;

enum ColliderType;
enum CollisionState;

enum UnitState
{
	UnitState_Idle,
	UnitState_Walk,
	UnitState_Attack,
	UnitState_Die
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

	// State machine
	void UnitStateMachine(float dt);

	void SetUnitState(UnitState unitState);
	UnitState GetUnitState() const;

	// Movement
	SingleUnit* GetSingleUnit() const;
	PathPlanner* GetPathPlanner() const;
	float GetSpeed() const;
	uint GetPriority() const;

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

	ColliderGroup* CreateRhombusCollider(ColliderType colliderType, uint radius);
	void UpdateRhombusColliderPos(ColliderGroup* collider, uint radius);

public:

	ENTITY_TYPE dynamicEntityType = EntityType_NONE;
	bool isSpawned = false;

protected:

	Animation* animation = nullptr;
	UnitState unitState = UnitState_Idle;

	// Movement
	UnitInfo unitInfo;
	fPoint direction = { 0.0f,0.0f };

	SingleUnit* singleUnit = nullptr;
	PathPlanner* pathPlanner = nullptr;
	Navgraph* navgraph = nullptr;

	// Attack
	bool isSightSatisfied = false;
	bool isAttackSatisfied = false;
	bool isAttacking = false;
	Entity* attackingTarget = nullptr;

	// Collision
	ColliderGroup* sightRadiusCollider = nullptr;
	ColliderGroup* attackRadiusCollider = nullptr;

	// Death
	bool isDead = false;
	j1Timer deadTimer;

	// Selection color
	SDL_Color color = ColorWhite;
	string colorName = "White";
};

#endif //__DynamicEntity_H__