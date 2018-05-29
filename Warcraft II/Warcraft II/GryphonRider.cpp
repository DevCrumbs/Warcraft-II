#include "Defs.h"
#include "p2Log.h"

#include "GryphonRider.h"

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
#include "j1Printer.h"

#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

// Ignores the walkability of the map

GryphonRider::GryphonRider(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), gryphonRiderInfo(gryphonRiderInfo)
{
	pathPlanner->SetIsWalkabilityChecked(false);

	// XML loading
	/// Animations
	GryphonRiderInfo info = (GryphonRiderInfo&)App->entities->GetUnitInfo(EntityType_GRYPHON_RIDER);
	this->unitInfo = this->gryphonRiderInfo.unitInfo;
	this->gryphonRiderInfo.up = info.up;
	this->gryphonRiderInfo.down = info.down;
	this->gryphonRiderInfo.left = info.left;
	this->gryphonRiderInfo.right = info.right;
	this->gryphonRiderInfo.upLeft = info.upLeft;
	this->gryphonRiderInfo.upRight = info.upRight;
	this->gryphonRiderInfo.downLeft = info.downLeft;
	this->gryphonRiderInfo.downRight = info.downRight;

	this->gryphonRiderInfo.attackUp = info.attackUp;
	this->gryphonRiderInfo.attackDown = info.attackDown;
	this->gryphonRiderInfo.attackLeft = info.attackLeft;
	this->gryphonRiderInfo.attackRight = info.attackRight;
	this->gryphonRiderInfo.attackUpLeft = info.attackUpLeft;
	this->gryphonRiderInfo.attackUpRight = info.attackUpRight;
	this->gryphonRiderInfo.attackDownLeft = info.attackDownLeft;
	this->gryphonRiderInfo.attackDownRight = info.attackDownRight;

	this->gryphonRiderInfo.deathUp = info.deathUp;
	this->gryphonRiderInfo.deathDown = info.deathDown;

	this->size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	// Set the color of the entity
	color = ColorWhite;
	colorName = "WhiteGryphon";

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

void GryphonRider::Move(float dt)
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

			isBlitSavedGroupSelection = false;
			isBlitSelectedGroupSelection = false;

			App->audio->PlayFx(App->audio->GetFX().griffonDeath, 0); //Gryphon death

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

			LOG("A Gryphon Rider has died");
		}
	}

	// Update currTarget
	if (currTarget != nullptr) {

		if (currTarget->isRemoveNeeded || currTarget->target->isRemove)
			currTarget = nullptr;
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
					brain->AddGoal_MoveToPosition(singleUnit->goal, true, false);

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

			if (newTarget != nullptr) {

				if (singleUnit->IsFittingTile()) {

					brain->RemoveAllSubgoals();
					brain->AddGoal_AttackTarget(newTarget);

					newTarget = nullptr;

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

	if (!isDead) {

		// PROCESS THE CURRENTLY ACTIVE GOAL
		brain->Process(dt);

		// Update targets to be removed
		UpdateTargetsToRemove();
	}

	UnitStateMachine(dt);
	HandleInput(entityEvent);

	// Update animations
	UpdateAnimationsSpeed(dt);

	ChangeAnimation();

	if (!isDead && lastColliderUpdateTile != singleUnit->currTile) {

		// Update colliders
		UpdateEntityColliderPos();
		UpdateRhombusColliderPos(sightRadiusCollider, unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
		UpdateRhombusColliderPos(attackRadiusCollider, unitInfo.attackRadius, DistanceHeuristic_DistanceTo);

		lastColliderUpdateTile = singleUnit->currTile;
	}

	// Update unit's life bar
	if (lifeBar != nullptr) {

		lifeBar->SetLocalPos({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY });
		lifeBar->SetLife(currLife);
		lifeBar->SetLifeBarPosition({ 0,0 });
	}

	// Blit group selection
	if (isBlitSelectedGroupSelection) {

		float alphaSpeed = 250.0f;
		alphaSelectedGroupSelection -= alphaSpeed * dt;

		if (alphaSelectedGroupSelection <= 0) {
			alphaSelectedGroupSelection = 0;
			isBlitSelectedGroupSelection = false;
		}

		SDL_Color alphaColor = color;
		alphaColor.a = alphaSelectedGroupSelection;

		const SDL_Rect entitySizeA = { pos.x + offsetSize.x - 1, pos.y + offsetSize.y - 1, size.x + 2, size.y + 2 };
		const SDL_Rect entitySizeB = { pos.x + offsetSize.x - 2, pos.y + offsetSize.y - 2, size.x + 4, size.y + 4 };
		const SDL_Rect entitySizeC = { pos.x + offsetSize.x - 3, pos.y + offsetSize.y - 3, size.x + 6, size.y + 6 };
		App->printer->PrintQuad(entitySizeA, alphaColor);
		App->printer->PrintQuad(entitySizeB, alphaColor);
		App->printer->PrintQuad(entitySizeC, alphaColor);
	}
	if (isBlitSavedGroupSelection) {

		float alphaSpeed = 250.0f;
		alphaSavedGroupSelection -= alphaSpeed * dt;

		if (alphaSavedGroupSelection <= 0) {
			alphaSavedGroupSelection = 0;
			isBlitSavedGroupSelection = false;
		}

		SDL_Color alphaColor = color;
		alphaColor.a = alphaSavedGroupSelection;

		const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
		App->printer->PrintQuad(entitySize, alphaColor, true, true, Layers_FloorColliders);
	}
}

void GryphonRider::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (animation == &gryphonRiderInfo.deathDown || animation == &gryphonRiderInfo.deathUp)
			offset = { animation->GetCurrentFrame().w / 2.8f, animation->GetCurrentFrame().h / 2.3f };
		else
			offset = { animation->GetCurrentFrame().w / 2.8f, animation->GetCurrentFrame().h / 2.5f };

		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_DragonGryphon);
	}

	if (isSelected)
		DebugDrawSelected();
}

void GryphonRider::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void GryphonRider::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
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
				LOG("Gryphon Rider Sight Radius %s", dynEnt->GetColorName().data());
			}

			// 1. UPDATE TARGETS LIST
			list<TargetInfo*>::const_iterator it = targets.begin();
			bool isTargetFound = false;

			// If the target is already in the targets list, set its isSightSatisfied to true
			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

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
				LOG("Gryphon Rider Attack Radius %s", dynEnt->GetColorName().data());
			}

			// 1. UPDATE TARGETS LIST
			list<TargetInfo*>::const_iterator it = targets.begin();
			bool isTargetFound = false;

			// If the target is already in the targets list, set its isAttackSatisfied + isSightSatisfied to true
			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isSightSatisfied = true;
					(*it)->isAttackSatisfied = true;
					isTargetFound = true;
					break;
				}
				it++;
			}
			// Else, add the new target to the targets list (and set its isAttackSatisfied + isSightSatisfied to true)
			if (!isTargetFound) {

				TargetInfo* targetInfo = new TargetInfo();
				targetInfo->target = c2->entity;
				targetInfo->isSightSatisfied = true;
				targetInfo->isAttackSatisfied = true;

				targets.push_back(targetInfo);
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
				LOG("NO MORE Gryphon Rider Sight Radius %s", dynEnt->GetColorName().data());
			}

			// Set the target's isSightSatisfied to false
			list<TargetInfo*>::iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isSightSatisfied = false;

					// Removing target process --
					if (!(*it)->IsTargetDead())

						(*it)->target->RemoveAttackingUnit(this);

					if (currTarget == *it)

						InvalidateCurrTarget();

					if ((*it)->isInGoals > 0 && !(*it)->isRemoveNeeded) {

						(*it)->isRemoveNeeded = true;
						targetsToRemove.splice(targetsToRemove.begin(), targets, it);
					}
					else if (!(*it)->isRemoveNeeded) {

						delete *it;
						targets.remove(*it);
					}
					// -- Removing target process

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
				LOG("NO MORE Gryphon Rider Attack Radius %s", dynEnt->GetColorName().data());
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
void GryphonRider::UnitStateMachine(float dt)
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
								brain->AddGoal_AttackTarget(newTarget, false);

							newTarget = nullptr;
						}
					}
				}
			}
		}

		break;

	case UnitState_Idle:

		isRunAway = false;

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
					if (currTarget != nullptr)
						currTarget->target->RemoveAttackingUnit(this);

					isHitting = false;

					if (SetCurrTarget(newTarget->target))
						brain->AddGoal_AttackTarget(newTarget);

					newTarget = nullptr;
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
						brain->AddGoal_AttackTarget(newTarget);

					newTarget = nullptr;
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
}

// -------------------------------------------------------------

// Animations
void GryphonRider::LoadAnimationsSpeed()
{
	upSpeed = gryphonRiderInfo.up.speed;
	downSpeed = gryphonRiderInfo.down.speed;
	leftSpeed = gryphonRiderInfo.left.speed;
	rightSpeed = gryphonRiderInfo.right.speed;
	upLeftSpeed = gryphonRiderInfo.upLeft.speed;
	upRightSpeed = gryphonRiderInfo.upRight.speed;
	downLeftSpeed = gryphonRiderInfo.downLeft.speed;
	downRightSpeed = gryphonRiderInfo.downRight.speed;

	attackUpSpeed = gryphonRiderInfo.attackUp.speed;
	attackDownSpeed = gryphonRiderInfo.attackDown.speed;
	attackLeftSpeed = gryphonRiderInfo.attackLeft.speed;
	attackRightSpeed = gryphonRiderInfo.attackRight.speed;
	attackUpLeftSpeed = gryphonRiderInfo.attackUpLeft.speed;
	attackUpRightSpeed = gryphonRiderInfo.attackUpRight.speed;
	attackDownLeftSpeed = gryphonRiderInfo.attackDownLeft.speed;
	attackDownRightSpeed = gryphonRiderInfo.attackDownRight.speed;

	deathUpSpeed = gryphonRiderInfo.deathUp.speed;
	deathDownSpeed = gryphonRiderInfo.deathDown.speed;
}

void GryphonRider::UpdateAnimationsSpeed(float dt)
{
	gryphonRiderInfo.up.speed = upSpeed * dt;
	gryphonRiderInfo.down.speed = downSpeed * dt;
	gryphonRiderInfo.left.speed = leftSpeed * dt;
	gryphonRiderInfo.right.speed = rightSpeed * dt;
	gryphonRiderInfo.upLeft.speed = upLeftSpeed * dt;
	gryphonRiderInfo.upRight.speed = upRightSpeed * dt;
	gryphonRiderInfo.downLeft.speed = downLeftSpeed * dt;
	gryphonRiderInfo.downRight.speed = downRightSpeed * dt;

	gryphonRiderInfo.attackUp.speed = attackUpSpeed * dt;
	gryphonRiderInfo.attackDown.speed = attackDownSpeed * dt;
	gryphonRiderInfo.attackLeft.speed = attackLeftSpeed * dt;
	gryphonRiderInfo.attackRight.speed = attackRightSpeed * dt;
	gryphonRiderInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	gryphonRiderInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	gryphonRiderInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	gryphonRiderInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	gryphonRiderInfo.deathUp.speed = deathUpSpeed * dt;
	gryphonRiderInfo.deathDown.speed = deathDownSpeed * dt;
}

bool GryphonRider::ChangeAnimation()
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

			animation = &gryphonRiderInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &gryphonRiderInfo.deathDown;
			ret = true;
		}

		return ret;
	}

	// The unit is hitting their target
	else if (isHitting) {

		// Set the direction of the unit as the orientation towards the attacking target
		if (currTarget != nullptr) {

			fPoint orientation = { -1,-1 };

			if (currTarget->attackingTile.x != -1 && currTarget->attackingTile.y != -1) {

				iPoint attackingPos = App->map->MapToWorld(currTarget->attackingTile.x, currTarget->attackingTile.y);
				orientation = { attackingPos.x - pos.x, attackingPos.y - pos.y };
			}
			else
				orientation = { currTarget->target->GetPos().x - pos.x, currTarget->target->GetPos().y - pos.y };

			float m = sqrtf(pow(orientation.x, 2.0f) + pow(orientation.y, 2.0f));

			if (m > 0.0f) {
				orientation.x /= m;
				orientation.y /= m;
			}

			SetUnitDirectionByValue(orientation);
		}

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			animation = &gryphonRiderInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &gryphonRiderInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &gryphonRiderInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &gryphonRiderInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &gryphonRiderInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &gryphonRiderInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &gryphonRiderInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &gryphonRiderInfo.attackDownRight;
			ret = true;
			break;
		}

		return ret;
	}

	// The unit is either moving or still
	else {

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			animation = &gryphonRiderInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			animation = &gryphonRiderInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			animation = &gryphonRiderInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			animation = &gryphonRiderInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &gryphonRiderInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &gryphonRiderInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &gryphonRiderInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &gryphonRiderInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}

float GryphonRider::GetFireSpeed() const
{
	return gryphonRiderInfo.fireSpeed;
}