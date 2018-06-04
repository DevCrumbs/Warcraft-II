#include "Defs.h"
#include "p2Log.h"

#include "ScoutTower.h"

#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Movement.h"

ScoutTower::ScoutTower(fPoint pos, iPoint size, int currLife, uint maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), scoutTowerInfo(scoutTowerInfo)
{
	*(ENTITY_CATEGORY*)&entityType = EntityCategory_STATIC_ENTITY;
	*(StaticEntityCategory*)&staticEntityCategory = StaticEntityCategory_HumanBuilding;
	*(ENTITY_TYPE*)&staticEntityType = EntityType_SCOUT_TOWER;
	*(EntitySide*)&entitySide = EntitySide_Player;
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

	texArea = &scoutTowerInfo.constructionPlanks1;

	buildingState = BuildingState_Building;
	App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound

	//Construction peasants
	peasants = App->particles->AddParticle(App->particles->peasantSmallBuild, { (int)pos.x - 20,(int)pos.y - 20 });
}

ScoutTower::~ScoutTower()
{
	if (peasants != nullptr) {
		peasants->isRemove = true;
		peasants = nullptr;
	}
}

void ScoutTower::Move(float dt)
{
	if (!isColliderCreated) {

		CreateEntityCollider(EntitySide_Player, true);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, scoutTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
		sightRadiusCollider->isTrigger = true;
		entityCollider->isTrigger = true;
		isColliderCreated = true;
	}

	if (listener != nullptr)
		HandleInput(EntityEvent);

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

	//Update animations for the construction cycle
	if (!isBuilt) {
		constructionTimer += dt;
		UpdateAnimations(dt);
	}

	//Check is building is built already
	if (!isBuilt && constructionTimer >= constructionTime) {
		isBuilt = true;

		if (peasants != nullptr) {
			peasants->isRemove = true;
			peasants = nullptr;
		}
	}

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

void ScoutTower::OnCollision(ColliderGroup * c1, ColliderGroup * c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		//Every time a enemy enters range it is added to the attack queue
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)) {
			
			LOG("Tower enter");
			enemyAttackList.push_back(c2->entity);
			
			if (attackingTarget == nullptr) {
				attackingTarget = enemyAttackList.front();
				attackTimer.Start();
			}
		}
		
		break;


	case CollisionState_OnExit:

		//Every time the enemy dies or exits sight this enemy is deleted from the atack queue
		if ((c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit)) {
			
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
				attackTimer.Start();

			}
		}
		
		break;
	
	}

}

void ScoutTower::TowerStateMachine(float dt)
{
	switch (towerState) {
	case TowerState_Idle:
		//Nothing
		break;

	case TowerState_Attack:
	{
		if (attackingTarget != nullptr) {
			if (attackTimer.Read() >= (scoutTowerInfo.attackWaitTime * 1000)) {
				attackTimer.Start();
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

void ScoutTower::CreateArrow()
{
	arrowParticle = App->particles->AddParticle(App->particles->playerArrows,
	{ (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 }, attackingTarget->GetPos(), scoutTowerInfo.arrowSpeed, scoutTowerInfo.damage);
}


// Animations
void ScoutTower::LoadAnimationsSpeed()
{
}

void ScoutTower::UpdateAnimations(float dt)
{
	if (constructionTimer >= (constructionTime / 3))
		texArea = &scoutTowerInfo.constructionPlanks2;

	if (constructionTimer >= (constructionTime / 3 * 2))
		texArea = &scoutTowerInfo.inProgressTexArea;

	if (constructionTimer >= constructionTime) {
		texArea = &scoutTowerInfo.completeTexArea;
		buildingState = BuildingState_Normal;
	}
}