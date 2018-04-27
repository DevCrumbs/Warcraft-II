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

			// Remove life bar
			if (lifeBar != nullptr)
				App->gui->RemoveElem((UIElement**)lifeBar);

			// If the player dies, remove all their goals
			//unitCommand = UnitCommand_Stop;
		}
	}

	if (!isDead && isValid) {

		if (auxIsSelected != isSelected) {

			auxIsSelected = isSelected;

			if (isSelected)
				App->audio->PlayFx(19, 0);
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

					App->audio->PlayFx(17, 0);

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

void ElvenArcher::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (animation == &elvenArcherInfo.deathDown || animation == &elvenArcherInfo.deathUp)
			offset = { animation->GetCurrentFrame().w / 4.0f,0.0f };
		else
			offset = { animation->GetCurrentFrame().w / 4.0f, animation->GetCurrentFrame().h / 2.0f };

		//App->render->Blit(sprites, pos.x - offset.x, pos.y - offset.y, &(animation->GetCurrentFrame()));
		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
	}

	if (isSelected)
		DebugDrawSelected();
}

void ElvenArcher::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	//App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	App->printer->PrintQuad(entitySize, color);

	//for (uint i = 0; i < unitInfo.priority; ++i) {
		//const SDL_Rect entitySize = { pos.x + 2 * i, pos.y + 2 * i, size.x - 4 * i, size.y - 4 * i };
	//const SDL_Rect entitySize = { pos.x , pos.y, size.x, size.y };
	//App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	//}
}

void ElvenArcher::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		// An enemy is within the sight of this player unit
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
			|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
		//	LOG("Player Sight Radius %s", dynEnt->GetColorName().data());

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
		|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)) { // || c2->colliderType == ColliderType_PlayerBuilding

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
		|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)) { // || c2->colliderType == ColliderType_PlayerBuilding

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
		|| (c1->colliderType == ColliderType_PlayerAttackRadius && c2->colliderType == ColliderType_NeutralUnit)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
	//		LOG("NO MORE Player Attack Radius %s", dynEnt->GetColorName().data());

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
void ElvenArcher::UnitStateMachine(float dt)
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