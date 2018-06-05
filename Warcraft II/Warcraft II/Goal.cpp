#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "Goal.h"
#include "Entity.h"
#include "DynamicEntity.h"
#include "j1Map.h"
#include "j1PathManager.h"
#include "j1Movement.h"
#include "j1EntityFactory.h"
#include "j1Particles.h"
#include "j1Gui.h"
#include "UILifeBar.h"

#include "j1Player.h"
#include "j1Scene.h"

#include "Brofiler\Brofiler.h"

Goal::Goal(DynamicEntity* owner, GoalType goalType) :owner(owner), goalType(goalType) {}

Goal::~Goal() {}

void Goal::Activate() {}

GoalStatus Goal::Process(float dt) { return goalStatus; }

void Goal::Terminate() {}

void Goal::AddSubgoal(Goal* goal) {}

void Goal::ActivateIfInactive()
{
	if (IsInactive())
		Activate();
}

bool Goal::IsActive() const
{
	return goalStatus == GoalStatus_Active;
}

bool Goal::IsInactive() const
{
	return goalStatus == GoalStatus_Inactive;
}

bool Goal::IsCompleted() const
{
	return goalStatus == GoalStatus_Completed;
}

bool Goal::HasFailed() const
{
	return goalStatus == GoalStatus_Failed;
}

GoalType Goal::GetType() const
{
	return goalType;
}

// AtomicGoal ---------------------------------------------------------------------

AtomicGoal::AtomicGoal(DynamicEntity* owner, GoalType goalType) :Goal(owner, goalType) {}

void AtomicGoal::Activate() {}

GoalStatus AtomicGoal::Process(float dt) { return goalStatus; }

void AtomicGoal::Terminate() {}

// CompositeGoal ---------------------------------------------------------------------

CompositeGoal::CompositeGoal(DynamicEntity* owner, GoalType goalType) : Goal(owner, goalType) {}

void CompositeGoal::Activate() {}

GoalStatus CompositeGoal::Process(float dt) { return goalStatus; }

void CompositeGoal::Terminate() {}

void CompositeGoal::AddSubgoal(Goal* goal)
{
	subgoals.push_front(goal);
}

list<Goal*> CompositeGoal::GetSubgoalsList() const 
{
	return subgoals;
}

GoalStatus CompositeGoal::ProcessSubgoals(float dt)
{
	// Remove all completed and failed goals from the front of the subgoal list
	while (subgoals.size() > 0
		&& (subgoals.front()->IsCompleted() || subgoals.front()->HasFailed())) {

		subgoals.front()->Terminate();
		delete subgoals.front();
		subgoals.pop_front();
	}

	// If any subgoals remain, process the one at the front of the list
	if (subgoals.size() > 0) {

		// Grab the status of the frontmost subgoal
		GoalStatus subgoalsStatus = subgoals.front()->Process(dt);

		// SPECIAL CASE: the frontmost subgoal reports 'completed' and the subgoal list
		// contains additional goals. To ensure the parent keeps processing its subgoal list,
		// the 'active' status is returned
		if (subgoalsStatus == GoalStatus_Completed && subgoals.size() > 1)
			return GoalStatus_Active;

		return subgoalsStatus;
	}
	else

		// No more subgoals to process. Return 'completed'
		return GoalStatus_Completed;
}

void CompositeGoal::ReactivateIfFailed()
{
	if (HasFailed())
		Activate();
}

void CompositeGoal::RemoveAllSubgoals()
{
	for (list<Goal*>::const_iterator it = subgoals.begin(); it != subgoals.end(); ++it) {

		(*it)->Terminate();
		delete *it;
	}

	subgoals.clear();
}

// COMPOSITE GOALS
// Goal_Think ---------------------------------------------------------------------

Goal_Think::Goal_Think(DynamicEntity* owner) :CompositeGoal(owner, GoalType_Think) {}

void Goal_Think::Activate()
{
	goalStatus = GoalStatus_Active;

	// If this goal is reactivated then there may be some existing subgoals that
	// must be removed
	RemoveAllSubgoals();

	// Initialize the goal
	// TODO: Add some code here

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

GoalStatus Goal_Think::Process(float dt)
{
	// Process the subgoals
	ProcessSubgoals(dt);

	// If any of the subgoals have failed then this goal replans
	ReactivateIfFailed();

	return goalStatus;
}

void Goal_Think::Terminate()
{
	// Switch the goal off
	// TODO: Add some code here

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

void Goal_Think::AddGoal_Wander(uint maxDistance, iPoint startTile, bool isCurrTile, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted)
{
	AddSubgoal(new Goal_Wander(owner, maxDistance, startTile, isCurrTile, minSecondsToChange, maxSecondsToChange, minSecondsUntilNextChange, maxSecondsUntilNextChange, probabilityGoalCompleted));
}

void Goal_Think::AddGoal_AttackTarget(TargetInfo* targetInfo, bool isStateChanged)
{
	AddSubgoal(new Goal_AttackTarget(owner, targetInfo, isStateChanged));
}

void Goal_Think::AddGoal_MoveToPosition(iPoint destinationTile, bool isStateChanged)
{
	AddSubgoal(new Goal_MoveToPosition(owner, destinationTile, isStateChanged));
}

void Goal_Think::AddGoal_Patrol(iPoint originTile, iPoint destinationTile, bool isLookAround)
{
	AddSubgoal(new Goal_Patrol(owner, originTile, destinationTile, isLookAround));
}

void Goal_Think::AddGoal_GatherGold(GoldMine* goldMine)
{
	AddSubgoal(new Goal_GatherGold(owner, goldMine));
}

void Goal_Think::AddGoal_HealRunestone(Runestone* runestone)
{
	AddSubgoal(new Goal_HealRunestone(owner, runestone));
}

void Goal_Think::AddGoal_RescuePrisoner(DynamicEntity* prisoner)
{
	AddSubgoal(new Goal_RescuePrisoner(owner, prisoner));
}

void Goal_Think::AddGoal_LookAround(uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted)
{
	AddSubgoal(new Goal_LookAround(owner, minSecondsToChange, maxSecondsToChange, minSecondsUntilNextChange, maxSecondsUntilNextChange, probabilityGoalCompleted));
}

// Goal_AttackTarget ---------------------------------------------------------------------

Goal_AttackTarget::Goal_AttackTarget(DynamicEntity* owner, TargetInfo* targetInfo, bool isStateChanged) :CompositeGoal(owner, GoalType_AttackTarget), targetInfo(targetInfo), isStateChanged(isStateChanged) 
{
	(this->targetInfo)->isInGoals++; // THE TARGET IS A GOAL
}

void Goal_AttackTarget::Activate()
{
	goalStatus = GoalStatus_Active;

	RemoveAllSubgoals();

	owner->SetHitting(false);
	owner->SetIsStill(true);

	if (targetInfo->isRemoveNeeded) {

		/// The target needs to be removed because, for example, is no longer within the sight of the unit (ordered from outside the goals)
		goalStatus = GoalStatus_Completed;
		return;
	}
	else if (targetInfo->IsTargetDead() || !targetInfo->IsTargetValid()) {

		/// The target has recently died || The target has recently become invalid
		goalStatus = GoalStatus_Completed;
		return;
	}

	// -----

	AddSubgoal(new Goal_HitTarget(owner, targetInfo, isStateChanged));

	iPoint targetTile = App->map->WorldToMap(targetInfo->target->GetPos().x, targetInfo->target->GetPos().y);

	// If the target is far from the unit, head directly at the target's position
	if (!targetInfo->isAttackSatisfied) {

		if (targetInfo->target->entityType == EntityCategory_STATIC_ENTITY) {

			StaticEntity* building = (StaticEntity*)targetInfo->target;
			targetTile = { -1,-1 };

			// Option a: the unit searches for a free surrounding tile of the building
			list<iPoint> buildingSurroundingTiles = App->entities->GetBuildingTiles(building, true);
			priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
			iPointPriority priorityNeighbors;

			list<iPoint>::const_iterator it = buildingSurroundingTiles.begin();
			while (it != buildingSurroundingTiles.end()) {

				if (App->pathfinding->IsWalkable(*it) && App->movement->IsValidTile(owner->GetSingleUnit(), *it, false, false, true)) {

					priorityNeighbors.point = *it;
					priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
					queue.push(priorityNeighbors);
				}
				it++;
			}

			if (!queue.empty())
				targetTile = queue.top().point;
			//_Option_a

			// Option b: if all the surrounding tiles of the building are either occupied by other units or unwalkable, the unit searches for the closest valid tile to the building
			if (targetTile.x == -1 && targetTile.y == -1) {

				list<iPoint> buildingTiles = App->entities->GetBuildingTiles(building);
				priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
				iPointPriority priorityNeighbors;

				list<iPoint>::const_iterator it = buildingTiles.begin();
				while (it != buildingTiles.end()) {

					priorityNeighbors.point = *it;
					priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
					queue.push(priorityNeighbors);

					it++;
				}

				targetTile = queue.top().point;

				targetTile = App->movement->FindClosestValidTile(targetTile, owner);
			}
			//_Option_b

			if (targetTile.x == -1 || targetTile.y == -1) {

				goalStatus = GoalStatus_Failed;
				return;
			}

			owner->GetSingleUnit()->SetGoal(targetTile);
		}

		AddSubgoal(new Goal_MoveToPosition(owner, targetTile, isStateChanged));
	}

	// The target is being attacked by this unit
	targetInfo->target->AddAttackingUnit(owner);

	if (isStateChanged && !owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_AttackTarget);

	// Time enemies chase player units
	chaseTimer.Start();
	chaseTime = 6.0f;

	// ----- The owner may have lost their currTarget because of the processing order of the AttackTarget goals

	if (!targetInfo->isRemoveNeeded)
		owner->SetCurrTarget(targetInfo->target);
	else
		owner->InvalidateCurrTarget();
}

GoalStatus Goal_AttackTarget::Process(float dt)
{
	ActivateIfInactive();

	if (targetInfo->isRemoveNeeded) {
	
		/// The target needs to be removed because, for example, is no longer within the sight of the unit (ordered from outside the goals)
		if (owner->GetSingleUnit()->IsFittingTile()) {

			goalStatus = GoalStatus_Completed;
			return goalStatus;
		}
	}
	else if (!targetInfo->IsTargetValid()) {

		/// The target has recently become invalid
		if (owner->GetSingleUnit()->IsFittingTile()) {

			goalStatus = GoalStatus_Completed;
			return goalStatus;
		}
	}
	else if (subgoals.size() > 0 && subgoals.front()->GetType() == GoalType_MoveToPosition) {

		// If DistanceManhattan is <= 1 -> isAttackSatisfied = true
		if (!targetInfo->isAttackSatisfied) {

			if (targetInfo->target->entityType == EntityCategory_STATIC_ENTITY) {

				list<iPoint> buildingTiles = App->entities->GetBuildingTiles((StaticEntity*)targetInfo->target);

				if (buildingTiles.size() > 0) {

					list<iPoint>::const_iterator it = buildingTiles.begin();

					while (it != buildingTiles.end()) {

						if (owner->GetSingleUnit()->currTile.DistanceTo(*it) <= 1) {

							targetInfo->isAttackSatisfied = true;
							isStaticAttackForced = true;
							break;
						}
						it++;
					}
				}
			}
			else if (targetInfo->target->entityType == EntityCategory_DYNAMIC_ENTITY) {
			
				DynamicEntity* dynEnt = (DynamicEntity*)targetInfo->target;
				SingleUnit* singleUnit = dynEnt->GetSingleUnit();

				if (singleUnit != nullptr) {

					if (owner->GetSingleUnit()->currTile.DistanceTo(singleUnit->currTile) <= 1) {

						targetInfo->isAttackSatisfied = true;
						isDynamicAttackForced = true;
					}
				}
			}
		}

		// The unit was chasing their target, but the attack distance has been suddenly satisfied
		if (targetInfo->isAttackSatisfied) {

			if (owner->GetSingleUnit()->IsFittingTile()) {

				subgoals.front()->Terminate();
				delete subgoals.front();
				subgoals.pop_front();
			}
		}
	}

	// Process the subgoals
	goalStatus = ProcessSubgoals(dt);

	ReactivateIfFailed();

	// ----- The owner may have lost their currTarget because of the processing order of the AttackTarget goals

	if (!targetInfo->isRemoveNeeded)
		owner->SetCurrTarget(targetInfo->target);
	else
		owner->InvalidateCurrTarget();

	if (isStateChanged && owner->GetUnitState() != UnitState_AttackTarget && !owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_AttackTarget);

	return goalStatus;
}

void Goal_AttackTarget::Terminate()
{
	RemoveAllSubgoals();

	owner->SetHitting(false);
	owner->SetIsStill(true);

	owner->InvalidateCurrTarget();

	if (!App->entities->isEntityFactoryCleanUp) {

		if (targetInfo->isRemoveNeeded) {

			/// The target needs to be removed because, for example, is no longer within the sight of the unit (ordered from outside the goals)
		}
		else if (targetInfo->IsTargetDead() || !targetInfo->IsTargetValid()) {

			/// The target has recently died || The target has recently become invalid

			// Removing target process --
			if (!targetInfo->isRemoveNeeded) {

				targetInfo->isRemoveNeeded = true;
				list<TargetInfo*> targets = owner->GetTargets();
				list<TargetInfo*> targetsToRemove = owner->GetTargetsToRemove();

				list<TargetInfo*>::const_iterator it = find(targets.begin(), targets.end(), targetInfo);
				targetsToRemove.splice(targetsToRemove.begin(), targets, it);
			}
			// -- Removing target process
		}
		else {

			targetInfo->target->RemoveAttackingUnit(owner);

			// If DistanceManhattan is <= 1 -> isAttackSatisfied = false
			if (targetInfo->isAttackSatisfied) {

				if (targetInfo->target->entityType == EntityCategory_STATIC_ENTITY) {

					if (isStaticAttackForced)
						targetInfo->isAttackSatisfied = false;
				}
				else if (targetInfo->target->entityType == EntityCategory_DYNAMIC_ENTITY) {

					if (isDynamicAttackForced)
						targetInfo->isAttackSatisfied = false;
				}
			}
		}

		targetInfo->isInGoals--; // THE TARGET IS NO LONGER A GOAL
	}

	// -----

	targetInfo = nullptr;
	chaseTime = 0.0f;

	if (isStateChanged)
		owner->SetUnitState(UnitState_AttackTarget);
}

// Goal_Patrol ---------------------------------------------------------------------

Goal_Patrol::Goal_Patrol(DynamicEntity* owner, iPoint originTile, iPoint destinationTile, bool isLookAround) :CompositeGoal(owner, GoalType_Patrol), originTile(originTile), destinationTile(destinationTile), isLookAround(isLookAround) {}

void Goal_Patrol::Activate()
{
	goalStatus = GoalStatus_Active;

	RemoveAllSubgoals();

	if (isLookAround)

		AddSubgoal(new Goal_LookAround(owner, 1, 3, 1, 2, 2));

	if (currGoal == destinationTile)
		currGoal = originTile;
	else
		currGoal = destinationTile;

	AddSubgoal(new Goal_MoveToPosition(owner, currGoal));

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Patrol);
}

GoalStatus Goal_Patrol::Process(float dt)
{
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)
		return goalStatus;

	goalStatus = ProcessSubgoals(dt);

	if (goalStatus == GoalStatus_Completed)
		Activate();

	ReactivateIfFailed();

	return goalStatus;
}

void Goal_Patrol::Terminate()
{
	RemoveAllSubgoals();

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_Wander ---------------------------------------------------------------------

Goal_Wander::Goal_Wander(DynamicEntity* owner, uint maxDistance, iPoint startTile, bool isCurrTile, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted) :CompositeGoal(owner, GoalType_Wander), maxDistance(maxDistance), startTile(startTile), isCurrTile(isCurrTile), minSecondsToChange(minSecondsToChange), maxSecondsToChange(maxSecondsToChange), minSecondsUntilNextChange(minSecondsUntilNextChange), maxSecondsUntilNextChange(maxSecondsUntilNextChange), probabilityGoalCompleted(probabilityGoalCompleted) {}

void Goal_Wander::Activate()
{
	goalStatus = GoalStatus_Active;

	AddSubgoal(new Goal_LookAround(owner, minSecondsToChange, maxSecondsToChange, minSecondsUntilNextChange, maxSecondsUntilNextChange, probabilityGoalCompleted));

	iPoint destinationTile = { -1,-1 };

	int sign = rand() % 2;
	if (sign == 0)
		sign = -1;

	if (isCurrTile)
		destinationTile.x = owner->GetSingleUnit()->currTile.x + sign * (rand() % (maxDistance + 1) + 1);
	else
		destinationTile.x = startTile.x + sign * (rand() % (maxDistance + 1) + 1);

	sign = rand() % 2;
	if (sign == 0)
		sign = -1;

	if (isCurrTile)
		destinationTile.y = owner->GetSingleUnit()->currTile.y + sign * (rand() % (maxDistance + 1) + 1);
	else
		destinationTile.y = startTile.y + sign * (rand() % (maxDistance + 1) + 1);

	AddSubgoal(new Goal_MoveToPosition(owner, destinationTile));

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Wander);
}

GoalStatus Goal_Wander::Process(float dt)
{
	ActivateIfInactive();

	goalStatus = ProcessSubgoals(dt);

	// Wander is performed in an infinite loop
	if (goalStatus == GoalStatus_Completed)

		Activate();

	if (goalStatus == GoalStatus_Failed) {

		RemoveAllSubgoals();
		Activate();
	}

	return goalStatus;
}

void Goal_Wander::Terminate()
{
	RemoveAllSubgoals();

	maxDistance = 0;
	startTile = { -1,-1 };
	isCurrTile = false;

	// Goal_LookAround
	minSecondsToChange = 0;
	maxSecondsToChange = 0;
	minSecondsUntilNextChange = 0;
	maxSecondsUntilNextChange = 0;
	probabilityGoalCompleted = 0;

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_GatherGold ---------------------------------------------------------------------

Goal_GatherGold::Goal_GatherGold(DynamicEntity* owner, GoldMine* goldMine) :CompositeGoal(owner, GoalType_GatherGold), goldMine(goldMine) {}

void Goal_GatherGold::Activate()
{
	goalStatus = GoalStatus_Active;

	if (goldMine == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}
	else if (goldMine->buildingState == BuildingState_Destroyed) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	// 2. Pick Nugget
	AddSubgoal(new Goal_PickNugget(owner, goldMine));

	// 1. Move near the Gold Mine
	iPoint goldMineTile = { -1,-1 };

	// Option a: the unit searches for a free surrounding tile of the building
	list<iPoint> buildingSurroundingTiles = App->entities->GetBuildingTiles(goldMine, true);
	priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
	iPointPriority priorityNeighbors;

	list<iPoint>::const_iterator it = buildingSurroundingTiles.begin();
	while (it != buildingSurroundingTiles.end()) {
	
		if (App->pathfinding->IsWalkable(*it) && App->movement->IsValidTile(owner->GetSingleUnit(), *it, false, false, true)) {

			priorityNeighbors.point = *it;
			priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
			queue.push(priorityNeighbors);
		}
		it++;
	}

	if (!queue.empty())
		goldMineTile = queue.top().point;
	//_Option_a

	// Option b: if all the surrounding tiles of the building are either occupied by other units or unwalkable, the unit searches for the closest valid tile to the building
	if (goldMineTile.x == -1 && goldMineTile.y == -1) {

		list<iPoint> buildingTiles = App->entities->GetBuildingTiles(goldMine);
		priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
		iPointPriority priorityNeighbors;

		list<iPoint>::const_iterator it = buildingTiles.begin();
		while (it != buildingTiles.end()) {

			priorityNeighbors.point = *it;
			priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
			queue.push(priorityNeighbors);

			it++;
		}

		goldMineTile = queue.top().point;

		goldMineTile = App->movement->FindClosestValidTile(goldMineTile, owner);
	}
	//_Option_b

	if (goldMineTile.x == -1 || goldMineTile.y == -1) {

		goalStatus = GoalStatus_Failed;
		return;
	}

	AddSubgoal(new Goal_MoveToPosition(owner, goldMineTile));
}

GoalStatus Goal_GatherGold::Process(float dt)
{
	ActivateIfInactive();

	goalStatus = ProcessSubgoals(dt);

	return goalStatus;
}

void Goal_GatherGold::Terminate()
{
	RemoveAllSubgoals();

	goldMine = nullptr;

	//owner->SetGoldMine(nullptr);
	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_HealRunestone ---------------------------------------------------------------------

Goal_HealRunestone::Goal_HealRunestone(DynamicEntity* owner, Runestone* runestone) :CompositeGoal(owner, GoalType_HealRunestone), runestone(runestone) {}

void Goal_HealRunestone::Activate()
{
	goalStatus = GoalStatus_Active;

	if (runestone == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}
	else if (runestone->buildingState == BuildingState_Destroyed) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	// 2. Heal Area
	AddSubgoal(new Goal_HealArea(owner, runestone));

	// 1. Move near the Runestone
	iPoint runestoneTile = { -1,-1 };

	// Option a: the unit searches for a free surrounding tile of the building
	list<iPoint> buildingSurroundingTiles = App->entities->GetBuildingTiles(runestone, true);
	priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
	iPointPriority priorityNeighbors;

	list<iPoint>::const_iterator it = buildingSurroundingTiles.begin();
	while (it != buildingSurroundingTiles.end()) {

		if (App->pathfinding->IsWalkable(*it) && App->movement->IsValidTile(owner->GetSingleUnit(), *it, false, false, true)) {

			priorityNeighbors.point = *it;
			priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
			queue.push(priorityNeighbors);
		}
		it++;
	}

	if (!queue.empty())
		runestoneTile = queue.top().point;
	//_Option_a

	// Option b: if all the surrounding tiles of the building are either occupied by other units or unwalkable, the unit searches for the closest valid tile to the building
	if (runestoneTile.x == -1 && runestoneTile.y == -1) {

		list<iPoint> buildingTiles = App->entities->GetBuildingTiles(runestone);
		priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> queue;
		iPointPriority priorityNeighbors;

		list<iPoint>::const_iterator it = buildingTiles.begin();
		while (it != buildingTiles.end()) {

			priorityNeighbors.point = *it;
			priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
			queue.push(priorityNeighbors);

			it++;
		}

		runestoneTile = queue.top().point;

		runestoneTile = App->movement->FindClosestValidTile(runestoneTile, owner);
	}
	//_Option_b

	if (runestoneTile.x == -1 || runestoneTile.y == -1) {

		goalStatus = GoalStatus_Failed;
		return;
	}

	AddSubgoal(new Goal_MoveToPosition(owner, runestoneTile));
}

GoalStatus Goal_HealRunestone::Process(float dt)
{
	ActivateIfInactive();

	goalStatus = ProcessSubgoals(dt);

	return goalStatus;
}

void Goal_HealRunestone::Terminate()
{
	RemoveAllSubgoals();

	runestone = nullptr;

	//owner->SetRunestone(nullptr);
	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_RescuePrisoner ---------------------------------------------------------------------

Goal_RescuePrisoner::Goal_RescuePrisoner(DynamicEntity* owner, DynamicEntity* prisoner) :CompositeGoal(owner, GoalType_HealRunestone), prisoner(prisoner) {}

void Goal_RescuePrisoner::Activate()
{
	goalStatus = GoalStatus_Active;

	if (prisoner == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}

	// 2. Free the prisoner
	AddSubgoal(new Goal_FreePrisoner(owner, prisoner));

	// 1. Move near the prisoner
	iPoint prisonerTile = App->map->WorldToMap(prisoner->GetPos().x, prisoner->GetPos().y);
	AddSubgoal(new Goal_MoveToPosition(owner, prisonerTile));
}

GoalStatus Goal_RescuePrisoner::Process(float dt)
{
	ActivateIfInactive();

	goalStatus = ProcessSubgoals(dt);

	return goalStatus;
}

void Goal_RescuePrisoner::Terminate()
{
	RemoveAllSubgoals();

	prisoner = nullptr;

	//owner->SetPrisoner(nullptr);
	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// ATOMIC GOALS
// Goal_MoveToPosition ---------------------------------------------------------------------

Goal_MoveToPosition::Goal_MoveToPosition(DynamicEntity* owner, iPoint destinationTile, bool isStateChanged) :AtomicGoal(owner, GoalType_MoveToPosition), destinationTile(destinationTile), isStateChanged(isStateChanged) {}

void Goal_MoveToPosition::Activate()
{
	goalStatus = GoalStatus_Active;

	owner->SetHitting(false);
	owner->SetIsStill(false);

	if (owner->dynamicEntityType != EntityType_GRYPHON_RIDER && owner->dynamicEntityType != EntityType_DRAGON) {

		if (!App->pathfinding->IsWalkable(destinationTile)) {

			goalStatus = GoalStatus_Failed;
			return;
		}
	}

	if (owner->GetSingleUnit()->currTile == destinationTile) {
	
		goalStatus = GoalStatus_Completed;
		return;
	}

	// If the owner is an enemy, the destinationTile must be in the same room
	if (owner->entitySide == EntitySide_Enemy) {
	
		iPoint currPos = App->map->MapToWorld(owner->GetSingleUnit()->currTile.x, owner->GetSingleUnit()->currTile.y);
		iPoint destinationPos = App->map->MapToWorld(destinationTile.x, destinationTile.y);
		
		if (!App->map->IsGoalOnRoom(currPos, destinationPos)) {
		
			goalStatus = GoalStatus_Failed;
			return;
		}
	}

	if (owner->GetSingleUnit()->goal != destinationTile)

		owner->GetSingleUnit()->SetGoal(destinationTile);

	if (!owner->GetSingleUnit()->GetReadyForNewMove()) {

		goalStatus = GoalStatus_Failed;
		return;
	}
}

GoalStatus Goal_MoveToPosition::Process(float dt)
{
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)

		return goalStatus;

	App->movement->MoveUnit(owner, dt);

	if (owner->GetSingleUnit()->movementState == MovementState_WaitForPath || owner->GetSingleUnit()->movementState == MovementState_FollowPath) {

		// The unit has changed their goal (because it was not valid) through the GroupMovement module
		if (owner->GetSingleUnit()->goal != destinationTile 
			&& owner->GetSingleUnit()->goal.x != -1 && owner->GetSingleUnit()->goal.y != -1)

			destinationTile = owner->GetSingleUnit()->goal;
	}
	else if (owner->GetSingleUnit()->movementState == MovementState_GoalReached) {

		// WARNING! movementState == MovementState_GoalReached, but destinationTile is not correct
		if (owner->GetSingleUnit()->goal != destinationTile) {

			Activate();
			return goalStatus;
		}

		if (owner->GetSingleUnit()->group->isShapedGoal) {

			if (owner->GetSingleUnit()->goal == owner->GetSingleUnit()->shapedGoal)

				goalStatus = GoalStatus_Completed;
		}
		else
			goalStatus = GoalStatus_Completed;
	}

	return goalStatus;
}

void Goal_MoveToPosition::Terminate()
{
	owner->SetHitting(false);
	owner->SetIsStill(true);

	owner->GetSingleUnit()->ResetUnitParameters();

	if (isStateChanged && !owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_HitTarget ---------------------------------------------------------------------

Goal_HitTarget::Goal_HitTarget(DynamicEntity* owner, TargetInfo* targetInfo, bool isStateChanged) :AtomicGoal(owner, GoalType_HitTarget), targetInfo(targetInfo), isStateChanged(isStateChanged) {}

void Goal_HitTarget::Activate()
{
	goalStatus = GoalStatus_Active;

	if (targetInfo->isRemoveNeeded) {
	
		/// The target needs to be removed because, for example, is no longer within the sight of the unit (ordered from outside the goals)
		goalStatus = GoalStatus_Completed;
		return;
	}
	else if (targetInfo->IsTargetDead() || !targetInfo->IsTargetValid()) {

		/// The target has recently died || The target has recently become invalid
		goalStatus = GoalStatus_Completed;
		return;
	}
	else if (!targetInfo->isAttackSatisfied || !targetInfo->isSightSatisfied) {

		/// The target is no longer within the attack nor sight radius of the unit
		goalStatus = GoalStatus_Failed;
		return;
	}

	// -----

	// Set the attacking tile of the building (the one that the unit will be facing to while attacking the building)
	if (targetInfo->target->entityType == EntityCategory_STATIC_ENTITY) {

		StaticEntity* building = (StaticEntity*)targetInfo->target;

		iPoint attackingTile = { -1,-1 };

		list<iPoint> buildingTiles = App->entities->GetBuildingTiles(building);
		priority_queue<iPointPriority, vector<iPointPriority>, iPointPriorityComparator> attackingTileQueue;
		iPointPriority priorityNeighbors;

		list<iPoint>::const_iterator it = buildingTiles.begin();
		while (it != buildingTiles.end()) {

			priorityNeighbors.point = *it;
			priorityNeighbors.priority = (*it).DistanceManhattan(owner->GetSingleUnit()->currTile);
			attackingTileQueue.push(priorityNeighbors);

			it++;
		}

		attackingTile = attackingTileQueue.top().point;

		if (attackingTile.x != -1 && attackingTile.y != -1)

			targetInfo->attackingTile = attackingTile;
	}

	owner->SetHitting(true);
	owner->SetIsStill(true);

	// ----- The owner may have lost their currTarget because of the processing order of the AttackTarget goals

	if (!targetInfo->isRemoveNeeded)
		owner->SetCurrTarget(targetInfo->target);
	else
		owner->InvalidateCurrTarget();
}

GoalStatus Goal_HitTarget::Process(float dt)
{
	ActivateIfInactive();

	if (targetInfo->isRemoveNeeded) {

		/// The target needs to be removed because, for example, is no longer within the sight of the unit (ordered from outside the goals)
		goalStatus = GoalStatus_Completed;
		return goalStatus;
	}
	else if (targetInfo->IsTargetDead()) {

		/// The target has recently died

		// If the target is a Sheep or a Boar, apply health to the unit that killed it (this unit)
		if (targetInfo->target->entityType == EntityCategory_DYNAMIC_ENTITY) {

			DynamicEntity* dyn = (DynamicEntity*)targetInfo->target;

			if (dyn->dynamicEntityType == EntityType_SHEEP) {

				CritterSheepInfo c = (CritterSheepInfo&)App->entities->GetUnitInfo(EntityType_SHEEP);
				owner->ApplyHealth(c.restoredHealth);

				CritterSheep* sheep = (CritterSheep*)dyn;
				
				if (!sheep->isHealth) {

					iPoint pos = App->map->MapToWorld(owner->GetSingleUnit()->currTile.x, owner->GetSingleUnit()->currTile.y);

					if (owner->entitySide == EntitySide_Player) {
						/// Green particles +++
						App->particles->AddParticle(App->particles->playerHealth, pos);
						App->audio->PlayFx(App->audio->GetFX().healSound);
					}
					else if (owner->entitySide == EntitySide_Enemy)
						/// Red particles +++
						App->particles->AddParticle(App->particles->enemyHealth, pos);

					sheep->isHealth = true;
				}
			}
			else if (dyn->dynamicEntityType == EntityType_BOAR) {

				CritterBoarInfo b = (CritterBoarInfo&)App->entities->GetUnitInfo(EntityType_BOAR);
				owner->ApplyHealth(b.restoredHealth);

				CritterBoar* boar = (CritterBoar*)dyn;

				if (!boar->isHealth) {

					iPoint pos = App->map->MapToWorld(owner->GetSingleUnit()->currTile.x, owner->GetSingleUnit()->currTile.y);

					if (owner->entitySide == EntitySide_Player) {
						/// Green particles +++
						App->particles->AddParticle(App->particles->playerHealth, pos);
						App->audio->PlayFx(App->audio->GetFX().healSound);
					}
					else if (owner->entitySide == EntitySide_Enemy)
						/// Red particles +++
						App->particles->AddParticle(App->particles->enemyHealth, pos);

					boar->isHealth = true;
				}
			}
		}

		goalStatus = GoalStatus_Completed;
		return goalStatus;
	}
	else if (!targetInfo->IsTargetValid()) {
	
		/// The target has recently become invalid
		goalStatus = GoalStatus_Completed;
		return goalStatus;
	}
	/// The target is no longer within the attack nor sight radius of the unit
	else if (!targetInfo->isAttackSatisfied || !targetInfo->isSightSatisfied) {

		goalStatus = GoalStatus_Failed;
		return goalStatus;
	}

	// -----

	if (!owner->IsHitting())
		owner->SetHitting(true);
	if (!owner->IsStill())
		owner->SetIsStill(true);

	// Do things at the end of the animation
	if (((DynamicEntity*)owner)->GetAnimation()->Finished()) {

		DynamicEntity* dyn = (DynamicEntity*)owner;

		fPoint destination = targetInfo->target->GetPos();

		// Calculate the orientation of the particle (Elven Archer, Troll Axethrower, Gryphon Rider and Dragon)
		if (targetInfo->attackingTile.x != -1 && targetInfo->attackingTile.y != -1) {

			iPoint attackingPos = App->map->MapToWorld(targetInfo->attackingTile.x, targetInfo->attackingTile.y);
			destination = { (float)attackingPos.x, (float)attackingPos.y };
			orientation = { attackingPos.x - owner->GetPos().x, attackingPos.y - owner->GetPos().y };
		}
		else
			orientation = { targetInfo->target->GetPos().x - owner->GetPos().x, targetInfo->target->GetPos().y - owner->GetPos().y };

		float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

		if (m > 0.0f) {

			orientation.x /= m;
			orientation.y /= m;
		}

		owner->SetUnitDirectionByValue(orientation);

		switch (owner->dynamicEntityType) {

		case EntityType_ELVEN_ARCHER:

			// Arrow
			App->audio->PlayFx(App->audio->GetFX().arrowThrow, 0);

			{
				ElvenArcher* elvenArcher = (ElvenArcher*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_GRYPHON_RIDER:

			// Flames
			App->audio->PlayFx(App->audio->GetFX().griffonAttack, 0);
			{
				GryphonRider* gryphonRider = (GryphonRider*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->gryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_TROLL_AXETHROWER:

			// Axe
			App->audio->PlayFx(App->audio->GetFX().axeThrow, 0);

			{
				TrollAxethrower* trollAxethrower = (TrollAxethrower*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, destination, trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_DRAGON:

			// Flames
			App->audio->PlayFx(App->audio->GetFX().griffonAttack, 0);
		{
			Dragon* dragon = (Dragon*)owner;

			switch (owner->GetDirection(orientation)) {

			case UnitDirection_DownRight:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_UpRight:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Right:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_DownLeft:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_UpLeft:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Left:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Down:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;

			case UnitDirection_Up:
			case UnitDirection_NoDirection:
			default:
				App->particles->AddParticle(App->particles->dragonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, destination, dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			}
		}
		break;

			// The rest of entities
		default:

			App->audio->PlayFx(App->audio->GetFX().swordClash, 0);

			if (targetInfo->target->entityType == EntityCategory_STATIC_ENTITY) {

				StaticEntity* building = (StaticEntity*)targetInfo->target;

				if (building->GetBuildingState() != BuildingState_Building) {

					targetInfo->target->ApplyDamage(owner->GetDamage(targetInfo->target));
					building->CheckBuildingState();
				}
			}
			else
				targetInfo->target->ApplyDamage(owner->GetDamage(targetInfo->target));

			break;
		}

		// Reset the animation
		((DynamicEntity*)owner)->GetAnimation()->Reset();
	}

	// ----- The owner may have lost their currTarget because of the processing order of the AttackTarget goals

	if (!targetInfo->isRemoveNeeded)
		owner->SetCurrTarget(targetInfo->target);
	else
		owner->InvalidateCurrTarget();

	if (isStateChanged && owner->GetUnitState() != UnitState_AttackTarget && !owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_AttackTarget);

	return goalStatus;
}

void Goal_HitTarget::Terminate()
{
	owner->SetHitting(false);
	owner->SetIsStill(true);

	targetInfo = nullptr;
	orientation = { 0,0 };

	if (isStateChanged)
		owner->SetUnitState(UnitState_AttackTarget);
}

// Goal_LookAround ---------------------------------------------------------------------

Goal_LookAround::Goal_LookAround(DynamicEntity* owner, uint minSecondsToChange, uint maxSecondsToChange, uint minSecondsUntilNextChange, uint maxSecondsUntilNextChange, uint probabilityGoalCompleted) :AtomicGoal(owner, GoalType_LookAround), minSecondsToChange(minSecondsToChange), maxSecondsToChange(maxSecondsToChange), minSecondsUntilNextChange(minSecondsUntilNextChange), maxSecondsUntilNextChange(maxSecondsUntilNextChange), probabilityGoalCompleted(probabilityGoalCompleted) {}

void Goal_LookAround::Activate()
{
	goalStatus = GoalStatus_Active;

	owner->SetIsStill(true);

	uint random = rand() % 3;

	switch (owner->GetUnitDirection()) {

	case UnitDirection_Up:

		if (random == 0)
			nextOrientation = UnitDirection_UpLeft;
		else if (random == 1)
			nextOrientation = UnitDirection_UpRight;
		else
			nextOrientation = UnitDirection_Up;

		break;

	case UnitDirection_NoDirection:
	case UnitDirection_Down:

		if (random == 0)
			nextOrientation = UnitDirection_DownLeft;
		else if (random == 1)
			nextOrientation = UnitDirection_DownRight;
		else
			nextOrientation = UnitDirection_Down;

		break;

	case UnitDirection_Left:

		if (random == 0)
			nextOrientation = UnitDirection_UpLeft;
		else if (random == 1)
			nextOrientation = UnitDirection_DownLeft;
		else
			nextOrientation = UnitDirection_Left;

		break;

	case UnitDirection_Right:

		if (random == 0)
			nextOrientation = UnitDirection_UpRight;
		else if (random == 1)
			nextOrientation = UnitDirection_DownRight;
		else
			nextOrientation = UnitDirection_Right;

		break;

	case UnitDirection_UpLeft:

		if (random == 0)
			nextOrientation = UnitDirection_Up;
		else if (random == 1)
			nextOrientation = UnitDirection_Left;
		else
			nextOrientation = UnitDirection_UpLeft;

		break;

	case UnitDirection_UpRight:

		if (random == 0)
			nextOrientation = UnitDirection_Up;
		else if (random == 1)
			nextOrientation = UnitDirection_Right;
		else
			nextOrientation = UnitDirection_UpRight;

		break;

	case UnitDirection_DownLeft:

		if (random == 0)
			nextOrientation = UnitDirection_Down;
		else if (random == 1)
			nextOrientation = UnitDirection_Left;
		else
			nextOrientation = UnitDirection_DownLeft;

		break;

	case UnitDirection_DownRight:

		if (random == 0)
			nextOrientation = UnitDirection_Down;
		else if (random == 1)
			nextOrientation = UnitDirection_Right;
		else
			nextOrientation = UnitDirection_DownRight;

		break;
	}

	timer.Start();
	secondsToChange = (float)(rand() % maxSecondsToChange + minSecondsToChange);
	secondsUntilNextChange = (float)(rand() % maxSecondsUntilNextChange + minSecondsUntilNextChange);
}

GoalStatus Goal_LookAround::Process(float dt)
{
	ActivateIfInactive();

	if (!isChanged) {

		if (timer.ReadSec() >= secondsToChange) {

			owner->SetUnitDirection(nextOrientation);

			isChanged = true;
			timer.Start();
		}
	}
	else {

		if (timer.ReadSec() >= secondsUntilNextChange) {

			uint random = 0;

			if (probabilityGoalCompleted > 0 && probabilityGoalCompleted <= 3)

				random = rand() % probabilityGoalCompleted;

			if (probabilityGoalCompleted <= 3) {

				if (random % 2 == 0)
					goalStatus = GoalStatus_Completed;
				else
					Activate();

				return goalStatus;
			}
			else {
			
				uint random = 0;
				random = rand() % 10;

				if (random <= 4)
					goalStatus = GoalStatus_Completed;
				else
					Activate();

				return goalStatus;
			}
		}
	}

	return goalStatus;
}

void Goal_LookAround::Terminate()
{
	owner->SetIsStill(true);

	secondsToChange = 0.0f;
	secondsUntilNextChange = 0.0f;

	minSecondsToChange = 0;
	maxSecondsToChange = 0;
	minSecondsUntilNextChange = 0;
	maxSecondsUntilNextChange = 0;
	probabilityGoalCompleted = 0;
	isChanged = false;

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_PickNugget ---------------------------------------------------------------------

Goal_PickNugget::Goal_PickNugget(DynamicEntity* owner, GoldMine* goldMine) :AtomicGoal(owner, GoalType_PickNugget), goldMine(goldMine) {}

void Goal_PickNugget::Activate()
{
	goalStatus = GoalStatus_Active;

	if (goldMine == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}
	else if (goldMine->buildingState == BuildingState_Destroyed) {

		owner->SetGoldMine(nullptr);
		goalStatus = GoalStatus_Failed;
		return;
	}

	if (!goldMine->IsUnitGatheringGold()) {

		owner->SetUnitGatheringGold(true);
		goldMine->SetUnitGatheringGold(true);
		goldMine->buildingState = BuildingState_Destroyed;
		goldMine->SetGoldMineState(GoldMineState_Gathering);
	}
	// Another unit has already entered the mine and is gathering the gold
	else {

		owner->SetGoldMine(nullptr);
		goalStatus = GoalStatus_Failed;
		return;
	}

	// Determine the amount of gold and the time (depending on the gold)
	int random = rand() % 4;
	int timesRepeatSound = 0;

	/// TODO Balancing
	int mapDifficulty = App->scene->mapDifficulty;

	if (mapDifficulty == 0 || mapDifficulty == 1) { //Easy
		switch (random) {
		case 0:
			gold = 2000;
			secondsGathering = 10.0f;
			timesRepeatSound = 5;
			break;
		case 1:
			gold = 2100;
			secondsGathering = 14.0f;
			timesRepeatSound = 6;
			break;
		case 2:
			gold = 2300;
			secondsGathering = 15.0f;
			timesRepeatSound = 7;
			break;
		case 3:
			gold = 2500;
			secondsGathering = 17.0f;
			timesRepeatSound = 8;
			break;

		default:
			gold = 3000;
			secondsGathering = 20.0f;
			timesRepeatSound = 9;
			break;
		}
	}

	else  { //Medium/Hard
		switch (random) {
		case 0:
			gold = 1400;
			secondsGathering = 10.0f;
			timesRepeatSound = 5;
			break;
		case 1:
			gold = 1550;
			secondsGathering = 14.0f;
			timesRepeatSound = 6;
			break;
		case 2:
			gold = 1700;
			secondsGathering = 15.0f;
			timesRepeatSound = 7;
			break;
		case 3:
			gold = 1850;
			secondsGathering = 17.0f;
			timesRepeatSound = 8;
			break;

		default:
			gold = 2000;
			secondsGathering = 20.0f;
			timesRepeatSound = 9;
			break;
		}
	}

	goldMine->totalGold =  gold;
	goldMine->currGold = gold;
	goldMine->secondsGathering = secondsGathering;

	App->audio->PlayFx(App->audio->GetFX().goldMine, timesRepeatSound); // Gold mine sound

	owner->SetBlitState(false);
	owner->SetIsValid(false);

	if (owner->GetLifeBar() != nullptr)
		owner->GetLifeBar()->isBlit = false;

	msAnimation = 600.0f;
	timerGathering = 0.0f;
	goldMine->currentSec = timerGathering;
	timerAnimation.Start();

	//Varibles of the process of gathering gold
	goldGathered = 0;
	goldToAdd = gold * ((float)timeForEachGoldUpdate / 1000) / secondsGathering;
	auxGatherTimer.Start();
}

GoalStatus Goal_PickNugget::Process(float dt)
{
	
	timerGathering += dt;
	
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)
		return goalStatus;

	if (timerAnimation.ReadMs() >= msAnimation) {

		goldMine->SwapTexArea();
		timerAnimation.Start();
	}

	// Wait for the unit to pick the gold
	if (timerGathering >= secondsGathering)
		goalStatus = GoalStatus_Completed;

	goldMine->currentSec = timerGathering;

	//Adds progressively gold to the player
	if (auxGatherTimer.Read() >= timeForEachGoldUpdate && dt != 0) {
		App->player->AddGold(goldToAdd);
		goldGathered += goldToAdd;
		auxGatherTimer.Start();
	}

	goldMine->currGold = goldGathered;

	return goalStatus;
}

void Goal_PickNugget::Terminate()
{
	if (goalStatus == GoalStatus_Completed) {

		goldMine->SetGoldMineState(GoldMineState_Gathered);

		// Give remaining gold to the player
		if(gold != goldGathered)
			App->player->AddGold(gold - goldGathered);
		goldMine->currGold = goldGathered;

		owner->SetGoldMine(nullptr);
		owner->SetUnitGatheringGold(false);
		goldMine->SetUnitGatheringGold(false);

		owner->SetBlitState(true);
		owner->SetIsValid(true);

		if (owner->GetLifeBar() != nullptr)
			owner->GetLifeBar()->isBlit = true;
	}

	goldMine = nullptr;
	gold = 0;

	secondsGathering = 0.0f;
	msAnimation = 0.0f;

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_HealArea ---------------------------------------------------------------------

Goal_HealArea::Goal_HealArea(DynamicEntity* owner, Runestone* runestone) :AtomicGoal(owner, GoalType_HealArea), runestone(runestone) {}

void Goal_HealArea::Activate()
{
	goalStatus = GoalStatus_Active;

	if (runestone == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}
	else if (runestone->buildingState == BuildingState_Destroyed) {

		owner->SetRunestone(nullptr);
		goalStatus = GoalStatus_Failed;
		return;
	}

	if (!runestone->IsUnitHealingArea()) {

		owner->SetUnitHealingRunestone(true);
		runestone->SetUnitHealingArea(true);
		runestone->buildingState = BuildingState_Destroyed;
		runestone->SetRunestoneState(RunestoneState_Gathering);
	}
	// Another unit is already interacting with the runestone
	else {

		owner->SetRunestone(nullptr);
		goalStatus = GoalStatus_Failed;
		return;
	}

	// Determine the amount of health
	int random = rand() % 4;

	switch (random) {

	case 0:
		health = 50;
		break;
	case 1:
		health = 50;
		break;
	case 2:
		health = 60;
		break;
	case 3:
		health = 70;
		break;

	default:
		health = 80;
		break;
	}

	App->audio->PlayFx(App->audio->GetFX().mageTower, 0); // Mage Tower FX

	owner->SetIsValid(false);

	msAnimation = 300.0f;
	timerAnimation.Start();
	secondsGathering = 2.0f;
	timerGathering.Start();

	alpha = 200;
}

GoalStatus Goal_HealArea::Process(float dt)
{
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)
		return goalStatus;

	if (timerGathering.ReadSec() <= secondsGathering) {

		if (timerAnimation.ReadMs() >= msAnimation) {

			runestone->SwapTexArea();
			timerAnimation.Start();
		}

		return goalStatus;
	}

	// Bright the area
	if (alpha > 0) {

		alpha -= 8.0f * dt;

		if (alpha < 0)
			alpha = 0;

		runestone->BlitSightArea(alpha);

		if (alpha > 50)
			return goalStatus;
	}

	// Perform the heal animation and heal the units within the area
	Collider* sightRadiusCollider = runestone->GetSightRadiusCollider()->offsetCollider;
	bool isHealSound = false;

	list<DynamicEntity*>::const_iterator it = App->entities->activeDynamicEntities.begin();

	while (it != App->entities->activeDynamicEntities.end()) {
	
		if (!(*it)->isDead && (*it)->entitySide == EntitySide_Player) {
		
			const SDL_Rect rectA = sightRadiusCollider->colliderRect;
			const SDL_Rect rectB = { (*it)->GetPos().x, (*it)->GetPos().y, (*it)->GetSize().x, (*it)->GetSize().y };

			if (SDL_HasIntersection(&rectA, &rectB)) {

				iPoint pos = App->map->MapToWorld((*it)->GetSingleUnit()->currTile.x, (*it)->GetSingleUnit()->currTile.y);

				App->particles->AddParticle(App->particles->playerHealth, pos);
				(*it)->ApplyHealth(health);

				if (!isHealSound) {

					App->audio->PlayFx(App->audio->GetFX().healSound);
					isHealSound = true;
				}
			}
		}
		it++;
	}
		
	goalStatus = GoalStatus_Completed;

	return goalStatus;
}

void Goal_HealArea::Terminate()
{
	if (goalStatus == GoalStatus_Completed) {

		runestone->SetRunestoneState(RunestoneState_Gathered);

		owner->SetRunestone(nullptr);
		owner->SetUnitHealingRunestone(false);
		runestone->SetUnitHealingArea(false);

		owner->SetIsValid(true);
	}

	runestone = nullptr;
	health = 0;

	secondsGathering = 0.0f;
	msAnimation = 0.0f;

	alpha = 0;

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}

// Goal_FreePrisoner ---------------------------------------------------------------------

Goal_FreePrisoner::Goal_FreePrisoner(DynamicEntity* owner, DynamicEntity* prisoner) :AtomicGoal(owner, GoalType_HealArea), prisoner(prisoner) {}

void Goal_FreePrisoner::Activate()
{
	goalStatus = GoalStatus_Active;

	if (prisoner == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}

	if (prisoner->dynamicEntityType == EntityType_ALLERIA)	
		alleria = (Alleria*)prisoner;
	else if (prisoner->dynamicEntityType == EntityType_TURALYON)
		turalyon = (Turalyon*)prisoner;

	if (alleria == nullptr && turalyon == nullptr) {

		goalStatus = GoalStatus_Failed;
		return;
	}
	else if (alleria != nullptr) {
	
		if (!alleria->IsUnitRescuingPrisoner() && !alleria->IsRescued()) {

			alleria->SetUnitRescuePrisoner(true);
			owner->SetUnitRescuePrisoner(true);
		}
		// Another unit is already interacting with the prisoner
		else {

			owner->SetPrisoner(nullptr);
			goalStatus = GoalStatus_Failed;
			return;
		}
	}
	else if (turalyon != nullptr) {

		if (!turalyon->IsUnitRescuingPrisoner() && !turalyon->IsRescued()) {

			turalyon->SetUnitRescuePrisoner(true);
			owner->SetUnitRescuePrisoner(true);
		}
		// Another unit is already interacting with the prisoner
		else {

			owner->SetPrisoner(nullptr);
			goalStatus = GoalStatus_Failed;
			return;
		}
	}

	App->audio->PlayFx(App->audio->GetFX().prisionerRescue, 0);

	owner->SetIsValid(false);
}

GoalStatus Goal_FreePrisoner::Process(float dt)
{
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)

		return goalStatus;

	/// TODO Sandra: make the prisoner say goodbye or something
	/// TODO Sandra: make the prisoner disappear with the appropiate animation

	goalStatus = GoalStatus_Completed;

	return goalStatus;
}

void Goal_FreePrisoner::Terminate()
{
	if (goalStatus == GoalStatus_Completed) {

		if (alleria != nullptr) {
		
			alleria->SetRescued(true);
			alleria->SetUnitRescuePrisoner(false);
			App->player->RescuePrisoner(TerenasDialog_RESCUE_ALLERIA, { 848,159,52,42 }, { 8, 244 });
			App->player->isAllRescued = true;
		}
		else if (turalyon != nullptr) {
		
			turalyon->SetRescued(true);
			turalyon->SetUnitRescuePrisoner(false);
			App->player->RescuePrisoner(TerenasDialog_RESCUE_TURALYON, { 796,159,52,42 }, { 8, 200 });
			App->player->isTurRescued = true;
		}

		owner->SetPrisoner(nullptr);
		owner->SetUnitRescuePrisoner(false);
		owner->SetIsValid(true);

		/// TODO Sandra: animate the room with a flash (fullscreen flash)

		/// TODO Sandra: if the prisoner is the first rescued, the room becomes a new player base
		/// TODO Sandra: if the prisoner is the second rescued, the victory of the game is achieved
	}

	prisoner = nullptr;
	alleria = nullptr;
	turalyon = nullptr;

	if (!owner->isDead && !owner->isRemove)
		owner->SetUnitState(UnitState_Idle);
}