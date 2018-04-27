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

// Ignores the walkability of the map. Avoids other Dragons + Gryphon Riders. Goes above every other unit.

GryphonRider::GryphonRider(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), gryphonRiderInfo(gryphonRiderInfo)
{
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

	LoadAnimationsSpeed();

	// Initialize the goals
	brain->RemoveAllSubgoals();

	// Collisions
	CreateEntityCollider(EntitySide_Player);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, this->unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	attackRadiusCollider = CreateRhombusCollider(ColliderType_PlayerAttackRadius, this->unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
	entityCollider->isTrigger = true;
	sightRadiusCollider->isTrigger = true;
	attackRadiusCollider->isTrigger = true;

	// LifeBar creation
	UILifeBar_Info lifeBarInfo;
	lifeBarInfo.background = { 241,336,45,8 };
	lifeBarInfo.bar = { 241, 369,44,8 };
	lifeBarInfo.maxLife = this->maxLife;
	lifeBarInfo.life = this->currLife;
	lifeBarInfo.maxWidth = lifeBarInfo.bar.w;

	lifeBar = App->gui->CreateUILifeBar({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY }, lifeBarInfo, (j1Module*)this, nullptr, true);
	lifeBar->SetPriorityDraw(PriorityDraw_LIFEBAR_INGAME);

	auxIsSelected = isSelected;
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

			App->audio->PlayFx(12, 0);

			isDead = true;
			isValid = false;

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			brain->RemoveAllSubgoals();

			unitState = UnitState_Idle;

			// Remove Movement (so other units can walk above them)
			App->entities->InvalidateMovementEntity(this);
			App->entities->InvalidateAttackEntity(this);

			if (singleUnit != nullptr)
				delete singleUnit;
			singleUnit = nullptr;

			// Invalidate colliders
			sightRadiusCollider->isValid = false;
			attackRadiusCollider->isValid = false;
			entityCollider->isValid = false;

			// If the player dies, remove all their goals
			//unitCommand = UnitCommand_Stop;
		}
	}

	if (!isDead && isValid) {

		if (auxIsSelected != isSelected) {

			auxIsSelected = isSelected;

			if (isSelected)
				App->audio->PlayFx(22, 0);
		}

		// PROCESS THE COMMANDS
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

					App->audio->PlayFx(20, 0);

					brain->RemoveAllSubgoals();
					brain->AddGoal_MoveToPosition(singleUnit->goal);

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

		case UnitCommand_NoCommand:
		default:

			break;
		}
	}

	// PROCESS THE CURRENTLY ACTIVE GOAL
	brain->Process(dt);

	UnitStateMachine(dt);

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

void GryphonRider::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };
		offset = { animation->GetCurrentFrame().w / 3.0f, animation->GetCurrentFrame().h / 3.0f };

		//App->render->Blit(sprites, pos.x - offset.x, pos.y - offset.y, &(animation->GetCurrentFrame()));
		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
	}

	if (isSelected)
		DebugDrawSelected();
}

void GryphonRider::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	//App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	App->printer->PrintQuad(entitySize, color);

	//for (uint i = 0; i < unitInfo.priority; ++i) {
	//const SDL_Rect entitySize = { pos.x + 2 * i, pos.y + 2 * i, size.x - 4 * i, size.y - 4 * i };
	//const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	//App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	//}
}

void GryphonRider::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		// An enemy is within the sight of this player unit
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding)) { // || c2->colliderType == ColliderType_PlayerBuilding

																														 // Static Entity
			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//LOG("Player Sight Radius %s", dynEnt->GetColorName().data());

			// The Horde is within the SIGHT radius

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

			if (targets.size() > 0) {

				if (currTarget == nullptr) {

					// Face towards the closest target (to show the player that the unit is going to be attacked)
					TargetInfo* targetInfo = GetBestTargetInfo();

					if (targetInfo != nullptr) {

						if (targetInfo->target != nullptr) {

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
		}
		else if ((c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyBuilding)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("Player Attack Radius %s", dynEnt->GetColorName().data());

			// The Horde is within the ATTACK radius

			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isAttackSatisfied = true;
					break;
				}
				it++;
			}
		}

		break;

	case CollisionState_OnExit:

		// Reset attack parameters
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("NO MORE Player Sight Radius %s", dynEnt->GetColorName().data());

			// The Horde is NO longer within the SIGHT radius

			// Remove the target from the targets list
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					// If currTarget matches the target that needs to be removed, set its isSightSatisfied to false and it will be removed later		
					if (currTarget != nullptr) {

						if (currTarget->target == c2->entity) {

							currTarget->isSightSatisfied = false;
							break;
						}
					}
					// If currTarget is different from the target that needs to be removed, remove the target from the list
					delete *it;
					targets.erase(it);

					break;
				}
				it++;
			}
		}
		else if ((c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_EnemyBuilding)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("NO MORE Player Attack Radius %s", dynEnt->GetColorName().data());

			// The Horde is NO longer within the ATTACK radius

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

		break;

	case UnitState_Patrol:

		// Check if there are available targets
		/// Prioritize a type of target (static or dynamic)
		if (singleUnit->IsFittingTile()) {

			newTarget = GetBestTargetInfo();

			if (newTarget != nullptr) {

				// A new target has found, update the attacking target
				if (currTarget != newTarget) {

					if (currTarget != nullptr) {

						if (!currTarget->isRemoved) {

							currTarget->target->RemoveAttackingUnit(this);
							isHitting = false;
						}
					}

					currTarget = newTarget;
					brain->AddGoal_AttackTarget(currTarget);
				}
			}
		}

		break;

	case UnitState_AttackTarget:

		break;

	case UnitState_HealRunestone:
	case UnitState_GatherGold:

		break;

	case UnitState_Die:

		// Remove the corpse when a certain time is reached
		if (deadTimer.ReadSec() >= TIME_REMOVE_CORPSE)
			isRemove = true;

		break;

	case UnitState_Idle:
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

			if (!currTarget->isRemoved) {

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

			if (isStill) {

				gryphonRiderInfo.up.loop = false;
				gryphonRiderInfo.up.Reset();
				gryphonRiderInfo.up.speed = 0.0f;
			}
			else
				gryphonRiderInfo.up.loop = true;

			animation = &gryphonRiderInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				gryphonRiderInfo.down.loop = false;
				gryphonRiderInfo.down.Reset();
				gryphonRiderInfo.down.speed = 0.0f;
			}
			else
				gryphonRiderInfo.down.loop = true;

			animation = &gryphonRiderInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				gryphonRiderInfo.left.loop = false;
				gryphonRiderInfo.left.Reset();
				gryphonRiderInfo.left.speed = 0.0f;
			}
			else
				gryphonRiderInfo.left.loop = true;

			animation = &gryphonRiderInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				gryphonRiderInfo.right.loop = false;
				gryphonRiderInfo.right.Reset();
				gryphonRiderInfo.right.speed = 0.0f;
			}
			else
				gryphonRiderInfo.right.loop = true;

			animation = &gryphonRiderInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				gryphonRiderInfo.upLeft.loop = false;
				gryphonRiderInfo.upLeft.Reset();
				gryphonRiderInfo.upLeft.speed = 0.0f;
			}
			else
				gryphonRiderInfo.upLeft.loop = true;

			animation = &gryphonRiderInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				gryphonRiderInfo.upRight.loop = false;
				gryphonRiderInfo.upRight.Reset();
				gryphonRiderInfo.upRight.speed = 0.0f;
			}
			else
				gryphonRiderInfo.upRight.loop = true;

			animation = &gryphonRiderInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				gryphonRiderInfo.downLeft.loop = false;
				gryphonRiderInfo.downLeft.Reset();
				gryphonRiderInfo.downLeft.speed = 0.0f;
			}
			else
				gryphonRiderInfo.downLeft.loop = true;

			animation = &gryphonRiderInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				gryphonRiderInfo.downRight.loop = false;
				gryphonRiderInfo.downRight.Reset();
				gryphonRiderInfo.downRight.speed = 0.0f;
			}
			else
				gryphonRiderInfo.downRight.loop = true;

			animation = &gryphonRiderInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}