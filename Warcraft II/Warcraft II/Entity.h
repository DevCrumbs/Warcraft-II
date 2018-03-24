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
	EntityType_NONE,

	// Player buildings
	/// Production buildings
	EntityType_TOWN_HALL,
	EntityType_CHICKEN_FARM,
	EntityType_BARRACKS,
	EntityType_ELVEN_LUMBER_MILL,
	EntityType_MAGE_TOWER,
	EntityType_GRYPHON_AVIARY,
	EntityType_STABLES,

	/// Defense buildings
	EntityType_SCOUT_TOWER,
	EntityType_PLAYER_GUARD_TOWER,
	EntityType_PLAYER_CANNON_TOWER,

	// Neutral buildings
	EntityType_GOLD_MINE,
	EntityType_RUNESTONE,

	// Enemy buildings
	/// Defense buildings
	EntityType_WATCH_TOWER,
	EntityType_ENEMY_GUARD_TOWER,
	EntityType_ENEMY_CANNON_TOWER,

	// Player types
	EntityType_FOOTMAN,
	EntityType_ELVEN_ARCHER,
	EntityType_GRYPHON_RIDER,
	EntityType_MAGE,
	EntityType_PALADIN,

	EntityType_TURALYON,
	EntityType_KHADGAR,
	EntityType_ALLERIA,

	// Enemy types
	EntityType_GRUNT,
	EntityType_TROLL_AXETHROWER,
	EntityType_DRAGON,

	EntityType_MAX
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