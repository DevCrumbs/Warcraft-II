#include "Defs.h"
#include "p2Log.h"

#include "PlayerCannonTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"

PlayerCannonTower::PlayerCannonTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerCannonTowerInfo& playerCannonTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), playerCannonTowerInfo(playerCannonTowerInfo)
{
	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	texArea = &playerCannonTowerInfo.constructionPlanks1;
	this->constructionTimer.Start();
	App->audio->PlayFx(2, 0); //Construction sound
}

void PlayerCannonTower::Move(float dt)
{
	if (!isColliderCreated) {

		CreateEntityCollider(EntitySide_Player);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, playerCannonTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
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
	if (!isBuilt)
		UpdateAnimations(dt);

	//Check is building is built already
	if (!isBuilt && constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;

	//Check the arrow movement if the tower has to attack
	if (attackingTarget != nullptr && cannonParticle != nullptr)
		CheckCannonBulletMovement(dt);
	if (attackingTarget == nullptr && cannonParticle != nullptr) {
		cannonParticle->isRemove = true;
		cannonParticle = nullptr;
	}

	//Check if the tower has to change the attacking target
	if (attackingTarget != nullptr && attackingTarget->GetCurrLife() <= 0) {

		attackingTarget = nullptr;
		enemyAttackList.pop_front();

		if (!enemyAttackList.empty())
			attackingTarget = enemyAttackList.front();
	}
}

void PlayerCannonTower::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState) {
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


void PlayerCannonTower::TowerStateMachine(float dt)
{
	switch (towerState) {
	case TowerState_Idle:
		//Nothing
		break;

	case TowerState_Attack:
	{
		if (attackingTarget != nullptr) {
			if (attackTimer.Read() >= (playerCannonTowerInfo.attackWaitTime * 1000)) {

				attackTimer.Start();
				DetermineCannonBulletDirection();
				CreateCannonBullet();
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

//Cannon bullet
void PlayerCannonTower::DetermineCannonBulletDirection()
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint towerTilePos = App->map->WorldToMap((int)this->GetPos().x, (int)this->GetPos().y);

	//Up
	if (targetTilePos.x == towerTilePos.x  && targetTilePos.y < towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y < towerTilePos.y)
		cannonDirection = UP;

	//Down
	else if (targetTilePos.x == towerTilePos.x  && targetTilePos.y > towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y > towerTilePos.y)
		cannonDirection = DOWN;

	//Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		cannonDirection = LEFT;

	//Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		cannonDirection = RIGHT;

	//Up Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y < towerTilePos.y)
		cannonDirection = UP_LEFT;

	//Up Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y < towerTilePos.y)
		cannonDirection = UP_RIGHT;

	//Down Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y > towerTilePos.y)
		cannonDirection = DOWN_LEFT;

	//Down Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y > towerTilePos.y)
		cannonDirection = DOWN_RIGHT;
}

void PlayerCannonTower::CreateCannonBullet()
{
	cannonParticle = App->particles->AddParticle(App->particles->playerCannonBullet, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });

	float m = sqrtf(pow(attackingTarget->GetPos().x - cannonParticle->pos.x, 2.0f) + pow(attackingTarget->GetPos().y - cannonParticle->pos.y, 2.0f));
	if (m > 0) {
		cannonParticle->destination.x = (attackingTarget->GetPos().x - cannonParticle->pos.x) / m;
		cannonParticle->destination.y = (attackingTarget->GetPos().y - cannonParticle->pos.y) / m;
	}
}

void PlayerCannonTower::CheckCannonBulletMovement(float dt)
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint arrowTilePos = App->map->WorldToMap((int)cannonParticle->pos.x, (int)cannonParticle->pos.y);

	switch (cannonDirection) {
	case UP:
		if (arrowTilePos.y > targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	case DOWN:
		if (arrowTilePos.y < targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	case LEFT:
		if (arrowTilePos.x > targetTilePos.x)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x)
			InflictDamageAndDestroyCannonBullet();
		break;

	case RIGHT:
		if (arrowTilePos.x < targetTilePos.x)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x)
			InflictDamageAndDestroyCannonBullet();
		break;

	case UP_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	case UP_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	case DOWN_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	case DOWN_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveCannonTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyCannonBullet();
		break;

	default:
		break;
	}
}

void PlayerCannonTower::MoveCannonTowardsTarget(float dt)
{
	cannonParticle->pos.x += cannonParticle->destination.x * dt * playerCannonTowerInfo.arrowSpeed;
	cannonParticle->pos.y += cannonParticle->destination.y * dt * playerCannonTowerInfo.arrowSpeed;
}

void PlayerCannonTower::InflictDamageAndDestroyCannonBullet()
{
	attackingTarget->ApplyDamage(playerCannonTowerInfo.damage);
	cannonParticle->isRemove = true;
	cannonParticle = nullptr;
}


// Animations
void PlayerCannonTower::LoadAnimationsSpeed()
{

}
void PlayerCannonTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &playerCannonTowerInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &playerCannonTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &playerCannonTowerInfo.completeTexArea;
}