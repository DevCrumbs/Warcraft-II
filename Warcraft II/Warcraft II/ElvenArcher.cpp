#include "Defs.h"
#include "p2Log.h"

#include "ElvenArcher.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1EntityFactory.h"
#include "j1Movement.h"
#include "j1PathManager.h"
#include "Goal.h"
#include "j1Audio.h"
#include "j1Particles.h"
#include "j1Printer.h"

#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

ElvenArcher::ElvenArcher(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const ElvenArcherInfo& elvenArcherInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), elvenArcherInfo(elvenArcherInfo)
{
	// XML loading
	/// Animations
	ElvenArcherInfo info = (ElvenArcherInfo&)App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER);
	this->unitInfo = this->elvenArcherInfo.unitInfo;
	this->elvenArcherInfo.up = info.up;
	this->elvenArcherInfo.down = info.down;
	this->elvenArcherInfo.left = info.left;
	this->elvenArcherInfo.right = info.right;
	this->elvenArcherInfo.upLeft = info.upLeft;
	this->elvenArcherInfo.upRight = info.upRight;
	this->elvenArcherInfo.downLeft = info.downLeft;
	this->elvenArcherInfo.downRight = info.downRight;

	this->elvenArcherInfo.attackUp = info.attackUp;
	this->elvenArcherInfo.attackDown = info.attackDown;
	this->elvenArcherInfo.attackLeft = info.attackLeft;
	this->elvenArcherInfo.attackRight = info.attackRight;
	this->elvenArcherInfo.attackUpLeft = info.attackUpLeft;
	this->elvenArcherInfo.attackUpRight = info.attackUpRight;
	this->elvenArcherInfo.attackDownLeft = info.attackDownLeft;
	this->elvenArcherInfo.attackDownRight = info.attackDownRight;

	this->elvenArcherInfo.deathUp = info.deathUp;
	this->elvenArcherInfo.deathDown = info.deathDown;

	this->size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	// Set the color of the entity
	color = ColorYellow;
	colorName = "YellowArcher";

	// Initialize the goals
	brain->RemoveAllSubgoals();

	// LifeBar creation
	UILifeBar_Info lifeBarInfo;
	lifeBarInfo.background = { 241,336,45,8 };
	lifeBarInfo.bar = { 241, 369,44,8 };
	lifeBarInfo.maxLife = this->maxLife;
	lifeBarInfo.life = this->currLife;
	lifeBarInfo.maxWidth = lifeBarInfo.bar.w;

	lifeBar = App->gui->CreateUILifeBar({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY }, lifeBarInfo, (j1Module*)this, nullptr, true);
	lifeBar->SetPriorityDraw(PriorityDraw_LIFEBAR_INGAME);

	// Collisions
	CreateEntityCollider(EntitySide_Player);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, this->unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	attackRadiusCollider = CreateRhombusCollider(ColliderType_PlayerAttackRadius, this->unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
	entityCollider->isTrigger = true;
	sightRadiusCollider->isTrigger = true;
	attackRadiusCollider->isTrigger = true;
}

void ElvenArcher::Move(float dt)
{
	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// ---------------------------------------------------------------------

	// Is the unit dead?
	/// The unit must fit the tile (it is more attractive for the player)
	if (singleUnit != nullptr) {

		if (currLife <= 0
			&& unitState != UnitState_Die
			&& singleUnit->IsFittingTile()
			&& !isDead) {

			App->audio->PlayFx(App->audio->GetFX().humanDeath, 0);

			isDead = true;

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			brain->RemoveAllSubgoals();

			unitState = UnitState_NoState;

			// Remove Movement (so other units can walk above them)
			App->entities->InvalidateMovementEntity(this);

			// Remove any path request
			pathPlanner->SetSearchRequested(false);
			pathPlanner->SetSearchCompleted(false);
			App->pathmanager->UnRegister(pathPlanner);

			if (singleUnit != nullptr)
				delete singleUnit;
			singleUnit = nullptr;

			if (!App->gui->isGuiCleanUp) {

				if (lifeBar != nullptr)

					lifeBar->isActive = false;
			}

			// Invalidate colliders
			sightRadiusCollider->isValid = false;
			attackRadiusCollider->isValid = false;
			entityCollider->isValid = false;

			LOG("An Elven Archer has died");
		}
	}

	if (!isDead && isValid) {

		// PROCESS THE COMMANDS

		// 1. Remove attack
		switch (unitCommand) {

		case UnitCommand_Stop:
		case UnitCommand_MoveToPosition:
		case UnitCommand_Patrol:
		case UnitCommand_GatherGold:
		case UnitCommand_HealRunestone:
		case UnitCommand_RescuePrisoner:

			/// The unit could be attacking before this command
			if (currTarget != nullptr) {

				if (!currTarget->isRemoved && currTarget->target != nullptr)

					currTarget->target->RemoveAttackingUnit(this);

				currTarget = nullptr;
			}

			isHitting = false;
			///

			break;

		case UnitCommand_NoCommand:
		default:

			break;
		}

		// 2. Actual command
		switch (unitCommand) {

		case UnitCommand_Stop:

			if (singleUnit->IsFittingTile()) {

				brain->RemoveAllSubgoals();

				unitState = UnitState_Idle;
				unitCommand = UnitCommand_NoCommand;
			}

			break;

		case UnitCommand_MoveToPosition:

			// The goal of the unit has been changed manually
			if (singleUnit->isGoalChanged) {

				if (singleUnit->IsFittingTile()) {

					brain->RemoveAllSubgoals();
					brain->AddGoal_MoveToPosition(singleUnit->goal);

					isRunAway = true;

					unitState = UnitState_Walk;
					unitCommand = UnitCommand_NoCommand;
				}
			}

			break;

		case UnitCommand_Patrol:

			// The goal of the unit has been changed manually (to patrol)
			if (singleUnit->isGoalChanged) {

				if (singleUnit->IsFittingTile()) {

					brain->RemoveAllSubgoals();
					brain->AddGoal_Patrol(singleUnit->currTile, singleUnit->goal);

					unitState = UnitState_Patrol;
					unitCommand = UnitCommand_NoCommand;
				}
			}

			break;

		case UnitCommand_AttackTarget:

			if (currTarget != nullptr) {

				if (singleUnit->IsFittingTile()) {

					brain->RemoveAllSubgoals();
					brain->AddGoal_AttackTarget(currTarget);

					unitState = UnitState_AttackTarget;
					unitCommand = UnitCommand_NoCommand;
				}
			}

			break;

		case UnitCommand_GatherGold:

			if (goldMine != nullptr) {

				if (goldMine->buildingState == BuildingState_Normal) {

					if (singleUnit->IsFittingTile()) {

						brain->RemoveAllSubgoals();
						brain->AddGoal_GatherGold(goldMine);

						unitState = UnitState_GatherGold;
						unitCommand = UnitCommand_NoCommand;
					}
				}
			}

			break;

		case UnitCommand_HealRunestone:

			if (runestone != nullptr) {

				if (runestone->buildingState == BuildingState_Normal) {

					if (singleUnit->IsFittingTile()) {

						brain->RemoveAllSubgoals();
						brain->AddGoal_HealRunestone(runestone);

						unitState = UnitState_HealRunestone;
						unitCommand = UnitCommand_NoCommand;
					}
				}
			}

			break;

		case UnitCommand_RescuePrisoner:

			if (prisoner != nullptr) {

				if (singleUnit->IsFittingTile()) {

					brain->RemoveAllSubgoals();
					brain->AddGoal_RescuePrisoner(prisoner);

					unitState = UnitState_RescuePrisoner;
					unitCommand = UnitCommand_NoCommand;
				}
			}

			break;

		case UnitCommand_NoCommand:
		default:

			break;
		}
	}

	if (!isDead)
		// PROCESS THE CURRENTLY ACTIVE GOAL
		brain->Process(dt);

	UnitStateMachine(dt);
	HandleInput(entityEvent);

	// Update FoW enemies
	/*
	list<TargetInfo*>::const_iterator it = targets.begin();

	while (it != targets.end()) {

		if ((*it)->target != nullptr && !(*it)->isRemovedFromSight && !(*it)->isRemoved) {

			if ((*it)->target->entityType == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dynEnt = (DynamicEntity*)(*it);
				dynEnt->SetLastSeenTile(App->map->WorldToMap(dynEnt->GetPos().x, dynEnt->GetPos().y));
			}
		}
		it++;
	}
	*/

	// Update animations
	if (!isStill || isHitting)
		UpdateAnimationsSpeed(dt);

	ChangeAnimation();

	if (!isDead && lastColliderUpdateTile != singleUnit->currTile) {

		// Update colliders
		UpdateEntityColliderPos();
		UpdateRhombusColliderPos(sightRadiusCollider, unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
		UpdateRhombusColliderPos(attackRadiusCollider, unitInfo.attackRadius, DistanceHeuristic_DistanceTo);

		lastColliderUpdateTile = singleUnit->currTile;
	}

	// Update Unit LifeBar
	if (lifeBar != nullptr) {

		lifeBar->SetLocalPos({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY });
		lifeBar->SetLife(currLife);
	}
}

void ElvenArcher::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (animation == &elvenArcherInfo.deathDown || animation == &elvenArcherInfo.deathUp) {

			offset = { animation->GetCurrentFrame().w / 6.3f, animation->GetCurrentFrame().h / 4.3f };
			App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_FloorColliders);
		}
		else {

			offset = { animation->GetCurrentFrame().w / 4.3f, animation->GetCurrentFrame().h / 2.1f };
			App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
		}
	}

	if (isSelected)
		DebugDrawSelected();
}

void ElvenArcher::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void ElvenArcher::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		/// SET ATTACK PARAMETERS
		// An enemy unit/building is within the SIGHT RADIUS of this player unit
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (c2->entity->entityType == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
				dynEnt->SetLastSeenTile(App->map->WorldToMap(dynEnt->GetPos().x, dynEnt->GetPos().y));
			}

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("Elven Archer Sight Radius %s", dynEnt->GetColorName().data());
			}

			// 1. UPDATE TARGETS LIST
			list<TargetInfo*>::const_iterator it = targets.begin();
			bool isTargetFound = false;

			// If the target is already in the targets list, set its isSightSatisfied to true
			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isRemovedFromSight = false;
					(*it)->isSightSatisfied = true;
					isTargetFound = true;
					break;
				}
				it++;
			}
			// Else, add the new target to the targets list (and set its isSightSatisfied to true)
			if (!isTargetFound) {

				TargetInfo* targetInfo = new TargetInfo();
				targetInfo->target = c2->entity;
				targetInfo->isSightSatisfied = true;

				targets.push_back(targetInfo);
			}

			// 2. MAKE UNIT FACE TOWARDS THE BEST TARGET
			if (targets.size() > 0) {

				bool isFacingTowardsTarget = false;

				// a) If the unit is not attacking any target
				if (currTarget == nullptr)
					isFacingTowardsTarget = true;
				else if (currTarget->target == nullptr)
					isFacingTowardsTarget = true;

				if (isFacingTowardsTarget) {

					// Face towards the best target (ONLY DYNAMIC ENTITIES!) (it is expected to be this target)
					TargetInfo* targetInfo = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

					if (targetInfo != nullptr) {

						fPoint orientation = { targetInfo->target->GetPos().x - pos.x, targetInfo->target->GetPos().y - pos.y };

						float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

						if (m > 0.0f) {
							orientation.x /= m;
							orientation.y /= m;
						}

						SetUnitDirectionByValue(orientation);
					}
				}
			}
		}

		// An enemy unit/building is within the ATTACK RADIUS of this player unit
		else if ((c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("Elven Archer Attack Radius %s", dynEnt->GetColorName().data());
			}

			// Set the target's isAttackSatisfied to true
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isRemovedFromSight = false;
					(*it)->isAttackSatisfied = true;
					break;
				}
				it++;
			}
		}
		break;

	case CollisionState_OnExit:

		/// RESET ATTACK PARAMETERS
		// An enemy unit/building is no longer within the SIGHT RADIUS of this player unit
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (c2->entity->entityType == EntityCategory_DYNAMIC_ENTITY) {

				DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
				dynEnt->SetLastSeenTile(App->map->WorldToMap(dynEnt->GetPos().x, dynEnt->GetPos().y));
			}

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Elven Archer Sight Radius %s", dynEnt->GetColorName().data());
			}

			// Set the target's isSightSatisfied to false
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isSightSatisfied = false;
					//(*it)->isAttackSatisfied = false;
					//(*it)->target->RemoveAttackingUnit(this);
					SetIsRemovedFromSightTargetInfo((*it)->target);
					break;
				}
				it++;
			}
		}

		// An enemy unit/building is no longer within the ATTACK RADIUS of this player unit
		else if ((c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Elven Archer Attack Radius %s", dynEnt->GetColorName().data());
			}

			// Set the target's isAttackSatisfied to false
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isAttackSatisfied = false;
					break;
				}
				it++;
			}
		}
		break;
	}
}

// State machine
void ElvenArcher::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_Walk:

		if (IsUnitGatheringGold() || IsUnitHealingRunestone() || IsUnitRescuingPrisoner())
			break;

		// DEFENSE NOTE: the unit automatically attacks back their attacking units (if they have any attacking units) to defend themselves
		if (unitsAttacking.size() > 0) {

			if (singleUnit->IsFittingTile()) {

				// If the unit is not ordered to run away...
				if (!isRunAway) {

					if (currTarget == nullptr) {

						// Check if there are available targets (DYNAMIC ENTITY) 
						newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

						if (newTarget != nullptr) {

							if (SetCurrTarget(newTarget->target))
								//currTarget = newTarget;
								brain->AddGoal_AttackTarget(currTarget, false);
						}
					}
				}
			}
		}

		break;

	case UnitState_Idle:

		if (IsUnitGatheringGold() || IsUnitHealingRunestone() || IsUnitRescuingPrisoner())
			break;

		// If the unit is doing nothing, make it look around
		if (brain->GetSubgoalsList().size() == 0)

			brain->AddGoal_LookAround(1, 3, 1, 2, 2);

	case UnitState_Patrol:

		if (IsUnitGatheringGold() || IsUnitHealingRunestone() || IsUnitRescuingPrisoner())
			break;

		// ATTACK NOTE (Idle and Patrol states): the unit automatically attacks any target (only DYNAMIC ENTITIES) that is in their targets list

		if (singleUnit->IsFittingTile()) {

			// Check if there are available targets (DYNAMIC ENTITY)
			newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

			if (newTarget != nullptr) {

				// A new target has found! Update the currTarget
				if (currTarget != newTarget) {

					// Anticipate the removing of this unit from the attacking units of the target
					if (currTarget != nullptr) {

						/*
						if (!currTarget->isRemoved)

							currTarget->target->RemoveAttackingUnit(this);
							*/
					}

					isHitting = false;

					if (SetCurrTarget(newTarget->target))
						//currTarget = newTarget;
						brain->AddGoal_AttackTarget(currTarget);
				}
			}
		}

		break;

	case UnitState_AttackTarget:

		if (IsUnitGatheringGold() || IsUnitHealingRunestone() || IsUnitRescuingPrisoner())
			break;

		// ATTACK NOTE (Attack state): if currTarget is dead, the unit automatically attacks the next target (only DYNAMIC ENTITIES) from their targets list

		if (singleUnit->IsFittingTile()) {

			if (currTarget == nullptr) {

				// Check if there are available targets (DYNAMIC ENTITY) 
				newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

				if (newTarget != nullptr) {

					if (SetCurrTarget(newTarget->target))
						//currTarget = newTarget;
						brain->AddGoal_AttackTarget(currTarget);
				}
			}
		}

		break;

	case UnitState_HealRunestone:
	case UnitState_GatherGold:
	case UnitState_RescuePrisoner:

		break;

	case UnitState_Die:

		// Remove the corpse when a certain time is reached
		if (deadTimer.ReadSec() >= TIME_REMOVE_CORPSE)
			isRemove = true;

		break;

	case UnitState_NoState:
	default:

		break;
	}

	/// DEFENSE
	if (unitsAttacking.size() == 0)

		isRunAway = false;
}

// -------------------------------------------------------------

// Animations
void ElvenArcher::LoadAnimationsSpeed()
{
	upSpeed = elvenArcherInfo.up.speed;
	downSpeed = elvenArcherInfo.down.speed;
	leftSpeed = elvenArcherInfo.left.speed;
	rightSpeed = elvenArcherInfo.right.speed;
	upLeftSpeed = elvenArcherInfo.upLeft.speed;
	upRightSpeed = elvenArcherInfo.upRight.speed;
	downLeftSpeed = elvenArcherInfo.downLeft.speed;
	downRightSpeed = elvenArcherInfo.downRight.speed;

	attackUpSpeed = elvenArcherInfo.attackUp.speed;
	attackDownSpeed = elvenArcherInfo.attackDown.speed;
	attackLeftSpeed = elvenArcherInfo.attackLeft.speed;
	attackRightSpeed = elvenArcherInfo.attackRight.speed;
	attackUpLeftSpeed = elvenArcherInfo.attackUpLeft.speed;
	attackUpRightSpeed = elvenArcherInfo.attackUpRight.speed;
	attackDownLeftSpeed = elvenArcherInfo.attackDownLeft.speed;
	attackDownRightSpeed = elvenArcherInfo.attackDownRight.speed;

	deathUpSpeed = elvenArcherInfo.deathUp.speed;
	deathDownSpeed = elvenArcherInfo.deathDown.speed;
}

void ElvenArcher::UpdateAnimationsSpeed(float dt)
{
	elvenArcherInfo.up.speed = upSpeed * dt;
	elvenArcherInfo.down.speed = downSpeed * dt;
	elvenArcherInfo.left.speed = leftSpeed * dt;
	elvenArcherInfo.right.speed = rightSpeed * dt;
	elvenArcherInfo.upLeft.speed = upLeftSpeed * dt;
	elvenArcherInfo.upRight.speed = upRightSpeed * dt;
	elvenArcherInfo.downLeft.speed = downLeftSpeed * dt;
	elvenArcherInfo.downRight.speed = downRightSpeed * dt;

	elvenArcherInfo.attackUp.speed = attackUpSpeed * dt;
	elvenArcherInfo.attackDown.speed = attackDownSpeed * dt;
	elvenArcherInfo.attackLeft.speed = attackLeftSpeed * dt;
	elvenArcherInfo.attackRight.speed = attackRightSpeed * dt;
	elvenArcherInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	elvenArcherInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	elvenArcherInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	elvenArcherInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	elvenArcherInfo.deathUp.speed = deathUpSpeed * dt;
	elvenArcherInfo.deathDown.speed = deathDownSpeed * dt;
}

bool ElvenArcher::ChangeAnimation()
{
	bool ret = false;

	// The unit is dead
	if (isDead) {

		UnitDirection dir = GetUnitDirection();

		if (dir == UnitDirection_Up || dir == UnitDirection_Up || dir == UnitDirection_UpLeft || dir == UnitDirection_UpRight
			|| dir == UnitDirection_Left || dir == UnitDirection_Right || dir == UnitDirection_NoDirection) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &elvenArcherInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &elvenArcherInfo.deathDown;
			ret = true;
		}

		return ret;
	}

	// The unit is hitting their target
	else if (isHitting) {

		// Set the direction of the unit as the orientation towards the target
		if (currTarget != nullptr) {

			if (!currTarget->isRemoved && !currTarget->isRemovedFromSight
				&& currTarget->target != nullptr && currTarget->IsTargetPresent()) {

				fPoint orientation = { currTarget->target->GetPos().x - pos.x, currTarget->target->GetPos().y - pos.y };

				float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

				if (m > 0.0f) {
					orientation.x /= m;
					orientation.y /= m;
				}

				SetUnitDirectionByValue(orientation);
			}
		}

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			animation = &elvenArcherInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &elvenArcherInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &elvenArcherInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &elvenArcherInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &elvenArcherInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &elvenArcherInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &elvenArcherInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &elvenArcherInfo.attackDownRight;
			ret = true;
			break;
		}

		return ret;
	}

	// The unit is either moving or still
	else {

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			if (isStill) {

				elvenArcherInfo.up.loop = false;
				elvenArcherInfo.up.Reset();
				elvenArcherInfo.up.speed = 0.0f;
			}
			else
				elvenArcherInfo.up.loop = true;

			animation = &elvenArcherInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				elvenArcherInfo.down.loop = false;
				elvenArcherInfo.down.Reset();
				elvenArcherInfo.down.speed = 0.0f;
			}
			else
				elvenArcherInfo.down.loop = true;

			animation = &elvenArcherInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				elvenArcherInfo.left.loop = false;
				elvenArcherInfo.left.Reset();
				elvenArcherInfo.left.speed = 0.0f;
			}
			else
				elvenArcherInfo.left.loop = true;

			animation = &elvenArcherInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				elvenArcherInfo.right.loop = false;
				elvenArcherInfo.right.Reset();
				elvenArcherInfo.right.speed = 0.0f;
			}
			else
				elvenArcherInfo.right.loop = true;

			animation = &elvenArcherInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				elvenArcherInfo.upLeft.loop = false;
				elvenArcherInfo.upLeft.Reset();
				elvenArcherInfo.upLeft.speed = 0.0f;
			}
			else
				elvenArcherInfo.upLeft.loop = true;

			animation = &elvenArcherInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				elvenArcherInfo.upRight.loop = false;
				elvenArcherInfo.upRight.Reset();
				elvenArcherInfo.upRight.speed = 0.0f;
			}
			else
				elvenArcherInfo.upRight.loop = true;

			animation = &elvenArcherInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				elvenArcherInfo.downLeft.loop = false;
				elvenArcherInfo.downLeft.Reset();
				elvenArcherInfo.downLeft.speed = 0.0f;
			}
			else
				elvenArcherInfo.downLeft.loop = true;

			animation = &elvenArcherInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				elvenArcherInfo.downRight.loop = false;
				elvenArcherInfo.downRight.Reset();
				elvenArcherInfo.downRight.speed = 0.0f;
			}
			else
				elvenArcherInfo.downRight.loop = true;

			animation = &elvenArcherInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}

float ElvenArcher::GetArrowSpeed() const
{
	return elvenArcherInfo.arrowSpeed;
}