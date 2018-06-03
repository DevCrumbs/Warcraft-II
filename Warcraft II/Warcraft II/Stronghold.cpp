#include "Defs.h"
#include "p2Log.h"

#include "Stronghold.h"

#include "j1Collision.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"
#include "j1Particles.h"

Stronghold::Stronghold(fPoint pos, iPoint size, int currLife, uint maxLife, const StrongholdInfo& strongholdInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), strongholdInfo(strongholdInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_OrcishBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_STRONGHOLD;
	*(EntitySide*)&entitySide = EntitySide_Enemy;
	*(StaticEntitySize*)&buildingSize = StaticEntitySize_Big;

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
	App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y });
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 1 });
	App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 2 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 3)] = 0u;
	walkability.push_back({ buildingTile.x + 3, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + buildingTile.x] = 0u;
	walkability.push_back({ buildingTile.x, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 1)] = 0u;
	walkability.push_back({ buildingTile.x + 1, buildingTile.y + 3 });
	App->scene->data[App->scene->w * (buildingTile.y + 3) + (buildingTile.x + 2)] = 0u;
	walkability.push_back({ buildingTile.x + 2, buildingTile.y + 3 });
	App->movement->UpdateUnitsWalkability(walkability);
	// -----

	texArea = &strongholdInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;

	secondsReconstruction = GetSecondsReconstruction(buildingSize);
}

Stronghold::~Stronghold() 
{
	if (peon != nullptr)
		peon->isRemove = true;
	peon = nullptr;
}

void Stronghold::Move(float dt) 
{
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
		peon = App->particles->AddParticle(App->particles->peonBigBuild, { (int)pos.x - 30,(int)pos.y - 30 });

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
}
