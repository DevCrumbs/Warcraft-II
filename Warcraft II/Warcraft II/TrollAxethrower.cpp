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
#include "j1EnemyWave.h"
#include "UIMinimap.h"
#include "UILifeBar.h"

#include "j1Scene.h" // isFrameByFrame
#include "j1Input.h" // isFrameByFrame

#include "Brofiler\Brofiler.h"

TrollAxethrower::TrollAxethrower(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TrollAxethrowerInfo& trollAxethrowerInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), trollAxethrowerInfo(trollAxethrowerInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_DYNAMIC_ENTITY;
	*(ENTITY_TYPE*)&dynamicEntityType = EntityType_TROLL_AXETHROWER;
	*(EntitySide*)&entitySide = EntitySide_Enemy;

	pathPlanner->SetIsInSameRoomChecked(true);

	// XML loading
	/// Animations
	TrollAxethrowerInfo info = (TrollAxethrowerInfo&)App->entities->GetUnitInfo(EntityType_TROLL_AXETHROWER);
	this->unitInfo = this->trollAxethrowerInfo.unitInfo;
	this->unitInfo.isWanderSpawnTile = unitInfo.isWanderSpawnTile;

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

	// Collisions
	CreateEntityCollider(EntitySide_Enemy);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, this->unitInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	attackRadiusCollider = CreateRhombusCollider(ColliderType_EnemyAttackRadius, this->unitInfo.attackRadius, DistanceHeuristic_DistanceTo);
	entityCollider->isTrigger = true;
	sightRadiusCollider->isTrigger = true;
	attackRadiusCollider->isTrigger = true;
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
			App->player->enemiesKill++;

			/// TODO Balancing
			// Give gold to the player
			if (App->scene->mapDifficulty != 4) {
				App->player->AddGold(trollAxethrowerInfo.droppedGold);
				App->audio->PlayFx(App->audio->GetFX().goldGetSound);
			}

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

			// Invalidate colliders
			sightRadiusCollider->isValid = false;
			attackRadiusCollider->isValid = false;
			entityCollider->isValid = false;

			LOG("A Troll Axethrower has died");

			/// Check if the room of this enemy has been cleared
			Room* room = App->map->GetEntityRoom(this);

			if (room != nullptr) {
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

							App->audio->PlayFx(App->audio->GetFX().roomClear, 0);
						}

						// WAVE DEFEATED
						else if (App->map->IsOnBase(spawnPos) && App->wave->currWaveFinished && App->entities->HaveAllOrcShipsSpawnedEntities()) {

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

							App->audio->PlayFx(App->audio->GetFX().roomClear, 0);
						}
					}
				}
			}
		}
	}

	// Update currTarget
	if (currTarget != nullptr) {

		if (currTarget->isRemoveNeeded || currTarget->target->isRemove)
			currTarget = nullptr;
	}

	if (!isDead) {

		if (brain->GetSubgoalsList().size() == 0) {

			iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

			if (App->map->IsOnBase(spawnPos)) {

				if (App->player->townHall != nullptr) {

					if (App->player->townHall->GetBuildingState() != BuildingState_Destroyed) {

						if (SetCurrTarget(App->player->townHall))
							brain->AddGoal_AttackTarget(newTarget, false);
					}
				}
			}
			else {

				if (unitInfo.isWanderSpawnTile)
					brain->AddGoal_Wander(5, spawnTile, false, 0, 1, 0, 1, 3);
				else
					brain->AddGoal_Wander(5, spawnTile, true, 0, 1, 0, 1, 3);
			}
		}

		// PROCESS THE CURRENTLY ACTIVE GOAL
		brain->Process(dt);

		// Update targets to be removed
		UpdateTargetsToRemove();
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

	// Update unit's life bar
	if (lifeBar != nullptr) {

		lifeBar->SetLocalPos({ (int)pos.x - lifeBarMarginX, (int)pos.y - lifeBarMarginY });

		if (currLife >= 0)
			lifeBar->SetLife(currLife);
	}
}

void TrollAxethrower::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {

		// Not draw if not on fow sight
		if (App->fow->IsOnSight(pos))
		{
			fPoint offset = { 0.0f,0.0f };

			if (animation == &trollAxethrowerInfo.deathDown || animation == &trollAxethrowerInfo.deathUp) {

				offset = { animation->GetCurrentFrame().w / 6.0f, animation->GetCurrentFrame().h / 3.8f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_FloorColliders);

				if (lifeBar != nullptr)
					lifeBar->isBlit = false;
			}
			else {

				offset = { animation->GetCurrentFrame().w / 4.3f, animation->GetCurrentFrame().h / 4.0f };
				App->printer->PrintSprite({ (int)(pos.x - offset.x), (int)(pos.y - offset.y) }, sprites, animation->GetCurrentFrame(), Layers_Entities);

				if (lifeBar != nullptr)
					lifeBar->isBlit = true;
			}
		}
		else {

			if (lifeBar != nullptr)
				lifeBar->isBlit = false;
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

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("Troll Axethrower Sight Radius %s", dynEnt->GetColorName().data());
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
				LOG("Troll Axethrower Attack Radius %s", dynEnt->GetColorName().data());
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
		// An player unit/building is no longer within the SIGHT RADIUS of this enemy unit
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Troll Axethrower Sight Radius %s", dynEnt->GetColorName().data());
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

		// An player unit/building is no longer within the ATTACK RADIUS of this enemy unit
		else if ((c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_NeutralUnit)
			|| (c1->colliderType == ColliderType_EnemyAttackRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			if (c2->entity == nullptr)
				return;

			if (isSelected) {

				DynamicEntity* dynEnt = (DynamicEntity*)c1->entity;
				LOG("NO MORE Troll Axethrower Attack Radius %s", dynEnt->GetColorName().data());
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
void TrollAxethrower::UnitStateMachine(float dt)
{
	switch (unitState) {

	case UnitState_AttackTarget:

		if (singleUnit->IsFittingTile()) {

			if (currTarget == nullptr) {

				// Check if there are available targets (DYNAMIC ENTITY) 
				newTarget = GetBestTargetInfo(EntityCategory_DYNAMIC_ENTITY);

				if (newTarget != nullptr) {

					if (SetCurrTarget(newTarget->target)) {

						brain->RemoveAllSubgoals();
						brain->AddGoal_AttackTarget(newTarget);
					}

					newTarget = nullptr;
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
						if (currTarget != nullptr)
							currTarget->target->RemoveAttackingUnit(this);

						isHitting = false;
						isHunting = false;

						if (SetCurrTarget(newTarget->target)) {

							brain->RemoveAllSubgoals();
							brain->AddGoal_AttackTarget(newTarget);
						}

						newTarget = nullptr;
						isSearchingForCritters = true;
					}
				}

				// If no critter is found, move randomly around the area to run away from the attacking units
				else {

					if (!isHunting) {

						brain->RemoveAllSubgoals();
						brain->AddGoal_Wander(5, singleUnit->currTile, true, 0, 1, 0, 1, 3);
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

							if (SetCurrTarget(newTarget->target)) {

								brain->RemoveAllSubgoals();
								brain->AddGoal_AttackTarget(newTarget, false);
							}

							newTarget = nullptr;
							isAttackingUnit = true;
							isHunting = false;
						}
					}

					// PHASE 2. Search for a target that is an attacking unit
					if (!isAttackingUnit) {

						list<TargetInfo*>::const_iterator it = targets.begin();

						while (it != targets.end()) {

							if (find(unitsAttacking.begin(), unitsAttacking.end(), (*it)->target) != unitsAttacking.end()) {

								if (SetCurrTarget((*it)->target)) {

									brain->RemoveAllSubgoals();
									brain->AddGoal_AttackTarget(newTarget, false);
								}

								newTarget = nullptr;
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

							TargetInfo* targetInfo = new TargetInfo();
							targetInfo->target = unitsAttacking.front();
							targetInfo->isSightSatisfied = true;

							targets.push_front(targetInfo);

							if (SetCurrTarget(targetInfo->target)) {

								brain->RemoveAllSubgoals();
								brain->AddGoal_AttackTarget(newTarget, false);
							}

							newTarget = nullptr;
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

							if (currTarget->target->entityType == EntityType_SHEEP || currTarget->target->entityType == EntityType_BOAR)
								break;

							currTarget->target->RemoveAttackingUnit(this);
						}

						isHitting = false;
						isHunting = false;

						if (SetCurrTarget(newTarget->target)) {

							brain->RemoveAllSubgoals();
							brain->AddGoal_AttackTarget(newTarget);
						}

						newTarget = nullptr;
					}
				}

				// If the unit is on base, also check for buildings (STATIC ENTITY)
				else {

					iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

					if (App->map->IsOnBase(spawnPos)) {

						// Check if there are available targets (STATIC ENTITY)
						newTarget = GetBestTargetInfo(EntityCategory_STATIC_ENTITY);

						if (newTarget != nullptr) {

							// A new target has found! Update the currTarget
							if (currTarget != newTarget) {

								// Anticipate the removing of this unit from the attacking units of the target
								if (currTarget != nullptr)
									currTarget->target->RemoveAttackingUnit(this);

								isHitting = false;
								isHunting = false;

								if (SetCurrTarget(newTarget->target)) {

									brain->RemoveAllSubgoals();
									brain->AddGoal_AttackTarget(newTarget);
								}

								newTarget = nullptr;
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