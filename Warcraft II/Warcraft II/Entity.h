#ifndef __Entity_H__
#define __Entity_H__

#include "p2Point.h"
#include "Animation.h"

struct SDL_Texture;

enum EntityType {

	EntityType_NoType,
	EntityType_StaticEntity,
	EntityType_DynamicEntity,
	EntityType_MaxTypes
};

struct EntityInfo; // empty container

class Entity
{
public:

	Entity(fPoint pos, iPoint size, int life);
	virtual ~Entity();
	virtual void HandleInput();

	fPoint GetPosition() const;

public:

	EntityType entityType = EntityType_NoType;
	bool remove = false;
	bool isSelected = false;

protected:

	fPoint pos = { 0.0f,0.0f };
	iPoint size = { 0,0 };
	int life = 0;
};

#endif //__Entity_H__