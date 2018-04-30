#include "Defs.h"
#include "p2Log.h"

#include "Dragon.h"

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
#include "j1Player.h"
#include "j1Printer.h"

#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

// Ignores the walkability of the map

Dragon::Dragon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const DragonInfo& dragonInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), dragonInfo(dragonInfo)
{
	pathPlanner->SetIsWalkabilityChecked(false);

	// XML loading
	/// Animations
	DragonInfo info = (DragonInfo&)App->entities->GetUnitInfo(EntityType_DRAGON);
	this->unitInfo = this->dragonInfo.unitInfo;
	this->dragonInfo.up = info.up;
	this->dragonInfo.down = info.down;
	this->dragonInfo.left = info.left;
	this->dragonInfo.right = info.right;
	this->dragonInfo.upLeft = info.upLeft;
	this->dragonInfo.upRight = info.upRight;
	this->dragonInfo.downLeft = info.downLeft;
	this->dragonInfo.downRight = info.downRight;

	this->dragonInfo.attackUp = info.attackUp;
	this->dragonInfo.attackDown = info.attackDown;
	this->dragonInfo.attackLeft = info.attackLeft;
	this->dragonInfo.attackRight = info.attackRight;
	this->dragonInfo.attackUpLeft = info.attackUpLeft;
	this->dragonInfo.attackUpRight = info.attackUpRight;
	this->dragonInfo.attackDownLeft = info.attackDownLeft;
	this->dragonInfo.attackDownRight = info.attackDownRight;

	this->dragonInfo.deathUp = info.deathUp;
	this->dragonInfo.deathDown = info.deathDown;

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
}

void Dragon::Move(float dt)
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

			App->audio->PlayFx(App->audio->GetFX().orcDeath, 0); //Dragon death

			isDead = true;
			isValid = false;
			App->player->enemiesKill++;

			// Give gold to the player
			App->player->currentGold += dragonInfo.droppedGold;
			App->scene->hasGoldChanged = true;

			// Remove the entity from the unitsSelected list
			App->entities->RemoveUnitFromUnitsSelected(this);

			// Initialize the goals
			brain->RemoveAllSubgoals();

			unitState = UnitState_Idle;

			// Remove Movement (so other units can walk above them)
			App->entities->InvalidateMovementEntity(this);

			// Remove any path request
			pathPlanner->SetSearchRequested(false);
			pathPlanner->SetSearchCompleted(false);
			App->pathmanager->UnRegister(pathPlanner);

			if (singleUnit != nullptr)
				delete singleUnit;
			singleUnit = nullptr;

			if (lifeBar != nullptr)
				lifeBar->isActive = false;

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

void Dragon::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		fPoint offset = { 0.0f,0.0f };
		if (animation == &dragonInfo.deathDown || animation == &dragonInfo.deathUp)
			offset = { animation->GetCurrentFrame().w / 2.8f, animation->GetCurrentFrame().h / 2.7f };
		else
			offset = { animation->GetCurrentFrame().w / 2.8f, animation->GetCurrentFrame().h / 2.5f };

		App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_DragonGryphon);
	}

	if (isSelected)
		DebugDrawSelected();
}

void Dragon::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void Dragon::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		// An player is within the sight of this enemy unit
		if (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit) { // || c2->colliderType == ColliderType_PlayerBuilding

			DynamicEntity* dynEnt = (DynamicEntity*)c2->entity;
			//	LOG("Enemy Sight Radius %s", dynEnt->GetColorName().data());

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
			//		LOG("Enemy Attack Radius %s", dynEnt->GetColorName().data());

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
			//LOG("NO MORE Enemy Attack Radius %s", dynEnt->GetColorName().data());

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
void Dragon::UnitStateMachine(float dt)
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
void Dragon::LoadAnimationsSpeed()
{
	upSpeed = dragonInfo.up.speed;
	downSpeed = dragonInfo.down.speed;
	leftSpeed = dragonInfo.left.speed;
	rightSpeed = dragonInfo.right.speed;
	upLeftSpeed = dragonInfo.upLeft.speed;
	upRightSpeed = dragonInfo.upRight.speed;
	downLeftSpeed = dragonInfo.downLeft.speed;
	downRightSpeed = dragonInfo.downRight.speed;

	attackUpSpeed = dragonInfo.attackUp.speed;
	attackDownSpeed = dragonInfo.attackDown.speed;
	attackLeftSpeed = dragonInfo.attackLeft.speed;
	attackRightSpeed = dragonInfo.attackRight.speed;
	attackUpLeftSpeed = dragonInfo.attackUpLeft.speed;
	attackUpRightSpeed = dragonInfo.attackUpRight.speed;
	attackDownLeftSpeed = dragonInfo.attackDownLeft.speed;
	attackDownRightSpeed = dragonInfo.attackDownRight.speed;

	deathUpSpeed = dragonInfo.deathUp.speed;
	deathDownSpeed = dragonInfo.deathDown.speed;
}

void Dragon::UpdateAnimationsSpeed(float dt)
{
	dragonInfo.up.speed = upSpeed * dt;
	dragonInfo.down.speed = downSpeed * dt;
	dragonInfo.left.speed = leftSpeed * dt;
	dragonInfo.right.speed = rightSpeed * dt;
	dragonInfo.upLeft.speed = upLeftSpeed * dt;
	dragonInfo.upRight.speed = upRightSpeed * dt;
	dragonInfo.downLeft.speed = downLeftSpeed * dt;
	dragonInfo.downRight.speed = downRightSpeed * dt;

	dragonInfo.attackUp.speed = attackUpSpeed * dt;
	dragonInfo.attackDown.speed = attackDownSpeed * dt;
	dragonInfo.attackLeft.speed = attackLeftSpeed * dt;
	dragonInfo.attackRight.speed = attackRightSpeed * dt;
	dragonInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	dragonInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	dragonInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	dragonInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	dragonInfo.deathUp.speed = deathUpSpeed * dt;
	dragonInfo.deathDown.speed = deathDownSpeed * dt;
}

bool Dragon::ChangeAnimation()
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

			animation = &dragonInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &dragonInfo.deathDown;
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

			animation = &dragonInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &dragonInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &dragonInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &dragonInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &dragonInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &dragonInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &dragonInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &dragonInfo.attackDownRight;
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

				dragonInfo.up.loop = false;
				dragonInfo.up.Reset();
				dragonInfo.up.speed = 0.0f;
			}
			else
				dragonInfo.up.loop = true;

			animation = &dragonInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				dragonInfo.down.loop = false;
				dragonInfo.down.Reset();
				dragonInfo.down.speed = 0.0f;
			}
			else
				dragonInfo.down.loop = true;

			animation = &dragonInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				dragonInfo.left.loop = false;
				dragonInfo.left.Reset();
				dragonInfo.left.speed = 0.0f;
			}
			else
				dragonInfo.left.loop = true;

			animation = &dragonInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				dragonInfo.right.loop = false;
				dragonInfo.right.Reset();
				dragonInfo.right.speed = 0.0f;
			}
			else
				dragonInfo.right.loop = true;

			animation = &dragonInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				dragonInfo.upLeft.loop = false;
				dragonInfo.upLeft.Reset();
				dragonInfo.upLeft.speed = 0.0f;
			}
			else
				dragonInfo.upLeft.loop = true;

			animation = &dragonInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				dragonInfo.upRight.loop = false;
				dragonInfo.upRight.Reset();
				dragonInfo.upRight.speed = 0.0f;
			}
			else
				dragonInfo.upRight.loop = true;

			animation = &dragonInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				dragonInfo.downLeft.loop = false;
				dragonInfo.downLeft.Reset();
				dragonInfo.downLeft.speed = 0.0f;
			}
			else
				dragonInfo.downLeft.loop = true;

			animation = &dragonInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				dragonInfo.downRight.loop = false;
				dragonInfo.downRight.Reset();
				dragonInfo.downRight.speed = 0.0f;
			}
			else
				dragonInfo.downRight.loop = true;

			animation = &dragonInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}

float Dragon::GetFireSpeed() const
{
	return dragonInfo.fireSpeed;
}