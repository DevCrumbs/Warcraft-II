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
enum TerenasDialogEvents;
struct UnitInfo;

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

struct GroupSelectedElements 
{
	Entity* owner = nullptr;

	UIImage* entityIcon = nullptr;
	UILifeBar* entityLifeBar = nullptr;
};

struct ToSpawnUnit 
{
	ToSpawnUnit(j1Timer toSpawnTimer, ENTITY_TYPE entityType) {
		this->toSpawnTimer = toSpawnTimer;
		this->entityType = entityType;
	}

	j1Timer toSpawnTimer;
	ENTITY_TYPE entityType;
};

struct GroupSpawning 
{
	ToSpawnUnit* owner = nullptr;

	UIImage* entityIcon = nullptr;
	UILifeBar* entityLifeBar = nullptr;
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

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	// -----

	void CheckIfPlaceBuilding();
	iPoint GetMouseTilePos() const;
	iPoint GetMousePos() const;

	void CheckUnitSpawning();
	void SpawnUnit(fPoint spawningBuildingPos, ENTITY_TYPE spawningEntity, UnitInfo unitInfo);

	void UpdateSpawnUnitsStats();

	// Gold
	void AddGold(int sumGold);
	int GetCurrentGold() const;


	void OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnDynamicEntitiesEvent(DynamicEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

	void MakeEntitiesMenu(string HPname, string entityNameName, SDL_Rect iconDim, Entity* currentEntity);
	void MakeUnitMenu(Entity* entity);
	void MakeUnitsMenu(list<DynamicEntity*> units);
	void DeleteEntitiesMenu();
	void MakeHoverInfoMenu(string unitProduce, string gold);
	void DeleteHoverInfoMenu();
	//void CheckBuildingState(Entity* ent);
	UIImage* CreateGroupIcon(iPoint iconPos, SDL_Rect texArea);
	UILifeBar* CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, Entity* entity);
	UILifeBar* CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea);
	
	//void CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity);
	//void DestroyHoverButton(Entity* ent);
	void CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button);
	void CreateBarracksButtons();
	void CreateTownHallButtons();
	void HandleBarracksUIElem();
	void CreateGryphonAviaryButtons();
	void CreateMageTowerButtons();
	void CreateAbilitiesButtons();

	void RescuePrisoner(TerenasDialogEvents dialogEvent, SDL_Rect iconText, iPoint iconPos);

public:

	//Player
	list<StaticEntity*> chickenFarm;
	list<StaticEntity*> scoutTower;
	list<StaticEntity*> cannonTower;
	list<StaticEntity*> guardTower;
	StaticEntity* barracks = nullptr;
	StaticEntity* townHall = nullptr;
	StaticEntity* blacksmith = nullptr;
	StaticEntity* stables = nullptr;
	StaticEntity* church = nullptr;
	StaticEntity* mageTower = nullptr;
	StaticEntity* gryphonAviary = nullptr;

	vector<UIImage*> imagePrisonersVector;

	//Neutral
	list<StaticEntity*> goldMine;
	list<StaticEntity*> runestone;
	//Update lifeBar
	//Entity* getEntityDamage = nullptr;

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

	EntitySelectedStats entitySelectedStats;

	bool isUnitSpawning = false;
	bool isMouseOnMine = false;

private:

	double timer = 0.0f; // game time
	HoverCheck hoverCheck = HoverCheck_None;
	uint totalEnemiesKilled = 0;
	uint totalUnitsDead = 0;

	//HoverButton hoverButtonStruct;
	HoverInfo hoverInfo;

	list<GroupSelectedElements> groupElementsList;

	list<GroupSpawning> toSpawnUnitStats;
	list<ToSpawnUnit*> newUnitsToSpawn;

	UIButton *produceFootmanButton = nullptr, *produceElvenArcherButton = nullptr, *produceMageButton = nullptr, *produceGryphonRiderButton = nullptr,
		     *producePaladinButton = nullptr, *upgradeTownHallButton = nullptr, *commandPatrolButton = nullptr, *commandStopButton = nullptr;

	list<UIElement*> UIMenuInfoList;

	//Spawning units from barracks queues and variables
	queue<ToSpawnUnit*> toSpawnUnitQueue;
	uint spawningTime = 5; //In seconds
	uint maxSpawnQueueSize = 2;
};

#endif //__j1PLAYER_H__
