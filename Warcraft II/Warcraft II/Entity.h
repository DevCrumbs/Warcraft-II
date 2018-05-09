#ifndef __Entity_H__
#define __Entity_H__

#include "p2Point.h"
#include "Animation.h"

#include "j1App.h"
#include "j1Audio.h"

class j1Module;

struct SDL_Texture;

struct Collider;
struct ColliderGroup;

enum CollisionState;

enum ENTITY_CATEGORY {

	EntityCategory_NONE,
	EntityCategory_STATIC_ENTITY,
	EntityCategory_DYNAMIC_ENTITY,
	EntityCategory_MAX
};

enum EntitySide
{
	EntitySide_NoSide,
	EntitySide_Player,
	EntitySide_Enemy,
	EntitySide_EnemyBuildings,
	EntitySide_EnemyShip,
	EntitySide_Neutral,
	EntitySide_MaxSides
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

	EntityType_GRUNT = 381,
	EntityType_TROLL_AXETHROWER = 382,
	EntityType_DRAGON = 383,
	EntityType_ORC_SHIP = 384,

	EntityType_WATCH_TOWER = 387,
	EntityType_ENEMY_GUARD_TOWER = 388,
	EntityType_ENEMY_CANNON_TOWER = 389,

	EntityType_GREAT_HALL = 390,
	EntityType_STRONGHOLD = 391,
	EntityType_FORTRESS = 392,
	EntityType_PIG_FARM = 393,
	EntityType_ENEMY_BARRACKS = 394,
	EntityType_TROLL_LUMBER_MILL = 395,
	EntityType_ENEMY_BLACKSMITH = 396,
	EntityType_OGRE_MOUND = 397,
	EntityType_DRAGON_ROOST = 398,
	EntityType_TEMPLE_OF_THE_DAMNED = 399,

	EntityType_GOLD_MINE = 400,
	EntityType_RUNESTONE = 401,

	EntityType_SHEEP = 402,

	EntityType_TOWN_HALL = 403,
	EntityType_BARRACKS = 404,
	EntityType_CHICKEN_FARM = 405,


	EntityType_FOOTMAN = 406,
	EntityType_ELVEN_ARCHER = 407,
	EntityType_TURALYON = 408,
	EntityType_ALLERIA = 409,

	EntityType_GRYPHON_RIDER = 415,
	EntityType_MAGE = 416,
	EntityType_PALADIN = 417,

	// Player buildings
	/// Production buildings
	EntityType_ELVEN_LUMBER_MILL = 418,
	EntityType_MAGE_TOWER = 419,
	EntityType_GRYPHON_AVIARY = 420,
	EntityType_STABLES = 421,

	/// Defense buildings
	EntityType_SCOUT_TOWER = 422,
	EntityType_PLAYER_GUARD_TOWER = 423,
	EntityType_PLAYER_CANNON_TOWER = 424,

	// Neutral buildings


	// Enemy buildings
	///Production buildings
	EntityType_ALTAR_OF_STORMS = 425,

	/// Defense buildings


	// Player types


	// Enemy types



	// Neutral types
	EntityType_BOAR = 426,

	EntityType_MAX = 500
};

class Entity;

struct EntityInfo; // empty container

struct TargetInfo
{
	/// NOTE: if the target is nullptr, TargetInfo must also be nullptr!

	TargetInfo();
	TargetInfo(const TargetInfo& t);

	bool isSightSatisfied = false; // if true, sight distance is satisfied
	bool isAttackSatisfied = false; // if true, attack distance is satisfied

	bool isRemoved = false; // if true, it means that the entity has been killed

	Entity* target = nullptr;

	// -----

	bool IsTargetPresent() const;
};

class Entity
{
public:

	Entity(fPoint pos, iPoint size, int currLife, uint maxLife, j1Module* listener);
	virtual ~Entity();
	virtual void Draw(SDL_Texture* sprites);
	virtual void DebugDrawSelected();
	virtual void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);

	// Position and size
	void SetPos(fPoint pos);
	void AddToPos(fPoint pos);
	fPoint GetPos() const;
	iPoint GetSize() const;
	iPoint GetOffsetSize() const;

	// Life and damage
	void SetMaxLife(int life);
	int GetMaxLife() const;
	void SetCurrLife(int currLife);
	int GetCurrLife() const;
	void ApplyDamage(int damage);
	void ApplyHealth(int health);

	string GetStringLife() const;
	void SetStringLife(int currentLife, int maxLife);

	// Collision
	ColliderGroup* GetEntityCollider() const;
	bool CreateEntityCollider(EntitySide entitySide, bool createOffset = false);
	void UpdateEntityColliderPos();

	// Attack
	/// Entity is being attacked by units
	bool AddAttackingUnit(Entity* entity);
	bool RemoveAttackingUnit(Entity* entity);
	uint GetAttackingUnitsSize(Entity* attackingUnit) const;

	// Selection color
	void SetColor(SDL_Color color, string colorName);
	SDL_Color GetColor() const;
	string GetColorName() const;

	// Valid
	void SetIsValid(bool isValid);
	bool GetIsValid() const;

public:

	ENTITY_CATEGORY entityType = EntityCategory_NONE;
	EntitySide entitySide = EntitySide_NoSide;

	bool isRemove = false;
	bool isSelected = false;

	fPoint pos = { 0.0f,0.0f };

	SDL_Color minimapDrawColor{ 0,0,0,0 };

protected:

	iPoint size = { 0,0 };
	iPoint offsetSize = { 0,0 };

	int currLife = 0;
	uint maxLife = 0;
	string lifeString;

	j1Module* listener = nullptr; // callback

	// Collision
	ColliderGroup* entityCollider = nullptr;

	// Attack
	list<Entity*> unitsAttacking;

	// Selection color
	SDL_Color color = ColorOlive;
	string colorName = "Default";

	// Valid
	bool isValid = true;
};

#endif //__Entity_H__