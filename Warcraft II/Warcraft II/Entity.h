#ifndef __Entity_H__
#define __Entity_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1App.h"

class j1Module;

struct SDL_Texture;

enum EntityType {

	EntityType_NoType,
	EntityType_StaticEntity,
	EntityType_DynamicEntity,
	EntityType_MaxTypes
};

enum EntitiesEvent
{
	EntitiesEvent_None,
	EntitiesEvent_RightClick,
	EntitiesEvent_LeftClick,
	EntitiesEvent_Hover,
	EntitiesEvent_Leave,
	EntitiesEvent_Created,

};

struct EntityInfo; // empty container

class Entity
{
public:

	Entity(fPoint pos, iPoint size, int maxLife, j1Module* listener);
	virtual ~Entity();

	void SetPosition(fPoint pos);
	fPoint GetPosition() const;
	int GetCurrLife() const;
	int GetMaxLife() const;
	string GetStringLife() const;
	void SetStringLife(int currentLife, int maxLife);
	void SetDamageLife(int dam);



public:

	EntityType entityType = EntityType_NoType;

	bool remove = false;
	//bool isSelected = false;


protected:

	fPoint pos = { 0.0f,0.0f };
	iPoint size = { 0,0 };
	int currentLife = 0;
	int maxLife = 0;
	j1Module* listener = nullptr;
	string lifeString;
};

#endif //__Entity_H__