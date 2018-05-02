#ifndef __j1ENTITY_FACTORY_H__
#define __j1ENTITY_FACTORY_H__

#include "j1Module.h"
#include "p2Point.h"

// Units
#include "Dragon.h"
#include "Grunt.h"
#include "TrollAxethrower.h"
#include "Alleria.h"
#include "Turalyon.h"
#include "ElvenArcher.h"
#include "Footman.h"
#include "GryphonRider.h"
#include "Mage.h"
#include "Paladin.h"
#include "CritterBoar.h"
#include "CritterSheep.h"

// Buildings
#include "EnemyCannonTower.h"
#include "EnemyGuardTower.h"
#include "WatchTower.h"
#include "GoldMine.h"
#include "Runestone.h"
#include "PlayerCannonTower.h"
#include "PlayerGuardTower.h"
#include "ScoutTower.h"
#include "Barracks.h"
#include "ChickenFarm.h"
#include "GryphonAviary.h"
#include "MageTower.h"
#include "Stables.h"
#include "TownHall.h"
#include "ElvenLumberMill.h"
#include "GreatHall.h"
#include "Stronghold.h"
#include "Fortress.h"
#include "EnemyBarracks.h"
#include "PigFarm.h"
#include "TrollLumberMill.h"
#include "AltarOfStorms.h"
#include "DragonRoost.h"
#include "TempleOfTheDamned.h"
#include "OgreMound.h"
#include "EnemyBlacksmith.h"

#include <string>
#include <list>
#include <algorithm>
#include <queue>

using namespace std;

#define MAX_UNITS_SELECTED 8

struct SDL_Texture;
struct SDL_Rect;

class Entity;
class StaticEntity;
class DynamicEntity;
struct EntityInfo;
struct UnitInfo;

enum ENTITY_TYPE;

struct EntitiesDraw_info {
	uint priority;
	ENTITY_TYPE type;
	Entity* ent;
};

//TODO 5: Create a struct that will compare entities information priorities
struct compareEntPriority {
	bool operator()(const EntitiesDraw_info& infoA, const EntitiesDraw_info& infoB)
	{
		return infoA.priority > infoB.priority;
	}
};


class j1EntityFactory : public j1Module
{
public:

	j1EntityFactory();
	virtual ~j1EntityFactory();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState);
	bool PostUpdate();
	bool CleanUp();

	void DrawStaticEntityPreview(ENTITY_TYPE staticEntityType, iPoint mousePos);
	void DrawStaticEntityPreviewTiles(bool isPlaceable, StaticEntitySize buildingSize, iPoint mousePos);

	void HandleStaticEntityPreviewTiles(ENTITY_TYPE staticEntityType, iPoint mousePos);
	
	const EntityInfo& GetBuildingInfo(ENTITY_TYPE staticEntityType);
	const EntityInfo& GetUnitInfo(ENTITY_TYPE dynamicEntityType);
	const EntityInfo& GetBuiltBuilding(ENTITY_TYPE staticEntityType);

	SDL_Texture* GetHumanBuildingTexture();
	SDL_Texture* GetNeutralBuildingTexture();

	bool IsPreviewBuildingOnEntity(iPoint tile, StaticEntitySize buildingSize) const;
	bool IsEntityOnTileBySize(iPoint tile) const;

	Entity* AddEntity(ENTITY_TYPE entityType, fPoint pos, const EntityInfo& entityInfo, const UnitInfo& unitInfo, j1Module* listener = nullptr);
	void DestroyStaticEntity(StaticEntity* staticEntity);

	uint CheckNumberOfEntities(ENTITY_TYPE entityType, ENTITY_CATEGORY entityCategory);
	uint GetNumberOfPlayerUnits() const;

	/// SANDRA
	// Returns a pointer to the Entity that is on the tile or nullptr
	Entity* IsEntityOnTile(iPoint tile, ENTITY_CATEGORY entityCategory = EntityCategory_NONE, EntitySide entitySide = EntitySide_NoSide) const;

	// Selects an Entity
	bool SelectEntity(Entity* entity);

	// Selects the entities within a rectangle
	void SelectEntitiesWithinRectangle(SDL_Rect rectangleRect, ENTITY_CATEGORY entityCategory = EntityCategory_NONE, EntitySide entitySide = EntitySide_NoSide);

	// Unselects all entities
	void UnselectAllEntities();

	// Returns a pointer to the DynamicEntity of an Entity
	DynamicEntity* GetDynamicEntityByEntity(Entity* entity) const;

	// Returns a list with the last selected units (unitsSelected list)
	list<DynamicEntity*> GetLastUnitsSelected() const;

	bool RemoveUnitFromUnitsSelected(Entity* entity);

	// Updates the selection color of all entities
	void SetUnitsSelectedColor();

	bool CommandToUnits(list<DynamicEntity*> units, UnitCommand unitCommand = UnitCommand_NoCommand);

	bool RemoveAllUnitsGoals(list<DynamicEntity*> units);

	// Attack
	bool InvalidateTargetInfo(Entity* target);

	// Movement
	void InvalidateMovementEntity(Entity* entity);

	// Entities
	Entity* IsEntityUnderMouse(iPoint mousePos, ENTITY_CATEGORY entityCategory = EntityCategory_NONE, EntitySide entitySide = EntitySide_NoSide) const;
	void SelectEntitiesOnScreen(ENTITY_TYPE entityType = EntityType_NONE);

	// Dynamic Entities
	bool IsOnlyThisTypeOfUnits(list<DynamicEntity*> units, ENTITY_TYPE entityType = EntityType_NONE);
	bool AreAllUnitsDoingSomething(list<DynamicEntity*> units, UnitState unitState = UnitState_NoState);

	// Static Entities
	bool SelectBuilding(StaticEntity* staticEntity);
	void UnselectAllBuildings();

	uint DetermineBuildingMaxLife(ENTITY_TYPE buildingType = EntityType_NONE, StaticEntitySize buildingSize = StaticEntitySize_None);
	uint DetermineBuildingGold(ENTITY_TYPE buildingType = EntityType_NONE, StaticEntitySize buildingSize = StaticEntitySize_None);
	
	list<iPoint> GetBuildingTiles(StaticEntity* building, bool isOnlySurroundingTiles = false);
	///_SANDRA

	// -----

	bool Save(pugi::xml_node& save) const;
	bool Load(pugi::xml_node& save);

public:

	list<Entity*> toSpawnEntities;
	list<DynamicEntity*> activeDynamicEntities;
	list<StaticEntity*> activeStaticEntities;
	list<DynamicEntity*> unitsSelected;

private:

	// Textures
	/// Dynamic entities
	string footmanTexName;
	string elvenArcherTexName;
	string gryphonRiderTexName;
	string mageTexName;
	string paladinTexName;
	string alleriaTexName;
	string khadgarTexName;
	string turalyonTexName;
	string gruntTexName;
	string trollAxethrowerTexName;
	string dragonTexName;
	string crittersTexName;

	SDL_Texture* footmanTex = nullptr;
	SDL_Texture* elvenArcherTex = nullptr;
	SDL_Texture* gryphonRiderTex = nullptr;
	SDL_Texture* mageTex = nullptr;
	SDL_Texture* paladinTex = nullptr;
	SDL_Texture* alleriaTex = nullptr;
	SDL_Texture* khadgarTex = nullptr;
	SDL_Texture* turalyonTex = nullptr;
	SDL_Texture* gruntTex = nullptr;
	SDL_Texture* trollAxethrowerTex = nullptr;
	SDL_Texture* dragonTex = nullptr;
	SDL_Texture* crittersTex = nullptr;

	/// Static entities
	string humanBuildingsTexName;
	string orcishBuildingsTexName;
	string neutralBuildingsTexName;

	SDL_Texture* humanBuildingsTex = nullptr;
	SDL_Texture* orcishBuildingsTex = nullptr;
	SDL_Texture* neutralBuildingsTex = nullptr;

	// Info structs
	/// Dynamic entities
	FootmanInfo footmanInfo;
	ElvenArcherInfo elvenArcherInfo;
	GryphonRiderInfo gryphonRiderInfo;
	MageInfo mageInfo;
	PaladinInfo paladinInfo;
	AlleriaInfo alleriaInfo;
	TuralyonInfo turalyonInfo;
	GruntInfo gruntInfo;
	TrollAxethrowerInfo trollAxethrowerInfo;
	DragonInfo dragonInfo;
	CritterSheepInfo critterSheepInfo;
	CritterBoarInfo critterBoarInfo;

	/// Static entities
	//Player buildings
	TownHallInfo townHallInfo;
	StrongholdInfo strongholdInfo;
	BarracksInfo barracksInfo;
	BarracksInfo builtBarracksInfo;
	ChickenFarmInfo chickenFarmInfo;
	ChickenFarmInfo builtChickenFarmInfo;
	ElvenLumberMillInfo elvenLumberMillInfo;
	StablesInfo stablesInfo;
	GryphonAviaryInfo gryphonAviaryInfo;
	MageTowerInfo mageTowerInfo;
	ScoutTowerInfo scoutTowerInfo;
	PlayerGuardTowerInfo playerGuardTowerInfo; 
	PlayerCannonTowerInfo playerCannonTowerInfo;

	//Neutral buildings
	GoldMineInfo goldMineInfo;
	RunestoneInfo runestoneInfo;

	//Enemy buildings
	GreatHallInfo greatHallInfo;
	EnemyBarracksInfo enemyBarracksInfo;
	PigFarmInfo pigFarmInfo;
	TrollLumberMillInfo trollLumberMillInfo;
	FortressInfo fortressInfo;
	AltarOfStormsInfo altarOfStormsInfo;
	DragonRoostInfo dragonRoostInfo;
	TempleOfTheDamnedInfo templeOfTheDamnedInfo;
	OgreMoundInfo ogreMoundInfo;
	EnemyBlacksmithInfo enemyBlacksmithInfo;
	WatchTowerInfo watchTowerInfo;
	EnemyGuardTowerInfo enemyGuardTowerInfo;
	EnemyCannonTowerInfo enemyCannonTowerInfo;

	//Preview tiles
	uint previewTilesopacity = 255;
	uint previewBuildingOpacity = 255;

	TownHall* townHall = nullptr;
};

#endif //__j1ENTITY_FACTORY_H__