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
#include "OrcShip.h"
#include <time.h>
#include <random>

using namespace std;

j1EnemyWave::j1EnemyWave() : j1Module()
{
	name.assign("surge");
}

// Destructor
j1EnemyWave::~j1EnemyWave()
{
}

// Called before render is available
bool j1EnemyWave::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LoadKeys(config.child("buttons"));

	this->config = App->config.child(this->name.data());

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
	nextWaveTimer = 0.0f;
	nextPhaseTimer = 0.0f;
	secondsToNextWave = 0.0f;
	secondsToNextPhase = 0.0f;

	// -----

	/// TODO Balancing (Waves)
	int mapDifficulty = App->scene->mapDifficulty;

	spawnProbability = 0.25f;

	if (mapDifficulty == 0 || mapDifficulty == 1) {
		maxSpawnPerPhase = 2;
		maxSpawnPerWave = 3;
	}
	else if (mapDifficulty == 2 || mapDifficulty == 3) {
		maxSpawnPerPhase = 3;
		maxSpawnPerWave = 3;
	}
	else {
		maxSpawnPerPhase = 3;
		maxSpawnPerWave = 4;
	}

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

	currWaveFinished = false;

	return ret;
}

// Called before quitting
bool j1EnemyWave::CleanUp()
{
	// Clear the spawn tiles vector (and lists inside it)
	vector<SpawnTiles>::const_iterator it = spawnTiles.begin();

	while (it != spawnTiles.end()) {
	
		SpawnTiles it2 = *it;
		it2.entitySpawn.clear();
	
		it++;
	}
	spawnTiles.clear();

	if (!active)
		return true;

	return true;
}

bool j1EnemyWave::Update(float dt)
{
	bool ret = true;

	// F7: spawns a random phase of a wave
	if (App->input->GetKey(buttonNewWave) == KEY_DOWN && App->isDebug)
		PerformWave();

	// F8: activates or stops the spawn of waves
	if (App->input->GetKey(buttonActivateWave) == KEY_DOWN && App->isDebug)

		isActiveWaves = !isActiveWaves;

	if (!isActiveWaves)

		return ret;

	// ---------------------------------------------------------------------------------

	nextWaveTimer += dt;

	/// WAVE!
	if (nextWaveTimer >= secondsToNextWave) {

		if (!isStartWave) {
			
			currWaveFinished = false;

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
			
				maxPhasesOfCurrWave = 3;
				minPhasesOfCurrWave = 2;
			}

			totalPhasesOfCurrWave = rand() % (maxPhasesOfCurrWave - minPhasesOfCurrWave + 1) + minPhasesOfCurrWave;

			totalWaves++;
			isStartWave = true;
			LOG("Wave %i with %i phases", totalWaves, totalPhasesOfCurrWave);
		}

		nextPhaseTimer += dt;

		/// PHASE!
		// Start a new phase of the current wave
		if ((nextPhaseTimer >= secondsToNextPhase && phasesOfCurrWave < totalPhasesOfCurrWave)
			|| phasesOfCurrWave == 0) {

			uint rng = rand() % 1 + 1;
			if (rng == 1)
				App->audio->PlayFx(App->audio->GetFX().baseUnderAttack1, 0);
			else if (rng == 2)
				App->audio->PlayFx(App->audio->GetFX().baseUnderAttack2, 0);

			if (phasesOfCurrWave == 0) {

				if (App->scene->adviceMessage != AdviceMessage_UNDER_ATTACK) {
					App->scene->adviceMessageTimer.Start();
					App->scene->adviceMessage = AdviceMessage_UNDER_ATTACK;
					App->scene->ShowAdviceMessage(App->scene->adviceMessage);
				}
			}

			// 1. Perform the small wave
			UnitInfo unitInfo;
			OrcShipInfo shipInfo = (OrcShipInfo&)App->entities->GetUnitInfo(EntityType_ORC_SHIP);
			if (!spawnTiles.empty())
			{
				int shipRand = rand() % spawnTiles.size();
				shipInfo.orcShipType = (ShipType)shipRand;
				App->entities->AddEntity(EntityType_ORC_SHIP, { (float)spawnTiles[shipRand].ship.x, (float)spawnTiles[shipRand].ship.y }, (EntityInfo&)shipInfo, unitInfo, this);
			}
			// 2. Update variables for the next phase
			nextPhaseTimer = 0;

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
			LOG("Phase %i of the wave %i. %f seconds to next phase", phasesOfCurrWave, totalWaves, secondsToNextPhase);
		}

		// No more phases of the current wave
		if (phasesOfCurrWave >= totalPhasesOfCurrWave) {

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

			nextWaveTimer = 0;

			secondsToNextWave = rand() % (maxMinutesToNextWave - minMinutesToNextWave + 1) + minMinutesToNextWave;
			int i = rand() % 10;
			float j = i / 10;
			secondsToNextWave += j;

			secondsToNextWave = MINUTES_TO_SECONDS(secondsToNextWave);

			phasesOfCurrWave = 0;
			totalSpawnOfCurrWave = 0;
			int random = rand() % 2;
			if (random == 0) {
				maxSpawnPerWave++;
			}
			isStartWave = false;

			currWaveFinished = true;

			LOG("%f seconds to next wave", secondsToNextWave);
		}
	}

	return ret;
}

void j1EnemyWave::AddTiles(list<iPoint> tiles, iPoint ship)
{
	spawnTiles.push_back({ ship,tiles });
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

void j1EnemyWave::PerformWave(int layer)
{
	//int layer = rand() % spawnTiles.size();

	list<iPoint> currentList = spawnTiles[layer].entitySpawn;

	int size = currentList.size();
	int spawned = 0;
	uint i = 0;

	for (list<iPoint>::const_iterator iterator = currentList.begin(); iterator != currentList.end(); ++iterator)
	{
		++i;

		if (spawned >= maxSpawnPerPhase) {
			break;
		}
		ENTITY_TYPE type = EntityType_NONE;
		//if (iterator == currentList.begin() && App->player->gryphonAviary != nullptr)
		//{
		//	if (SpawnEnemy(spawnProbability)) {
		//
		//		spawned++;
		//		totalSpawnOfCurrWave++;
		//
		//		type = EntityType_DRAGON;
		//		UnitInfo unitInfo;
		//		fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };
		//		iPoint tile = App->map->WorldToMap((int)pos.x, (int)pos.y);
		//
		//		// Spawn the unit (the following searches for a new spawn tile if the tile is not valid)
		//		App->player->SpawnUnitAtTile(tile, type, unitInfo);
		//		LOG("Spawned %i entities from %i, type %i", spawned, size, type);
		//	}
		//}
		if (SpawnEnemy(spawnProbability) && totalSpawnOfCurrWave < maxSpawnPerWave) {

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

		// Always spawn an enemy
		if (i == currentList.size() && spawned == 0) {
		
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

	int random = rand() % 2;
	if (random == 0) {
		if (maxSpawnPerPhase >= 5)
			maxSpawnPerPhase++;
	}

	spawnProbability += 0.05f;
}

// ---------------------------------------------------------------------------------

bool j1EnemyWave::Load(pugi::xml_node& save) 
{
	pugi::xml_node general = save.child("general");

	spawnProbability = general.child("spawnProbability").attribute("spawnProbability").as_float();
	maxSpawnPerPhase = general.child("maxSpawnPerPhase").attribute("maxSpawnPerPhase").as_uint();

	maxSpawnPerWave = general.child("maxSpawnPerWave").attribute("maxSpawnPerWave").as_uint();
	totalWaves = general.child("totalWaves").attribute("totalWaves").as_uint();
	isActiveWaves = general.child("isActiveWaves").attribute("isActiveWaves").as_bool();
	isStartWave = general.child("isStartWave").attribute("isStartWave").as_bool();
	totalSpawnOfCurrWave = general.child("totalSpawnOfCurrWave").attribute("totalSpawnOfCurrWave").as_uint();

	pugi::xml_node currWave = save.child("currWave");

	totalPhasesOfCurrWave = currWave.child("totalPhasesOfCurrWave").attribute("totalPhasesOfCurrWave").as_uint();
	phasesOfCurrWave = currWave.child("phasesOfCurrWave").attribute("phasesOfCurrWave").as_uint();

	// Waves timeline
	pugi::xml_node timeline = save.child("timeline");

	nextWaveTimer = timeline.child("nextWaveTimer").attribute("nextWaveTimer").as_float();
	nextPhaseTimer = timeline.child("nextPhaseTimer").attribute("nextPhaseTimer").as_float();
	secondsToNextWave = timeline.child("secondsToNextWave").attribute("secondsToNextWave").as_float();
	secondsToNextPhase = timeline.child("secondsToNextPhase").attribute("secondsToNextPhase").as_float();

	return true;
}

bool j1EnemyWave::Save(pugi::xml_node& save) const 
{
	bool create = false;

	pugi::xml_node general;
	if (save.child("general") == NULL)
	{
		general = save.append_child("general");
		create = true;
	}
	else
	{
		general = save.child("general");
	}

	SaveAttribute(spawnProbability, "spawnProbability", general, create);
	SaveAttribute(maxSpawnPerPhase, "maxSpawnPerPhase", general, create);
	SaveAttribute(maxSpawnPerWave, "maxSpawnPerWave", general, create);
	SaveAttribute(totalWaves, "totalWaves", general, create);
	SaveAttribute(isActiveWaves, "isActiveWaves", general, create);
	SaveAttribute(isStartWave, "isStartWave", general, create);
	SaveAttribute(totalSpawnOfCurrWave, "totalSpawnOfCurrWave", general, create);

	create = false;


	pugi::xml_node currWave;
	if (save.child("currWave") == NULL)
	{
		currWave = save.append_child("currWave");
		create = true;
	}
	else
	{
		currWave = save.child("currWave");
	}

	SaveAttribute(totalPhasesOfCurrWave, "totalPhasesOfCurrWave", currWave, create);
	SaveAttribute(phasesOfCurrWave, "phasesOfCurrWave", currWave, create);

	create = false;

	// Waves timeline
	pugi::xml_node timeline;
	if (save.child("timeline") == NULL)
	{
		timeline = save.append_child("timeline");
		create = true;
	}
	else
	{
		timeline = save.child("timeline");
	}

	SaveAttribute(nextWaveTimer, "nextWaveTimer", timeline, create);
	SaveAttribute(nextPhaseTimer, "nextPhaseTimer", timeline, create);
	SaveAttribute(secondsToNextWave, "secondsToNextWave", timeline, create);
	SaveAttribute(secondsToNextPhase, "secondsToNextPhase", timeline, create);

	create = false;

	return true;
}

bool j1EnemyWave::LoadKeys(pugi::xml_node& buttons)
{
	bool ret = true;

	ret = LoadKey(&buttonNewWave, "buttonNewWave", buttons);
	ret = LoadKey(&buttonActivateWave, "buttonActivateWave", buttons);

	return ret;
}

void j1EnemyWave::SaveKeys()
{
	App->configFile.child("config").child(name.data()).remove_child("buttons");
	pugi::xml_node buttons = App->configFile.child("config").child(name.data()).append_child("buttons");

	buttons.remove_child("buttonNewWave");
	buttons.remove_child("buttonActivateWave");

	buttons.append_child("buttonNewWave").append_attribute("buttonNewWave") = *buttonNewWave;
	buttons.append_child("buttonActivateWave").append_attribute("buttonActivateWave") = *buttonActivateWave;
}
