#include <iostream>

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EnemyWave.h"
#include "j1Map.h"

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

	spawnTiles[5];

	return true;
}

