#include "Defs.h"
#include "p2Log.h"

#include "EnemyGuardTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

EnemyGuardTower::EnemyGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const EnemyGuardTowerInfo& enemyGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), enemyGuardTowerInfo(enemyGuardTowerInfo)
{
	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	texArea = &enemyGuardTowerInfo.completeTexArea;

	//Colliders
	CreateEntityCollider(EntitySide_Enemy, true);
	entityCollider->isTrigger = true;
	sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, enemyGuardTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
	sightRadiusCollider->isTrigger = true;

}

void EnemyGuardTower::Move(float dt)
{
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
	if (!isBuilt)
		UpdateAnimations(dt);

	//Check is building is built already
	if (!isBuilt && constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;

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
				attackTimer.Start();
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
				attackTimer.Start();

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
		if (attackingTarget != nullptr) {
			if (attackTimer.Read() >= (enemyGuardTowerInfo.attackWaitTime * 1000)) {

				attackTimer.Start();
				DetermineArrowDirection();
				CreateArrow();
				App->audio->PlayFx(24, 0); //Arrow sound
			}
		}
	}
	break;
	case TowerState_Die:
		//Nothing
		break;
	}
}

//Arrows
void EnemyGuardTower::DetermineArrowDirection()
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint towerTilePos = App->map->WorldToMap((int)this->GetPos().x, (int)this->GetPos().y);

	//Up
	if (targetTilePos.x == towerTilePos.x  && targetTilePos.y < towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP;

	//Down
	else if (targetTilePos.x == towerTilePos.x  && targetTilePos.y > towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN;

	//Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowDirection = LEFT;

	//Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowDirection = RIGHT;

	//Up Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP_LEFT;

	//Up Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowDirection = UP_RIGHT;

	//Down Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN_LEFT;

	//Down Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowDirection = DOWN_RIGHT;
}

void EnemyGuardTower::CreateArrow()
{
	switch (arrowDirection) {

	case UP:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case DOWN:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case UP_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case UP_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case DOWN_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	case DOWN_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->enemyArrows,
		{ (int)GetPos().x + 16, (int)GetPos().y + 16 }, attackingTarget->GetPos(), enemyGuardTowerInfo.arrowSpeed, enemyGuardTowerInfo.damage);
		break;
	default:
		break;
	}
}


// Animations
void EnemyGuardTower::LoadAnimationsSpeed()
{

}
void EnemyGuardTower::UpdateAnimations(float dt)
{

}