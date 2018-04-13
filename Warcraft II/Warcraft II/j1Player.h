#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "Defs.h"

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"


#include <list>
using namespace std;

enum HoverCheck
{
	HoverCheck_None,
	HoverCheck_Upgrate,
	HoverCheck_Repair
};

struct UILabel;
struct UIImage;
struct UIButton;
struct UILifeBar;
class Entity;

class StaticEntity;

struct HoverButton
{
	UIButton* hoverButton = nullptr;
	StaticEntity* currentEntity = nullptr;
	StaticEntity* nextEntity = nullptr;
	StaticEntity* prevEntity = nullptr;

	bool isCreated = false;

};

struct EntitySelectedStats
{
	UILabel* HP = nullptr;
	UILabel* entityName = nullptr;
	UIImage* entityIcon = nullptr;
	UILifeBar* lifeBar = nullptr;

	Entity* entitySelected = nullptr;
};

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
	bool PostUpdate();

	void CheckIfPlaceBuilding();
	iPoint GetMouseTilePos();
	iPoint GetMousePos();

	// Called before quitting
	bool CleanUp();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	void OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);


	void MakeEntitiesMenu(string HPname, string entityNameName, SDL_Rect iconDim, Entity* currentEntity);
	void DeleteEntitiesMenu();
	//void CheckBuildingState(Entity* ent);
	void CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity);
	void DestroyHoverButton(Entity* ent);
	void CreateBarracksButtons();

	void HandleBarracksUIElem();
	void CreateGryphonAviaryButtons();
	void CreateMageTowerButtons();
	void CreateAbilitiesButtons();


public:

	list<StaticEntity*> chickenFarm;
	list<StaticEntity*> scoutTower;
	StaticEntity* barracks = nullptr;
	StaticEntity* townHall = nullptr;
	StaticEntity* blacksmith = nullptr;
	StaticEntity* stables = nullptr;
	StaticEntity* church = nullptr;
	StaticEntity* mageTower = nullptr;
	StaticEntity* cannonTower = nullptr;
	StaticEntity* guardTower = nullptr;
	StaticEntity* gryphonAviary = nullptr;
	
private:

	int totalGold = 0; // total gold earned during the game
	int currentGold = 0; // amount of gold that the player has at the current moment
	double timer = 0.0f; // game time
	HoverCheck hoverCheck = HoverCheck_None;
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


	HoverButton hoverButtonStruct;

	EntitySelectedStats entitySelectedStats;

	GroupSelectedStats groupSelectedStats;

	ToSpawnUnitsStats toSpawnUnitStats;

	UIButton *produceFootmanButton, *produceElvenArcherButton, *produceMageButton, *produceGryphonRiderButton, *producePaladinButton, *commandPatrolButton, *commandStopButton;

	list<UIElement*> UIMenuInfoList;

};

#endif //__j1PLAYER_H__
