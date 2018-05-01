#ifndef __j1ENEMYSURGE_H__
#define __j1ENEMYSURGE_H__

#include "j1Module.h"
#include "Entity.h"

#include <list>
#include <vector>
#include <string>

using namespace std;

struct MapLayer;

struct EnemyInWave
{
	ENTITY_TYPE type = EntityType_NONE;
	iPoint pos{ 0,0 };
};

struct EnemyWave
{
	//list<EnemyInWave> 
};

class j1EnemyWave : public j1Module
{
public:

	j1EnemyWave();

	// Destructor
	virtual ~j1EnemyWave();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();

	bool Update(float ft);

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	void AddTiles(list<iPoint> tiles);

public:

private:
	j1Timer timer;

	vector<list<iPoint>> spawnTiles;
	

};

#endif //__j1EnemySurge_H__
