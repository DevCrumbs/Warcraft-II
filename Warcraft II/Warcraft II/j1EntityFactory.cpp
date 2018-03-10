#include "Defs.h"
#include "p2Log.h"

#include "j1Module.h"
#include "j1App.h"

#include "j1EntityFactory.h"
#include "j1Render.h"

#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"

#include "Entity.h"
#include "DynamicEntity.h"
#include "StaticEntity.h"

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");
}

// Destructor
j1EntityFactory::~j1EntityFactory()
{
}

bool j1EntityFactory::Awake(pugi::xml_node& config) {

	bool ret = true;

	pugi::xml_node spritesheets = config.child("spritesheets");

	footmanTexName = spritesheets.child("footman").attribute("name").as_string();

	return ret;
}

bool j1EntityFactory::Start()
{
	bool ret = true;

	LOG("Loading entities textures");

	footmanTex = App->tex->Load(footmanTexName.data());

	return ret;
}

bool j1EntityFactory::PreUpdate()
{
	bool ret = true;

	// Spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		fPoint pos = (*it)->GetPosition();
		int x = pos.x * App->scene->scale;
		int y = pos.y * App->scene->scale;
		App->map->WorldToMap(x, y);

		// Move the entity from the waiting list to the active list
		if ((*it)->entityType == EntityType_DynamicEntity) {

			activeDynamicEntities.push_back((DynamicEntity*)(*it));
			LOG("Spawning dynamic entity at tile %d,%d", x, y);
		}
		else if ((*it)->entityType == EntityType_StaticEntity) {

			activeStaticEntities.push_back((StaticEntity*)(*it));
			LOG("Spawning static entity at tile %d,%d", x, y);
		}

		it++;
	}
	toSpawnEntities.clear();

	return ret;
}

// Called before render is available
bool j1EntityFactory::Update(float dt)
{
	bool ret = true;

	// Update active dynamic entities
	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {
		(*it)->Move(dt);
		it++;
	}

	return ret;
}

void j1EntityFactory::Draw()
{
	// Blit active entities

	// Dynamic entities (one texture per dynamic entity)
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		switch ((*dynEnt)->dynamicEntityType) {

			// Player
		case DynamicEntityType_Footman:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_ElvenArcher:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_GryphonRider:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_Mage:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_Paladin:
			(*dynEnt)->Draw(footmanTex);
			break;

		case DynamicEntityType_Turalyon:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_Khadgar:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_Alleria:
			(*dynEnt)->Draw(footmanTex);
			break;

			// Enemy
		case DynamicEntityType_Grunt:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_TrollAxethrower:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_Dragon:
			(*dynEnt)->Draw(footmanTex);
			break;

		default:
			break;
		}

		dynEnt++;
	}

	// Static entities (altas textures for every type of static entity)
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		switch ((*statEnt)->staticEntityCategory) {

		case StaticEntityCategory_HumanBuilding:
			(*statEnt)->Draw(footmanTex);
			break;
		case StaticEntityCategory_NeutralBuilding:
			(*statEnt)->Draw(footmanTex);
			break;
		case StaticEntityCategory_OrcishBuilding:
			(*statEnt)->Draw(footmanTex);
			break;

		default:
			break;
		}

		statEnt++;
	}
}

bool j1EntityFactory::PostUpdate()
{
	bool ret = true;

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}

	return ret;
}

// Called before quitting
bool j1EntityFactory::CleanUp()
{
	bool ret = true;

	LOG("Freeing all entities");

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}
	activeDynamicEntities.clear();

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}
	activeStaticEntities.clear();

	// Remove to spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {
		delete *it;
		it++;
	}
	toSpawnEntities.clear();

	// Free all textures
	App->tex->UnLoad(footmanTex);

	return ret;
}

StaticEntity* j1EntityFactory::AddStaticEntity(StaticEntityType staticEntityType, fPoint pos, iPoint size, uint life, const EntityInfo& entityInfo)
{
	switch (staticEntityType) {
	
	case StaticEntityType_TownHall:
	{
		TownHall* townHall = new TownHall(pos, size, life, (const TownHallInfo&)entityInfo);
		townHall->entityType = EntityType_StaticEntity;
		townHall->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		townHall->staticEntityType = StaticEntityType_TownHall;

		toSpawnEntities.push_back((Entity*)townHall);
		return (StaticEntity*)townHall;
	}
		break;

	case StaticEntityType_ChickenFarm:
	{
		ChickenFarm* chickenFarm = new ChickenFarm(pos, size, life, (const ChickenFarmInfo&)entityInfo);
		chickenFarm->entityType = EntityType_StaticEntity;
		chickenFarm->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		chickenFarm->staticEntityType = StaticEntityType_ChickenFarm;

		toSpawnEntities.push_back((Entity*)chickenFarm);
		return (StaticEntity*)chickenFarm;
	}
		break;

	case StaticEntityType_Barracks:
	{
		Barracks* barracks = new Barracks(pos, size, life, (const BarracksInfo&)entityInfo);
		barracks->entityType = EntityType_StaticEntity;
		barracks->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		barracks->staticEntityType = StaticEntityType_Barracks;

		toSpawnEntities.push_back((Entity*)barracks);
		return (StaticEntity*)barracks;
	}
		break;

	case StaticEntityType_MageTower:
	{
		MageTower* mageTower = new MageTower(pos, size, life, (const MageTowerInfo&)entityInfo);
		mageTower->entityType = EntityType_StaticEntity;
		mageTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		mageTower->staticEntityType = StaticEntityType_MageTower;

		toSpawnEntities.push_back((Entity*)mageTower);
		return (StaticEntity*)mageTower;
	}
		break;

	case StaticEntityType_GryphonAviary:
	{
		GryphonAviary* gryphonAviary = new GryphonAviary(pos, size, life, (const GryphonAviaryInfo&)entityInfo);
		gryphonAviary->entityType = EntityType_StaticEntity;
		gryphonAviary->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		gryphonAviary->staticEntityType = StaticEntityType_GryphonAviary;

		toSpawnEntities.push_back((Entity*)gryphonAviary);
		return (StaticEntity*)gryphonAviary;
	}
		break;

	case StaticEntityType_Stables:
	{
		Stables* stables = new Stables(pos, size, life, (const StablesInfo&)entityInfo);
		stables->entityType = EntityType_StaticEntity;
		stables->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		stables->staticEntityType = StaticEntityType_Stables;

		toSpawnEntities.push_back((Entity*)stables);
		return (StaticEntity*)stables;
	}
		break;

	case StaticEntityType_ScoutTower:
	{
		ScoutTower* scoutTower = new ScoutTower(pos, size, life, (const ScoutTowerInfo&)entityInfo);
		scoutTower->entityType = EntityType_StaticEntity;
		scoutTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		scoutTower->staticEntityType = StaticEntityType_ScoutTower;

		toSpawnEntities.push_back((Entity*)scoutTower);
		return (StaticEntity*)scoutTower;
	}
		break;

	case StaticEntityType_PlayerGuardTower:
	{
		PlayerGuardTower* playerGuardTower = new PlayerGuardTower(pos, size, life, (const PlayerGuardTowerInfo&)entityInfo);
		playerGuardTower->entityType = EntityType_StaticEntity;
		playerGuardTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerGuardTower->staticEntityType = StaticEntityType_PlayerGuardTower;

		toSpawnEntities.push_back((Entity*)playerGuardTower);
		return (StaticEntity*)playerGuardTower;
	}
		break;

	case StaticEntityType_PlayerCannonTower:
	{
		PlayerCannonTower* playerCannonTower = new PlayerCannonTower(pos, size, life, (const PlayerCannonTowerInfo&)entityInfo);
		playerCannonTower->entityType = EntityType_StaticEntity;
		playerCannonTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerCannonTower->staticEntityType = StaticEntityType_PlayerCannonTower;

		toSpawnEntities.push_back((Entity*)playerCannonTower);
		return (StaticEntity*)playerCannonTower;
	}
		break;

	case StaticEntityType_GoldMine:
	{
		GoldMine* goldMine = new GoldMine(pos, size, life, (const GoldMineInfo&)entityInfo);
		goldMine->entityType = EntityType_StaticEntity;
		goldMine->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		goldMine->staticEntityType = StaticEntityType_GoldMine;

		toSpawnEntities.push_back((Entity*)goldMine);
		return (StaticEntity*)goldMine;
	}
		break;

	case StaticEntityType_Runestone:
	{
		Runestone* runestone = new Runestone(pos, size, life, (const RunestoneInfo&)entityInfo);
		runestone->entityType = EntityType_StaticEntity;
		runestone->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		runestone->staticEntityType = StaticEntityType_Runestone;

		toSpawnEntities.push_back((Entity*)runestone);
		return (StaticEntity*)runestone;
	}
		break;

	case StaticEntityType_WatchTower:
	{
		WatchTower* watchTower = new WatchTower(pos, size, life, (const WatchTowerInfo&)entityInfo);
		watchTower->entityType = EntityType_StaticEntity;
		watchTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		watchTower->staticEntityType = StaticEntityType_WatchTower;

		toSpawnEntities.push_back((Entity*)watchTower);
		return (StaticEntity*)watchTower;
	}
		break;

	case StaticEntityType_EnemyGuardTower:
	{
		EnemyGuardTower* enemyGuardTower = new EnemyGuardTower(pos, size, life, (const EnemyGuardTowerInfo&)entityInfo);
		enemyGuardTower->entityType = EntityType_StaticEntity;
		enemyGuardTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyGuardTower->staticEntityType = StaticEntityType_EnemyGuardTower;

		toSpawnEntities.push_back((Entity*)enemyGuardTower);
		return (StaticEntity*)enemyGuardTower;
	}
		break;

	case StaticEntityType_EnemyCannonTower:
	{
		EnemyCannonTower* enemyCannonTower = new EnemyCannonTower(pos, size, life, (const EnemyCannonTowerInfo&)entityInfo);
		enemyCannonTower->entityType = EntityType_StaticEntity;
		enemyCannonTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyCannonTower->staticEntityType = StaticEntityType_EnemyCannonTower;

		toSpawnEntities.push_back((Entity*)enemyCannonTower);
		return (StaticEntity*)enemyCannonTower;
	}
		break;

	default:

		return nullptr;
	}
}

DynamicEntity* j1EntityFactory::AddDynamicEntity(DynamicEntityType dynamicEntityType, fPoint pos, iPoint size, uint life, float speed, const EntityInfo& entityInfo)
{
	switch (dynamicEntityType) {

	case DynamicEntityType_Footman:
	{
		Footman* footman = new Footman(pos, size, life, speed, (const FootmanInfo&)entityInfo);
		footman->entityType = EntityType_DynamicEntity;
		footman->dynamicEntityType = DynamicEntityType_Footman;

		toSpawnEntities.push_back((Entity*)footman);
		return (DynamicEntity*)footman;
	}
		break;

	case DynamicEntityType_ElvenArcher:
	{
		ElvenArcher* elvenArcher = new ElvenArcher(pos, size, life, speed, (const ElvenArcherInfo&)entityInfo);
		elvenArcher->entityType = EntityType_DynamicEntity;
		elvenArcher->dynamicEntityType = DynamicEntityType_ElvenArcher;

		toSpawnEntities.push_back((Entity*)elvenArcher);
		return (DynamicEntity*)elvenArcher;
	}
		break;

	case DynamicEntityType_GryphonRider:
	{
		GryphonRider* gryphonRider = new GryphonRider(pos, size, life, speed, (const GryphonRiderInfo&)entityInfo);
		gryphonRider->entityType = EntityType_DynamicEntity;
		gryphonRider->dynamicEntityType = DynamicEntityType_GryphonRider;

		toSpawnEntities.push_back((Entity*)gryphonRider);
		return (DynamicEntity*)gryphonRider;
	}
		break;

	case DynamicEntityType_Mage:
	{
		Mage* mage = new Mage(pos, size, life, speed, (const MageInfo&)entityInfo);
		mage->entityType = EntityType_DynamicEntity;
		mage->dynamicEntityType = DynamicEntityType_Mage;

		toSpawnEntities.push_back((Entity*)mage);
		return (DynamicEntity*)mage;
	}
		break;

	case DynamicEntityType_Paladin:
	{
		Paladin* paladin = new Paladin(pos, size, life, speed, (const PaladinInfo&)entityInfo);
		paladin->entityType = EntityType_DynamicEntity;
		paladin->dynamicEntityType = DynamicEntityType_Paladin;

		toSpawnEntities.push_back((Entity*)paladin);
		return (DynamicEntity*)paladin;
	}
		break;

	case DynamicEntityType_Turalyon:
	{
		Turalyon* turalyon = new Turalyon(pos, size, life, speed, (const TuralyonInfo&)entityInfo);
		turalyon->entityType = EntityType_DynamicEntity;
		turalyon->dynamicEntityType = DynamicEntityType_Turalyon;

		toSpawnEntities.push_back((Entity*)turalyon);
		return (DynamicEntity*)turalyon;
	}
		break;

	case DynamicEntityType_Khadgar:
	{
		Khadgar* khadgar = new Khadgar(pos, size, life, speed, (const KhadgarInfo&)entityInfo);
		khadgar->entityType = EntityType_DynamicEntity;
		khadgar->dynamicEntityType = DynamicEntityType_Khadgar;

		toSpawnEntities.push_back((Entity*)khadgar);
		return (DynamicEntity*)khadgar;
	}
		break;

	case DynamicEntityType_Alleria:
	{
		Alleria* alleria = new Alleria(pos, size, life, speed, (const AlleriaInfo&)entityInfo);
		alleria->entityType = EntityType_DynamicEntity;
		alleria->dynamicEntityType = DynamicEntityType_Alleria;

		toSpawnEntities.push_back((Entity*)alleria);
		return (DynamicEntity*)alleria;
	}
		break;

	case DynamicEntityType_Grunt:
	{
		Grunt* grunt = new Grunt(pos, size, life, speed, (const GruntInfo&)entityInfo);
		grunt->entityType = EntityType_DynamicEntity;
		grunt->dynamicEntityType = DynamicEntityType_Grunt;

		toSpawnEntities.push_back((Entity*)grunt);
		return (DynamicEntity*)grunt;
	}
		break;

	case DynamicEntityType_TrollAxethrower:
	{
		TrollAxethrower* trollAxethrower = new TrollAxethrower(pos, size, life, speed, (const TrollAxethrowerInfo&)entityInfo);
		trollAxethrower->entityType = EntityType_DynamicEntity;
		trollAxethrower->dynamicEntityType = DynamicEntityType_TrollAxethrower;

		toSpawnEntities.push_back((Entity*)trollAxethrower);
		return (DynamicEntity*)trollAxethrower;
	}
		break;

	case DynamicEntityType_Dragon:
	{
		Dragon* dragon = new Dragon(pos, size, life, speed, (const DragonInfo&)entityInfo);
		dragon->entityType = EntityType_DynamicEntity;
		dragon->dynamicEntityType = DynamicEntityType_Dragon;

		toSpawnEntities.push_back((Entity*)dragon);
		return (DynamicEntity*)dragon;
	}
		break;

	default:

		return nullptr;
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

bool j1EntityFactory::Load(pugi::xml_node& save)
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}

bool j1EntityFactory::Save(pugi::xml_node& save) const
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}