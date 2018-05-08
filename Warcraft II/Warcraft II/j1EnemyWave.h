#ifndef __j1ENEMYSURGE_H__
#define __j1ENEMYSURGE_H__

#include "j1Module.h"
#include "Entity.h"

#include <list>
#include <vector>
#include <string>

#define MINUTES_TO_SECONDS(minutes) minutes*60

using namespace std;

struct MapLayer;
class OrcShip;

struct EnemyInWave
{
	ENTITY_TYPE type = EntityType_NONE;
	iPoint pos{ 0,0 };
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

	bool Update(float dt);

	bool SpawnEnemy(float prob);
	void AddTiles(list<iPoint> tiles);
	void PerformWave();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

private:

	vector<list<iPoint>> spawnTiles;
	
	float spawnProbability = 0.0f;
	uint maxSpawnPerPhase = 0;
	uint maxSpawnPerWave = 0;
	uint totalWaves = 0;

	bool isActiveWaves = true;

	// Current wave
	uint totalPhasesOfCurrWave = 0;
	uint phasesOfCurrWave = 0;

	bool isStartWave = false;

	uint totalSpawnOfCurrWave = 0;

	// Waves timeline
	float nextWaveTimer = 0.0f;
	float nextPhaseTimer = 0.0f;

	float secondsToNextWave = 0.0f;
	float secondsToNextPhase = 0.0f;

	OrcShip* ship = nullptr;
};

#endif //__j1EnemySurge_H__
