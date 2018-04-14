#include "Defs.h"
#include "p2Log.h"

#include "Footman.h"

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

#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

Footman::Footman(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const FootmanInfo& footmanInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), footmanInfo(footmanInfo)
{
	// XML loading
	/// Animations
	FootmanInfo info = (FootmanInfo&)App->entities->GetUnitInfo(EntityType_FOOTMAN);
	this->footmanInfo.up = info.up;
	this->footmanInfo.down = info.down;
	this->footmanInfo.left = info.left;
	this->footmanInfo.right = info.right;
	this->footmanInfo.upLeft = info.upLeft;
	this->footmanInfo.upRight = info.upRight;
	this->footmanInfo.downLeft = info.downLeft;
	this->footmanInfo.downRight = info.downRight;

	this->footmanInfo.attackUp = info.attackUp;
	this->footmanInfo.attackDown = info.attackDown;
	this->footmanInfo.attackLeft = info.attackLeft;
	this->footmanInfo.attackRight = info.attackRight;
	this->footmanInfo.attackUpLeft = info.attackUpLeft;
	this->footmanInfo.attackUpRight = info.attackUpRight;
	this->footmanInfo.attackDownLeft = info.attackDownLeft;
	this->footmanInfo.attackDownRight = info.attackDownRight;

	this->footmanInfo.deathUp = info.deathUp;
	this->footmanInfo.deathDown = info.deathDown;

	LoadAnimationsSpeed();

	// Initialize the goals
	brain->RemoveAllSubgoals();

	// Collisions
	CreateEntityCollider(EntitySide_Player);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	attackRadiusCollider = CreateRhombusCollider(ColliderType_PlayerAttackRadius, unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
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

void Footman::Move(float dt)
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

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			// Remove Movement (so other units can walk above them)
			App->entities->InvalidateMovementEntity(this);

			if (singleUnit != nullptr)
				delete singleUnit;
			singleUnit = nullptr;

			// Invalidate colliders
			sightRadiusCollider->isValid = false;
			attackRadiusCollider->isValid = false;
			entityCollider->isValid = false;

			// If the player dies, remove all their goals
			unitCommand = UnitCommand_Stop;
		}
	}

	if (!isDead) {

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

				brain->RemoveAllSubgoals();
				brain->AddGoal_Patrol(singleUnit->currTile, singleUnit->goal);

				unitState = UnitState_Patrol;
				unitCommand = UnitCommand_NoCommand;
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

		case UnitCommand_NoCommand:
		default:

			break;
		}

		// ---------------------------------------------------------------------

		// PROCESS THE CURRENTLY ACTIVE GOAL
		brain->Process(dt);
	}

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

void Footman::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { animation->GetCurrentFrame().w / 4.0f, animation->GetCurrentFrame().h / 2.0f };
		App->render->Blit(sprites, pos.x - offset.x, pos.y - offset.y, &(animation->GetCurrentFrame()));
	}

	if (isSelected)
		DebugDrawSelected();
}

void Footman::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);

	for (uint i = 0; i < unitInfo.priority; ++i) {
		const SDL_Rect entitySize = { pos.x + 2 * i, pos.y + 2 * i, size.x - 4 * i, size.y - 4 * i };
		App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	}
}

void Footman::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		// An enemy is within the sight of this player unit
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)
		|| (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_NeutralUnit)) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			LOG("Player Sight Radius %s", dynEnt->GetColorName().data());

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
			LOG("Player Attack Radius %s", dynEnt->GetColorName().data());

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
			LOG("NO MORE Player Sight Radius %s", dynEnt->GetColorName().data());

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
			LOG("NO MORE Player Attack Radius %s", dynEnt->GetColorName().data());

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
void Footman::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_Walk:

		break;

	case UnitState_Patrol:

		break;

	case UnitState_AttackTarget:

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
void Footman::LoadAnimationsSpeed()
{
	upSpeed = footmanInfo.up.speed;
	downSpeed = footmanInfo.down.speed;
	leftSpeed = footmanInfo.left.speed;
	rightSpeed = footmanInfo.right.speed;
	upLeftSpeed = footmanInfo.upLeft.speed;
	upRightSpeed = footmanInfo.upRight.speed;
	downLeftSpeed = footmanInfo.downLeft.speed;
	downRightSpeed = footmanInfo.downRight.speed;

	attackUpSpeed = footmanInfo.attackUp.speed;
	attackDownSpeed = footmanInfo.attackDown.speed;
	attackLeftSpeed = footmanInfo.attackLeft.speed;
	attackRightSpeed = footmanInfo.attackRight.speed;
	attackUpLeftSpeed = footmanInfo.attackUpLeft.speed;
	attackUpRightSpeed = footmanInfo.attackUpRight.speed;
	attackDownLeftSpeed = footmanInfo.attackDownLeft.speed;
	attackDownRightSpeed = footmanInfo.attackDownRight.speed;

	deathUpSpeed = footmanInfo.deathUp.speed;
	deathDownSpeed = footmanInfo.deathDown.speed;
}

void Footman::UpdateAnimationsSpeed(float dt)
{
	footmanInfo.up.speed = upSpeed * dt;
	footmanInfo.down.speed = downSpeed * dt;
	footmanInfo.left.speed = leftSpeed * dt;
	footmanInfo.right.speed = rightSpeed * dt;
	footmanInfo.upLeft.speed = upLeftSpeed * dt;
	footmanInfo.upRight.speed = upRightSpeed * dt;
	footmanInfo.downLeft.speed = downLeftSpeed * dt;
	footmanInfo.downRight.speed = downRightSpeed * dt;

	footmanInfo.attackUp.speed = attackUpSpeed * dt;
	footmanInfo.attackDown.speed = attackDownSpeed * dt;
	footmanInfo.attackLeft.speed = attackLeftSpeed * dt;
	footmanInfo.attackRight.speed = attackRightSpeed * dt;
	footmanInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	footmanInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	footmanInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	footmanInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	footmanInfo.deathUp.speed = deathUpSpeed * dt;
	footmanInfo.deathDown.speed = deathDownSpeed * dt;
}

bool Footman::ChangeAnimation()
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

			animation = &footmanInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &footmanInfo.deathDown;
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

			animation = &footmanInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &footmanInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &footmanInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &footmanInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &footmanInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &footmanInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &footmanInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &footmanInfo.attackDownRight;
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

				footmanInfo.up.loop = false;
				footmanInfo.up.Reset();
				footmanInfo.up.speed = 0.0f;
			}
			else
				footmanInfo.up.loop = true;

			animation = &footmanInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				footmanInfo.down.loop = false;
				footmanInfo.down.Reset();
				footmanInfo.down.speed = 0.0f;
			}
			else
				footmanInfo.down.loop = true;

			animation = &footmanInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				footmanInfo.left.loop = false;
				footmanInfo.left.Reset();
				footmanInfo.left.speed = 0.0f;
			}
			else
				footmanInfo.left.loop = true;

			animation = &footmanInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				footmanInfo.right.loop = false;
				footmanInfo.right.Reset();
				footmanInfo.right.speed = 0.0f;
			}
			else
				footmanInfo.right.loop = true;

			animation = &footmanInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				footmanInfo.upLeft.loop = false;
				footmanInfo.upLeft.Reset();
				footmanInfo.upLeft.speed = 0.0f;
			}
			else
				footmanInfo.upLeft.loop = true;

			animation = &footmanInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				footmanInfo.upRight.loop = false;
				footmanInfo.upRight.Reset();
				footmanInfo.upRight.speed = 0.0f;
			}
			else
				footmanInfo.upRight.loop = true;

			animation = &footmanInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				footmanInfo.downLeft.loop = false;
				footmanInfo.downLeft.Reset();
				footmanInfo.downLeft.speed = 0.0f;
			}
			else
				footmanInfo.downLeft.loop = true;

			animation = &footmanInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				footmanInfo.downRight.loop = false;
				footmanInfo.downRight.Reset();
				footmanInfo.downRight.speed = 0.0f;
			}
			else
				footmanInfo.downRight.loop = true;

			animation = &footmanInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}