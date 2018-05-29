#ifndef __GOAL_H__
#define __GOAL_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Defs.h"

#include <queue>
#include <list>
#include <algorithm>
using namespace std;

class Entity;
class DynamicEntity;
class GoldMine;
class Runestone;
class Alleria;
class Turalyon;

struct TargetInfo;
struct Particle;

enum UnitDirection;

enum GoalType {

	GoalType_NoType,

	// Composite Goals
	GoalType_Think,
	GoalType_AttackTarget,
	GoalType_Wander,
	GoalType_Patrol,
	GoalType_GatherGold,
	GoalType_HealRunestone,

	// Atomic Goals
	GoalType_MoveToPosition,
	GoalType_HitTarget,
	GoalType_LookAround,
	GoalType_PickNugget,
	GoalType_HealArea,

	GoalType_MaxTypes,

	// Composite Goals
	/// Goal_Think

	/// Goal_GetItem
	/// Goal_Patrol

	/// Goal_MoveToPosition
	/// Goal_FollowPath

	/// Goal_HuntTarget
	/// Goal_AttackTarget

	// Atomic Goals
	/// Goal_Wander
};

enum GoalStatus {

	GoalStatus_Inactive, // the goal is waiting to be activated
	GoalStatus_Active, // the goal has been activated and will be processed each update step
	GoalStatus_Completed, // the goal has completed and will be removed on the next update
	GoalStatus_Failed // the goal has failed and will either replan or be removed on the next update
};

class Goal
{
public:

	Goal(DynamicEntity* owner, GoalType goalType);
	~Goal();

	// Contains initialization logic (planning phase of the goal)
	/// It can be called any number of times to replan
	virtual void Activate();

	// It is executed each update step
	virtual GoalStatus Process(float dt);

	// Undertakes any necessary tidying up before a goal is exited
	/// It is called just before a goal is destroyed
	virtual void Terminate();

	virtual void AddSubgoal(Goal* goal);

	// -----

	void ActivateIfInactive();

	bool IsActive() const;
	bool IsInactive() const;
	bool IsCompleted() const;
	bool HasFailed() const;

	GoalType GetType() const;

protected:

	DynamicEntity* owner = nullptr;
	GoalStatus goalStatus = GoalStatus_Inactive;
	GoalType goalType = GoalType_NoType;
};

// Atomic Goal: it cannot aggregate child goals
// Composite Goal: it can aggregate child goals

class AtomicGoal :public Goal
{
public:

	AtomicGoal(DynamicEntity* owner, GoalType goalType);

	virtual void Activate();
	virtual GoalStatus Process(float dt);
	virtual void Terminate();
};

class CompositeGoal :public Goal
{
public:

	CompositeGoal(DynamicEntity* owner, GoalType goalType);

	virtual void Activate();
	virtual GoalStatus Process(float dt);
	virtual void Terminate();

	// -----

	void AddSubgoal(Goal* goal);
	list<Goal*> GetSubgoalsList() const;

	// It is called each update step to process the subgoals
	// It ensures that all completed and failed goals are removed from the list before
	// processing the next subgoal in line and returning its status
	// If the subgoal list is empty, 'completed' is returned
	GoalStatus ProcessSubgoals(float dt);

	void ReactivateIfFailed();

	// Clears the subgoals list
	// It ensures that all subgoals are destroyed cleanly by calling each one's 'Terminate' method
	// before deletion
	void RemoveAllSubgoals();

protected:

	list<Goal*> subgoals;
};

// Composite Goals ---------------------------------------------------------------------

class Goal_Think :public CompositeGoal
{
public:

	Goal_Think(DynamicEntity* owner);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

	// Arbitrate between available strategies, choosing the most appropriate
	// to be pursued. Calculate the desirability of the strategies
	//void Arbitrate();
	void AddGoal_Wander(uint maxDistance, iPoint startTile, bool isCurrTile, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted);
	void AddGoal_AttackTarget(TargetInfo* targetInfo, bool isStateChanged = true);
	void AddGoal_MoveToPosition(iPoint destinationTile, bool isStateChanged = true, bool isShapedGoalIgnored = true);
	void AddGoal_Patrol(iPoint originTile, iPoint destinationTile, bool isLookAround = false);
	void AddGoal_GatherGold(GoldMine* goldMine);
	void AddGoal_HealRunestone(Runestone* runestone);
	void AddGoal_RescuePrisoner(DynamicEntity* prisoner);
	void AddGoal_LookAround(uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted);
};

class Goal_AttackTarget :public CompositeGoal
{
public:

	Goal_AttackTarget(DynamicEntity* owner, TargetInfo* targetInfo, bool isStateChanged = true);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	TargetInfo* targetInfo = nullptr;

	bool isStateChanged = true;

	// Time enemies chase player units
	j1Timer chaseTimer;
	float chaseTime = 0.0f;
};

class Goal_Patrol :public CompositeGoal
{
public:

	Goal_Patrol(DynamicEntity* owner, iPoint originTile, iPoint destinationTile, bool isLookAround = false);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	iPoint originTile = { -1,-1 }; // the position from which the bot starts
	iPoint destinationTile = { -1,-1 }; // the position the bot wants to reach

	iPoint currGoal = { -1,-1 };

	bool isLookAround = false;
};

class Goal_Wander :public CompositeGoal
{
public:

	Goal_Wander(DynamicEntity* owner, uint maxDistance, iPoint startTile, bool isCurrTile, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	uint maxDistance = 0;
	iPoint startTile = { -1,-1 };
	bool isCurrTile = false;

	// Goal_LookAround
	uint minSecondsToChange = 0;
	uint maxSecondsToChange = 0;
	uint minSecondsUntilNextChange = 0;
	uint maxSecondsUntilNextChange = 0;
	uint probabilityGoalCompleted = 0;
};

class Goal_GatherGold :public CompositeGoal
{
public:

	Goal_GatherGold(DynamicEntity* owner, GoldMine* goldMine);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	GoldMine* goldMine = nullptr;
};

class Goal_HealRunestone :public CompositeGoal
{
public:

	Goal_HealRunestone(DynamicEntity* owner, Runestone* runestone);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	Runestone* runestone = nullptr;
};

class Goal_RescuePrisoner :public CompositeGoal
{
public:

	Goal_RescuePrisoner(DynamicEntity* owner, DynamicEntity* prisoner);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	DynamicEntity* prisoner = nullptr;
};

// Atomic Goals ---------------------------------------------------------------------

class Goal_MoveToPosition :public AtomicGoal
{
public:

	Goal_MoveToPosition(DynamicEntity* owner, iPoint destinationTile, bool isStateChanged = true, bool isShapedGoalIgnored = true);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	iPoint destinationTile = { -1,-1 }; // the position the bot wants to reach

	bool isStateChanged = true;
	bool isShapedGoalIgnored = true;
};

class Goal_HitTarget :public AtomicGoal
{
public:

	Goal_HitTarget(DynamicEntity* owner, TargetInfo* target, bool isStateChanged = true);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	TargetInfo* targetInfo = nullptr;
	fPoint orientation = { 0,0 };

	bool isStateChanged = true;
};

class Goal_LookAround :public AtomicGoal
{
public:

	Goal_LookAround(DynamicEntity* owner, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	UnitDirection nextOrientation;
	float secondsToChange = 0.0f;
	float secondsUntilNextChange = 0.0f;

	uint minSecondsToChange = 0;
	uint maxSecondsToChange = 0;
	uint minSecondsUntilNextChange = 0;
	uint maxSecondsUntilNextChange = 0;
	uint probabilityGoalCompleted = 0;
	bool isChanged = false;

	j1Timer timer;
};

class Goal_PickNugget :public AtomicGoal
{
public:

	Goal_PickNugget(DynamicEntity* owner, GoldMine* goldMine);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	GoldMine* goldMine = nullptr;

	uint gold = 0;
	float secondsGathering = 0.0f;
	j1Timer timerGathering;

	double msAnimation = 0.0f;
	j1PerfTimer timerAnimation;

	//Varibles of the process of gathering gold
	uint timeForEachGoldUpdate = 100; //Is in ms!
	j1Timer auxGatherTimer;
	uint goldGathered = 0;
	uint goldToAdd = 0;
};

class Goal_HealArea :public AtomicGoal
{
public:

	Goal_HealArea(DynamicEntity* owner, Runestone* runestone);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	Runestone* runestone = nullptr;

	uint health = 0;

	float secondsGathering = 0.0f;
	j1Timer timerGathering;

	double msAnimation = 0.0f;
	j1PerfTimer timerAnimation;

	int alpha = 0;
};

class Goal_FreePrisoner :public AtomicGoal
{
public:

	Goal_FreePrisoner(DynamicEntity* owner, DynamicEntity* prisoner);

	void Activate();
	GoalStatus Process(float dt);
	void Terminate();

private:

	DynamicEntity* prisoner = nullptr;

	Alleria* alleria = nullptr;
	Turalyon* turalyon = nullptr;
};

#endif //__GOAL_H__