#ifndef __j1MOVEMENT_H__
#define __j1MOVEMENT_H__

#include "j1Module.h"
#include "p2Point.h"

#include "j1Pathfinding.h"
#include "j1EntityFactory.h"

#include <list>
#include <vector>
using namespace std;

enum MovementState {

	MovementState_NoState,
	MovementState_WaitForPath,
	MovementState_FollowPath,
	MovementState_GoalReached,
	MovementState_IncreaseWaypoint
};

enum CollisionType {

	CollisionType_NoCollision,
	CollisionType_SameCell,
	CollisionType_TowardsCell,
	CollisionType_ItsCell,
	CollisionType_DiagonalCrossing
};

// forward declaration
struct SingleUnit;
struct UnitGroup;

class Entity;
class DynamicEntity;

class j1Movement : public j1Module
{
public:

	j1Movement();

	// Destructor
	~j1Movement();

	bool Update(float dt);

	void DebugDraw() const;

	// Called before quitting
	bool CleanUp();

	// Creates a group from a list of units. Returns the pointer to the group created or nullptr
	UnitGroup* CreateGroupFromUnits(list<DynamicEntity*> units);

	// Creates a group from a single unit. Returns the pointer to the group created or nullptr
	UnitGroup* CreateGroupFromUnit(DynamicEntity* unit);

	// Returns the last group created or nullptr
	UnitGroup* GetLastGroup() const;

	// Returns an existing group by a pointer to one of its units or nullptr
	UnitGroup* GetGroupByUnit(DynamicEntity* unit) const;

	// Returns an existing group by a list to all of its units or nullptr
	UnitGroup* GetGroupByUnits(list<DynamicEntity*> units) const;

	// Moves a unit applying the principles of group movement. Returns the state of the unit's movement
	/// Call this method from a unit's update
	MovementState MoveUnit(DynamicEntity* unit, float dt); /// Not const because it needs to keep track of the number of paths created at the current update

	// -----

	// Checks for future collisions between the current unit and the rest of the units
	/// Sets the collision of the unit with the type of collision found or CollisionType_NoCollision
	void CheckForFutureCollision(SingleUnit* singleUnit) const;

	// Returns true if a tile is valid
	/// Checks for all the units (except for the unit passed as an argument) if a unit's tile (currTile, nextTile or goalTile) matches the tile passed as an argument
	bool IsValidTile(SingleUnit* singleUnit, iPoint tile, bool currTile = false, bool nextTile = false, bool goalTile = false) const;

	// Returns a new valid tile for the unit or { -1,-1 }
	/// If checkOnlyFront is true, it only checks the three tiles that are in front of the unit passed as an argument
	/// If checkOnlyFront is false, it checks the eight tiles surrounding the unit passed as an argument
	/// The new tile is searched using a Priority Queue containing the neighbors of the current tile of the unit passed as an argument
	iPoint FindNewValidTile(SingleUnit* singleUnit, bool checkOnlyFront = false) const;

	// Returns true if two units are heading towards opposite directions
	bool IsOppositeDirection(SingleUnit* singleUnitA, SingleUnit* singleUnitB) const;

	// Returns true if another unit has any of the booleans passed as arguments to true
	bool IsAnyUnitDoingSomething(SingleUnit* singleUnit, bool isSearching = false) const;

	bool IsNeighborTile(iPoint tile, iPoint neighbor);

private:

	list<UnitGroup*> unitGroups; // contains all the existing groups
};

// ---------------------------------------------------------------------
// UnitGroup: struct representing a group of units
// ---------------------------------------------------------------------

struct UnitGroup
{
	UnitGroup(DynamicEntity* unit);

	UnitGroup(list<DynamicEntity*> units);

	~UnitGroup();

	// Adds a singleUnit (unit) to the group. Returns false if the singleUnit was already in the group
	bool AddUnit(SingleUnit* singleUnit);

	// Removes a singleUnit (unit) from the group. Returns true if success
	bool RemoveUnit(SingleUnit* singleUnit);

	// Returns true if the singleUnit (unit) is in the group
	bool IsUnitInGroup(SingleUnit* singleUnit) const;

	// Returns the size of the group (the number of singleUnits in the group)
	uint GetSize() const;

	// Sets the destination tile (goal) of the group
	bool SetGoal(iPoint goal);

	// Returns the destination tile (goal) of the group
	iPoint GetGoal() const;

	bool DrawShapedGoal(iPoint mouseTile);

	bool SetShapedGoal();

	// -----

	list<SingleUnit*> units; // contains all the units of a given group
	iPoint goal = { -1,-1 }; // current goal of the group

	vector<iPoint> shapedGoal;	
	bool isShapedGoal = false;
};

// ---------------------------------------------------------------------
// SingleUnit: struct representing a single unit
// ---------------------------------------------------------------------

struct SingleUnit
{
	SingleUnit(DynamicEntity* entity, UnitGroup* group);

	// Returns true if the unit would reach its next tile during this move
	/// nextPos is the next tile that the unit is heading to
	/// endPos is the tile that the unit would reach during this move
	bool IsTileReached(iPoint nextPos, fPoint endPos) const;

	// Stops the unit
	void StopUnit();

	// Resets the parameters of the unit (general info)
	void ResetUnitParameters();

	// Resets the collision parameters of the unit
	void ResetUnitCollisionParameters();

	// When detected a collision, to set the collision parameters of the unit
	void SetCollisionParameters(CollisionType collisionType, SingleUnit* waitUnit, iPoint waitTile);

	// Prepares the unit for its next movement cycle
	void GetReadyForNewMove();

	// Sets the state of the unit to UnitState_Walk
	void WakeUp();

	// -----

	DynamicEntity* unit = nullptr;
	UnitGroup* group = nullptr;
	MovementState movementState = MovementState_NoState;
	bool wakeUp = false; // sets a unit's unitState to UnitState_Walk

	vector<iPoint> path; // path to the unit's goal
	iPoint currTile = { -1,-1 }; // position of the unit in map coords
	iPoint nextTile = { -1,-1 }; // next waypoint of the path (next tile the unit is heading to in map coords)

	iPoint goal = { -1,-1 }; // goal of the unit
	iPoint roomGoal{ -1,-1 };
	iPoint shapedGoal = { -1,-1 };
	bool isGoalChanged = false; // if true, it means that the goal has been changed
	bool isSearching = false; // if true, it means that the unit is searching a tile using Dijkstra
	bool isGoalNeeded = false;

	bool reversePriority = false; // if true, the priority of the unit is not taken into account

	// COLLISION AVOIDANCE
	bool wait = false;
	/// If a unit is not in the UnitState_Walk and another unit needs this unit to move away, set wakeUp to true
	iPoint waitTile = { -1,-1 }; // conflict tile (tile where the collision has been found)
	SingleUnit* waitUnit = nullptr; // conflict unit (unit whom the collision has been found with)
	CollisionType coll = CollisionType_NoCollision; // type of collision
};

// ---------------------------------------------------------------------
// Helper class to establish a priority to an iPoint
// ---------------------------------------------------------------------
class iPointPriority
{
public:
	iPointPriority() {}
	iPointPriority(iPoint point, int priority) :point(point), priority(priority) {}
	iPointPriority(const iPointPriority& i)
	{
		point = i.point;
		priority = i.priority;
	}

	iPoint point = { 0,0 };
	uint priority = 0;
};

// ---------------------------------------------------------------------
// Helper class to compare two iPoints by its priority values
// ---------------------------------------------------------------------
class Comparator
{
public:
	int operator() (const iPointPriority a, const iPointPriority b)
	{
		return a.priority > b.priority;
	}
};

#endif //__j1MOVEMENT_H__