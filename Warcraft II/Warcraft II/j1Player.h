#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "j1Timer.h"
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

struct HoverInfo
{
	UILabel* cost = nullptr;
	UILabel* info = nullptr;
	UIImage* background = nullptr;
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

	void AddGold(int sumGold);
	int GetCurrentGold();

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
	void DeleteHoverInfoMenu();
	//void CheckBuildingState(Entity* ent);

	void CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity);
	void DestroyHoverButton(Entity* ent);
	void CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button);
	void DestroyUIElem(UIElement* elem);
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

	int totalGold = 0; // total gold earned during the game
	int currentGold = 0; // amount of gold that the player has at the current moment
	//Units costs
	int footmanCost = 500;
	int elvenArcherCost = 400;
	int paladinCost = 800;
	int ballistaCost = 900;
	int mageCost = 1200;
	int gryphonRiderCost = 2500;

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

	UIButton *produceFootmanButton, *produceElvenArcherButton, *produceMageButton, *produceGryphonRiderButton, *producePaladinButton;

	list<UIElement*> UIMenuInfoList;

};

#endif //__j1PLAYER_H__
