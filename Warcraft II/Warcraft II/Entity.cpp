#include "Defs.h"
#include "p2Log.h"

#include "Entity.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1EntityFactory.h"

Entity::Entity(fPoint pos, iPoint size, int currLife, uint maxLife, j1Module* listener) : pos(pos), size(size), currLife(currLife), maxLife(maxLife), listener(listener)
{
	if (this->currLife == 0)
		this->currLife = this->maxLife;
}

Entity::~Entity()
{
	/*
	if (entityCollider != nullptr)
		entityCollider->isRemove = true;
	entityCollider = nullptr;
	*/
}

void Entity::Draw(SDL_Texture* sprites)
{
}

void Entity::DebugDrawSelected()
{
}

void Entity::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState) {}

// -------------------------------------------------------------

// Position and size
void Entity::SetPos(fPoint pos)
{
	this->pos = pos;
}

void Entity::AddToPos(fPoint pos)
{
	this->pos.x += pos.x;
	this->pos.y += pos.y;
}

fPoint Entity::GetPos() const
{
	return pos;
}

iPoint Entity::GetSize() const
{
	return size;
}

// Life and damage
int Entity::GetMaxLife() const
{
	return maxLife;
}

void Entity::SetCurrLife(int currLife)
{
	this->currLife = currLife;
	SetStringLife(currLife, maxLife);
}

int Entity::GetCurrLife() const
{
	return currLife;
}

void Entity::ApplyDamage(int damage)
{
	currLife -= damage;
	if (currLife < 0)
		currLife = 0;
	SetStringLife(currLife, maxLife);
}

string Entity::GetStringLife() const
{
	return lifeString;
}

void Entity::SetStringLife(int currentLife, int maxLife) 
{
	lifeString = to_string(currentLife) + "/" + to_string(maxLife);
}

// Collision
ColliderGroup* Entity::GetEntityCollider() const
{
	return entityCollider;
}

bool Entity::CreateEntityCollider(EntitySide entitySide)
{
	ColliderType collType = ColliderType_NoType;

	if (entitySide == EntitySide_Player)
		collType = ColliderType_PlayerUnit;
	else if (entitySide == EntitySide_Enemy)
		collType = ColliderType_EnemyUnit;

	if (collType == ColliderType_NoType)
		return false;

	vector<Collider*> collider;
	SDL_Rect rect = { pos.x, pos.y, size.x, size.y };
	collider.push_back(App->collision->CreateCollider(rect));
	entityCollider = App->collision->CreateAndAddColliderGroup(collider, collType, App->entities, this);

	if (entityCollider != nullptr)
		return true;

	return false;
}

void Entity::UpdateEntityColliderPos()
{
	entityCollider->colliders.front()->SetPos(pos.x, pos.y);
}