#include "Defs.h"
#include "p2Log.h"

#include "PlayerGuardTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1Scene.h"

PlayerGuardTower::PlayerGuardTower(fPoint pos, iPoint size, int currLife, uint maxLife, const PlayerGuardTowerInfo& playerGuardTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), playerGuardTowerInfo(playerGuardTowerInfo)
{
	buildingSize = Small;

	iPoint buildingTile = App->map->WorldToMap(pos.x, pos.y);
	App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
	App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
	App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);

	texArea = &playerGuardTowerInfo.constructionPlanks1;
	this->constructionTimer.Start();
	App->audio->PlayFx(2, 0); //Construction sound
}

void PlayerGuardTower::Move(float dt)
{
	if (!isColliderCreated) {
		CreateEntityCollider(EntitySide_Player);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, playerGuardTowerInfo.sightRadius, DistanceHeuristic_DistanceManhattan);
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
	if (attackingTarget != nullptr && arrowParticle != nullptr)
		CheckArrowMovement(dt);
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

void PlayerGuardTower::OnCollision(ColliderGroup * c1, ColliderGroup * c2, CollisionState collisionState)
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

void PlayerGuardTower::TowerStateMachine(float dt)
{
	switch (towerState) {
	case TowerState_Idle:
		//Nothing
		break;

	case TowerState_Attack:
	{
		if (attackingTarget != nullptr) {
			if (attackTimer.Read() >= (playerGuardTowerInfo.attackWaitTime * 1000)) {

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
void PlayerGuardTower::DetermineArrowDirection()
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

void PlayerGuardTower::CreateArrow()
{
	switch (arrowDirection) {

	case UP:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case UP_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case UP_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN_LEFT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	case DOWN_RIGHT:
		arrowParticle = App->particles->AddParticle(App->particles->playerArrows, { (int)this->GetPos().x + 16, (int)this->GetPos().y + 16 });
		break;
	default:
		break;
	}
}

void PlayerGuardTower::CheckArrowMovement(float dt)
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint arrowTilePos = App->map->WorldToMap((int)arrowParticle->pos.x, (int)arrowParticle->pos.y);

	switch (arrowDirection) {
	case UP:
		if (arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN:
		if (arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case LEFT:
		if (arrowTilePos.x > targetTilePos.x)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x)
			InflictDamageAndDestroyArrow();
		break;

	case RIGHT:
		if (arrowTilePos.x < targetTilePos.x)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x)
			InflictDamageAndDestroyArrow();
		break;

	case UP_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case UP_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y > targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y <= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN_LEFT:
		if (arrowTilePos.x > targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x <= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	case DOWN_RIGHT:
		if (arrowTilePos.x < targetTilePos.x && arrowTilePos.y < targetTilePos.y)
			MoveArrowTowardsTarget(dt);

		else if (arrowTilePos.x >= targetTilePos.x || arrowTilePos.y >= targetTilePos.y)
			InflictDamageAndDestroyArrow();
		break;

	default:
		break;
	}

}

void PlayerGuardTower::MoveArrowTowardsTarget(float dt)
{
	/*
	arrowParticle->pos.x += arrowParticle->destination.x * dt * playerGuardTowerInfo.arrowSpeed;
	arrowParticle->pos.y += arrowParticle->destination.y * dt * playerGuardTowerInfo.arrowSpeed;
	*/
}

void PlayerGuardTower::InflictDamageAndDestroyArrow()
{
	attackingTarget->ApplyDamage(playerGuardTowerInfo.damage);
	arrowParticle->isRemove = true;
	arrowParticle = nullptr;
}

// Animations
void PlayerGuardTower::LoadAnimationsSpeed()
{

}
void PlayerGuardTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &playerGuardTowerInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &playerGuardTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &playerGuardTowerInfo.completeTexArea;
}