#include "Defs.h"
#include "p2Log.h"

#include "Grunt.h"

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
#include "j1EnemyWave.h"
#include "UIMinimap.h"
#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

Grunt::Grunt(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const GruntInfo& gruntInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), gruntInfo(gruntInfo)
{
	pathPlanner->SetIsInSameRoomChecked(true);

	// XML loading
	/// Animations
	GruntInfo info = (GruntInfo&)App->entities->GetUnitInfo(EntityType_GRUNT);
	this->unitInfo = this->gruntInfo.unitInfo;
	this->gruntInfo.up = info.up;
	this->gruntInfo.down = info.down;
	this->gruntInfo.left = info.left;
	this->gruntInfo.right = info.right;
	this->gruntInfo.upLeft = info.upLeft;
	this->gruntInfo.upRight = info.upRight;
	this->gruntInfo.downLeft = info.downLeft;
	this->gruntInfo.downRight = info.downRight;

	this->gruntInfo.attackUp = info.attackUp;
	this->gruntInfo.attackDown = info.attackDown;
	this->gruntInfo.attackLeft = info.attackLeft;
	this->gruntInfo.attackRight = info.attackRight;
	this->gruntInfo.attackUpLeft = info.attackUpLeft;
	this->gruntInfo.attackUpRight = info.attackUpRight;
	this->gruntInfo.attackDownLeft = info.attackDownLeft;
	this->gruntInfo.attackDownRight = info.attackDownRight;

	this->gruntInfo.deathUp = info.deathUp;
	this->gruntInfo.deathDown = info.deathDown;

	this->size = this->unitInfo.size;
	offsetSize = this->unitInfo.offsetSize;

	LoadAnimationsSpeed();

	// Initialize the goals
	brain->RemoveAllSubgoals();

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
	iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

	// Different behaviors for units on the base and units around the map
	if (!App->map->IsOnBase(spawnPos))

		brain->AddGoal_Wander(6, spawnTile, false, 1, 3, 1, 2, 2);
}

void Grunt::Move(float dt)
{
	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// ---------------------------------------------------------------------

	if (!isSpawned) {
	
		// Collisions
		CreateEntityCollider(EntitySide_Enemy);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, this->unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
		attackRadiusCollider = CreateRhombusCollider(ColliderType_EnemyAttackRadius, this->unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
		entityCollider->isTrigger = true;
		sightRadiusCollider->isTrigger = true;
		attackRadiusCollider->isTrigger = true;

		isSpawned = true;
	}

	// Is the unit dead?
	/// The unit must fit the tile (it is more attractive for the player)
	if (singleUnit != nullptr) {

		if (currLife <= 0
			&& unitState != UnitState_Die
			&& singleUnit->IsFittingTile()
			&& !isDead) {

			App->audio->PlayFx(App->audio->GetFX().orcDeath, 0);

			isDead = true;
			App->player->enemiesKill++;

			// Give gold to the player
			App->player->AddGold(gruntInfo.droppedGold);
			App->audio->PlayFx(App->audio->GetFX().goldGetSound);

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

			LOG("A Grunt has died");

			/// Check if the room of this enemy has been cleared
			Room* room = App->map->GetEntityRoom(this);

			if (!room->isCleared) {

				if (App->map->GetEntitiesOnRoomByCategory(*room, EntityCategory_NONE, EntitySide_Enemy).size() == 0) {

					// ROOM CLEARED!
					iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);
					if (!App->map->IsOnBase(spawnPos)) {

						// Give gold to the player
						if (room->roomRect.w == 30 * 32)
							App->player->AddGold(300);
						else if (room->roomRect.w == 50 * 32)
							App->player->AddGold(800);

						if (App->player->minimap != nullptr)
							App->player->minimap->DrawRoomCleared(*room);

						room->isCleared = true;
						App->player->roomsCleared++;

						if (App->scene->adviceMessage != AdviceMessage_ROOM_CLEAR) {

							App->scene->adviceMessageTimer.Start();
							App->scene->adviceMessage = AdviceMessage_ROOM_CLEAR;
							App->scene->ShowAdviceMessage(App->scene->adviceMessage);
						}

						App->scene->alpha = 200;
						App->scene->isRoomCleared = true;
						App->scene->roomCleared = room->roomRect;

						/// TODO Valdivia: sonido sala limpiada
						App->audio->PlayFx(App->audio->GetFX().roomClear, 0);
					}

					// WAVE DEFEATED
					else if (App->map->IsOnBase(spawnPos) && App->wave->phasesOfCurrWave == App->wave->totalPhasesOfCurrWave - 1) {
					
						// Give gold to the player
						App->player->AddGold(500);

						if (App->scene->adviceMessage != AdviceMessage_BASE_DEFENDED) {

							App->scene->adviceMessageTimer.Start();
							App->scene->adviceMessage = AdviceMessage_BASE_DEFENDED;
							App->scene->ShowAdviceMessage(App->scene->adviceMessage);
						}

						App->scene->alpha = 200;
						App->scene->isRoomCleared = true;
						App->scene->roomCleared = room->roomRect;

						/// TODO Valdivia: sonido sala limpiada
						App->audio->PlayFx(App->audio->GetFX().roomClear, 0);				
					}
				}
			}
		}
	}

	/// **Debug purposes**
	if (isSelected)

		int nothingToDoHere = 0;
	///_**Debug_purposes**

	if (!isDead) {

		iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

		if (App->map->IsOnBase(spawnPos) && brain->GetSubgoalsList().size() == 0) {

			if (App->player->townHall != nullptr) {

				if (App->player->townHall->GetBuildingState() != BuildingState_Destroyed) {

					TargetInfo* targetTownHall = new TargetInfo();

					targetTownHall->target = App->player->townHall;
					targets.push_back(targetTownHall);

					brain->AddGoal_AttackTarget(targetTownHall);
				}
			}
		}

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

void Grunt::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
	{
		// Not draw if not on fow sight
		if (App->fow->IsOnSight(pos))
		{
			fPoint offset = { 0.0f,0.0f };
			if (animation == &gruntInfo.deathDown || animation == &gruntInfo.deathUp)
			{

				offset = { animation->GetCurrentFrame().w / 2.5f, animation->GetCurrentFrame().h / 5.5f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_FloorColliders);
			}
			else
			{

				offset = { animation->GetCurrentFrame().w / 3.2f, animation->GetCurrentFrame().h / 3.1f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);
			}

			if (lifeBar != nullptr)
			{
				lifeBar->isBlit = true;
			}
		}
		else
		{
			if (lifeBar != nullptr)
				lifeBar->isBlit = false;
		}
	}

	//if (isSelected)
		//DebugDrawSelected();
}

void Grunt::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x + offsetSize.x, pos.y + offsetSize.y, size.x, size.y };
	App->printer->PrintQuad(entitySize, color);
}

void Grunt::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		/// SET ATTACK PARAMETERS
		// An player unit/building is within the SIGHT RADIUS of this enemy unit
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("Grunt Sight Radius %s", dynEnt->GetColorName().data());
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

				/// Do it only if the target is valid
				if (c2->entity->GetIsValid()) {

					TargetInfo* targetInfo = new TargetInfo();
					targetInfo->target = c2->entity;
					targetInfo->isSightSatisfied = true;

					targets.push_back(targetInfo);
				}
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

		// An player unit/building is within the ATTACK RADIUS of this enemy unit
		else if ((c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("Grunt Attack Radius %s", dynEnt->GetColorName().data());
			}

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

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Grunt Sight Radius %s", dynEnt->GetColorName().data());
			}

			// Set the target's isSightSatisfied to false
			list<TargetInfo*>::const_iterator it = targets.begin();

			while (it != targets.end()) {

				if ((*it)->target == c2->entity) {

					(*it)->isSightSatisfied = false;

					//(*it)->target->RemoveAttackingUnit(this);
					SetIsRemovedFromSightTargetInfo((*it)->target);
					break;
				}
				it++;
			}
		}

		// An player unit/building is no longer within the ATTACK RADIUS of this enemy unit
		else if ((c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Grunt Attack Radius %s", dynEnt->GetColorName().data());
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
void Grunt::UnitStateMachine(float dt)
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

					isHunting = false;
				}
				else
					unitState = UnitState_Idle;
			}
		}

		break;

	case UnitState_Idle:
	case UnitState_Wander:
	case UnitState_Patrol:
		
		if (singleUnit->IsFittingTile()) {

			float maxLifeValue = maxLife;
			bool isSearchingForCritters = false;

			iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

			// 1. Low life? Search for critters!
			/// 1. NOTE: units on base don't do this, because they are more agressive
			if (!App->map->IsOnBase(spawnPos) && currLife <= 0.2f * maxLifeValue) {

				// Check if there are available critters
				newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY, EntityType_NONE, false, true);

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
						isHunting = false;

						currTarget = newTarget;
						brain->AddGoal_AttackTarget(currTarget);

						isSearchingForCritters = true;
					}
				}

				// If no critter is found, move randomly around the area to run away from the attacking units
				else {

					if (!isHunting) {

						brain->AddGoal_Wander(6, singleUnit->currTile, true, 0, 1, 0, 1, 0);
						isHunting = true;
					}
				}
			}

			if (isSearchingForCritters)
				break;

			// 2. Defend
			// DEFENSE NOTE: the unit automatically attacks back their attacking units (if they have any attacking units) to defend themselves
			if (unitsAttacking.size() > 0) {

				bool isDefend = false;

				if (currTarget == nullptr)
					isDefend = true;
				else if (currTarget->target->entityType == EntityCategory_STATIC_ENTITY)
					isDefend = true;					

				if (isDefend) {

					// PHASE 1. Check if there are available targets (DYNAMIC ENTITY) 
					newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);
					bool isAttackingUnit = false;

					if (newTarget != nullptr) {

						// Is the best target an attacking unit?
						if (find(unitsAttacking.begin(), unitsAttacking.end(), newTarget->target) != unitsAttacking.end()) {

							currTarget = newTarget;
							brain->AddGoal_AttackTarget(currTarget, false);

							isAttackingUnit = true;
							isHunting = false;
						}
					}

					// PHASE 2. Search for a target that is an attacking unit
					if (!isAttackingUnit) {

						list<TargetInfo*>::const_iterator it = targets.begin();

						while (it != targets.end()) {

							if (find(unitsAttacking.begin(), unitsAttacking.end(), (*it)->target) != unitsAttacking.end()) {

								currTarget = *it;
								brain->AddGoal_AttackTarget(currTarget, false);

								isAttackingUnit = true;
								isHunting = false;
							}

							it++;
						}
					}

					// PHASE 3. Move randomly around the area to see if the unit is able to see the attacking units
					/// PHASE 3 NOTE: units on base don't do this, because they are more agressive
					iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

					if (!App->map->IsOnBase(spawnPos) && !isAttackingUnit && !isHunting) {

						if (unitsAttacking.size() > 0) {
							//brain->AddGoal_Wander(6, singleUnit->currTile, true, 0, 1, 0, 1, 0);
							TargetInfo* targetInfo = new TargetInfo();
							targetInfo->target = unitsAttacking.front();
							targetInfo->isSightSatisfied = true;

							targets.push_back(targetInfo);

							currTarget = targetInfo;
							brain->AddGoal_AttackTarget(currTarget, false);

							isHunting = true;
						}
					}
				}
			}

			// 3. Attack
			/// Goal_AttackTarget
			else {

				// Check if there are available targets (DYNAMIC ENTITY)
				newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

				if (newTarget != nullptr) {

					// A new target has found! Update the currTarget
					if (currTarget != newTarget) {

						// Anticipate the removing of this unit from the attacking units of the target
						if (currTarget != nullptr) {

							/*
							if (!currTarget->isRemoved) {

								if (currTarget->target->entityType == EntityType_SHEEP || currTarget->target->entityType == EntityType_BOAR)
									break;

								currTarget->target->RemoveAttackingUnit(this);
							}
							*/
						}

						isHitting = false;
						isHunting = false;

						currTarget = newTarget;
						brain->AddGoal_AttackTarget(currTarget);
					}
				}

				// If the unit is on base, also check for buildings (STATIC ENTITY)
				else {

					iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

					if (App->map->IsOnBase(spawnPos)) {

						// Check if there are available targets (DYNAMIC ENTITY)
						newTarget = GetBestTargetInfo(EntityCategory_STATIC_ENTITY);

						if (newTarget != nullptr) {

							// A new target has found! Update the currTarget
							if (currTarget != newTarget) {

								// Anticipate the removing of this unit from the attacking units of the target
								if (currTarget != nullptr) {

									/*
									if (!currTarget->isRemoved) {

										currTarget->target->RemoveAttackingUnit(this);
									}
									*/
								}

								isHitting = false;
								isHunting = false;

								currTarget = newTarget;
								brain->AddGoal_AttackTarget(currTarget);
							}
						}
					}
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
void Grunt::LoadAnimationsSpeed()
{
	upSpeed = gruntInfo.up.speed;
	downSpeed = gruntInfo.down.speed;
	leftSpeed = gruntInfo.left.speed;
	rightSpeed = gruntInfo.right.speed;
	upLeftSpeed = gruntInfo.upLeft.speed;
	upRightSpeed = gruntInfo.upRight.speed;
	downLeftSpeed = gruntInfo.downLeft.speed;
	downRightSpeed = gruntInfo.downRight.speed;

	attackUpSpeed = gruntInfo.attackUp.speed;
	attackDownSpeed = gruntInfo.attackDown.speed;
	attackLeftSpeed = gruntInfo.attackLeft.speed;
	attackRightSpeed = gruntInfo.attackRight.speed;
	attackUpLeftSpeed = gruntInfo.attackUpLeft.speed;
	attackUpRightSpeed = gruntInfo.attackUpRight.speed;
	attackDownLeftSpeed = gruntInfo.attackDownLeft.speed;
	attackDownRightSpeed = gruntInfo.attackDownRight.speed;

	deathUpSpeed = gruntInfo.deathUp.speed;
	deathDownSpeed = gruntInfo.deathDown.speed;
}

void Grunt::UpdateAnimationsSpeed(float dt)
{
	gruntInfo.up.speed = upSpeed * dt;
	gruntInfo.down.speed = downSpeed * dt;
	gruntInfo.left.speed = leftSpeed * dt;
	gruntInfo.right.speed = rightSpeed * dt;
	gruntInfo.upLeft.speed = upLeftSpeed * dt;
	gruntInfo.upRight.speed = upRightSpeed * dt;
	gruntInfo.downLeft.speed = downLeftSpeed * dt;
	gruntInfo.downRight.speed = downRightSpeed * dt;

	gruntInfo.attackUp.speed = attackUpSpeed * dt;
	gruntInfo.attackDown.speed = attackDownSpeed * dt;
	gruntInfo.attackLeft.speed = attackLeftSpeed * dt;
	gruntInfo.attackRight.speed = attackRightSpeed * dt;
	gruntInfo.attackUpLeft.speed = attackUpLeftSpeed * dt;
	gruntInfo.attackUpRight.speed = attackUpRightSpeed * dt;
	gruntInfo.attackDownLeft.speed = attackDownLeftSpeed * dt;
	gruntInfo.attackDownRight.speed = attackDownRightSpeed * dt;

	gruntInfo.deathUp.speed = deathUpSpeed * dt;
	gruntInfo.deathDown.speed = deathDownSpeed * dt;
}

bool Grunt::ChangeAnimation()
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

			animation = &gruntInfo.deathUp;
			ret = true;
		}
		else if (dir == UnitDirection_Down || dir == UnitDirection_DownLeft || dir == UnitDirection_DownRight) {

			if (animation->Finished() && unitState != UnitState_Die) {
				unitState = UnitState_Die;
				deadTimer.Start();
			}

			animation = &gruntInfo.deathDown;
			ret = true;
		}

		return ret;
	}

	// The unit is hitting their target
	else if (isHitting) {

		// Set the direction of the unit as the orientation towards the target
		/*
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
		*/

		switch (GetUnitDirection()) {

		case UnitDirection_Up:

			animation = &gruntInfo.attackUp;
			ret = true;
			break;

		case UnitDirection_Down:

			animation = &gruntInfo.attackDown;
			ret = true;
			break;

		case UnitDirection_Left:

			animation = &gruntInfo.attackLeft;
			ret = true;
			break;

		case UnitDirection_Right:

			animation = &gruntInfo.attackRight;
			ret = true;
			break;

		case UnitDirection_UpLeft:

			animation = &gruntInfo.attackUpLeft;
			ret = true;
			break;

		case UnitDirection_UpRight:

			animation = &gruntInfo.attackUpRight;
			ret = true;
			break;

		case UnitDirection_DownLeft:

			animation = &gruntInfo.attackDownLeft;
			ret = true;
			break;

		case UnitDirection_DownRight:

			animation = &gruntInfo.attackDownRight;
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

				gruntInfo.up.loop = false;
				gruntInfo.up.Reset();
				gruntInfo.up.speed = 0.0f;
			}
			else
				gruntInfo.up.loop = true;

			animation = &gruntInfo.up;

			ret = true;
			break;

		case UnitDirection_NoDirection:
		case UnitDirection_Down:

			if (isStill) {

				gruntInfo.down.loop = false;
				gruntInfo.down.Reset();
				gruntInfo.down.speed = 0.0f;
			}
			else
				gruntInfo.down.loop = true;

			animation = &gruntInfo.down;

			ret = true;
			break;

		case UnitDirection_Left:

			if (isStill) {

				gruntInfo.left.loop = false;
				gruntInfo.left.Reset();
				gruntInfo.left.speed = 0.0f;
			}
			else
				gruntInfo.left.loop = true;

			animation = &gruntInfo.left;

			ret = true;
			break;

		case UnitDirection_Right:

			if (isStill) {

				gruntInfo.right.loop = false;
				gruntInfo.right.Reset();
				gruntInfo.right.speed = 0.0f;
			}
			else
				gruntInfo.right.loop = true;

			animation = &gruntInfo.right;

			ret = true;
			break;

		case UnitDirection_UpLeft:

			if (isStill) {

				gruntInfo.upLeft.loop = false;
				gruntInfo.upLeft.Reset();
				gruntInfo.upLeft.speed = 0.0f;
			}
			else
				gruntInfo.upLeft.loop = true;

			animation = &gruntInfo.upLeft;

			ret = true;
			break;

		case UnitDirection_UpRight:

			if (isStill) {

				gruntInfo.upRight.loop = false;
				gruntInfo.upRight.Reset();
				gruntInfo.upRight.speed = 0.0f;
			}
			else
				gruntInfo.upRight.loop = true;

			animation = &gruntInfo.upRight;

			ret = true;
			break;

		case UnitDirection_DownLeft:

			if (isStill) {

				gruntInfo.downLeft.loop = false;
				gruntInfo.downLeft.Reset();
				gruntInfo.downLeft.speed = 0.0f;
			}
			else
				gruntInfo.downLeft.loop = true;

			animation = &gruntInfo.downLeft;

			ret = true;
			break;

		case UnitDirection_DownRight:

			if (isStill) {

				gruntInfo.downRight.loop = false;
				gruntInfo.downRight.Reset();
				gruntInfo.downRight.speed = 0.0f;
			}
			else
				gruntInfo.downRight.loop = true;

			animation = &gruntInfo.downRight;

			ret = true;
			break;
		}
		return ret;
	}
	return ret;
}