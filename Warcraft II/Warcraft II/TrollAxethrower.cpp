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
	pathPlanner->SetIsInSameRoomChecked(true);

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

	this->size = this->unitInfo.size;
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

	// IA
	spawnTile = { singleUnit->currTile.x, singleUnit->currTile.y };

	// Different behaviors for units on the base and units around the map
	if (!App->map->IsOnBase(spawnTile))
		brain->AddGoal_Wander(5, spawnTile, false, 1, 3, 1, 2, 2);
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

			// Give gold to the player
			App->player->currentGold += trollAxethrowerInfo.droppedGold;
			App->scene->hasGoldChanged = true;

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			// Initialize the goals
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

void TrollAxethrower::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };

		if (animation == &trollAxethrowerInfo.deathDown || animation == &trollAxethrowerInfo.deathUp) {

			offset = { animation->GetCurrentFrame().w / 6.0f, animation->GetCurrentFrame().h / 3.8f };
			App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_FloorColliders);
		}
		else {

			offset = { animation->GetCurrentFrame().w / 4.3f, animation->GetCurrentFrame().h / 4.0f };
			App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
		}
	}

	//if (isSelected)
		//DebugDrawSelected();
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

		/// SET ATTACK PARAMETERS
		// An player unit/building is within the SIGHT RADIUS of this enemy unit
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			//DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
			//LOG("Enemy Sight Radius %s", dynEnt->GetColorName().data());

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
				// b) If the unit is attacking a static target
				else if (currTarget->target->entityType == EntityCategory_STATIC_ENTITY)
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

		// An player unit/building is within the ATTACK RADIUS of this enemy unit
		else if ((c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			//DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
			//LOG("Enemy Attack Radius %s", dynEnt->GetColorName().data());

			// Set the target's isAttackSatisfied to true
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

		/// RESET ATTACK PARAMETERS
		// An player unit/building is no longer within the SIGHT RADIUS of this enemy unit
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			//DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
			//LOG("NO MORE Enemy Sight Radius %s", dynEnt->GetColorName().data());

			// Set the target's isSightSatisfied to false
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isSightSatisfied = false;
					RemoveTargetInfo(*it);
					break;
				}
				it++;
			}
		}

		// An player unit/building is no longer within the ATTACK RADIUS of this enemy unit
		else if ((c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			//DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
			//LOG("NO MORE Enemy Attack Radius %s", dynEnt->GetColorName().data());

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
void TrollAxethrower::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_AttackTarget:

		if (singleUnit->IsFittingTile()) {

			if (currTarget == nullptr) {

				// Check if there are available targets (DYNAMIC ENTITY) 
				newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

				if (newTarget != nullptr) {

					currTarget = newTarget;
					brain->AddGoal_AttackTarget(currTarget);
				}
				else
					unitState = UnitState_Idle;
			}
		}

		break;

		break;

	case UnitState_Idle:
	case UnitState_Wander:
	case UnitState_Patrol:

		/// Goal_AttackTarget

		if (singleUnit->IsFittingTile()) {

			// Check if there are available targets (DYNAMIC ENTITY)
			newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

			if (newTarget != nullptr) {

				// A new target has found! Update the currTarget
				if (currTarget != newTarget) {

					// Anticipate the removing of this unit from the attacking units of the target
					if (currTarget != nullptr) {

						if (!currTarget->isRemoved)

							currTarget->target->RemoveAttackingUnit(this);
					}

					isHitting = false;

					currTarget = newTarget;
					brain->AddGoal_AttackTarget(currTarget);
				}
			}
		}

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