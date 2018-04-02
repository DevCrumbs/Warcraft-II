#include "ScoutTower.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Map.h"
#include <utility> 

ScoutTower::ScoutTower(fPoint pos, iPoint size, int currLife, uint maxLife, const ScoutTowerInfo& scoutTowerInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), scoutTowerInfo(scoutTowerInfo)
{
	texArea = &scoutTowerInfo.constructionPlanks1;
	this->constructionTimer.Start();

	CreateEntityCollider(EntitySide_Player);
	sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, scoutTowerInfo.sightRadius);
	sightRadiusCollider->isTrigger = true;
}

void ScoutTower::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	//Is building destroyed?
	if (currLife <= 0)
		towerState = TowerState_Die;

	//Check if tower has to attack
	if (isBuilt) {
		if (!enemyAttackQueue.empty())
			towerState = TowerState_Attack;
		else
			towerState = TowerState_Idle;
	}

	TowerStateMachine(dt);

	if(!isBuilt)
	UpdateAnimations(dt);

	if (!isBuilt && constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;

	if (arrowParticle != nullptr)
		CheckArrowMovement(dt);
}

void ScoutTower::OnCollision(ColliderGroup * c1, ColliderGroup * c2, CollisionState collisionState)
{
	switch (collisionState) {

	case CollisionState_OnEnter:

		//Every time a enemy enters range it is added to the attack queue
		if (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit
			|| c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding) {
			
			enemyAttackQueue.push (c2->entity);
			
			if (attackingTarget == nullptr) {
				attackingTarget = enemyAttackQueue.back();
				attackTimer.Start();
			}
		}
		
		break;


	case CollisionState_OnExit:

		//Every time the enemy dies or exits sight this enemy is deleted from the atack queue
		if (c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyUnit
			|| c1->colliderType == ColliderType_PlayerSightRadius && c2->colliderType == ColliderType_EnemyBuilding) {
			
			if (c2->entity == attackingTarget) {
				attackingTarget = nullptr;
				enemyAttackQueue.pop();
			}
			else if (c2->entity != enemyAttackQueue.back()) {
				Entity* aux = c2->entity;
				std::swap(enemyAttackQueue.back(), aux); //I don't know if this will work. Have to try it
				enemyAttackQueue.pop();
			}
			
			if (!enemyAttackQueue.empty() && attackingTarget == nullptr) {
				attackingTarget = enemyAttackQueue.back();
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

				attackingTarget->ApplyDamage(scoutTowerInfo.damage);
				DetermineArrowDirection();

				float m = sqrtf(pow(attackingTarget->GetPos().x - arrowParticle->position.x, 2.0f) + pow(attackingTarget->GetPos().y - arrowParticle->position.y, 2.0f));
				arrowParticle->destination.x = (attackingTarget->GetPos().x - arrowParticle->position.x) / m;
				arrowParticle->destination.y = (attackingTarget->GetPos().y - arrowParticle->position.y) / m;
			}
		}
	}
		break;
	case TowerState_Die:
		
		break;
	}
}

//Arrows
void ScoutTower::CheckArrowMovement(float dt)
{
	if (arrowParticle->position != arrowParticle->destination) {
		arrowParticle->position.x += arrowParticle->destination.x * dt * scoutTowerInfo.arrowSpeed;
		arrowParticle->position.y += arrowParticle->destination.y * dt * scoutTowerInfo.arrowSpeed;
	}
	else if (arrowParticle->position == arrowParticle->destination) {
		arrowParticle->position.x == arrowParticle->destination.x;
		arrowParticle->position.y == arrowParticle->destination.y;
	}
}

void ScoutTower::DetermineArrowDirection()
{
	iPoint targetTilePos = App->map->WorldToMap((int)attackingTarget->GetPos().x, (int)attackingTarget->GetPos().y);
	iPoint towerTilePos = App->map->WorldToMap((int)this->GetPos().x, (int)this->GetPos().y);

	//Up
	if (targetTilePos.x == towerTilePos.x  && targetTilePos.y < towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y < towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.up, this->GetPos().x + 16, this->GetPos().y + 16);
	
	//Down
	else if (targetTilePos.x == towerTilePos.x  && targetTilePos.y > towerTilePos.y
		|| targetTilePos.x == towerTilePos.x + 1 && targetTilePos.y > towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.down, this->GetPos().x + 16, this->GetPos().y + 16);

	//Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x < towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.left, this->GetPos().x + 16, this->GetPos().y + 16);
	
	//Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y
		|| targetTilePos.x > towerTilePos.x && targetTilePos.y == towerTilePos.y + 1)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.right, this->GetPos().x + 16, this->GetPos().y + 16);
	
	//Up Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.upLeft, this->GetPos().x + 16, this->GetPos().y + 16);

	//Up Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y < towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.upRight, this->GetPos().x + 16, this->GetPos().y + 16);

	//Down Left
	else if (targetTilePos.x < towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.downLeft, this->GetPos().x + 16, this->GetPos().y + 16);

	//Down Right
	else if (targetTilePos.x > towerTilePos.x && targetTilePos.y > towerTilePos.y)
		arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.downRight, this->GetPos().x + 16, this->GetPos().y + 16);
	//else
		//arrowParticle = App->particles->AddParticle(App->particles->towerArrowParticles.up, this->GetPos().x + 16, this->GetPos().y + 16);
}

// Animations
void ScoutTower::LoadAnimationsSpeed()
{

}
void ScoutTower::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &scoutTowerInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &scoutTowerInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &scoutTowerInfo.completeTexArea;
}