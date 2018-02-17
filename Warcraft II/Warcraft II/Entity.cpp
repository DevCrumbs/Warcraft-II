#include "j1App.h"
#include "Defs.h"
#include "p2Log.h"
#include "Entity.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Render.h"

Entity::Entity(float x, float y) : position(x, y), startPos(x, y) {}

Entity::~Entity()
{
	if (collider != nullptr)
		collider->toDelete = true;
}

const Collider* Entity::GetCollider() const
{
	return collider;
}

void Entity::Draw(SDL_Texture* sprites)
{
	if (animation != nullptr)
		App->render->Blit(sprites, position.x, position.y, &(animation->GetCurrentFrame()));
}

void Entity::OnCollision(Collider* c1, Collider* c2) {}
