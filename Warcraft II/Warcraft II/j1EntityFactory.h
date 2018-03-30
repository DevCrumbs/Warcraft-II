#ifndef __j1ENTITY_FACTORY_H__
#define __j1ENTITY_FACTORY_H__

#include "j1Module.h"
#include "p2Point.h"

// Units
#include "Dragon.h"
#include "Grunt.h"
#include "TrollAxethrower.h"
#include "Alleria.h"
#include "Khadgar.h"
#include "Turalyon.h"
#include "ElvenArcher.h"
#include "Footman.h"
#include "GryphonRider.h"
#include "Mage.h"
#include "Paladin.h"

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

#include <list>
#include <algorithm>
using namespace std;

#define MAX_UNITS_SELECTED 10

struct SDL_Texture;
struct SDL_Rect;

class Entity;
class StaticEntity;
class DynamicEntity;
struct EntityInfo;
enum ENTITY_TYPE;

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
	void Draw();

	void DrawStaticEntityPreview(ENTITY_TYPE staticEntityType, iPoint mousePos);
	void DrawStaticEntityPreviewTiles(bool isPlaceable, StaticEntitySize buildingSize, iPoint mousePos);

	void HandleStaticEntityPreviewTiles(ENTITY_TYPE staticEntityType, iPoint mousePos);
	
	const EntityInfo& GetBuildingInfo(ENTITY_TYPE staticEntityType);
	const EntityInfo& GetUnitInfo(ENTITY_TYPE dynamicEntityType);

	SDL_Texture* GetHumanBuildingTexture();
	SDL_Texture* GetNeutralBuildingTexture();

	bool IsPreviewBuildingOnEntity(iPoint tile, StaticEntitySize buildingSize) const;
	bool IsEntityOnTileBySize(iPoint tile) const;

	Entity* AddEntity(ENTITY_TYPE entityType, fPoint pos, const EntityInfo& entityInfo, const UnitInfo& unitInfo, j1Module* listener = nullptr);
	void DestroyStaticEntity(StaticEntity* staticEntity);

	/// SANDRA
	// Returns a pointer to the Entity that is on the tile or nullptr
	Entity* IsEntityOnTile(iPoint tile, ENTITY_CATEGORY entityCategory = EntityCategory_NONE, EntitySide entitySide = EntitySide_NoSide) const;

	// Selects an Entity
	bool SelectEntity(Entity* entity);

	// Selects the entities within a rectangle
	void SelectEntitiesWithinRectangle(SDL_Rect rectangleRect, EntitySide entitySide = EntitySide_NoSide);

	// Unselects all entities
	void UnselectAllEntities();

	// Returns a pointer to the DynamicEntity of an Entity
	DynamicEntity* GetDynamicEntityByEntity(Entity* entity) const;

	// Returns a list with the last selected units (unitsSelected list)
	list<DynamicEntity*> GetLastUnitsSelected() const;

	// Updates the selection color of all entities
	void SetUnitsSelectedColor();
	///_SANDRA

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
	KhadgarInfo khadgarInfo;
	TuralyonInfo turalyonInfo;
	GruntInfo gruntInfo;
	TrollAxethrowerInfo trollAxethrowerInfo;
	DragonInfo dragonInfo;

	/// Static entities
	TownHallInfo townHallInfo;
	BarracksInfo barracksInfo;
	ChickenFarmInfo chickenFarmInfo;
	ElvenLumberMillInfo elvenLumberMillInfo;
	StablesInfo stablesInfo;
	GryphonAviaryInfo gryphonAviaryInfo;
	MageTowerInfo mageTowerInfo;
	ScoutTowerInfo scoutTowerInfo;
	PlayerGuardTowerInfo playerGuardTowerInfo; // TODO
	PlayerCannonTowerInfo playerCannonTowerInfo; // TODO
	GoldMineInfo goldMineInfo;
	RunestoneInfo runestoneInfo;
	WatchTowerInfo watchTowerInfo;
	EnemyGuardTowerInfo enemyGuardTowerInfo;
	EnemyCannonTowerInfo enemyCannonTowerInfo;

	//Preview tiles
	BuildingPreviewTiles buildingPreviewTiles;
	uint previewBuildingOpacity;

	TownHall* townHall = nullptr;
};

#endif //__j1ENTITY_FACTORY_H__