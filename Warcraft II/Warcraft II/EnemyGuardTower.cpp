#include "Defs.h"
#include "p2Log.h"

#include "EnemyGuardTower.h"

#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"
#include "j1FadeToBlack.h"

EnemyGuardTower::EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyGuardTowerInfo(enemyGuardTowerInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_OrcishBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_ENEMY_GUARD_TOWER;
	*(EntitySide*)&entitySide = EntitySide_Enemy;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Small;

	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 1 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &enemyGuardTowerInfo.completeTexArea;

	//Colliders
	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
	sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, enemyGuardTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	sightRadiusCollider->isTrigger = true;

	secondsReconstruction = GetSecondsReconstruction(buildingSize);

	isBuilt = true;
}

EnemyGuardTower::~EnemyGuardTower() 
{
	if (peon != nullptr)
		peon->isRemove = true;
	peon = nullptr;
}

void EnemyGuardTower::Move(float dt)
{
	if (!isCheckedBuildingState && !App->fade->IsFading()) {

		CheckBuildingState();
		isCheckedBuildingState = true;
	}

	if (listener != nullptr)
		HandleInput(EntityEvent);

	// ------------------------------------------------------------------------------------

	// Reconstruction conditions
	if (buildingState == BuildingState_LowFire && !isStartReconstructionTimer) {

		isRestartReconstructionTimer = false;
		isStartReconstructionTimer = true;
		startReconstructionTimer = 0.0f;
	}

	else if (buildingState == BuildingState_HardFire && !isRestartReconstructionTimer) {

		startReconstructionTimer = 0.0f;
		isRestartReconstructionTimer = true;
	}

	else if (buildingState == BuildingState_Destroyed && isStartReconstructionTimer) {

		isRestartReconstructionTimer = false;
		isStartReconstructionTimer = false;
		startReconstructionTimer = 0.0f;
	}

	// START Reconstruction timer
	if (isStartReconstructionTimer)

		startReconstructionTimer += dt;

	//LOG("startReconstructionTimer: %f", startReconstructionTimer);

	// Reconstruction start
	if (startReconstructionTimer >= SECONDS_START_RECONSTRUCTION) {

		buildingStateBeforeReconstruction = buildingState;
		buildingState = BuildingState_Building;

		App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0);
		peon = App->particles->AddParticle(App->particles->peonMediumBuild, { (int)pos.x - 30,(int)pos.y - 30 });

		isRestartReconstructionTimer = false;
		isStartReconstructionTimer = false;
		startReconstructionTimer = 0.0f;

		isInProgressReconstructionTimer = true;
		inProgressReconstructionTimer = 0.0f;
	}

	// IN PROGRESS Reconstruction timer
	if (isInProgressReconstructionTimer)

		inProgressReconstructionTimer += dt;

	//LOG("inProgressReconstructionTimer: %f", inProgressReconstructionTimer);

	// Reconstruction end
	if (buildingStateBeforeReconstruction == BuildingState_HardFire) {

		// Remove the hard fire and add a low fire
		if (inProgressReconstructionTimer >= secondsReconstruction / 2.0f) {

			if (fire != nullptr)
				fire->isRemove = true;
			fire = App->particles->AddParticle(App->particles->lowFire, { (int)this->GetPos().x + this->GetSize().x / 3, (int)this->GetPos().y + this->GetSize().y / 3 });

			buildingStateBeforeReconstruction = BuildingState_NoState;
		}
	}

	if (inProgressReconstructionTimer >= secondsReconstruction) {

		buildingState = BuildingState_Normal;
		SetCurrLife(maxLife);

		// Remove the low fire
		if (fire != nullptr)
			fire->isRemove = true;

		// Remove the peon
		if (peon != nullptr)
			peon->isRemove = true;
		peon = nullptr;

		isInProgressReconstructionTimer = false;
		inProgressReconstructionTimer = 0.0f;
	}

	// ------------------------------------------------------------------------------------

	//Check if building is destroyed
	if (currLife <= 0)
		towerState = TowerState_Die;

	//Check if tower has to attack or not
	if (isBuilt) {
		if (attackingTarget != nullptr && !enemyAttackList.empty())
			towerState = TowerState_Attack;
		else
			towerState = TowerState_Idle;
	}

	TowerStateMachine(dt);

	//Delete arrow if it is fired when an enemy is already dead 
	if (attackingTarget == nullptr && arrowParticle != nullptr) {
		arrowParticle->isRemove = true;
		arrowParticle = nullptr;
	}

	//Check if the tower has to change the attacking target
	if (attackingTarget != nullptr && attackingTarget->GetCurrLife() <= 0) {

		attackingTarget = nullptr;
		enemyAttackList.pop_front();

		if (!enemyAttackList.empty())
			attackingTarget = enemyAttackList.front();
	}
}

void EnemyGuardTower::OnCollision(ColliderGroup * c1, ColliderGroup * c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		//Every time a enemy enters range it is added to the attack queue
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			LOG("Tower enter");
			enemyAttackList.push_back(c2->entity);

			if (attackingTarget == nullptr) {
				attackingTarget = enemyAttackList.front();
				attackTimer = 0.0f;
			}
		}

		break;


	case CollisionState_OnExit:

		//Every time the enemy dies or exits sight this enemy is deleted from the atack queue
		if ((c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerUnit)
			|| (c1->colliderType == ColliderType_EnemySightRadius && c2->colliderType == ColliderType_PlayerBuilding)) {

			LOG("Tower exit");

			if (c2->entity == attackingTarget) {
				attackingTarget = nullptr;
				enemyAttackList.pop_front();
			}

			else if (c2->entity != attackingTarget) {
				enemyAttackList.remove(c2->entity);

			}

			if (!enemyAttackList.empty() && attackingTarget == nullptr) {
				attackingTarget = enemyAttackList.front();
				attackTimer = 0.0f;

			}
		}
		break;
	}
}

void EnemyGuardTower::TowerStateMachine(float dt)
{
	switch (towerState) {
	case TowerState_Idle:
		//Nothing
		break;

	case TowerState_Attack:
	{
		if (attackingTarget != nullptr) 
		{
			attackTimer += dt;
			
			if (attackTimer >= enemyGuardTowerInfo.attackWaitTime) {
				attackTimer = 0.0f;
				CreateArrow();
				App->audio->PlayFx(App->audio->GetFX().arrowThrow, 0);
			}
		}
	}
	break;
	case TowerState_Die:
		//Nothing
		break;
	}
}

void EnemyGuardTower::CreateArrow()
{
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
}