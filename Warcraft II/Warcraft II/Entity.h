#ifndef __Entity_H__
#define __Entity_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1App.h"

class j1Module;

struct SDL_Texture;

enum ENTITY_CATEGORY {

	EntityCategory_NONE,
	EntityCategory_STATIC_ENTITY,
	EntityCategory_DYNAMIC_ENTITY,
	EntityCategory_MAX
};

enum EntitiesEvent
{
	EntitiesEvent_NONE,
	EntitiesEvent_RIGHT_CLICK,
	EntitiesEvent_LEFT_CLICK,
	EntitiesEvent_HOVER,
	EntitiesEvent_LEAVE,
	EntitiesEvent_CREATED,
};

enum ENTITY_TYPE
{
	EntityType_NONE = 0,

	// Player buildings
	/// Production buildings
	EntityType_TOWN_HALL = 403,
	EntityType_CHICKEN_FARM = 405,
	EntityType_BARRACKS = 404,
	EntityType_ELVEN_LUMBER_MILL = 406,
	EntityType_MAGE_TOWER = 407,
	EntityType_GRYPHON_AVIARY = 408,
	EntityType_STABLES = 409,

	/// Defense buildings
	EntityType_SCOUT_TOWER = 410,
	EntityType_PLAYER_GUARD_TOWER = 411,
	EntityType_PLAYER_CANNON_TOWER = 412,

	// Neutral buildings
	EntityType_GOLD_MINE = 400,
	EntityType_RUNESTONE = 401,

	// Enemy buildings
	/// Defense buildings
	EntityType_WATCH_TOWER = 387,
	EntityType_ENEMY_GUARD_TOWER = 388,
	EntityType_ENEMY_CANNON_TOWER = 389,

	// Player types
	EntityType_FOOTMAN = 413,
	EntityType_ELVEN_ARCHER = 414,
	EntityType_GRYPHON_RIDER = 415,
	EntityType_MAGE = 416,
	EntityType_PALADIN = 417,

	EntityType_TURALYON = 418,
	EntityType_KHADGAR = 419,
	EntityType_ALLERIA = 420,

	// Enemy types
	EntityType_GRUNT = 381,
	EntityType_TROLL_AXETHROWER = 382,
	EntityType_DRAGON = 383,

	EntityType_MAX = 500
};

struct EntityInfo; // empty container

class Entity
{
public:

	Entity(fPoint pos, iPoint size, int maxLife, j1Module* listener);
	virtual ~Entity();

	void SetPosition(fPoint pos);
	fPoint GetPosition() const;
	iPoint GetSize() const;
	int GetCurrLife() const;
	int GetMaxLife() const;
	string GetStringLife() const;
	void SetStringLife(int currentLife, int maxLife);
	void SetDamageLife(int dam);
	void SetCurrLife(int life);
	void SetMaxLife(int life);




public:

	ENTITY_CATEGORY entityType = EntityCategory_NONE;

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