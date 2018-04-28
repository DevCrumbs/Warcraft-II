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

	owner->SetUnitState(UnitState_Idle);
}

void Goal_Think::AddGoal_Wander(uint maxDistance)
{
	AddSubgoal(new Goal_Wander(owner, maxDistance));
}

void Goal_Think::AddGoal_AttackTarget(TargetInfo* targetInfo)
{
	AddSubgoal(new Goal_AttackTarget(owner, targetInfo));
}

void Goal_Think::AddGoal_MoveToPosition(iPoint destinationTile)
{
	AddSubgoal(new Goal_MoveToPosition(owner, destinationTile));
}

void Goal_Think::AddGoal_Patrol(iPoint originTile, iPoint destinationTile)
{
	AddSubgoal(new Goal_Patrol(owner, originTile, destinationTile));
}

void Goal_Think::AddGoal_GatherGold(GoldMine* goldMine)
{
	AddSubgoal(new Goal_GatherGold(owner, goldMine));
}

void Goal_Think::AddGoal_HealRunestone(Runestone* runestone)
{
	AddSubgoal(new Goal_HealRunestone(owner, runestone));
}

// Goal_AttackTarget ---------------------------------------------------------------------

Goal_AttackTarget::Goal_AttackTarget(DynamicEntity* owner, TargetInfo* targetInfo) :CompositeGoal(owner, GoalType_AttackTarget), targetInfo(targetInfo) {}

void Goal_AttackTarget::Activate()
{
	goalStatus = GoalStatus_Active;

	RemoveAllSubgoals();

	if (targetInfo == nullptr) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	/// The target has been removed from another unit!!!
	if (targetInfo->isRemoved) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	// It is possible for a bot's target to die while this goal is active,
	// so we must test to make sure the bot always has an active target
	if (!targetInfo->IsTargetPresent()) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	// The attack is only performed if the sight distance is satisfied
	/*
	else if (!targetInfo->isSightSatisfied) {

	goalStatus = GoalStatus_Failed;
	return;
	}
	*/

	// -----

	AddSubgoal(new Goal_HitTarget(owner, targetInfo));

	// If the target is far from the unit, head directly at the target's position
	if (!targetInfo->isAttackSatisfied) {

		iPoint targetTile = App->map->WorldToMap(targetInfo->target->GetPos().x, targetInfo->target->GetPos().y);
		AddSubgoal(new Goal_MoveToPosition(owner, targetTile));
	}

	/// The target is being attacked by this unit
	targetInfo->target->AddAttackingUnit(owner);

	owner->SetUnitState(UnitState_AttackTarget);
}

GoalStatus Goal_AttackTarget::Process(float dt)
{
	ActivateIfInactive();

	// The unit was chasing their target, but the attack distance has been suddenly satisfied
	if (subgoals.size() > 0) {

		if (owner->GetSingleUnit()->IsFittingTile()
			&& (targetInfo->isRemoved || (targetInfo->isAttackSatisfied && subgoals.front()->GetType() == GoalType_MoveToPosition))) {

			subgoals.front()->Terminate();
			delete subgoals.front();
			subgoals.pop_front();
		}
	}

	// Process the subgoals
	goalStatus = ProcessSubgoals(dt);

	ReactivateIfFailed();

	return goalStatus;
}

void Goal_AttackTarget::Terminate()
{
	RemoveAllSubgoals();

	// -----

	if (targetInfo == nullptr)
		return;

	/// The target has been removed from another unit!!!
	if (targetInfo->isRemoved || targetInfo->target == nullptr) {

		// Remove the target from this owner
		owner->RemoveTargetInfo(targetInfo);

		// -----

		targetInfo = nullptr;
		owner->SetUnitState(UnitState_Idle);

		return;
	}

	// Remove this attacking unit from the owner's unitsAttacking list
	targetInfo->target->RemoveAttackingUnit(owner);

	// If the target has died, invalidate it from the rest of the units
	if (!targetInfo->IsTargetPresent())

		App->entities->InvalidateAttackEntity(targetInfo->target);

	// If the sight distance is not satisfied, remove the target from the entity targets list
	// !targetInfo->isSightSatisfied ||
	if (!targetInfo->IsTargetPresent())

		// Remove the target from this owner
		owner->RemoveTargetInfo(targetInfo);

	// -----

	targetInfo = nullptr;

	owner->SetUnitState(UnitState_Idle);
}

// Goal_Patrol ---------------------------------------------------------------------

Goal_Patrol::Goal_Patrol(DynamicEntity* owner, iPoint originTile, iPoint destinationTile) :CompositeGoal(owner, GoalType_Patrol), originTile(originTile), destinationTile(destinationTile) {}

void Goal_Patrol::Activate()
{
	goalStatus = GoalStatus_Active;

	RemoveAllSubgoals();

	if (currGoal == destinationTile)
		currGoal = originTile;
	else
		currGoal = destinationTile;

	AddSubgoal(new Goal_MoveToPosition(owner, currGoal));

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

	owner->SetUnitState(UnitState_Idle);
}

// Goal_Wander ---------------------------------------------------------------------

Goal_Wander::Goal_Wander(DynamicEntity* owner, uint maxDistance) :CompositeGoal(owner, GoalType_Wander), maxDistance(maxDistance) {}

void Goal_Wander::Activate()
{
	maxDistance = 5;
	goalStatus = GoalStatus_Active;

	if (owner != nullptr)
		AddSubgoal(new Goal_LookAround(owner));

	iPoint destinationTile = { -1,-1 };

	int sign = rand() % 2;
	if (sign == 0)
		sign = -1;

	destinationTile.x = owner->GetSingleUnit()->currTile.x + sign * (rand() % (maxDistance + 1) + 1);

	sign = rand() % 2;
	if (sign == 0)
		sign = -1;

	destinationTile.y = owner->GetSingleUnit()->currTile.y + sign * (rand() % (maxDistance + 1) + 1);

	AddSubgoal(new Goal_MoveToPosition(owner, destinationTile));
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
	iPoint goldMineTile = App->map->WorldToMap(goldMine->GetPos().x, goldMine->GetPos().y);
	goldMineTile = App->movement->FindClosestValidTile(goldMineTile);

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

	owner->SetGoldMine(nullptr);
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
	iPoint runestoneTile = App->map->WorldToMap(runestone->GetPos().x, runestone->GetPos().y);
	runestoneTile = App->movement->FindClosestValidTile(runestoneTile);

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

	owner->SetRunestone(nullptr);
	owner->SetUnitState(UnitState_Idle);
}

// ATOMIC GOALS
// Goal_MoveToPosition ---------------------------------------------------------------------

Goal_MoveToPosition::Goal_MoveToPosition(DynamicEntity* owner, iPoint destinationTile) :AtomicGoal(owner, GoalType_MoveToPosition), destinationTile(destinationTile) {}

void Goal_MoveToPosition::Activate()
{
	goalStatus = GoalStatus_Active;

	owner->SetHitting(false);

	if (!App->pathfinding->IsWalkable(destinationTile)) {

		goalStatus = GoalStatus_Failed;
		return;
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

	if (owner->GetSingleUnit()->movementState == MovementState_GoalReached) {

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
	owner->GetSingleUnit()->ResetUnitParameters();

	owner->SetIsStill(true);
}

// Goal_HitTarget ---------------------------------------------------------------------

Goal_HitTarget::Goal_HitTarget(DynamicEntity* owner, TargetInfo* targetInfo) :AtomicGoal(owner, GoalType_HitTarget), targetInfo(targetInfo) {}

void Goal_HitTarget::Activate()
{
	goalStatus = GoalStatus_Active;

	/// The target has been removed from another unit!!!
	if (targetInfo->isRemoved) {

		goalStatus = GoalStatus_Completed;
		return;
	}

	// It is possible for a bot's target to die while this goal is active,
	// so we must test to make sure the bot always has an active target
	if (!targetInfo->IsTargetPresent()) {

		goalStatus = GoalStatus_Completed;
		return;
	}
	else if (!targetInfo->isAttackSatisfied || !targetInfo->isSightSatisfied
		|| !owner->GetSingleUnit()->IsFittingTile()) {

		goalStatus = GoalStatus_Failed;
		return;
	}

	owner->SetHitting(true);
}

GoalStatus Goal_HitTarget::Process(float dt)
{
	ActivateIfInactive();

	/// The target has been removed from another unit!!!
	if (targetInfo->isRemoved) {

		goalStatus = GoalStatus_Completed;
		return goalStatus;
	}

	if (!targetInfo->IsTargetPresent()) {

		if (targetInfo->target == nullptr) {

			goalStatus = GoalStatus_Completed;
			return goalStatus;
		}

		// The target is dead
		if (targetInfo->target->GetCurrLife() <= 0) {

			if (targetInfo->target->entityType == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dyn = (DynamicEntity*)targetInfo->target;

				if (dyn->dynamicEntityType == EntityType_SHEEP) {
					CritterSheepInfo c = (CritterSheepInfo&)App->entities->GetUnitInfo(EntityType_SHEEP);
					owner->ApplyHealth(c.restoredHealth);
				}
				else if (dyn->dynamicEntityType == EntityType_BOAR) {
					CritterBoarInfo b = (CritterBoarInfo&)App->entities->GetUnitInfo(EntityType_BOAR);
					owner->ApplyHealth(b.restoredHealth);
				}
			}
			goalStatus = GoalStatus_Completed;
			return goalStatus;
		}
	}
	else if (!targetInfo->isAttackSatisfied || !targetInfo->isSightSatisfied) {

		goalStatus = GoalStatus_Failed;
		return goalStatus;
	}

	// Do things when the animation of the unit has finished
	if (((DynamicEntity*)owner)->GetAnimation()->Finished()) {

		DynamicEntity* dyn = (DynamicEntity*)owner;

		// Calculate particle orientation (for Elven Archer and Troll Axethrower)
		orientation = { targetInfo->target->GetPos().x - owner->GetPos().x, targetInfo->target->GetPos().y - owner->GetPos().y };

		float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

		if (m > 0.0f) {
			orientation.x /= m;
			orientation.y /= m;
		}

		switch (owner->dynamicEntityType) {

		case EntityType_ELVEN_ARCHER:

			// ARROW
			App->audio->PlayFx(App->audio->GetFX().arrowThrow, 0);

			{
				ElvenArcher* elvenArcher = (ElvenArcher*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->playerArrows, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), elvenArcher->GetArrowSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_GRYPHON_RIDER:

			{
				GryphonRider* gryphonRider = (GryphonRider*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), gryphonRider->GetFireSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_TROLL_AXETHROWER:

			// AXE
			App->audio->PlayFx(App->audio->GetFX().axeThrow, 0);

			{
				TrollAxethrower* trollAxethrower = (TrollAxethrower*)owner;

				switch (owner->GetDirection(orientation)) {

				case UnitDirection_DownRight:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpRight:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Right:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_DownLeft:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_UpLeft:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Left:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				case UnitDirection_Down:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;

				case UnitDirection_Up:
				case UnitDirection_NoDirection:
				default:
					App->particles->AddParticle(App->particles->trollAxe, { (int)owner->GetPos().x + 8, (int)owner->GetPos().y + 8 }, targetInfo->target->GetPos(), trollAxethrower->GetAxeSpeed(), owner->GetDamage(targetInfo->target));
					break;
				}
			}
			break;

		case EntityType_DRAGON:

		{
			Dragon* dragon = (Dragon*)owner;

			switch (owner->GetDirection(orientation)) {

			case UnitDirection_DownRight:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_UpRight:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Right:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_DownLeft:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_UpLeft:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Left:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			case UnitDirection_Down:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;

			case UnitDirection_Up:
			case UnitDirection_NoDirection:
			default:
				App->particles->AddParticle(App->particles->dragonGryphonFire, { (int)owner->GetPos().x, (int)owner->GetPos().y }, targetInfo->target->GetPos(), dragon->GetFireSpeed(), owner->GetDamage(targetInfo->target));
				break;
			}
		}
		break;

			// The rest of entities
		default:

			App->audio->PlayFx(App->audio->GetFX().swordClash, 0);

			targetInfo->target->ApplyDamage(owner->GetDamage(targetInfo->target));
			break;
		}

		// Reset the animation
		((DynamicEntity*)owner)->GetAnimation()->Reset();
	}

	return goalStatus;
}

void Goal_HitTarget::Terminate()
{
	owner->SetHitting(false);
}

// Goal_LookAround ---------------------------------------------------------------------

Goal_LookAround::Goal_LookAround(DynamicEntity* owner) :AtomicGoal(owner, GoalType_LookAround) {}

void Goal_LookAround::Activate()
{
	goalStatus = GoalStatus_Active;

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
	secondsToChange = (float)(rand() % 4 + 2);
	secondsUntilNextChange = (float)(rand() % 6 + 2);
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

			uint random = rand() % 3;

			if (random % 2 == 0)
				goalStatus = GoalStatus_Completed;
			else
				Activate();
		}
	}

	return goalStatus;
}

void Goal_LookAround::Terminate()
{
	secondsToChange = 0.0f;
	secondsUntilNextChange = 0.0f;
	isChanged = false;
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

		goalStatus = GoalStatus_Failed;
		return;
	}

	// Determine the amount of gold and the time (depending on the gold)
	int random = rand() % 4;

	switch (random) {

	case 0:
		gold = 550;
		secondsGathering = 5.0f;
		break;
	case 1:
		gold = 600;
		secondsGathering = 6.0f;
		break;
	case 2:
		gold = 650;
		secondsGathering = 6.5f;
		break;
	case 3:
		gold = 700;
		secondsGathering = 7.0f;
		break;

	default:
		gold = 800;
		secondsGathering = 8.0f;
		break;
	}

	goldMine->totalGold = gold;
	goldMine->secondsGathering = secondsGathering;


	App->audio->PlayFx(App->audio->GetFX().goldMine, 3); //Gold mine sound

	owner->SetBlitState(false);
	owner->SetIsValid(false);

	if (owner->GetLifeBar() != nullptr)
		owner->GetLifeBar()->isBlit = false;

	msAnimation = 600.0f;
	timerGathering.Start();
	goldMine->currentSec = timerGathering.ReadSec();
	timerAnimation.Start();
}

GoalStatus Goal_PickNugget::Process(float dt)
{
	ActivateIfInactive();

	if (goalStatus == GoalStatus_Failed)
		return goalStatus;

	if (timerAnimation.ReadMs() >= msAnimation) {

		goldMine->SwapTexArea();
		timerAnimation.Start();
	}

	// Wait for the unit to pick the gold
	if (timerGathering.ReadSec() >= secondsGathering)

		goalStatus = GoalStatus_Completed;

	goldMine->currentSec = timerGathering.ReadSec();

	return goalStatus;
}

void Goal_PickNugget::Terminate()
{
	if (goalStatus == GoalStatus_Completed) {

		goldMine->SetGoldMineState(GoldMineState_Gathered);

		// Give gold to the player
		App->player->AddGold(gold);
		App->scene->hasGoldChanged = true;

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

		alpha -= 8;

		if (alpha < 0)
			alpha = 0;

		runestone->BlitSightArea(alpha);

		if (alpha > 50)
			return goalStatus;
	}

	// Perform the heal animation and heal the units within the area
	ColliderGroup* sightRadiusCollider = runestone->GetSightRadiusCollider();

	for (uint i = 0; i < sightRadiusCollider->colliders.size(); ++i) {

		iPoint colliderTile = App->map->WorldToMap(sightRadiusCollider->colliders[i]->colliderRect.x, sightRadiusCollider->colliders[i]->colliderRect.y);

		Entity* entity = App->entities->IsEntityOnTile(colliderTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);

		if (entity != nullptr) {

			DynamicEntity* dynEnt = (DynamicEntity*)entity;
			iPoint pos = App->map->MapToWorld(dynEnt->GetSingleUnit()->currTile.x, dynEnt->GetSingleUnit()->currTile.y);

			App->particles->AddParticle(App->particles->health, pos);
			entity->ApplyHealth(health);
		}
	}

	goalStatus = GoalStatus_Completed;

	return goalStatus;
}

void Goal_HealArea::Terminate()
{
	if (goalStatus == GoalStatus_Completed) {

		runestone->SetRunestoneState(RunestoneState_Gathered);

		owner->SetUnitHealingRunestone(false);
		runestone->SetUnitHealingArea(false);

		owner->SetIsValid(true);
	}

	runestone = nullptr;
	health = 0;

	secondsGathering = 0.0f;
	msAnimation = 0.0f;

	alpha = 0;
}