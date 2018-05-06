#include <iostream>

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EnemyWave.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "DynamicEntity.h"
#include "j1EntityFactory.h"
#include <time.h>
#include <random>

using namespace std;

j1EnemyWave::j1EnemyWave() : j1Module()
{
	name.assign("surge");
}

// Destructor
j1EnemyWave::~j1EnemyWave()
{}

// Called before render is available
bool j1EnemyWave::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

bool j1EnemyWave::Start()
{
	bool ret = true;

	// Reset waves general info
	totalWaves = 0;
	isActiveWaves = true;
	totalPhasesOfCurrWave = 0;
	phasesOfCurrWave = 0;
	isStartWave = false;
	totalSpawnOfCurrWave = 0;
	secondsToNextWave = 0.0f;
	secondsToNextPhase = 0.0f;

	// -----

	/// TODO Balancing (Waves)
	spawnProbability = 0.25f;
	maxSpawnPerPhase = 3;
	maxSpawnPerWave = 10;

	nextWaveTimer.Start();

	// Calculate the seconds until the first wave arrives
	/// NOTE: the first wave takes more time to arrive than the following waves
	/// rand() % (max - min + 1) + min

	/// TODO Balancing (Waves)
	int maxMinutesToNextWave = 5;
	int minMinutesToNextWave = 4;

	secondsToNextWave = rand() % (maxMinutesToNextWave - minMinutesToNextWave + 1) + minMinutesToNextWave;
	int i = rand() % 10;
	float j = i / 10;
	secondsToNextWave += j;

	secondsToNextWave = MINUTES_TO_SECONDS(secondsToNextWave);

	//secondsToNextWave = 0;

	return ret;
}

// Called before quitting
bool j1EnemyWave::CleanUp()
{
	if (!active)
		return true;

	return true;
}

bool j1EnemyWave::Update(float ft)
{
	bool ret = true;

	// F3: spawns a random phase of a wave
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)

		PerformWave();

	// F4: activates or stops the spawn of waves
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)

		isActiveWaves = !isActiveWaves;

	if (!isActiveWaves)

		return ret;

	// ---------------------------------------------------------------------------------

	/// WAVE!
	if (nextWaveTimer.ReadSec() >= secondsToNextWave) {

		if (!isStartWave) {

			/// TODO Valdivia: sonido oleadas
			uint rng = rand() % 1 + 1;
			if(rng == 1)
				App->audio->PlayFx(App->audio->GetFX().baseUnderAttack1, 0);
			else if (rng == 2)
				App->audio->PlayFx(App->audio->GetFX().baseUnderAttack2, 0);

			if (App->scene->adviceMessage != AdviceMessage_UNDER_ATTACK) {
				App->scene->adviceMessageTimer.Start();
				App->scene->adviceMessage = AdviceMessage_UNDER_ATTACK;
				App->scene->ShowAdviceMessage(App->scene->adviceMessage);
			}
			
			// Calculate the phases of the current wave
			int maxPhasesOfCurrWave = 2;
			int minPhasesOfCurrWave = 1;

			/// TODO Balancing (Waves)
			if (totalWaves == 0) {
			
				maxPhasesOfCurrWave = 2;
				minPhasesOfCurrWave = 1;
			}
			else if (totalWaves <= 2) {
			
				maxPhasesOfCurrWave = 3;
				minPhasesOfCurrWave = 1;
			}
			else {
			
				maxPhasesOfCurrWave = 4;
				minPhasesOfCurrWave = 2;
			}

			totalPhasesOfCurrWave = rand() % (maxPhasesOfCurrWave - minPhasesOfCurrWave + 1) + minPhasesOfCurrWave;

			totalWaves++;
			isStartWave = true;
			LOG("Wave %i", totalWaves);
		}

		/// PHASE!
		// Start a new phase of the current wave
		if (nextPhaseTimer.ReadSec() >= secondsToNextPhase && phasesOfCurrWave < totalPhasesOfCurrWave) {

			// 1. Perform the small wave
			PerformWave();

			// 2. Update variables for the next phase
			nextPhaseTimer.Start();

			// Calculate the seconds until the next phase of the wave arrives
			int maxSecondsToNextPhase = 15;
			int minSecondsToNextPhase = 5;

			/// TODO Balancing (Waves)
			if (totalWaves == 0) {

				maxSecondsToNextPhase = 30;
				minSecondsToNextPhase = 20;
			}
			else if (totalWaves <= 2) {

				maxSecondsToNextPhase = 25;
				minSecondsToNextPhase = 15;
			}
			else {

				maxSecondsToNextPhase = 20;
				minSecondsToNextPhase = 15;
			}

			secondsToNextPhase = rand() % (maxSecondsToNextPhase - minSecondsToNextPhase + 1) + minSecondsToNextPhase;

			phasesOfCurrWave++;
			LOG("Phase %i of the wave %i", phasesOfCurrWave, totalWaves);
		}
		// No more phases of the current wave
		else if (phasesOfCurrWave >= totalPhasesOfCurrWave) {

			nextWaveTimer.Start();

			// Calculate the seconds until the next wave arrives
			int maxMinutesToNextWave = 1;
			int minMinutesToNextWave = 1;

			/// TODO Balancing (Waves)
			if (totalWaves == 1) {

				maxMinutesToNextWave = 5;
				minMinutesToNextWave = 3;
			}
			else if (totalWaves <= 3) {

				maxMinutesToNextWave = 4;
				minMinutesToNextWave = 3;
			}
			else {

				maxMinutesToNextWave = 4;
				minMinutesToNextWave = 2;
			}

			secondsToNextWave = rand() % (maxMinutesToNextWave - minMinutesToNextWave + 1) + minMinutesToNextWave;
			int i = rand() % 10;
			float j = i / 10;
			secondsToNextWave += j;

			secondsToNextWave = MINUTES_TO_SECONDS(secondsToNextWave);

			phasesOfCurrWave = 0;
			totalSpawnOfCurrWave = 0;
			maxSpawnPerWave++;
			isStartWave = false;
		}
	}

	return ret;
}

void j1EnemyWave::AddTiles(list<iPoint> tiles)
{
	spawnTiles.push_back(tiles);
}

bool j1EnemyWave::SpawnEnemy(float prob)  // probability between 0.0 and 1.0
{
	bool ret = false;

	if (prob <= 0.0f)
		ret = false;
	else if (prob >= 1.0f)
		ret = true;

	else
		ret = (rand() / float(RAND_MAX)) < prob;

	return ret;
}

void j1EnemyWave::PerformWave()
{
	int layer = rand() % spawnTiles.size();

	list<iPoint> currentList = spawnTiles[layer];

	int size = currentList.size();
	int spawned = 0;
	for (list<iPoint>::const_iterator iterator = currentList.begin(); iterator != currentList.end(); ++iterator)
	{
		if (spawned >= maxSpawnPerPhase || totalSpawnOfCurrWave >= maxSpawnPerWave) {
			break;
		}
		ENTITY_TYPE type = EntityType_NONE;
		if (iterator == currentList.begin() && App->player->gryphonAviary != nullptr)
		{
			if (SpawnEnemy(spawnProbability)) {

				spawned++;
				totalSpawnOfCurrWave++;

				type = EntityType_DRAGON;
				UnitInfo unitInfo;
				fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };
				iPoint tile = App->map->WorldToMap((int)pos.x, (int)pos.y);

				// Spawn the unit (the following searches for a new spawn tile if the tile is not valid)
				App->player->SpawnUnitAtTile(tile, type, unitInfo);
				LOG("Spawned %i entities from %i, type %i", spawned, size, type);
			}
		}
		else if (SpawnEnemy(spawnProbability)) {

			spawned++;
			totalSpawnOfCurrWave++;

			type = ENTITY_TYPE(rand() % 2 + 381);
			UnitInfo unitInfo;
			fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };
			iPoint tile = App->map->WorldToMap((int)pos.x, (int)pos.y);

			// Spawn the unit (the following searches for a new spawn tile if the tile is not valid)
			App->player->SpawnUnitAtTile(tile, type, unitInfo);
			LOG("Spawned %i entities from %i, type %i", spawned, size, type);
		}
	}
	maxSpawnPerPhase++;
	spawnProbability += 0.05f;
}

// ---------------------------------------------------------------------------------

bool j1EnemyWave::Load(pugi::xml_node& save) 
{
	return true;
}

bool j1EnemyWave::Save(pugi::xml_node& save) const 
{
	return true;
}