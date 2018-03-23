#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "Entity.h"

Entity::Entity(fPoint pos, iPoint size, int maxLife, j1Module* listener) : pos(pos), size(size), maxLife(maxLife), listener(listener) {}

Entity::~Entity() {}

void Entity::SetPosition(fPoint pos) 
{
	this->pos = pos;
}

fPoint Entity::GetPosition() const 
{
	return pos;
}

iPoint Entity::GetSize() const
{
	return size;
}

int Entity::GetCurrLife() const
{
	return currentLife;
}

int Entity::GetMaxLife() const
{
	return maxLife;
}

string Entity::GetStringLife() const
{
	return lifeString;
}

void Entity::SetStringLife(int currentLife, int maxLife) 
{
	lifeString = to_string(currentLife) + "/" + to_string(maxLife);
}

void Entity::SetDamageLife(int dam)
{
	currentLife -= dam;
	if (currentLife < 0)
		currentLife = 0;
}

void Entity::SetCurrLife(int life) {
	currentLife = life;
	SetStringLife(currentLife, maxLife);
}

