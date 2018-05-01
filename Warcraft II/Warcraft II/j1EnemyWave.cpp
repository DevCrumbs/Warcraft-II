#include <iostream>

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EnemyWave.h"
#include "j1Map.h"
#include "j1Input.h"
#include "DynamicEntity.h"
#include  "j1EntityFactory.h"
#include <time.h>

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

		int layer = rand() % spawnTiles.size();

		list<iPoint> currentList = spawnTiles[layer];

		for (list<iPoint>::const_iterator iterator = currentList.begin(); iterator != currentList.end(); ++iterator)
		{
			bool spawn = rand() % 2;

			if (spawn)
			{
				ENTITY_TYPE type = ENTITY_TYPE(rand() % 3 + 381);
				UnitInfo unitInfo;
				fPoint pos{ (float)(*iterator).x, (float)(*iterator).y };

				App->entities->AddEntity(type, pos, App->entities->GetUnitInfo(type), unitInfo);
			}

			else
			{
				LOG("NOPE");
			}

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

