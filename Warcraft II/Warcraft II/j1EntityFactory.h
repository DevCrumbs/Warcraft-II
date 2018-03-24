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

#include <algorithm>
using namespace std;

#define MAX_ENTITIES_SELECTED 8

struct SDL_Texture;
struct SDL_Rect;

class Entity;
class StaticEntity;
class DynamicEntity;
struct EntityInfo;
enum StaticEntityType;
enum DynamicEntityType;

class j1EntityFactory : public j1Module
{
public:

	j1EntityFactory();
	virtual ~j1EntityFactory();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Draw();
	void DrawStaticEntityPreview(StaticEntityType staticEntityType, iPoint mousePos);
	const EntityInfo& GetBuildingInfo(StaticEntityType staticEntityType);
	SDL_Texture* GetHumanBuildingTexture();
	bool isEntityOnTile(iPoint tile, bool isBigBuilding) const;

	StaticEntity* AddStaticEntity(StaticEntityType staticEntityType, fPoint pos, const EntityInfo& entityInfo, j1Module* listener = nullptr);
	DynamicEntity* AddDynamicEntity(DynamicEntityType dynamicEntityType, fPoint pos, iPoint size, uint life, float speed, const EntityInfo& entityInfo, j1Module* listener = nullptr);

	bool Save(pugi::xml_node& save) const;
	bool Load(pugi::xml_node& save);

	list<Entity*> toSpawnEntities;
	list<DynamicEntity*> activeDynamicEntities;
	list<StaticEntity*> activeStaticEntities;

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

	TownHall* townHall = nullptr;
};

#endif //__j1ENTITY_FACTORY_H__