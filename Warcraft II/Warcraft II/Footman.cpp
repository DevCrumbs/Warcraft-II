#include "Footman.h"

Footman::Footman(fPoint pos, const FootmanInfo& footmanInfo, j1Module* listener) :DynamicEntity(pos, footmanInfo.size, footmanInfo.life, footmanInfo.speed, listener),footmanInfo(footmanInfo)
{
	currentLife = footmanInfo.life;

<<<<<<< HEAD
=======
	this->footmanInfo.attackUp = info.attackUp;
	this->footmanInfo.attackDown = info.attackDown;
	this->footmanInfo.attackLeft = info.attackLeft;
	this->footmanInfo.attackRight = info.attackRight;
	this->footmanInfo.attackUpLeft = info.attackUpLeft;
	this->footmanInfo.attackUpRight = info.attackUpRight;
	this->footmanInfo.attackDownLeft = info.attackDownLeft;
	this->footmanInfo.attackDownRight = info.attackDownRight;

	this->footmanInfo.deathUp = info.deathUp;
	this->footmanInfo.deathDown = info.deathDown;

	LoadAnimationsSpeed();
>>>>>>> Develompent
}

void Footman::Move(float dt)
{
<<<<<<< HEAD
=======
	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// Create colliders
	if (!isSpawned) {

		// Collisions
		CreateEntityCollider(EntitySide_Player);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_PlayerSightRadius, unitInfo.sightRadius);
		attackRadiusCollider = CreateRhombusCollider(ColliderType_PlayerAttackRadius, unitInfo.attackRadius);

		entityCollider->isTrigger = true;
		sightRadiusCollider->isTrigger = true;
		attackRadiusCollider->isTrigger = true;

		isSpawned = true;
	}

	// ---------------------------------------------------------------------
>>>>>>> Develompent

}

// Animations
void Footman::LoadAnimationsSpeed()
{

}
void Footman::UpdateAnimations(float dt)
{

}