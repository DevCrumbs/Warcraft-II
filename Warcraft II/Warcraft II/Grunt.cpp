#include "Grunt.h"

Grunt::Grunt(fPoint pos, const GruntInfo& gruntInfo, j1Module* listener) :DynamicEntity(pos, gruntInfo.size, gruntInfo.life, gruntInfo.speed, listener), gruntInfo(gruntInfo)
{
	currentLife = gruntInfo.life;

<<<<<<< HEAD
=======
	LoadAnimationsSpeed();
>>>>>>> Develompent
}

void Grunt::Move(float dt)
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
		CreateEntityCollider(EntitySide_Enemy);
		sightRadiusCollider = CreateRhombusCollider(ColliderType_EnemySightRadius, unitInfo.sightRadius);
		attackRadiusCollider = CreateRhombusCollider(ColliderType_EnemyAttackRadius, unitInfo.attackRadius);

		entityCollider->isTrigger = true;
		sightRadiusCollider->isTrigger = true;
		attackRadiusCollider->isTrigger = true;

		isSpawned = true;
	}

	// ---------------------------------------------------------------------

	// Is the unit dead?
	if (currLife <= 0)
		isDead = true;

	if (singleUnit != nullptr)
		if ((isSelected && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) || singleUnit->wakeUp)
			unitState = UnitState_Walk;

	UnitStateMachine(dt);

	// Update animations
	UpdateAnimationsSpeed(dt);
	ChangeAnimation();

	// Update colliders
	UpdateEntityColliderPos();
	UpdateRhombusColliderPos(sightRadiusCollider, unitInfo.sightRadius);
	UpdateRhombusColliderPos(attackRadiusCollider, unitInfo.attackRadius);
}

void Grunt::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr) {
	
		fPoint offset = { animation->GetCurrentFrame().w / 4.0f, animation->GetCurrentFrame().h / 2.0f };
		App->render->Blit(sprites, pos.x - offset.x, pos.y - offset.y, &(animation->GetCurrentFrame()));
	}

	if (isSelected)
		DebugDrawSelected();
}

void Grunt::DebugDrawSelected()
{
	const SDL_Rect entitySize = { pos.x, pos.y, size.x, size.y };
	App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);

	for (uint i = 0; i < unitInfo.priority; ++i) {
		const SDL_Rect entitySize = { pos.x + 2 * i, pos.y + 2 * i, size.x - 4 * i, size.y - 4 * i };
		App->render->DrawQuad(entitySize, color.r, color.g, color.b, 255, false);
	}
}
>>>>>>> Develompent

}

// Animations
void Grunt::LoadAnimationsSpeed()
{

}
void Grunt::UpdateAnimations(float dt)
{

}