#ifndef __j1ENTITY_FACTORY_H__
#define __j1ENTITY_FACTORY_H__

#include "j1Module.h"
#include "Animation.h"

#include "p2Point.h"
#include "p2List.h"

#define MAX_ENTITIES 100

struct SDL_Texture;

// Utility: use ENTITY_TYPES values to assign a type to entities in Tiled 
enum ENTITY_TYPES
{
	NO_TYPE,
	IMP_,
	CAT_PEASANT_,
	MONKEY_,
	PLANT_,
	PLAYER_,
	CAT_,
};

class Entity;

struct EntityInfo
{
	ENTITY_TYPES type = ENTITY_TYPES::NO_TYPE;
	iPoint position = { 0,0 };
};

class j1EntityFactory : public j1Module
{
public:

	j1EntityFactory();
	virtual ~j1EntityFactory();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	bool AddEntities();
	bool AddEntity(EntityInfo& info);
	Entity* SpawnEntity(const EntityInfo& info);

	// Get entities info
	/*
	PlayerInfo& GetPlayerInfo() { return player; }
	*/

	bool Save(pugi::xml_node& save) const;
	bool Load(pugi::xml_node& save);

private:

	EntityInfo queue[MAX_ENTITIES];
	Entity* entities[MAX_ENTITIES];

	p2SString CatPeasant_spritesheet = nullptr;

	// Entities textures
	/*
	SDL_Texture* CatPeasantTex = nullptr;
	*/

	// Entities info
	/*
	PlayerInfo player;
	*/

public:

	/*
	Player* playerData = nullptr;
	*/
};

#endif //__j1ENTITY_FACTORY_H__
