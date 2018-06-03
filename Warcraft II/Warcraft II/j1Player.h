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

struct UILabel;
struct UIImage;
struct UIButton;
struct UILifeBar;
class UIMinimap;
class Entity;

class StaticEntity;
enum ENTITY_TYPE;
enum TerenasDialogEvents;
enum StaticEntitySize;
struct UnitInfo;

struct HoverInfo
{
	UILabel* cost = nullptr;
	UILabel* info = nullptr;
	UIImage* background = nullptr;
};

struct ToSpawnUnit {
	ToSpawnUnit(j1Timer toSpawnTimer, ENTITY_TYPE entityType) {
		this->toSpawnTimer = toSpawnTimer;
		this->entityType = entityType;
	}
	j1Timer toSpawnTimer;
	ENTITY_TYPE entityType;
};

struct GroupSpawning {
	ToSpawnUnit** owner = nullptr;

	UIImage* entityIcon = nullptr;
	UILifeBar* entityLifeBar = nullptr;

	void IsActive(bool isActive);
};

struct EntitySelectedStats
{
	UILabel* HP = nullptr;
	UILabel* entityName = nullptr;
	UILabel* entitySight = nullptr;
	UILabel* entityRange = nullptr;
	UILabel* entityDamage = nullptr;
	UILabel* entitySpeed = nullptr;
	UIImage* entityIcon = nullptr;
	UILifeBar* lifeBar = nullptr;

	Entity* entitySelected = nullptr;
};

struct GroupSelectionButtons
{
	UIButton* selectFootmans = nullptr;
	UIButton* selectElvenArchers = nullptr;
	UIButton* selectGryphonRiders = nullptr;
};

struct PlayerGroupsButtons
{
	UIButton* group1 = nullptr;
	UIButton* group2 = nullptr;
	UIButton* group3 = nullptr;
};

enum PlayerGroupTypes 
{
	PlayerGroupTypes_NONE,
	PlayerGroupTypes_FOOTMAN,
	PlayerGroupTypes_ARCHER,
	PlayerGroupTypes_GRYPHON,
	PlayerGroupTypes_FOOTMAN_ARCHER,
	PlayerGroupTypes_FOOTMAN_GRYPHON,
	PlayerGroupTypes_ARCHER_GRYPHON,
	PlayerGroupTypes_ALL	
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

	void CheckIfPlaceBuilding();
	iPoint GetMouseTilePos() const;
	iPoint GetMousePos() const;

	void CheckUnitSpawning(queue<ToSpawnUnit*>* queue);

	void SpawnUnitFromBuilding(StaticEntity* spawnBuilding, ENTITY_TYPE spawningEntity, UnitInfo unitInfo);
	void SpawnUnitAtTile(iPoint spawnTile, ENTITY_TYPE spawningEntity, UnitInfo unitInfo);

	void UpdateSpawnUnitsStats(list<GroupSpawning>* spawningList);

	void AddGold(int sumGold);
	int GetCurrentGold() const;

	// Called before quitting
	bool CleanUp();

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	void OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnDynamicEntitiesEvent(DynamicEntity* staticEntity, EntitiesEvent entitiesEvent);
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

	bool LoadKeys(pugi::xml_node & buttons);

	void SaveKeys();

	void CreateEntitiesStatsUI();
	void CreateGroupSelectionButtons();
	void CreatePlayerGroupsButtons();
	void CreateUISpawningUnits();
	void ShowEntitySelectedInfo(string HPname, string entityNameName, SDL_Rect iconDim, Entity* currentEntity);
	void ShowMineOrRuneStoneSelectedInfo(ENTITY_TYPE entType, SDL_Rect iconDim, string entName, Entity* currentEntity);
	void ShowDynEntityLabelsInfo(string damage, string speed, string sight, string range);
	void ShowEntitySelectedButt(ENTITY_TYPE type);
	void ShowPlayerGroupsButton(int group, PlayerGroupTypes playerGroupType = PlayerGroupTypes_NONE);
	void HideEntitySelectedInfo();
	void MakeUnitMenu(Entity* entity);
	void MakePrisionerMenu(Entity* entity);
	void DeleteEntitiesMenu();
	void DeleteGroupSelectionButtons();
	void DeletePlayerGroupsButtons();
	void CreateHoverInfoMenu(HoverInfo* hoverInfo);
	void ShowHoverInfoMenu(string unitProduce, string gold, HoverInfo* hoverInfo, iPoint pos = { 644, 473 });
	void HideHoverInfoMenu(HoverInfo* hoverInfo);
	void DeleteHoverInfoMenu();
	UIImage * CreateGroupIcon(iPoint iconPos, SDL_Rect texArea, bool isActive = true);
	UILifeBar* CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, bool isActive = true);
	
	void CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button);
	void CreateSimpleSelectionButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button);
	void CreateBarracksButtons();
	void CreateTownHallButtons();
	void CreateDestructionButton();
	void CreateRepairButton();
	void HandleSpawningUnitsUIElem(ToSpawnUnit** toSpawnUnit, list<GroupSpawning>* groupList);
	void HandleGoldMineUIStates();
	void CreateGryphonAviaryButtons();
	void CreateMageTowerButtons();
	uint GetGroupSpawningSize(list<GroupSpawning> listSpawning);
	uint CalculateGoldRepair(StaticEntity * entity);
	void CheckBuildingsState();

	//void DestroyBuilding();

	void RescuePrisoner(TerenasDialogEvents dialogEvent, SDL_Rect iconText, iPoint iconPos);


public:

	//Player
	list<StaticEntity*> chickenFarm;
	list<StaticEntity*> scoutTower;
	list<StaticEntity*> cannonTower;
	list<StaticEntity*> guardTower;
	StaticEntity* barracks = nullptr;
	StaticEntity* townHall = nullptr;
	StaticEntity* gryphonAviary = nullptr;

	vector<UIImage*> imagePrisonersVector;

	//Neutral
	list<StaticEntity*> goldMine;
	list<StaticEntity*> runestone;

	bool barracksUpgrade = false;
	bool townHallUpgrade = false;
	bool keepUpgrade = false;

	uint totalGold = 0u; // total gold earned during the game
	int currentFood = 0; // amount of food (from chicken farms) that the player has at the current moment (1 food feeds 1 unit)
	uint roomsCleared = 0;

	//Units costs
	int footmanCost = 500;
	int elvenArcherCost = 400;
	int paladinCost = 800;
	int ballistaCost = 900;
	int mageCost = 1200;
	int gryphonRiderCost = 750;

	//For finish Screen
	bool isWin = false;
	j1Timer startGameTimer;
	uint unitProduce = 0u;
	uint enemiesKill = 0u;
	uint buildDestroy = 0u;

	EntitySelectedStats entitySelectedStats;

	//bool isUnitSpawning = false;
	bool isMouseOnMine = false;

	HoverInfo firstHoverInfo;
	HoverInfo secondHoverInfo;
	HoverInfo thirdHoverInfo;

	UIMinimap* minimap = nullptr;

	//Spawning units from barracks queues and variables
	queue<ToSpawnUnit*> toSpawnUnitBarracks;
	queue<ToSpawnUnit*> toSpawnUnitGrypho;

	bool isUnitSpawning = false;

	SDL_Scancode* buttonSelectFootman = nullptr;
	SDL_Scancode* buttonSelectArcher = nullptr;
	SDL_Scancode* buttonSelectGryphon = nullptr;
	SDL_Scancode* buttonSelectAll = nullptr;

	SDL_Scancode* buttonShowPlayerButt = nullptr;

	SDL_Scancode* buttonDamageCF = nullptr;

	SDL_Scancode* buttonAddGold = nullptr;
	SDL_Scancode* buttonAddFood = nullptr;

private:

	int currentGold = 0; // amount of gold that the player has at the current moment

	uint maxUnitsSelected = 8;

	double timer = 0.0f; // game time
	uint totalEnemiesKilled = 0;
	uint totalUnitsDead = 0;

	PlayerGroupsButtons playerGroupsButtons;
	GroupSelectionButtons groupSelectionButtons;

	UIButton *produceFootmanButton = nullptr, *produceElvenArcherButton = nullptr, *produceMageButton = nullptr, *produceGryphonRiderButton = nullptr,
		*producePaladinButton = nullptr, *upgradeTownHallButton = nullptr, *destroyBuildingButton = nullptr, *repairBuildingButton = nullptr;

	list<UIElement*> UIMenuInfoList;

	uint spawningTime = 5; //In seconds
	uint maxSpawnQueueSize = 2;

	list<GroupSpawning> barracksSpawningListUI;
	list<GroupSpawning> gryphoSpawningListUI;

	pugi::xml_node config;
};

#endif //__j1PLAYER_H__
