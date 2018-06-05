#include "Defs.h"
#include "p2Log.h"

#include "EnemyBarracks.h"

#include "j1Collision.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"
#include "j1Particles.h"
#include "j1Player.h"

EnemyBarracks::EnemyBarracks(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyBarracksInfo& enemyBarracksInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyBarracksInfo(enemyBarracksInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_OrcishBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_ENEMY_BARRACKS;
	*(EntitySide*)&entitySide = EntitySide_Enemy;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Medium;

	// Update the walkability map (invalidate the tiles of the building placed)
	vector<iPoint> walkability;
	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y });
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 1 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &enemyBarracksInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;

	secondsReconstruction = GetSecondsReconstruction(buildingSize);
}

EnemyBarracks::~EnemyBarracks() 
{
	if (peon != nullptr)
		peon->isRemove = true;
	peon = nullptr;
}

void EnemyBarracks::Move(float dt) 
{
	// 1.RECONSTRUCTION

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
		fire = nullptr;

		// Remove the peon
		if (peon != nullptr)
			peon->isRemove = true;
		peon = nullptr;

		isInProgressReconstructionTimer = false;
		inProgressReconstructionTimer = 0.0f;
	}

	// ------------------------------------------------------------------------------------

	// 2. RESPAWN

	if (buildingState == BuildingState_Normal) {
	
		if (!isRespawnTimer) {

			secondsRespawn = GetRandomSecondsRespawn();
			isRespawnTimer = true;
			respawnTimer = 0.0f;
		}
	}
	else {
	
		if (isRespawnTimer) {

			isRespawnTimer = false;
			respawnTimer = 0.0f;
		}
	}

	if (isRespawnTimer)

		respawnTimer += dt;

	if (respawnTimer >= secondsRespawn) {

		Room* room = App->map->GetEntityRoom(this);

		if (room != nullptr) {

			list<Entity*> roomEntities = App->map->GetEntitiesOnRoomByCategory(*room, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);

			if (roomEntities.size() < GetMaxEnemiesPerRoom(room->roomType)) {

				// Spawn the enemies
				/// TODO Balancing
				int minValue = 1;
				int maxValue = 3;

				int totalEnemies = rand() % (maxValue - minValue + 1) + minValue;

				for (uint i = 0; i < totalEnemies; ++i) {

					ENTITY_TYPE enemyType = EntityType_NONE;
					UnitInfo unitInfo;
					unitInfo.isWanderSpawnTile = false;

					/// Enemy type
					int randomValue = rand() % 2;

					if (randomValue == 0)
						enemyType = EntityType_GRUNT;
					else
						enemyType = EntityType_TROLL_AXETHROWER;

					App->player->SpawnUnitFromBuilding(this, enemyType, unitInfo);
				}
			}
		}

		isRespawnTimer = false;
		respawnTimer = 0.0f;
	}
}

