#include <iostream>

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EnemyWave.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Input.h"
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

	/// Load from xml
	spawnProbability = 0.35f;

	return ret;
}

bool j1EnemyWave::Start()
{
	timer.Start();
	return true;
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
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		srand(time(NULL));

		int layer = rand() % spawnTiles.size();

		list<iPoint> currentList = spawnTiles[layer];

		int size = currentList.size();
		int spawned = 0;
		for (list<iPoint>::const_iterator iterator = currentList.begin(); iterator != currentList.end(); ++iterator)
		{
			ENTITY_TYPE type = EntityType_NONE;
			if (iterator == currentList.begin() && App->player->gryphonAviary != nullptr)
			{
				if (SpawnEnemy(spawnProbability)) {
					spawned++;

					type = EntityType_DRAGON;
					UnitInfo unitInfo;
					fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };

					App->entities->AddEntity(type, pos, App->entities->GetUnitInfo(type), unitInfo);

				}
			}
			else if (SpawnEnemy(spawnProbability))
			{
				spawned++;

				type = ENTITY_TYPE(rand() % 2 + 381);
				UnitInfo unitInfo;
				fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };

				App->entities->AddEntity(type, pos, App->entities->GetUnitInfo(type), unitInfo);
			}

			LOG("Spawned %i entities from %i, type %i", spawned, size, type);
			
		}
	}
	return true;
}

bool j1EnemyWave::Load(pugi::xml_node& save) {

	return true;
}


void j1EnemyWave::AddTiles(list<iPoint> tiles)
{
	spawnTiles.push_back(tiles);
}

bool j1EnemyWave::Save(pugi::xml_node& save) const {

	

	return true;
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