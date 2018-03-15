#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "Defs.h"

using namespace std;

class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	bool Update(float dt);

	void CheckIfPlaceBuilding();

	// Called before quitting
	bool CleanUp();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

private:

	int totalGold = 0; // total gold earned during the game
	int currentGold = 0; // amount of gold that the player has at the current moment
	double timer = 0.0f; // game time
	uint totalEnemiesKilled = 0;
	uint totalUnitsDead = 0;

	// Buildings
	uint nTownHall = 1;
	uint nBarracks = 1;
	uint nChickenFarm = 2;

	uint nStables = 0;
	uint nGryphonAviary = 0;
	uint nMageTower = 0;
	uint nScoutTower = 0;
	uint nGuardTower = 0;
	uint nCannonTower = 0;

	// Units
	uint totalUnits = 0; // total units created during the game
	uint activeUnits = 0; // units that the player has at the current moment
	uint maxUnits = 0; // max units that the player can have at the current moment (it depends on the Chicken Farms built)
};

#endif //__j1PLAYER_H__
