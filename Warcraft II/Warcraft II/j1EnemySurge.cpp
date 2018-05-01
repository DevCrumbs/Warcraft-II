#include <iostream>

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EnemySurge.h"

using namespace std;

j1EnemySurge::j1EnemySurge() : j1Module()
{
	name.assign("surge");
}

// Destructor
j1EnemySurge::~j1EnemySurge()
{}

// Called before render is available
bool j1EnemySurge::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;

	return ret;
}

bool j1EnemySurge::Start()
{

	return true;
}

// Called before quitting
bool j1EnemySurge::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	return true;
}

bool j1EnemySurge::Update(float ft)
{


	return true;
}

bool j1EnemySurge::Load(pugi::xml_node& save) {

	return true;
}

bool j1EnemySurge::Save(pugi::xml_node& save) const {


	return true;
}

