#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "j1Timer.h"
#include "Defs.h"

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"

#include <queue>
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
enum ENTITY_TYPE;

struct HoverButton
{
	UIButton* hoverButton = nullptr;
	StaticEntity* currentEntity = nullptr;
	StaticEntity* nextEntity = nullptr;
	StaticEntity* prevEntity = nullptr;

	bool isCreated = false;

};

struct HoverInfo
{
	UILabel* cost = nullptr;
	UILabel* info = nullptr;
	UIImage* background = nullptr;
};

struct GroupSelectedStats
{
	UIImage* entity1Icon = nullptr;
	UIImage* entity2Icon = nullptr;
	UIImage* entity3Icon = nullptr;
	UIImage* entity4Icon = nullptr;
	UIImage* entity5Icon = nullptr;
	UIImage* entity6Icon = nullptr;
	UIImage* entity7Icon = nullptr;
	UIImage* entity8Icon = nullptr;
	UILifeBar* lifeBar1 = nullptr;
	UILifeBar* lifeBar2 = nullptr;
	UILifeBar* lifeBar3 = nullptr;
	UILifeBar* lifeBar4 = nullptr;
	UILifeBar* lifeBar5 = nullptr;
	UILifeBar* lifeBar6 = nullptr;
	UILifeBar* lifeBar7 = nullptr;
	UILifeBar* lifeBar8 = nullptr;

	list<DynamicEntity*> units;
};

struct ToSpawnUnitsStats 
{
	UIImage* frstInQueueIcon = nullptr;
	UIImage* sndInQueueIcon = nullptr;
	UIImage* trdInQueueIcon = nullptr;
	UILifeBar* frstInQueueBar = nullptr;
	UILifeBar* sndInQueueBar = nullptr;
	UILifeBar* trdInQueueBar = nullptr;
};

struct EntitySelectedStats
{
	UILabel* HP = nullptr;
	UILabel* entityName = nullptr;
	UILabel* entitySight = nullptr;
	UILabel* entityRange = nullptr;
	UILabel* entityDamage = nullptr;
	UILabel* entityMana = nullptr;
	UILabel* entityMovementSpeed = nullptr;
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
	iPoint GetMouseTilePos() const;
	iPoint GetMousePos() const;
	void CheckUnitSpawning();


	void AddGold(int sumGold);
	int GetCurrentGold() const;

	// Called before quitting
	bool CleanUp();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	void OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);


	void MakeEntitiesMenu(string HPname, string entityNameName, SDL_Rect iconDim, Entity* currentEntity);
	void MakeUnitMenu(Entity* entity);
	void MakeUnitsMenu(list<DynamicEntity*> units);
	void DeleteEntitiesMenu();
	void MakeHoverInfoMenu(string unitProduce, string gold);
	void DeleteHoverInfoMenu();
	//void CheckBuildingState(Entity* ent);
	void CreateGroupIcon(iPoint iconPos, SDL_Rect texArea, UIImage* &image);
	void CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, UILifeBar* &lifeBar, Entity * entity);

	void CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity);
	void DestroyHoverButton(Entity* ent);
	void CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button);
	void CreateBarracksButtons();
	void CreateGryphonAviaryButtons();
	void CreateMageTowerButtons();

	void DeleteStaticEntity(StaticEntity* &staticEntity);

public:

	//Player
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

	//Neutral
	list<StaticEntity*> goldMine;
	list<StaticEntity*> runestone;

	bool barracksUpgrade = false;
	bool townHallUpgrade = false;
	bool keepUpgrade = false;

	int currentGold = 0; // amount of gold that the player has at the current moment
	uint totalGold = 0u; // total gold earned during the game
	int currentFood = 0; // amount of food (from chicken farms) that the player has at the current moment (1 food feeds 1 unit)

	//Units costs
	int footmanCost = 500;
	int elvenArcherCost = 400;
	int paladinCost = 800;
	int ballistaCost = 900;
	int mageCost = 1200;
	int gryphonRiderCost = 2500;

	//For finish Screen
	bool isWin = false;
	j1Timer startGameTimer;
	uint unitProduce = 0u;
	uint enemiesKill = 0u;
	uint buildDestroy = 0u;

private:

	double timer = 0.0f; // game time
	HoverCheck hoverCheck = HoverCheck_None;
	uint totalEnemiesKilled = 0;
	uint totalUnitsDead = 0;

	// Buildings
	uint nTownHall = 1;
	uint nBarracks = 1;
	uint nChickenFarm = 2;
	uint nGoldMine = 10;

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

	HoverInfo hoverInfo;

	EntitySelectedStats entitySelectedStats;

	GroupSelectedStats groupSelectedStats;

	ToSpawnUnitsStats toSpawnUnitStats;

	UIButton *produceFootmanButton, *produceElvenArcherButton, *produceMageButton, *produceGryphonRiderButton, *producePaladinButton;

	list<UIElement*> UIMenuInfoList;

	//Spawning units from barracks queues and variables
	queue<j1Timer> toSpawnUnitTimerQueue;
	queue<ENTITY_TYPE> toSpawnUnitTypeQueue;
	uint spawningTime = 5; //In seconds
	uint maxSpawnQueueSize = 2;

};

#endif //__j1PLAYER_H__
