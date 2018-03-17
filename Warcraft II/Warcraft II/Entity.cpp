#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "Entity.h"

Entity::Entity(fPoint pos, iPoint size, int life, j1Module* listener) : pos(pos), size(size), life(life), listener(listener) {}

Entity::~Entity() {}

void Entity::SetPosition(fPoint pos) 
{
	this->pos = pos;
}

fPoint Entity::GetPosition() const 
{
	return pos;
}