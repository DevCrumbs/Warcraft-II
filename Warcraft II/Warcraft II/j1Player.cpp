#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"
#include "j1Scene.h"
#include "j1Menu.h"
#include "j1Gui.h"
#include "j1Pathfinding.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "j1Movement.h"
#include "Goal.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILifeBar.h"
#include "UICursor.h"
#include "UIMinimap.h"

j1Player::j1Player() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Player::~j1Player() {}

// Called before render is available
bool j1Player::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Player::Start()
{
	bool ret = true;

	currentGold = totalGold = 0;
	currentFood = 0;
	unitProduce = 0u;
	enemiesKill = 0u;
	buildDestroy = 0u;
	roomsCleared = 0;
	isWin = false;
	townHallUpgrade = false;

	totalEnemiesKilled = 0;
	totalUnitsDead = 0;

	startGameTimer.Start();

	return ret;
}

bool j1Player::PreUpdate() 
{
	if (minimap == nullptr)
	{
		UIMinimap_Info info;

		info.entityHeight = 32;
		info.entityHeight = 32;
		info.minimapInfo = { 30,31,160,161 };

		minimap = App->gui->CreateUIMinimap(info, this);
	}


	return true;
}

bool j1Player::Update(float dt)  
{
	//Check if a building needs to be placed
	if(App->scene->GetAlphaBuilding() != EntityType_NONE)
		CheckIfPlaceBuilding();

	//Check if the units need to spawn
	isUnitSpawning = false;
	if (!toSpawnUnitBarracks.empty())
		CheckUnitSpawning(&toSpawnUnitBarracks);

	if (!toSpawnUnitGrypho.empty())
		CheckUnitSpawning(&toSpawnUnitGrypho);

	// Select all units on screen shortcuts
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {

		App->entities->SelectEntitiesOnScreen(EntityType_FOOTMAN);

		list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0)
			App->scene->PlayUnitSound(units, true); // Unit selected sound
		else
			App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
	}
	else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) {

		App->entities->SelectEntitiesOnScreen(EntityType_ELVEN_ARCHER);

		list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0)
			App->scene->PlayUnitSound(units, true); // Unit selected sound
		else
			App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
	}
	else if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {

		App->entities->SelectEntitiesOnScreen(EntityType_GRYPHON_RIDER);

		list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0)
			App->scene->PlayUnitSound(units, true); // Unit selected sound
		else
			App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
	}

	/*
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		if (stables != nullptr) {
			if (stables->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)stables;
				ent->ApplyDamage(20);
				if (!stables->CheckBuildingState() && entitySelectedStats.entitySelected == ent) 
					DeleteEntitiesMenu();			
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		if (mageTower != nullptr)
			if (mageTower->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)mageTower;
				ent->ApplyDamage(20);
				if (!mageTower->CheckBuildingState() && entitySelectedStats.entitySelected == ent) {
					DeleteEntitiesMenu();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		if (!scoutTower.empty())
			if (scoutTower.back()->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)scoutTower.back();
				ent->ApplyDamage(20);
				if (!scoutTower.back()->CheckBuildingState()) {
					if (entitySelectedStats.entitySelected == ent)
						DeleteEntitiesMenu();
					scoutTower.pop_back();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	
	
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		if (barracks != nullptr)
			if (barracks->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)barracks;
				ent->ApplyDamage(200);
				if (!barracks->CheckBuildingState()){
					if(entitySelectedStats.entitySelected == ent)
						HideEntitySelectedInfo();
					barracks = nullptr;
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(200);
				}
			}
	*/

	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		if (!chickenFarm.empty())
			if (chickenFarm.back()->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)chickenFarm.back();
				ent->ApplyDamage(20);
				if (!chickenFarm.back()->CheckBuildingState()) {
					if (entitySelectedStats.entitySelected == ent)
						HideEntitySelectedInfo();
					chickenFarm.pop_back();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	}
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
		App->audio->PlayFx(App->audio->GetFX().goldMine, 0); // Gold mine sound
		AddGold(500);
	}
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		currentFood += 3;
		App->scene->hasFoodChanged = true;
	}
	//Handle the gold mine label
	//idk if put this here or in PreUpdate

	//Update Selectet unit HP
	if (entitySelectedStats.entitySelected != nullptr)
	{
		StaticEntity* building = (StaticEntity*)entitySelectedStats.entitySelected;
		if (entitySelectedStats.entitySelected->entitySide == EntitySide_Player)
		{
			if (entitySelectedStats.entitySelected->entityType == EntityCategory_STATIC_ENTITY) {
				//1st time that building finished to build
				if (building->GetConstructionTimer() == building->GetConstructionTime()) {
					entitySelectedStats.lifeBar->SetMaxLife(building->GetMaxLife());
					entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17 });
					ShowEntitySelectedButt(building->staticEntityType);
				}
				//Update HP when building is finished
				if (building->GetIsFinishedBuilt()) {
					entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
					entitySelectedStats.lifeBar->SetLife(entitySelectedStats.entitySelected->GetCurrLife());
				}
				//Update timer
				else {
					entitySelectedStats.lifeBar->SetLife(building->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
				}
			}
			else {
				//Update HP Dynamic units
				entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
				entitySelectedStats.lifeBar->SetLife(entitySelectedStats.entitySelected->GetCurrLife());

			}
			//Erase info when entity dies
			if (entitySelectedStats.entitySelected->GetCurrLife() <= 0)
				HideEntitySelectedInfo();
		}

		else if (building->staticEntityType == EntityType_GOLD_MINE) {
			HandleGoldMineUIStates();
		}
	}
	CheckBuildingsState();
	return true;
}

bool j1Player::PostUpdate() {


	return true;
}

iPoint j1Player::GetMouseTilePos() const{

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);

	return mouseTile;
}

iPoint j1Player::GetMousePos() const{

	iPoint mouseTilePos = App->map->MapToWorld(GetMouseTilePos().x, GetMouseTilePos().y);

	return mouseTilePos;
}

void j1Player::AddGold(int sumGold)
{
	currentGold += sumGold;
	if (sumGold > 0) {
		totalGold += sumGold;
		App->scene->hasGoldChanged = GoldChange_Win;
	}
	else 
		App->scene->hasGoldChanged = GoldChange_Lose;
}

int j1Player::GetCurrentGold() const
{
	return currentGold;
}

void j1Player::CheckIfPlaceBuilding()
{
	// Mouse position (world and map coords)
	float auxX = (int)GetMousePos().x;
	float auxY = (int)GetMousePos().y;
	fPoint buildingPos = { auxX, auxY };
	iPoint buildingTile = App->map->WorldToMap(buildingPos.x, buildingPos.y);

	ENTITY_TYPE alphaBuilding = App->scene->GetAlphaBuilding();

	//Creates static entities (buildings)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !App->gui->IsMouseOnUI())
	{
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

		UnitInfo unitInfo;

		switch (alphaBuilding) 
		{
		case EntityType_CHICKEN_FARM:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small)) {
				StaticEntity* c;
				c = (StaticEntity*)App->entities->AddEntity(EntityType_CHICKEN_FARM, buildingPos, App->entities->GetBuildingInfo(EntityType_CHICKEN_FARM), unitInfo, this);
				chickenFarm.push_back(c);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->chickenFarmCost); //Discount gold
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		/*case EntityType_STABLES:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium)) {
				stables = (StaticEntity*)App->entities->AddEntity(EntityType_STABLES, buildingPos, App->entities->GetBuildingInfo(EntityType_STABLES), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->stablesCost); //Discount gold
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
			*/
		case EntityType_GRYPHON_AVIARY:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium)) {
				gryphonAviary = (StaticEntity*)App->entities->AddEntity(EntityType_GRYPHON_AVIARY, buildingPos, App->entities->GetBuildingInfo(EntityType_GRYPHON_AVIARY), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->gryphonAviaryCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		/*case EntityType_MAGE_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium)) {
				mageTower = (StaticEntity*)App->entities->AddEntity(EntityType_MAGE_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_MAGE_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->mageTowerCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
*/
		case EntityType_SCOUT_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small)) {
				StaticEntity* s;
				s = (StaticEntity*)App->entities->AddEntity(EntityType_SCOUT_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_SCOUT_TOWER), unitInfo, this);
				scoutTower.push_back(s);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->scoutTowerCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_PLAYER_GUARD_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small)) {
				guardTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_GUARD_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_GUARD_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->guardTowerCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
		case EntityType_PLAYER_CANNON_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small)) {
				cannonTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_CANNON_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_CANNON_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->cannonTowerCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
		case EntityType_BARRACKS:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium)) {
				barracks = (StaticEntity*)App->entities->AddEntity(EntityType_BARRACKS, buildingPos, App->entities->GetBuildingInfo(EntityType_BARRACKS), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->barracksCost); //Discount gold
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), StaticEntitySize_Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
		case EntityType_NONE:
			break;

		default:
			break;
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !App->gui->IsMouseOnUI())
		App->scene->SetAplphaBuilding(EntityType_NONE);

}

//This method checks for the spawning queue of the units and if they're ready to spawn or not
void j1Player::CheckUnitSpawning(queue<ToSpawnUnit*>* queue)
{
	UnitInfo unitInfo;

	list<GroupSpawning>::iterator lastElem;

	if (!queue->empty())
	{
		if (queue->front()->toSpawnTimer.Read() > (spawningTime * 1000)) {
			ENTITY_TYPE toSpawnEntity = queue->front()->entityType;

			switch (toSpawnEntity) {

			case EntityType_FOOTMAN:
				if (barracks != nullptr) {
					SpawnUnitFromBuilding(barracks, EntityType_FOOTMAN, unitInfo);
					App->audio->PlayFx(App->audio->GetFX().footmanReady, 0);
					barracksSpawningListUI.front().entityIcon->isActive = false;
					barracksSpawningListUI.front().entityLifeBar->isActive = false;
					barracksSpawningListUI.front().owner = nullptr;
				}
				break;

			case EntityType_ELVEN_ARCHER:
				if (barracks != nullptr) {
					SpawnUnitFromBuilding(barracks, EntityType_ELVEN_ARCHER, unitInfo);
					App->audio->PlayFx(App->audio->GetFX().archerReady, 0);
					barracksSpawningListUI.front().entityIcon->isActive = false;
					barracksSpawningListUI.front().entityLifeBar->isActive = false;
					barracksSpawningListUI.front().owner = nullptr;
				}
				break;

			case EntityType_GRYPHON_RIDER:
				if (gryphonAviary != nullptr) {
					SpawnUnitFromBuilding(gryphonAviary, EntityType_GRYPHON_RIDER, unitInfo);
					App->audio->PlayFx(App->audio->GetFX().griffonReady, 0);
					gryphoSpawningListUI.front().entityIcon->isActive = false;
					gryphoSpawningListUI.front().entityLifeBar->isActive = false;
					gryphoSpawningListUI.front().owner = nullptr;
				}
				break;

			default:
				break;
			}

			delete queue->front();
			queue->front() = nullptr;
			queue->pop();

			if (entitySelectedStats.entitySelected == barracks) 
				UpdateSpawnUnitsStats(&barracksSpawningListUI);

			else if (entitySelectedStats.entitySelected == gryphonAviary)
				UpdateSpawnUnitsStats(&gryphoSpawningListUI);
		}
	}
	for (list<GroupSpawning>::iterator gryphIter = gryphoSpawningListUI.begin(), barrackIter = barracksSpawningListUI.begin(); gryphIter != gryphoSpawningListUI.end(); ++gryphIter, ++barrackIter)
	{
		if ((*gryphIter).entityLifeBar->isActive && *(*gryphIter).owner != nullptr)
			(*gryphIter).entityLifeBar->SetLife((*(*gryphIter).owner)->toSpawnTimer.ReadSec());

		if ((*barrackIter).entityLifeBar->isActive && *(*barrackIter).owner != nullptr)
			(*barrackIter).entityLifeBar->SetLife((*(*barrackIter).owner)->toSpawnTimer.ReadSec());
	}

}

void j1Player::SpawnUnitFromBuilding(StaticEntity* spawnBuilding, ENTITY_TYPE spawningEntity, UnitInfo unitInfo)
{
	if (spawnBuilding == nullptr)
		return;

	iPoint spawnTile = { -1, -1 };

	// Option a: find a spawn tile from the surrounding tiles of the spawn building
	list<iPoint> surroundingBuildingTiles = App->entities->GetBuildingTiles(spawnBuilding, true);
	list<iPoint>::const_iterator it = surroundingBuildingTiles.begin();
	while (it != surroundingBuildingTiles.end()) {
	
		if (App->pathfinding->IsWalkable(*it)
			&& App->entities->IsEntityOnTile(*it) == nullptr) {

			spawnTile = *it;
			break;
		}
		it++;
	}

	// Option b: find a new, valid spawn tile
	if (spawnTile.x == -1 && spawnTile.y == -1) {
	
		iPoint buildingTile = App->map->WorldToMap((int)spawnBuilding->GetPos().x, (int)spawnBuilding->GetPos().y);
		buildingTile.x -= 1;
		spawnTile = App->movement->FindClosestValidTile(buildingTile);
	}

	// Spawn the entity on the spawn tile
	if (spawnTile.x != -1 && spawnTile.y != -1) {

		iPoint spawnPos = App->map->MapToWorld(spawnTile.x, spawnTile.y);

		App->entities->AddEntity(spawningEntity, { (float)spawnPos.x, (float)spawnPos.y }, (EntityInfo&)App->entities->GetUnitInfo(spawningEntity), unitInfo, this);
	}
	isUnitSpawning = true;
}

void j1Player::SpawnUnitAtTile(iPoint spawnTile, ENTITY_TYPE spawningEntity, UnitInfo unitInfo) 
{
	iPoint toSpawnTile = spawnTile;

	// If the spawnTile is not valid, find a new, valid spawn tile
	if ((toSpawnTile.x == -1 && toSpawnTile.y == -1)
		|| App->entities->IsEntityOnTile(toSpawnTile) != nullptr || !App->pathfinding->IsWalkable(toSpawnTile)) {

		toSpawnTile = App->movement->FindClosestValidTile(toSpawnTile);
	}

	// Spawn the entity on the spawn tile
	if (toSpawnTile.x != -1 && toSpawnTile.y != -1) {

		iPoint spawnPos = App->map->MapToWorld(toSpawnTile.x, toSpawnTile.y);

		App->entities->AddEntity(spawningEntity, { (float)spawnPos.x, (float)spawnPos.y }, (EntityInfo&)App->entities->GetUnitInfo(spawningEntity), unitInfo, this);
	}
}

void j1Player::UpdateSpawnUnitsStats(list<GroupSpawning>* spawningList)
{
	
	auto SpawnIter2 = barracksSpawningListUI.begin();
	auto SpawnIter1 = SpawnIter2++;
	auto lastSpwan = barracksSpawningListUI.end();

	if (spawningList == &gryphoSpawningListUI) {
		SpawnIter2 = gryphoSpawningListUI.begin();
		SpawnIter1 = SpawnIter2++;
		lastSpwan = gryphoSpawningListUI.end();
	}
	for (;;)
	{
		if ((*SpawnIter1).owner == nullptr && (*SpawnIter2).owner != nullptr)
			swap(*SpawnIter1, *SpawnIter2);


		SpawnIter1 = SpawnIter2++;
		if ((*SpawnIter1).owner == nullptr && (*SpawnIter2).owner != nullptr)
			swap(*SpawnIter1, *SpawnIter2);
		if (SpawnIter2 == lastSpwan)
			break;
		SpawnIter1 = SpawnIter2++;
		if (SpawnIter2 == lastSpwan)
			break;

	}
	
	int cont = 0;
	for (list<GroupSpawning>::iterator iterator = spawningList->begin(); iterator != spawningList->end(); ++iterator)
	{
		(*iterator).entityIcon->SetLocalPos({ 48 * cont + 72, 17 });
		(*iterator).entityLifeBar->SetLocalPos({ 48 * cont + 72, 47 });
		cont++;
		if ((*iterator).owner != nullptr)
			if (*(*iterator).owner == nullptr)
				(*iterator).IsActive(false);

	}
	LOG("Updating units stats");
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;

	// UI Elements
	if (!App->gui->isGuiCleanUp) {

		for (; !UIMenuInfoList.empty(); UIMenuInfoList.pop_back())
		{
			UIMenuInfoList.back()->toRemove = true;
		}
		UIMenuInfoList.clear();

		for (; !imagePrisonersVector.empty(); imagePrisonersVector.pop_back())
		{
			imagePrisonersVector.back()->toRemove = true;
		}
		imagePrisonersVector.clear();

		DeleteHoverInfoMenu();
		DeleteEntitiesMenu();
		DeleteGroupSelectionButtons();

		App->gui->RemoveElem((UIElement**)&minimap);
	}

	// Entities
	for (; !chickenFarm.empty(); chickenFarm.pop_back())
	{
		chickenFarm.back()->isRemove = true;
	}
	chickenFarm.clear();

	for (; !scoutTower.empty(); scoutTower.pop_back())
	{
		scoutTower.back()->isRemove = true;
	}
	scoutTower.clear();

	for (; !cannonTower.empty(); cannonTower.pop_back())
	{
		cannonTower.back()->isRemove = true;
	}
	cannonTower.clear();

	for (; !guardTower.empty(); guardTower.pop_back())
	{
		guardTower.back()->isRemove = true;
	}
	guardTower.clear();

	for (; !goldMine.empty(); goldMine.pop_back())
	{
		goldMine.back()->isRemove = true;
	}
	goldMine.clear();

	for (; !runestone.empty(); runestone.pop_back())
	{
		runestone.back()->isRemove = true;
	}
	runestone.clear();

	if (barracks != nullptr) {
		barracks->isRemove = true;
		barracks = nullptr;
	}

	if (townHall) {
		townHall->isRemove = true;
		townHall = nullptr;
	}

	while(!toSpawnUnitBarracks.empty())
	{
		ToSpawnUnit* unit = toSpawnUnitBarracks.front();

		delete unit;
		unit = nullptr;

		toSpawnUnitBarracks.pop();
	}

	while (!toSpawnUnitGrypho.empty())
	{
		delete toSpawnUnitGrypho.front();
		toSpawnUnitGrypho.pop();
	}

	return ret;
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Save
bool j1Player::Save(pugi::xml_node& save) const
{
	bool ret = true;

	/*
	if (save.child("gate") == NULL) {
	save.append_child("gate").append_attribute("opened") = gate;
	save.child("gate").append_attribute("fx") = fx;
	}
	else {
	save.child("gate").attribute("opened") = gate;
	save.child("gate").attribute("fx") = fx;
	}
	*/

	return ret;
}

// Load
bool j1Player::Load(pugi::xml_node& save)
{
	bool ret = true;

	/*
	if (save.child("gate") != NULL) {
	gate = save.child("gate").attribute("opened").as_bool();
	fx = save.child("gate").attribute("fx").as_bool();
	}
	*/

	return ret;

}

void j1Player::OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent)
{
	Entity* ent = (Entity*)staticEntity;

	if (App->scene->GetPauseMenuActions() == PauseMenuActions_NOT_EXIST && App->scene->GetAlphaBuilding() == EntityType_NONE) {

		switch (entitiesEvent)
		{

		case EntitiesEvent_NONE:
			break;

		case EntitiesEvent_RIGHT_CLICK:

			/// TODO Sandra: only Footman and Elven Archer must be able to gather gold (King Terenas says that gold cannot be gathered by using a Gryphon Rider)
			// Gold Mine (right click to send a unit to gather gold)
 			if (staticEntity->staticEntityType == EntityType_GOLD_MINE && staticEntity->buildingState == BuildingState_Normal) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					bool isGryphonRider = false;

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						if ((*it)->dynamicEntityType == EntityType_GRYPHON_RIDER) {
						
							isGryphonRider = true;
							break;
						}

						it++;
					}

					if (!isGryphonRider) {

						it = units.begin();

						while (it != units.end()) {

							(*it)->SetGoldMine((GoldMine*)staticEntity);

							it++;
						}

						App->entities->CommandToUnits(units, UnitCommand_GatherGold);
					}
					else if (App->scene->adviceMessage != AdviceMessage_GRYPH_MINE) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GRYPH_MINE;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (App->scene->adviceMessage != AdviceMessage_MINE) {
					App->scene->adviceMessageTimer.Start();
					App->scene->adviceMessage = AdviceMessage_MINE;
					App->scene->ShowAdviceMessage(App->scene->adviceMessage);
				}
			}

			// Runestone (right click to send a unit to heal the group)
			else if (staticEntity->staticEntityType == EntityType_RUNESTONE && staticEntity->buildingState == BuildingState_Normal) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						(*it)->SetRunestone((Runestone*)staticEntity);

						it++;
					}

					App->entities->CommandToUnits(units, UnitCommand_HealRunestone);
				}
				else if (App->scene->terenasDialogEvent != TerenasDialog_RUNESTONE) {
					App->scene->terenasDialogTimer.Start();
					App->scene->terenasDialogEvent = TerenasDialog_RUNESTONE;
					App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
				}
			}
			break;

		case EntitiesEvent_LEFT_CLICK:

			if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM) {
				App->audio->PlayFx(App->audio->GetFX().chickenFarm, 0); //Chicken farm sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY) {
				App->audio->PlayFx(App->audio->GetFX().gryphonAviary, 0); //Gryphon aviary sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().mageTower, 0); //Mage tower sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Mage Tower", { 394,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_PLAYER_GUARD_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Guard Tower", { 394,76,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_PLAYER_CANNON_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Cannon Tower", { 394,118,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_STABLES) {
				App->audio->PlayFx(App->audio->GetFX().stables, 0); //Stables sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Stables", { 241,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_BARRACKS && staticEntity->GetIsFinishedBuilt()) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Barracks", { 546,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && keepUpgrade && staticEntity->GetIsFinishedBuilt()) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Castle", { 546,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && townHallUpgrade && staticEntity->GetIsFinishedBuilt()) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Keep", { 597,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && staticEntity->GetIsFinishedBuilt()) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Town Hall", { 597,160,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_GOLD_MINE && App->fow->IsOnSight(GetMousePos())) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowMineOrRuneStoneSelectedInfo(EntityType_GOLD_MINE,  { 848, 112, 50, 41 }, "Gold Mine", ent);
			}
			else if (staticEntity->staticEntityType == EntityType_RUNESTONE && App->fow->IsOnSight(GetMousePos())) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowMineOrRuneStoneSelectedInfo(EntityType_RUNESTONE, { 848, 67, 50, 41 }, "RuneStone", ent);
			}
			break;

		case EntitiesEvent_HOVER:

			// Mouse turns into a lens when hovering
			if (staticEntity->staticEntityCategory == StaticEntityCategory_HumanBuilding ||
				staticEntity->staticEntityCategory == StaticEntityCategory_NeutralBuilding)

				if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos()))
					App->menu->mouseText->SetTexArea({ 503, 524, 30, 32 }, { 503, 524, 30, 32 });
			
			if (staticEntity->staticEntityType == EntityType_GOLD_MINE) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					bool isGryphonRider = false;

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						if ((*it)->dynamicEntityType == EntityType_GRYPHON_RIDER) {

							isGryphonRider = true;
							break;
						}

						it++;
					}

					if (!isGryphonRider) {

						GoldMine* goldMine = (GoldMine*)staticEntity;

						if (goldMine->GetGoldMineState() == GoldMineState_Untouched) {

							if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos())) {
								App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 }); //Mouse Hammer
								App->player->isMouseOnMine = true;
							}
						}
						else if (goldMine->GetGoldMineState() == GoldMineState_Gathering ||
							goldMine->GetGoldMineState() == GoldMineState_Gathered) {

							if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos())) {
								App->menu->mouseText->SetTexArea({ 503, 524, 30, 32 }, { 503, 524, 30, 32 }); //Mouse Lens
								App->player->isMouseOnMine = true;
							}
						}
					}
				}
			}
			if (staticEntity->staticEntityType == EntityType_RUNESTONE) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					Runestone* runestone = (Runestone*)staticEntity;

					if (runestone->GetRunestoneState() == RunestoneState_Untouched) {

						if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos())) {
							App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 }); //Mouse Hammer
							App->player->isMouseOnMine = true;
						}
					}
					if (runestone->GetRunestoneState() == RunestoneState_Gathering || 
						runestone->GetRunestoneState() == RunestoneState_Gathered) {

						if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos())) {
							App->menu->mouseText->SetTexArea({ 503, 524, 30, 32 }, { 503, 524, 30, 32 }); //Mouse Lens
							App->player->isMouseOnMine = true;
						}
					}
				}
			}

			break;
		case EntitiesEvent_LEAVE:
			App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
			App->player->isMouseOnMine = false;
			break;
		case EntitiesEvent_CREATED:
			if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Chicken Farm", { 241,34,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Gryphon Aviary", { 394,160,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Mage Tower", { 394,202,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Scout Tower", { 394,34,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_PLAYER_GUARD_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Guard Tower", { 394,76,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_PLAYER_CANNON_TOWER) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Cannon Tower", { 394,118,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_STABLES) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Stables", { 241,160,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_BARRACKS) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Barracks", { 546,160,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo("Building...", "Keep", { 597,202,50,41 }, ent);
			}
			break;

		default:
			break;
		}
	}
}

void j1Player::OnDynamicEntitiesEvent(DynamicEntity* dynamicEntity, EntitiesEvent entitiesEvent) {

	switch (entitiesEvent)
	{
	case EntitiesEvent_NONE:
		break;
	case EntitiesEvent_RIGHT_CLICK:

		// Alleria (right click to send a unit to rescue her)
		/// TODO Sandra: only Footman and Elven Archer must be able to rescue a Prisoner (King Terenas says that Alleria cannot be rescued by using a Gryphon Rider)
		if (dynamicEntity->dynamicEntityType == EntityType_ALLERIA) {

			Alleria* alleria = (Alleria*)dynamicEntity;

			if (!alleria->IsRescued()) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					bool isGryphonRider = false;

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						if ((*it)->dynamicEntityType == EntityType_GRYPHON_RIDER) {

							isGryphonRider = true;
							break;
						}

						it++;
					}

					if (!isGryphonRider) {

						it = units.begin();

						while (it != units.end()) {

							(*it)->SetPrisoner(dynamicEntity);

							it++;
						}

						App->entities->CommandToUnits(units, UnitCommand_RescuePrisoner);
					}
					else if (App->scene->adviceMessage != AdviceMessage_GRYPH_PRISONER) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GRYPH_PRISONER;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (App->scene->adviceMessage != AdviceMessage_PRISONER) {
					App->scene->adviceMessageTimer.Start();
					App->scene->adviceMessage = AdviceMessage_PRISONER;
					App->scene->ShowAdviceMessage(App->scene->adviceMessage);
				}
			}
		}
		// Turalyon (right click to send a unit to rescue him)
		else if (dynamicEntity->dynamicEntityType == EntityType_TURALYON) {

			Turalyon* turalyon = (Turalyon*)dynamicEntity;

			if (!turalyon->IsRescued()) {

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					bool isGryphonRider = false;

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						if ((*it)->dynamicEntityType == EntityType_GRYPHON_RIDER) {

							isGryphonRider = true;
							break;
						}

						it++;
					}

					if (!isGryphonRider) {

						it = units.begin();

						while (it != units.end()) {

							(*it)->SetPrisoner(dynamicEntity);

							it++;
						}

						App->entities->CommandToUnits(units, UnitCommand_RescuePrisoner);
					}
					else if (App->scene->adviceMessage != AdviceMessage_GRYPH_PRISONER) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GRYPH_PRISONER;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (App->scene->adviceMessage != AdviceMessage_PRISONER) {
					App->scene->adviceMessageTimer.Start();
					App->scene->adviceMessage = AdviceMessage_PRISONER;
					App->scene->ShowAdviceMessage(App->scene->adviceMessage);
				}
			}
		}
		break;

	case EntitiesEvent_LEFT_CLICK:
		if (App->fow->IsOnSight(GetMousePos())) {
			if (dynamicEntity->dynamicEntityType == EntityType_ALLERIA || dynamicEntity->dynamicEntityType == EntityType_TURALYON)
				MakePrisionerMenu(dynamicEntity);
		}
		
		break;
	case EntitiesEvent_HOVER:

		if (dynamicEntity->entitySide == EntitySide_Player ||
			dynamicEntity->dynamicEntityType == EntityType_ALLERIA || dynamicEntity->dynamicEntityType == EntityType_TURALYON) {

			if (!App->gui->IsMouseOnUI() && App->fow->IsOnSight(GetMousePos()))
				App->menu->mouseText->SetTexArea({ 503, 524, 30, 32 }, { 503, 524, 30, 32 });
		}
		break;
	case EntitiesEvent_LEAVE:
		if (dynamicEntity->entitySide == EntitySide_Player ||
			dynamicEntity->dynamicEntityType == EntityType_ALLERIA || dynamicEntity->dynamicEntityType == EntityType_TURALYON)

			if (!App->gui->IsMouseOnUI())
				App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });

		break;
	case EntitiesEvent_CREATED:
		break;
	default:
		break;
	}
}

void j1Player::RescuePrisoner(TerenasDialogEvents dialogEvent, SDL_Rect iconText, iPoint iconPos) 
{
	if (App->scene->terenasDialogEvent != dialogEvent) {
		App->scene->terenasDialogTimer.Start();
		App->scene->terenasDialogEvent = dialogEvent;
		App->scene->ShowTerenasDialog(dialogEvent);
	}

	UIImage_Info imageInfo;
	imageInfo.texArea = iconText;
	
	imagePrisonersVector.push_back(App->gui->CreateUIImage(iconPos,imageInfo));
}

void j1Player::CreateUISpawningUnits()
{
	for (int i = 0; i < 3; ++i)
	{
		UIImage* image = nullptr;
		UILifeBar* lifeBar = nullptr;

		image = CreateGroupIcon({ 48 * i + 72, 20 }, { 649,160,39,30 },false);
		lifeBar = CreateGroupLifeBar({ 48 * i + 72, 40 }, { 241, 362, 46, 7 }, { 243, 358, 42, 3 },false); //To spawn unit lifeBar timer
		
		barracksSpawningListUI.push_back({ nullptr, image, lifeBar });

		image = CreateGroupIcon({ 48 * i + 72, 20 }, { 649,160,39,30 },false);
		lifeBar = CreateGroupLifeBar({ 48 * i + 72, 40 }, { 241, 362, 46, 7 }, { 243, 358, 42, 3 },false); //To spawn unit lifeBar timer
		gryphoSpawningListUI.push_back({ nullptr, image, lifeBar });

	}
}

void j1Player::CreateEntitiesStatsUI()
{
	UIImage_Info imageInfo;
	imageInfo.texArea = { 241,34,50,41 };
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h / 2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entityIcon->isActive = false;

	UILifeBar_Info lifeInfo;
	lifeInfo.background = { 289,346,145,23 };
	lifeInfo.bar = { 300,373,128,8 };
	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 12, 10 };
	entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50 }, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.lifeBar->isActive = false;

	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entityName->isActive = false;

	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	labelInfo.text = "Building...";
	entitySelectedStats.HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.HP->isActive = false;

	labelInfo.fontName = FONT_NAME_WARCRAFT11;
	entitySelectedStats.entityDamage = App->gui->CreateUILabel({ 75, 33 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entityDamage->isActive = false;

	entitySelectedStats.entitySpeed = App->gui->CreateUILabel({ 145, 33 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entitySpeed->isActive = false;

	entitySelectedStats.entitySight = App->gui->CreateUILabel({ 75, 48 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entitySight->isActive = false;

	entitySelectedStats.entityRange = App->gui->CreateUILabel({ 145, 48 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	entitySelectedStats.entityRange->isActive = false;

	entitySelectedStats.entitySelected = nullptr;

	CreateBarracksButtons();
	CreateGryphonAviaryButtons();	
	CreateTownHallButtons();
	CreateDestructionButton();
	CreateRepairButton();
	CreateHoverInfoMenu(&firstHoverInfo);
	CreateHoverInfoMenu(&secondHoverInfo);
	CreateHoverInfoMenu(&thirdHoverInfo);
}

void j1Player::CreateGroupSelectionButtons()
{
	CreateSimpleSelectionButton({ 241,244,50,41 }, { 496, 244, 50, 41 }, { 751,244,50,41 }, { 14, 540 }, groupSelectionButtons.selectFootmans);
	CreateSimpleSelectionButton({ 292,244,50,41 }, { 547, 244, 50, 41 }, { 802,244,50,41 }, { 65, 540 }, groupSelectionButtons.selectElvenArchers);
	CreateSimpleSelectionButton({ 649,286,50,41 }, { 700, 286, 50, 41 }, { 751,286,50,41 }, { 116, 540 }, groupSelectionButtons.selectGryphonRiders);
}

void j1Player::ShowEntitySelectedInfo(string HP_text, string entityName_text, SDL_Rect iconDim, Entity* currentEntity) 
{
	//Hide Last Entity info
	HideEntitySelectedInfo();

	if (currentEntity->entityType == EntityCategory_STATIC_ENTITY) 
		App->entities->UnselectAllEntities();

	//Set Entity Name
	entitySelectedStats.entityName->SetText(entityName_text);
	entitySelectedStats.entityName->isActive = true;

	//Set Entity HP
	entitySelectedStats.HP->SetText(HP_text);
	if (HP_text == "Building...")//Change pos if its building
		entitySelectedStats.HP->SetLocalPos({ 80, App->scene->entitiesStats->GetLocalRect().h - 47 });
	else
		entitySelectedStats.HP->SetLocalPos({5, App->scene->entitiesStats->GetLocalRect().h - 17} );
	entitySelectedStats.HP->isActive = true;

	//Set Entity icon image
	entitySelectedStats.entityIcon->SetNewRect(iconDim);
	entitySelectedStats.entityIcon->isActive = true;

	//Set Entity LifeBar
	entitySelectedStats.lifeBar->SetMaxLife(currentEntity->GetMaxLife());

	if (currentEntity->entityType == EntityCategory_DYNAMIC_ENTITY) {
		//Construction lifeBar 0 to MaxLife
		entitySelectedStats.lifeBar->SetLife(((StaticEntity*)currentEntity)->GetConstructionTimer() * currentEntity->GetMaxLife() / 10);

		//Show the complete bar in certain circumstances
		if (currentEntity->GetCurrLife() > currentEntity->GetMaxLife() || ((StaticEntity*)currentEntity)->GetIsFinishedBuilt() ||
			((StaticEntity*)currentEntity)->staticEntityType == EntityType_TOWN_HALL || ((StaticEntity*)currentEntity)->staticEntityType == EntityType_BARRACKS)
			entitySelectedStats.lifeBar->SetLife(currentEntity->GetCurrLife());
	}
	else
		entitySelectedStats.lifeBar->SetLife(currentEntity->GetCurrLife());

	entitySelectedStats.lifeBar->isActive = true;

	StaticEntity* selectedEnt = (StaticEntity*)currentEntity;

	if (selectedEnt->GetIsFinishedBuilt()) {
		if (entityName_text == "Barracks")
			ShowEntitySelectedButt(EntityType_BARRACKS);

		else if (entityName_text == "Gryphon Aviary")
			ShowEntitySelectedButt(EntityType_GRYPHON_AVIARY);

		//if (entityName_text == "Mage Tower" && mageTower->buildingState == BuildingState_Normal) 
		//	ShowEntitySelectedButt(EntityType_MAGE_TOWER);

		else if (entityName_text == "Town Hall" || entityName_text == "Keep") {
			ShowEntitySelectedButt(EntityType_TOWN_HALL);
		}

		else if (entityName_text == "Chicken Farm")
			ShowEntitySelectedButt(EntityType_CHICKEN_FARM);

		else if (entityName_text == "Scout Tower")
			ShowEntitySelectedButt(EntityType_SCOUT_TOWER);

		else if (entityName_text == "Guard Tower")
			ShowEntitySelectedButt(EntityType_PLAYER_GUARD_TOWER);

		else if (entityName_text == "Cannon Tower")
			ShowEntitySelectedButt(EntityType_PLAYER_CANNON_TOWER);
	}
	entitySelectedStats.entitySelected = currentEntity;

}

void j1Player::ShowMineOrRuneStoneSelectedInfo(ENTITY_TYPE entType, SDL_Rect iconDim, string entName, Entity* currentEntity)
{
	if (currentEntity->entityType == EntityCategory_STATIC_ENTITY)
		App->entities->UnselectAllEntities();
	//Hide Last Entity info
	HideEntitySelectedInfo();

	//Set Entity Name
	entitySelectedStats.entityName->SetText(entName);
	entitySelectedStats.entityName->isActive = true;

	//Set Entity icon image
	entitySelectedStats.entityIcon->SetNewRect(iconDim);
	entitySelectedStats.entityIcon->isActive = true;

	switch (entType) {

	case EntityType_RUNESTONE:

		break;

	case EntityType_GOLD_MINE:
	{
		GoldMine* goldMine = (GoldMine*)currentEntity;
		switch (goldMine->GetGoldMineState()) {

		case GoldMineState_Untouched:
			entitySelectedStats.HP->SetText("Gold = ???");
			break;

		case GoldMineState_Gathering:
		{
			uint currentGold = 0;
			for (float i = goldMine->secondsGathering; i >= 0; i--) {
				if (goldMine->currentSec <= goldMine->secondsGathering - i + 1) {
					currentGold = goldMine->totalGold - ((goldMine->secondsGathering - i) * 100);
					break;
				}
			}
			string goldString = "Gold = " + to_string(currentGold);
			entitySelectedStats.HP->SetText(goldString);	
		}
		break;

		case GoldMineState_Gathered:
			entitySelectedStats.HP->SetText("Gold = 0");
			break;

		default:
			break;
		}

		//Set the gold text
		entitySelectedStats.HP->SetLocalPos({ 70, App->scene->entitiesStats->GetLocalRect().h - 45 });
		entitySelectedStats.HP->isActive = true;
		break;
	}
	default:
		break;
	}

	entitySelectedStats.entitySelected = currentEntity;
}

//Show buttons depends which entity is slected
void j1Player::ShowEntitySelectedButt(ENTITY_TYPE type)
{
	switch (type)
	{
	case EntityType_TOWN_HALL:
		//Have to cap this //TODO
		if(!townHallUpgrade)
		upgradeTownHallButton->isActive = true;

		repairBuildingButton->isActive = true;
		break;
	case EntityType_BARRACKS:
		produceElvenArcherButton->isActive = true;
		produceFootmanButton->isActive = true;
		repairBuildingButton->isActive = true;
		for (list<GroupSpawning>::iterator iterator = barracksSpawningListUI.begin(); iterator != barracksSpawningListUI.end(); ++iterator)
		{
			if ((*iterator).owner != nullptr)
				if (*(*iterator).owner != nullptr)
				{
					(*iterator).entityIcon->isActive = true;
					(*iterator).entityLifeBar->isActive = true;
				}
		}
		UpdateSpawnUnitsStats(&barracksSpawningListUI);
		break;
	//case EntityType_MAGE_TOWER:
	//	break;
	case EntityType_GRYPHON_AVIARY:
		produceGryphonRiderButton->isActive = true;
		destroyBuildingButton->isActive = true;
		repairBuildingButton->isActive = true;
		for (list<GroupSpawning>::iterator iterator = gryphoSpawningListUI.begin(); iterator != gryphoSpawningListUI.end(); ++iterator)
		{
			if ((*iterator).owner != nullptr)
				if (*(*iterator).owner != nullptr)
				{
					(*iterator).entityIcon->isActive = true;
					(*iterator).entityLifeBar->isActive = true;
				}
		}
		UpdateSpawnUnitsStats(&gryphoSpawningListUI);

		break;
	case EntityType_CHICKEN_FARM:
	case EntityType_SCOUT_TOWER:
	case EntityType_PLAYER_GUARD_TOWER:
	case EntityType_PLAYER_CANNON_TOWER:
		repairBuildingButton->isActive = true;
		destroyBuildingButton->isActive = true;
		break;
	default:
		break;
	}
}

void j1Player::ShowDynEntityLabelsInfo(string damage, string speed, string sight, string range)
{
	entitySelectedStats.entityDamage->SetText(damage);
	entitySelectedStats.entitySpeed->SetText(speed);
	entitySelectedStats.entitySight->SetText(sight);
	entitySelectedStats.entityRange->SetText(range);

	entitySelectedStats.entityDamage->isActive = true;
	entitySelectedStats.entitySpeed->isActive = true;
	entitySelectedStats.entitySight->isActive = true;
	entitySelectedStats.entityRange->isActive = true;
}

void j1Player::MakeUnitMenu(Entity* entity)
{
	if (entity == nullptr)
		return;

	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_FOOTMAN)
	{
		entity->SetStringLife(entity->GetCurrLife(), entity->GetMaxLife());
		ShowEntitySelectedInfo(entity->GetStringLife(), "Footman", { 240,244, 50, 41 }, entity);
		ShowDynEntityLabelsInfo("Damage: 6", "Speed: 10", "Sight: 4", "Range: 9");
	}
	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_ELVEN_ARCHER) 
	{
		entity->SetStringLife(entity->GetCurrLife(), entity->GetMaxLife());
		ShowEntitySelectedInfo(entity->GetStringLife(), "Elven Archer", { 291,244, 50, 41 }, entity);
		ShowDynEntityLabelsInfo("Damage: 5", "Speed: 10", "Sight: 9", "Range: 4");
	}	
	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_GRYPHON_RIDER)
	{
		entity->SetStringLife(entity->GetCurrLife(), entity->GetMaxLife());
		ShowEntitySelectedInfo(entity->GetStringLife(), "Gryphon Rider", { 700, 287, 50, 41 }, entity);
		ShowDynEntityLabelsInfo("Damage: 12", "Speed: 14", "Sight: 6", "Range: 4");
	}
}

void j1Player::MakePrisionerMenu(Entity * entity)
{
	if (entity == nullptr)
		return;

	App->entities->UnselectAllEntities();
	HideEntitySelectedInfo();

	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_ALLERIA)
	{
		//Set entity name
		entitySelectedStats.entityName->SetText("Alleria");
		entitySelectedStats.entityName->isActive = true;

		//Set Entity icon image
		SDL_Rect icon = { 848, 159, 52, 42 };
		entitySelectedStats.entityIcon->SetNewRect(icon);
		entitySelectedStats.entityIcon->isActive = true;

		//Prisioner description
		entitySelectedStats.HP->SetLocalPos({ 65, App->scene->entitiesStats->GetLocalRect().h - 60 });
		entitySelectedStats.HP->SetText("Head scout of the Alliance Expedition to Draenor.", 140);
		entitySelectedStats.HP->isActive = true;


	}
	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_TURALYON)
	{
		//Set entity name
		entitySelectedStats.entityName->SetText("Turalyon");
		entitySelectedStats.entityName->isActive = true;

		//Set Entity icon image
		SDL_Rect icon = { 744, 159, 52, 42 };
		entitySelectedStats.entityIcon->SetNewRect(icon);
		entitySelectedStats.entityIcon->isActive = true;

		//Prisioner description
		entitySelectedStats.HP->SetLocalPos({ 65, App->scene->entitiesStats->GetLocalRect().h - 64 });
		entitySelectedStats.HP->SetText("One of the first five paladins of the Knights of the Silver Hand.", 140);
		entitySelectedStats.HP->isActive = true;
	}

	entitySelectedStats.entitySelected = entity;
}

UIImage* j1Player::CreateGroupIcon(iPoint iconPos, SDL_Rect texArea, bool isActive)
{
	UIImage* ret = nullptr;
	UIImage_Info imageInfo;
	imageInfo.texArea = texArea;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	ret = App->gui->CreateUIImage(iconPos, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	ret->isActive = isActive;
	return ret;
}

void j1Player::HideEntitySelectedInfo()
{
	if (entitySelectedStats.entitySelected == barracks) {
		produceElvenArcherButton->isActive = false;
		produceFootmanButton->isActive = false;
		for (list<GroupSpawning>::iterator iterator = barracksSpawningListUI.begin(); iterator != barracksSpawningListUI.end(); ++iterator)
		{
			(*iterator).entityIcon->isActive = false;
			(*iterator).entityLifeBar->isActive = false;
		}
	}

	else if (entitySelectedStats.entitySelected == townHall)
		upgradeTownHallButton->isActive = false;


	//Hide Dynamic stats
	else if (App->scene->groupElementsList.front().owner != nullptr || (entitySelectedStats.entitySelected != nullptr && entitySelectedStats.entitySelected->entityType == EntityCategory_DYNAMIC_ENTITY))
		App->scene->HideUnselectedUnits();



	else if (entitySelectedStats.entitySelected == gryphonAviary) {
		produceGryphonRiderButton->isActive = false;

		for (list<GroupSpawning>::iterator iterator = gryphoSpawningListUI.begin(); iterator != gryphoSpawningListUI.end(); ++iterator)
		{
			(*iterator).entityIcon->isActive = false;
			(*iterator).entityLifeBar->isActive = false;
		}

	}
		/*
	else if (entitySelectedStats.entitySelected != nullptr && entitySelectedStats.entitySelected->entityType == EntityCategory_STATIC_ENTITY) {
		StaticEntity* ent = (StaticEntity*)entitySelectedStats.entitySelected;
		switch (ent->staticEntityType) {
		case EntityType_CHICKEN_FARM:
		case EntityType_SCOUT_TOWER:
		case EntityType_PLAYER_GUARD_TOWER:
		case EntityType_PLAYER_CANNON_TOWER:
			destroyBuildingButton->isActive = false;
			break;
		}
	}*/


	if (entitySelectedStats.entitySelected != nullptr) {
		entitySelectedStats.HP->isActive = false;
		entitySelectedStats.entityName->isActive = false;
		entitySelectedStats.entityIcon->isActive = false;
		entitySelectedStats.lifeBar->isActive = false;
		entitySelectedStats.entitySelected = nullptr;
		destroyBuildingButton->isActive = false;
		repairBuildingButton->isActive = false;
	}
}

void j1Player::DeleteEntitiesMenu()
{
	//Barracs Butt
	App->gui->RemoveElem((UIElement**)&produceElvenArcherButton);
	App->gui->RemoveElem((UIElement**)&produceFootmanButton);
	App->gui->RemoveElem((UIElement**)&producePaladinButton);
	App->gui->RemoveElem((UIElement**)&produceGryphonRiderButton);

	for (list<GroupSpawning>::iterator barrackIter = barracksSpawningListUI.begin(), gryphIter = gryphoSpawningListUI.begin(); barrackIter != barracksSpawningListUI.end(); ++barrackIter, ++gryphIter)
	{
		//Barracks Spawning
		App->gui->RemoveElem((UIElement**)&(*barrackIter).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*barrackIter).entityLifeBar);


		//Gryphon Spawning
		App->gui->RemoveElem((UIElement**)&(*gryphIter).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*gryphIter).entityLifeBar);
	}
	barracksSpawningListUI.clear();
	gryphoSpawningListUI.clear();

	//TownHall Butt
	App->gui->RemoveElem((UIElement**)&upgradeTownHallButton);

	//Destroy Building Butt
	App->gui->RemoveElem((UIElement**)&destroyBuildingButton);

	//Repair Butt
	App->gui->RemoveElem((UIElement**)&repairBuildingButton);

	//Entity Selected Info
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.HP);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityName);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityIcon);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.lifeBar);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityDamage);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entitySpeed);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityRange);
	App->gui->RemoveElem((UIElement**)&entitySelectedStats.entitySight);
	entitySelectedStats.entitySelected = nullptr;
}

void j1Player::DeleteGroupSelectionButtons()
{
	App->gui->RemoveElem((UIElement**)&groupSelectionButtons.selectFootmans);
	App->gui->RemoveElem((UIElement**)&groupSelectionButtons.selectElvenArchers);
	App->gui->RemoveElem((UIElement**)&groupSelectionButtons.selectGryphonRiders);
}

void j1Player::ShowHoverInfoMenu(string unitProduce, string gold, HoverInfo* hoverInfo, iPoint pos) {

	hoverInfo->background->isActive = true;
	if (hoverInfo->background->GetLocalPos() != pos) {
		hoverInfo->background->SetLocalPos(pos);
	}

	hoverInfo->info->SetText(unitProduce);
	hoverInfo->cost->SetText(gold);


	hoverInfo->info->isActive = true;
	hoverInfo->cost->isActive = true;
}

void j1Player::HideHoverInfoMenu(HoverInfo* hoverInfo)
{
	hoverInfo->background->isActive = false;
	hoverInfo->info->isActive = false;
	hoverInfo->cost->isActive = false;
}

void j1Player::DeleteHoverInfoMenu()
{
	App->gui->RemoveElem((UIElement**)&firstHoverInfo.background);
	App->gui->RemoveElem((UIElement**)&firstHoverInfo.cost);
	App->gui->RemoveElem((UIElement**)&firstHoverInfo.info);
	App->gui->RemoveElem((UIElement**)&secondHoverInfo.background);
	App->gui->RemoveElem((UIElement**)&secondHoverInfo.cost);
	App->gui->RemoveElem((UIElement**)&secondHoverInfo.info);
	App->gui->RemoveElem((UIElement**)&thirdHoverInfo.background);
	App->gui->RemoveElem((UIElement**)&thirdHoverInfo.cost);
	App->gui->RemoveElem((UIElement**)&thirdHoverInfo.info);

}

UILifeBar* j1Player::CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, bool isActive)
{
	UILifeBar* ret = nullptr;
	UILifeBar_Info barInfo;
	barInfo.background = backgroundTexArea;
	barInfo.bar = barTexArea;
	barInfo.maxLife = spawningTime;
	barInfo.life = (barInfo.maxLife);
	barInfo.maxWidth = barInfo.bar.w;
	barInfo.lifeBarPosition = { 2, 2 };
	ret = App->gui->CreateUILifeBar({ lifeBarPos.x, lifeBarPos.y }, barInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	ret->isActive = isActive;
	return ret;
}

void j1Player::CreateBarracksButtons()
{
	CreateSimpleButton({ 241,244,50,41 }, { 496, 244, 50, 41 }, { 751,244,50,41 }, { 217, 2 }, produceFootmanButton);
	CreateSimpleButton({ 292,244,50,41 }, { 547, 244, 50, 41 }, { 802,244,50,41 }, { 268, 2 }, produceElvenArcherButton);

	//if (barracksUpgrade && stables != nullptr && stables->buildingState == BuildingState_Normal)
	//	CreateSimpleButton({ 444,244,50,41 }, { 699, 244, 50, 41 }, { 954,244,50,41 }, { 319, 2 }, producePaladinButton);
}

void j1Player::CreateTownHallButtons()
{
	CreateSimpleButton({ 579,118,50,41 }, { 629, 118, 50, 41 }, { 679,118,50,41 }, { 217, 2 }, upgradeTownHallButton);
}
void j1Player::CreateDestructionButton()
{
	CreateSimpleButton({ 579,76,49,41 }, { 629, 76, 49, 41 }, { 679,76,49,41 }, { 320, 42 }, destroyBuildingButton);
}
void j1Player::CreateRepairButton()
{
	CreateSimpleButton({ 579,34,49,41 }, { 629, 34, 49, 41 }, { 679,34,49,41 }, { 320, 2 }, repairBuildingButton);
}
void j1Player::CreateHoverInfoMenu(HoverInfo* hoverInfo) {

	UIImage_Info backgroundImageInfo;
	backgroundImageInfo.texArea = { 344, 475, 167, 48 };
	hoverInfo->background = App->gui->CreateUIImage({ 6,487 }, backgroundImageInfo, nullptr);
	hoverInfo->background->isActive = false;

	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT11;
	hoverInfo->info = App->gui->CreateUILabel({ 5,8 }, labelInfo, nullptr, hoverInfo->background);
	hoverInfo->info->isActive = false;

	labelInfo.fontName = FONT_NAME_WARCRAFT11;
	hoverInfo->cost = App->gui->CreateUILabel({ 5, 25 }, labelInfo, nullptr, hoverInfo->background);
	hoverInfo->cost->isActive = false;
}
void j1Player::HandleSpawningUnitsUIElem(ToSpawnUnit** toSpawnUnit, list<GroupSpawning>* groupList)
{
	for (list<GroupSpawning>::iterator iterator = groupList->begin(); iterator != groupList->end(); ++iterator)
	{
		if (!(*iterator).entityIcon->isActive) {

			switch ((*toSpawnUnit)->entityType) {
				SDL_Rect icon;
			case EntityType_FOOTMAN:
				icon = { 649,160,39,30 };
				(*iterator).entityIcon->SetNewRect(icon);
				break;
			case EntityType_ELVEN_ARCHER:
				icon = { 696,160,39,30 };
				(*iterator).entityIcon->SetNewRect(icon);
				break;
			case EntityType_GRYPHON_RIDER:
				icon = { 702,290,39,30 };
				(*iterator).entityIcon->SetNewRect(icon);
				break;
			default:
				break;
			}

			(*iterator).entityLifeBar->SetLife((*toSpawnUnit)->toSpawnTimer.ReadSec());

			iterator->entityLifeBar->isActive = true;
			iterator->entityIcon->isActive = true;

			iterator->owner = toSpawnUnit;

			break;
		}
	}
}

void j1Player::HandleGoldMineUIStates()
{
	GoldMine* goldMine = (GoldMine*)entitySelectedStats.entitySelected;

	switch (goldMine->GetGoldMineState()) {

	case GoldMineState_Untouched:
		break;

	case GoldMineState_Gathering:
	{
		uint currentGold = 0;

		for (float i = goldMine->secondsGathering; i >= 0; i--) {
			if (goldMine->currentSec <= goldMine->secondsGathering - i + 1) {
				currentGold = goldMine->totalGold - ((goldMine->secondsGathering - i) * 100);
				break;
			}
		}
		string goldString = "Gold = " + to_string(currentGold);
		entitySelectedStats.HP->SetText(goldString);
	}
	break;

	case GoldMineState_Gathered:
		entitySelectedStats.HP->SetText("Gold = 0");
		break;

	default:
		break;
	}
}

void j1Player::CreateGryphonAviaryButtons()
{
	CreateSimpleButton({ 648,286,50,41 }, { 699, 286, 50, 41 }, { 750,286,50,41 }, { 217, 2 }, produceGryphonRiderButton);
}

void j1Player::CreateMageTowerButtons()
{
	CreateSimpleButton({ 342,244,50,41 }, { 597, 244, 50, 41 }, { 852,244,50,41 }, { 217, 2 }, produceMageButton);
}

//This button have ACTIVE false, you wont see it
void j1Player::CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button) {

	UIButton_Info infoButton;

	infoButton.normalTexArea = normal;
	infoButton.hoverTexArea = hover;
	infoButton.pressedTexArea = pressed;
	button = App->gui->CreateUIButton(pos, infoButton, this, (UIElement*)App->scene->entitiesStats);
	button->isActive = false;
}

void j1Player::CreateSimpleSelectionButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button) {

	UIButton_Info infoButton;

	infoButton.normalTexArea = normal;
	infoButton.hoverTexArea = hover;
	infoButton.pressedTexArea = pressed;
	button = App->gui->CreateUIButton(pos, infoButton, this);
	button->isActive = true;
}

uint j1Player::GetGroupSpawningSize(list<GroupSpawning> listSpawning)
{
	uint size = 0u;
	for (list<GroupSpawning>::iterator playerIterator = listSpawning.begin(); playerIterator != listSpawning.end(); ++playerIterator)
	{
		if (playerIterator->owner != nullptr) {
			size++;
		}
		else
			break;
	}
	return size;
}

uint j1Player::CalculateGoldRepair(StaticEntity* entity)
{
	uint gold = 0u;

	switch (entity->GetBuildingState())
	{
	case BuildingState_Normal:
		gold = 50 * entity->buildingSize;
		break;
	case BuildingState_LowFire:
		gold = 100 * entity->buildingSize;
		break;
	case BuildingState_HardFire:
		gold = 150 * entity->buildingSize;
		break;

	default:
		break;
	}

	return gold;
}

void j1Player::CheckBuildingsState() 
{
	if (townHall != nullptr) {
		if(townHall->GetCurrLife() <= 0 || townHall->isRemove)
			townHall = nullptr;
	}

	if (barracks != nullptr) {
		if (barracks->GetCurrLife() <= 0 || barracks->isRemove)
			barracks = nullptr;
	}

	if (gryphonAviary != nullptr) {
		if (gryphonAviary->GetCurrLife() <= 0 || gryphonAviary->isRemove)
			gryphonAviary = nullptr;
	}

	if (!chickenFarm.empty())
		for (list<StaticEntity*>::iterator iterator = chickenFarm.begin(); iterator != chickenFarm.end(); ++iterator)
		{
			if ((*iterator)->GetCurrLife() <= 0 || (*iterator)->isRemove) {
				if (chickenFarm.size() == 1) {
					chickenFarm.remove((*iterator));
					break;
				}
				chickenFarm.remove((*iterator));
				iterator = chickenFarm.begin();
			}
		}
	if (!scoutTower.empty())
		for (list<StaticEntity*>::iterator iterator = scoutTower.begin(); iterator != scoutTower.end(); ++iterator)
		{
			if ((*iterator)->GetCurrLife() <= 0 || (*iterator)->isRemove) {
				if (scoutTower.size() == 1) {
					scoutTower.remove((*iterator));
					break;
				}

				scoutTower.remove((*iterator));
				iterator = scoutTower.begin();
			}
		}
	if (!guardTower.empty())
		for (list<StaticEntity*>::iterator iterator = guardTower.begin(); iterator != guardTower.end(); ++iterator)
		{
			if (guardTower.size() == 1) {
				guardTower.remove((*iterator));
				break;
			}

			if ((*iterator)->GetCurrLife() <= 0 || (*iterator)->isRemove) {
				guardTower.remove((*iterator));
				iterator = guardTower.begin();
			}
		}
	if(!cannonTower.empty())
		for (list<StaticEntity*>::iterator iterator = cannonTower.begin(); iterator != cannonTower.end(); ++iterator)
		{
			if (cannonTower.size() == 1) {
				cannonTower.remove((*iterator));
				break;
			}

			if ((*iterator)->GetCurrLife() <= 0 || (*iterator)->isRemove) {
				cannonTower.remove((*iterator));
				iterator = cannonTower.begin();
			}
		}
}

void j1Player::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent)
{
	if (App->scene->GetPauseMenuActions() == PauseMenuActions_NOT_EXIST)
		switch (UIevent)
		{
		case UI_EVENT_NONE:
			break;
		case UI_EVENT_MOUSE_ENTER:

			if (UIelem == produceFootmanButton) {
				ShowHoverInfoMenu("Produces Footman", "Cost: 500 gold", &firstHoverInfo);
			}
			else if (UIelem == produceElvenArcherButton) {
				ShowHoverInfoMenu("Produces Elven Archer", "Cost: 400 gold", &secondHoverInfo);
			}
			else if (UIelem == produceGryphonRiderButton) {
				ShowHoverInfoMenu("Produces Gryphon Rider", "Cost: 750 gold", &thirdHoverInfo);
			}

			else if (UIelem == destroyBuildingButton) {
				ShowHoverInfoMenu("Destroy Building", "Cost: 0 gold", &firstHoverInfo);
			}
			else if (UIelem == repairBuildingButton) {
				uint gold = CalculateGoldRepair((StaticEntity*)entitySelectedStats.entitySelected);
				string cost = "Cost: " + to_string(gold) + " gold";
				ShowHoverInfoMenu("Repair Building", cost, &thirdHoverInfo);
			}
			else if (UIelem == upgradeTownHallButton) {
				ShowHoverInfoMenu("Upgrade TownHall to Keep", "Cost: 500 gold", &firstHoverInfo);
			}

			else if (UIelem == groupSelectionButtons.selectFootmans) {
				ShowHoverInfoMenu("Select all Footman on screen", "Shortcut [Z]", &firstHoverInfo, { 5,487 });
			}
			else if (UIelem == groupSelectionButtons.selectElvenArchers) {
				ShowHoverInfoMenu("Select all Archer on screen", "Shortcut [X]", &secondHoverInfo, { 5,487 });
			}
			else if (UIelem == groupSelectionButtons.selectGryphonRiders) {
				ShowHoverInfoMenu("Select all Gryphon on screen", "Shortcut [C]", &thirdHoverInfo, { 5,487 });
			}

			break;
		case UI_EVENT_MOUSE_LEAVE:
			if (UIelem == produceFootmanButton || UIelem == destroyBuildingButton || UIelem == groupSelectionButtons.selectFootmans || UIelem == upgradeTownHallButton)
				HideHoverInfoMenu(&firstHoverInfo);
			else if (UIelem == produceElvenArcherButton || UIelem == groupSelectionButtons.selectElvenArchers)
				HideHoverInfoMenu(&secondHoverInfo);
			else if (UIelem == produceGryphonRiderButton || UIelem == groupSelectionButtons.selectGryphonRiders || UIelem == repairBuildingButton)
				HideHoverInfoMenu(&thirdHoverInfo);
			break;
		case UI_EVENT_MOUSE_RIGHT_CLICK:

			// Order units to move to the minimap position
			if (UIelem == minimap) {

				iPoint goalPos = minimap->GetEntitiesGoal();
				iPoint goalTile = App->map->WorldToMap(goalPos.x, goalPos.y);

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0) {

					UnitGroup* group = App->movement->GetGroupByUnits(units);

					if (group == nullptr)

						// Selected units will now behave as a group
						group = App->movement->CreateGroupFromUnits(units);

					if (group != nullptr) {

						if (group->SetGoal(goalTile)) /// normal goal

							App->scene->isGoalFromMinimap = true;
					}
				}
			}
			break;

		case UI_EVENT_MOUSE_LEFT_CLICK:

			if (UIelem == upgradeTownHallButton) {
				
				//if (townHallUpgrade && currentGold >= 1500) {
				//	keepUpgrade = true;
				//	AddGold(-1500);
				//	App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Constructuion sound
				//	HideHoverInfoMenu(&firstHoverInfo);
				//	entitySelectedStats.entitySelected = townHall;
				//	ShowEntitySelectedInfo(entitySelectedStats.entitySelected->GetStringLife(), "Keep", { 597,202,50,41 }, entitySelectedStats.entitySelected);
				//}
				if (currentGold >= 500) {
					townHallUpgrade = true;
					AddGold(-500);
					App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
					HideHoverInfoMenu(&firstHoverInfo);
					upgradeTownHallButton->isActive = false;
					ShowEntitySelectedInfo("Building...", "Keep", { 597,202,50,41 }, entitySelectedStats.entitySelected);
				}
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			//For destroying a building
			else if (UIelem == destroyBuildingButton) {
				App->audio->PlayFx(App->audio->GetFX().destroyBuild);
				HideHoverInfoMenu(&firstHoverInfo);
				entitySelectedStats.entitySelected->isRemove = true;
				HideEntitySelectedInfo();
			}

			else if (UIelem == repairBuildingButton) {
				if (entitySelectedStats.entitySelected->GetCurrLife() < entitySelectedStats.entitySelected->GetMaxLife()) {
					StaticEntity* ent = (StaticEntity*)entitySelectedStats.entitySelected;
					uint gold = CalculateGoldRepair(ent);
					if (GetCurrentGold() >= gold) {
						int negative = gold;
						AddGold(-negative);
						entitySelectedStats.entitySelected->SetCurrLife(entitySelectedStats.entitySelected->GetMaxLife());
						ent->CheckBuildingState();
						App->audio->PlayFx(App->audio->GetFX().button);
						App->audio->PlayFx(App->audio->GetFX().repairBuild);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt);
				}
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt);
			}

			else if (UIelem == produceFootmanButton) {
				if (currentGold >= footmanCost && GetGroupSpawningSize(barracksSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(gryphoSpawningListUI) + GetGroupSpawningSize(barracksSpawningListUI))) {
						App->audio->PlayFx(1, 0); //Button sound
						AddGold(-footmanCost);
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_FOOTMAN);
						toSpawnUnitBarracks.push(toSpawnUnit);
						toSpawnUnitBarracks.back()->toSpawnTimer.Start();
						if (App->scene->adviceMessage == AdviceMessage_FOOD || App->scene->adviceMessage == AdviceMessage_GOLD) {
							App->scene->HideAdviceMessage();
						}
						HandleSpawningUnitsUIElem(&toSpawnUnitBarracks.back(), &barracksSpawningListUI);
					}
					else if (App->scene->adviceMessage != AdviceMessage_FOOD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_FOOD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (currentGold < footmanCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
			}
			else if (UIelem == produceElvenArcherButton) {
				if (currentGold >= elvenArcherCost && GetGroupSpawningSize(barracksSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(barracksSpawningListUI) + GetGroupSpawningSize(gryphoSpawningListUI))) {
						App->audio->PlayFx(1, 0); //Button sound
						AddGold(-elvenArcherCost);
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_ELVEN_ARCHER);
						toSpawnUnitBarracks.push(toSpawnUnit);
						toSpawnUnitBarracks.back()->toSpawnTimer.Start();
						if (App->scene->adviceMessage == AdviceMessage_FOOD || App->scene->adviceMessage == AdviceMessage_GOLD) {
							App->scene->HideAdviceMessage();
						}
						HandleSpawningUnitsUIElem(&toSpawnUnitBarracks.back(), &barracksSpawningListUI);
					}
					else if (App->scene->adviceMessage != AdviceMessage_FOOD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_FOOD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (currentGold < elvenArcherCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
			}
			/*if (UIelem == produceMageButton && mageTower != nullptr) {
				if (currentGold >= mageCost  && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					currentGold -= 1200;
					//Timer for the spawning
					j1Timer spawnTimer;
					ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_MAGE);
					toSpawnUnitQueue.push(toSpawnUnit);
					newUnitsToSpawn.push_back(toSpawnUnit);
					toSpawnUnitQueue.back()->toSpawnTimer.Start();
				}
				else if (currentGold < mageCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}*/
			/*if (UIelem == producePaladinButton) {
				if (currentGold >= paladinCost && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					currentGold -= 800;
					//Timer for the spawning
					j1Timer spawnTimer;
					ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_PALADIN);
					toSpawnUnitQueue.push(toSpawnUnit);
					newUnitsToSpawn.push_back(toSpawnUnit);
					toSpawnUnitQueue.back()->toSpawnTimer.Start();
				}
				else if (currentGold < paladinCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}*/
			else if (UIelem == produceGryphonRiderButton) {
				if (currentGold >= gryphonRiderCost && GetGroupSpawningSize(gryphoSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(gryphoSpawningListUI) + GetGroupSpawningSize(barracksSpawningListUI))) {
						App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
						AddGold(-gryphonRiderCost);
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_GRYPHON_RIDER);
						toSpawnUnitGrypho.push(toSpawnUnit);
						toSpawnUnitGrypho.back()->toSpawnTimer.Start();
						if (App->scene->adviceMessage == AdviceMessage_FOOD || App->scene->adviceMessage == AdviceMessage_GOLD) {
							App->scene->HideAdviceMessage();
						}
						HandleSpawningUnitsUIElem(&toSpawnUnitGrypho.back(), &gryphoSpawningListUI);
					}
					else if (App->scene->adviceMessage != AdviceMessage_FOOD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_FOOD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
				else if (currentGold < gryphonRiderCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
				}
			}
			else if (UIelem == groupSelectionButtons.selectFootmans) {

				App->entities->SelectEntitiesOnScreen(EntityType_FOOTMAN);

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0)
					App->scene->PlayUnitSound(units, true); // Unit selected sound
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
			}
			else if (UIelem == groupSelectionButtons.selectElvenArchers) {

				App->entities->SelectEntitiesOnScreen(EntityType_ELVEN_ARCHER);

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0)
					App->scene->PlayUnitSound(units, true); // Unit selected sound
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
			}
			else if (UIelem == groupSelectionButtons.selectGryphonRiders) {

				App->entities->SelectEntitiesOnScreen(EntityType_GRYPHON_RIDER);

				list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

				if (units.size() > 0)
					App->scene->PlayUnitSound(units, true); // Unit selected sound
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 1);
			}
			break;
		case UI_EVENT_MOUSE_RIGHT_UP:
			break;
		case UI_EVENT_MOUSE_LEFT_UP:
			break;
		case UI_EVENT_MAX_EVENTS:
			break;
		default:
			break;
		}
}

// GroupSpawning
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void GroupSpawning::IsActive(bool isActive)
{
	entityIcon->isActive = isActive;
	entityLifeBar->isActive = isActive;
}
