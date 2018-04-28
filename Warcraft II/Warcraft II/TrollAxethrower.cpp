#include "Defs.h"
#include "p2Log.h"

#include "TrollAxethrower.h"

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
#include "j1Player.h"
#include "j1Printer.h"

#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

TrollAxethrower::TrollAxethrower(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), trollAxethrowerInfo(trollAxethrowerInfo)
{
	// XML loading
	/// Animations
	TrollAxethrowerInfo info = (TrollAxethrowerInfo&)App->entities->GetUnitInfo(EntityType_TROLL_AXETHROWER);
	this->unitInfo = this->trollAxethrowerInfo.unitInfo;
	this->trollAxethrowerInfo.up = info.up;
	this->trollAxethrowerInfo.down = info.down;
	this->trollAxethrowerInfo.left = info.left;
	this->trollAxethrowerInfo.right = info.right;
	this->trollAxethrowerInfo.upLeft = info.upLeft;
	this->trollAxethrowerInfo.upRight = info.upRight;
	this->trollAxethrowerInfo.downLeft = info.downLeft;
	this->trollAxethrowerInfo.downRight = info.downRight;

	this->trollAxethrowerInfo.attackUp = info.attackUp;
	this->trollAxethrowerInfo.attackDown = info.attackDown;
	this->trollAxethrowerInfo.attackLeft = info.attackLeft;
	this->trollAxethrowerInfo.attackRight = info.attackRight;
	this->trollAxethrowerInfo.attackUpLeft = info.attackUpLeft;
	this->trollAxethrowerInfo.attackUpRight = info.attackUpRight;
	this->trollAxethrowerInfo.attackDownLeft = info.attackDownLeft;
	this->trollAxethrowerInfo.attackDownRight = info.attackDownRight;

	this->trollAxethrowerInfo.deathUp = info.deathUp;
	this->trollAxethrowerInfo.deathDown = info.deathDown;

	size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	// Initialize the goals
	brain->RemoveAllSubgoals();

	// Collisions
	CreateEntityCollider(EntitySide_Enemy);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, this->unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	attackRadiusCollider = CreateRhombusCollider(ColliderType_EnemyAttackRadius, this->unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
	entityCollider->isTrigger = true;
	sightRadiusCollider->isTrigger = true;
	attackRadiusCollider->isTrigger = true;

	// LifeBar creation
	UILifeBar_Info lifeBarInfo;
	lifeBarInfo.background = { 241,336,45,8 };
	lifeBarInfo.bar = { 241, 327,44,8 };
	lifeBarInfo.maxLife = this->maxLife;
	lifeBarInfo.life = this->currLife;
	lifeBarInfo.maxWidth = lifeBarInfo.bar.w;

	lifeBar = App->gui->CreateUILifeBar({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY }, lifeBarInfo, (j1Module*)this, nullptr, true);
	lifeBar->SetPriorityDraw(PriorityDraw_LIFEBAR_INGAME);
}

void TrollAxethrower::Move(float dt)
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

			App->audio->PlayFx(App->audio->GetFX().orcDeath, 0);

			isDead = true;
			isValid = false;
			App->player->enemiesKill++;

			App->player->currentGold += 15;

			App->scene->hasGoldChanged = true;

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			// Initialize the goals
			brain->RemoveAllSubgoals();

			unitState = UnitState_Idle;

			// Remove Movement (so other units can walk above them)
			App->entities->InvalidateMovementEntity(this);
			//App->entities->InvalidateAttackEntity(this);

			if (singleUnit != nullptr)
				delete singleUnit;
			singleUnit = nullptr;

			//if (lifeBar != nullptr)
			//	App->gui->RemoveElem((UIElement**)lifeBar);

			// Invalidate colliders
			sightRadiusCollider->isValid = false;
			attackRadiusCollider->isValid = false;
			entityCollider->isValid = false;
		}
	}

	if (!isDead && isValid) {

		/// GOAL: MoveToPosition
		// The goal of the unit has been changed manually
		if (singleUnit->isGoalChanged)

			brain->AddGoal_MoveToPosition(singleUnit->goal);

		/// GOAL: AttackTarget
		// Check if there are available targets
		/// Prioritize a type of target (static or dynamic)
		/*
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
		*/
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

void TrollAxethrower::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (animation == &trollAxethrowerInfo.deathDown || animation == &trollAxethrowerInfo.deathUp)
			offset = { animation->GetCurrentFrame().w / 6.0f, animation->GetCurrentFrame().h / 3.8f };
		else
			offset = { animation->GetCurrentFrame().w / 4.3f, animation->GetCurrentFrame().h / 4.0f };

		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
	}

	if (isSelected)
		DebugDrawSelected();
}

void TrollAxethrower::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void TrollAxethrower::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		// An player is within the sight of this enemy unit
		if (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//		LOG("Enemy Sight Radius %s", dynEnt->GetColorName().data());

			// The Alliance is within the SIGHT radius

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
		}
		else if (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("Enemy Attack Radius %s", dynEnt->GetColorName().data());

			// The Alliance is within the ATTACK radius

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
		if (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("NO MORE Enemy Sight Radius %s", dynEnt->GetColorName().data());

			// The Alliance is NO longer within the SIGHT radius

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
		else if (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("NO MORE Enemy Attack Radius %s", dynEnt->GetColorName().data());

			// The Alliance is NO longer within the ATTACK radius

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
void TrollAxethrower::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_AttackTarget:

		break;

	case UnitState_Patrol:

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
void TrollAxethrower::LoadAnimationsSpeed()
{
	upSpeed = trollAxethrowerInfo.up.speed;
	downSpeed = trollAxethrowerInfo.down.speed;
	leftSpeed = trollAxethrowerInfo.left.speed;
	rightSpeed = trollAxethrowerInfo.right.speed;
	upLeftSpeed = trollAxethrowerInfo.upLeft.speed;
	upRightSpeed = trollAxethrowerInfo.upRight.speed;
	downLeftSpeed = trollAxethrowerInfo.downLeft.speed;
	downRightSpeed = trollAxethrowerInfo.downRight.speed;

	attackUpSpeed = trollAxethrowerInfo.attackUp.speed;
	attackDownSpeed = trollAxethrowerInfo.attackDown.speed;
	attackLeftSpeed = trollAxethrowerInfo.attackLeft.speed;
	attackRightSpeed = trollAxethrowerInfo.attackRight.speed;
	attackUpLeftSpeed = trollAxethrowerInfo.attackUpLeft.speed;
	attackUpRightSpeed = trollAxethrowerInfo.attackUpRight.speed;
	attackDownLeftSpeed = trollAxethrowerInfo.attackDownLeft.speed;
	attackDownRightSpeed = trollAxethrowerInfo.attackDownRight.speed;

	deathUpSpeed = trollAxethrowerInfo.deathUp.speed;
	deathDownSpeed = trollAxethrowerInfo.deathDown.speed;
}

void TrollAxethrower::UpdateAnimationsSpeed(float dt)
{
	trollAxethrowerInfo.up.speed = upSpeed * dt;
	trollAxethrowerInfo.down.speed = downSpeed * dt;
	trollAxethrowerInfo.left.speed = leftSpeed * dt;
	trollAxethrowerInfo.right.speed = rightSpeed * dt;
	trollAxethrowerInfo.upLeft.speed = upLeftSpeed * dt;
	trollAxethrowerInfo.upRight.speed = upRightSpeed * dt;
	trollAxethrowerInfo.downLeft.speed = downLeftSpeed * dt;
	trollAxethrowerInfo.downRight.speed = downRightSpeed * dt;

	trollAxethrowerInfo.attackUp.speed = attackUpSpeed * dt;
	trollAxethrowerInfo.attackDown.speed = attackDownSpeed * dt;
	trollAxethrowerInfo.attackLeft.speed = attackLeftSpeed * dt;
	trollAxethrowerInfo.attackRight.speed = attackRightSpeed * dt;
	trollAxethrowerInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	trollAxethrowerInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	trollAxethrowerInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	trollAxethrowerInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	trollAxethrowerInfo.deathUp.speed = deathUpSpeed * dt;
	trollAxethrowerInfo.deathDown.speed = deathDownSpeed * dt;
}

bool TrollAxethrower::ChangeAnimation()
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

			animation = &trollAxethrowerInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &trollAxethrowerInfo.deathDown;
			ret = true;
		}

		return ret;
	}

	// The unit is hitting their target
	else if (isHitting) {

		// Set the direction of the unit as the orientation towards the target
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

			animation = &trollAxethrowerInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &trollAxethrowerInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &trollAxethrowerInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &trollAxethrowerInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &trollAxethrowerInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &trollAxethrowerInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &trollAxethrowerInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &trollAxethrowerInfo.attackDownRight;
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

				trollAxethrowerInfo.up.loop = false;
				trollAxethrowerInfo.up.Reset();
				trollAxethrowerInfo.up.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.up.loop = true;

			animation = &trollAxethrowerInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				trollAxethrowerInfo.down.loop = false;
				trollAxethrowerInfo.down.Reset();
				trollAxethrowerInfo.down.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.down.loop = true;

			animation = &trollAxethrowerInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				trollAxethrowerInfo.left.loop = false;
				trollAxethrowerInfo.left.Reset();
				trollAxethrowerInfo.left.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.left.loop = true;

			animation = &trollAxethrowerInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				trollAxethrowerInfo.right.loop = false;
				trollAxethrowerInfo.right.Reset();
				trollAxethrowerInfo.right.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.right.loop = true;

			animation = &trollAxethrowerInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				trollAxethrowerInfo.upLeft.loop = false;
				trollAxethrowerInfo.upLeft.Reset();
				trollAxethrowerInfo.upLeft.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.upLeft.loop = true;

			animation = &trollAxethrowerInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				trollAxethrowerInfo.upRight.loop = false;
				trollAxethrowerInfo.upRight.Reset();
				trollAxethrowerInfo.upRight.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.upRight.loop = true;

			animation = &trollAxethrowerInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				trollAxethrowerInfo.downLeft.loop = false;
				trollAxethrowerInfo.downLeft.Reset();
				trollAxethrowerInfo.downLeft.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.downLeft.loop = true;

			animation = &trollAxethrowerInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				trollAxethrowerInfo.downRight.loop = false;
				trollAxethrowerInfo.downRight.Reset();
				trollAxethrowerInfo.downRight.speed = 0.0f;
			}
			else
				trollAxethrowerInfo.downRight.loop = true;

			animation = &trollAxethrowerInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}

float TrollAxethrower::GetAxeSpeed() const
{
	return trollAxethrowerInfo.axeSpeed;
}