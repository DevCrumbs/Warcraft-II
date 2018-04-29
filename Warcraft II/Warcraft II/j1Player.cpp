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
	isWin = false;

	totalEnemiesKilled = 0;
	totalUnitsDead = 0;

	startGameTimer.Start();
	return ret;
}

bool j1Player::PreUpdate() {

	//Life Bar on building 
	if (entitySelectedStats.entitySelected != nullptr) {
		if (entitySelectedStats.entitySelected->entityType == EntityCategory_STATIC_ENTITY) {
			StaticEntity* building = (StaticEntity*)entitySelectedStats.entitySelected;

			if (building->staticEntityType != EntityType_GOLD_MINE && building->staticEntityType != EntityType_RUNESTONE) {
				if (!((StaticEntity*)entitySelectedStats.entitySelected)->GetIsFinishedBuilt()) {
					entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
				}
				else if (((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() == ((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTime()) {
					entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
					entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
					entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17 });
				}

			}
		}
	}

	return true;
}

bool j1Player::Update(float dt) 
{
	//Check if a building needs to be placed
	if(App->scene->GetAlphaBuilding() != EntityType_NONE)
		CheckIfPlaceBuilding();

	//Check if the units need to spawn
	if (!toSpawnUnitBarracks.empty())
		CheckUnitSpawning(&toSpawnUnitBarracks);

	if (!toSpawnUnitGrypho.empty())
		CheckUnitSpawning(&toSpawnUnitGrypho);

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
		if (gryphonAviary != nullptr)
			if (gryphonAviary->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)gryphonAviary;
				ent->ApplyDamage(20);
				if (!gryphonAviary->CheckBuildingState() && entitySelectedStats.entitySelected == ent) {
					DeleteEntitiesMenu();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	*/
	
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
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
	
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
		App->audio->PlayFx(App->audio->GetFX().goldMine, 0); //Gold mine sound
		AddGold(500);
		App->scene->hasGoldChanged = true;
	}
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		currentFood += 3;
		App->scene->hasFoodChanged = true;
	}


	//Handle the gold mine label
	//idk if put this here or in PreUpdate
	if (entitySelectedStats.entitySelected != nullptr) {
		StaticEntity* building = (StaticEntity*)entitySelectedStats.entitySelected;
		if (building->staticEntityType == EntityType_GOLD_MINE) {
			HandleGoldMineUIStates();
		}
	}
	
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
	if (sumGold > 0)
		totalGold += sumGold;
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
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) 
	{
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

		UnitInfo unitInfo;

		switch (alphaBuilding) 
		{
		case EntityType_CHICKEN_FARM:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				StaticEntity* c;
				c = (StaticEntity*)App->entities->AddEntity(EntityType_CHICKEN_FARM, buildingPos, App->entities->GetBuildingInfo(EntityType_CHICKEN_FARM), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				chickenFarm.push_back(c);
				AddGold(-App->scene->chickenFarmCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_STABLES:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				stables = (StaticEntity*)App->entities->AddEntity(EntityType_STABLES, buildingPos, App->entities->GetBuildingInfo(EntityType_STABLES), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->stablesCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_GRYPHON_AVIARY:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				gryphonAviary = (StaticEntity*)App->entities->AddEntity(EntityType_GRYPHON_AVIARY, buildingPos, App->entities->GetBuildingInfo(EntityType_GRYPHON_AVIARY), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->gryphonAviaryCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_MAGE_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				mageTower = (StaticEntity*)App->entities->AddEntity(EntityType_MAGE_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_MAGE_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->mageTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_SCOUT_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				StaticEntity* s;
				s = (StaticEntity*)App->entities->AddEntity(EntityType_SCOUT_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_SCOUT_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				scoutTower.push_back(s);
				AddGold(-App->scene->scoutTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;

		case EntityType_PLAYER_GUARD_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				guardTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_GUARD_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_GUARD_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->guardTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
		case EntityType_PLAYER_CANNON_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				cannonTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_CANNON_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_CANNON_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->cannonTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(App->audio->GetFX().errorButtBuilding, 0); //Placement building error button sound
			break;
		case EntityType_NONE:
			break;

		default:
			break;
		}
	}


	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		App->scene->SetAplphaBuilding(EntityType_NONE);

}

//This method checks for the spawning queue of the units and if they're ready to spawn or not
void j1Player::CheckUnitSpawning(queue<ToSpawnUnit*>* queue)
{
	UnitInfo unitInfo;

	list<GroupSpawning>::iterator lastElem;

	if (!queue->empty()) {

		if (queue->front()->toSpawnTimer.Read() > (spawningTime * 1000)) {
			ENTITY_TYPE toSpawnEntity = queue->front()->entityType;

			switch (toSpawnEntity) {

			case EntityType_FOOTMAN:
				SpawnUnit(barracks->GetPos(), EntityType_FOOTMAN, unitInfo);
				App->audio->PlayFx(App->audio->GetFX().footmanReady, 0);
				barracksSpawningListUI.front().entityIcon->isActive = false;
				barracksSpawningListUI.front().entityLifeBar->isActive = false;
				barracksSpawningListUI.front().owner = nullptr;
				break;

			case EntityType_ELVEN_ARCHER:
				SpawnUnit(barracks->GetPos(), EntityType_ELVEN_ARCHER, unitInfo);
				App->audio->PlayFx(App->audio->GetFX().archerReady, 0);
				barracksSpawningListUI.front().entityIcon->isActive = false;
				barracksSpawningListUI.front().entityLifeBar->isActive = false;
				barracksSpawningListUI.front().owner = nullptr;
				break;

			//case EntityType_MAGE:
			//	App->entities->AddEntity(EntityType_MAGE, { mageTowerPos.x + 30, mageTowerPos.y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_MAGE), unitInfo, this);
			//	break;

			//case EntityType_PALADIN:
			//	App->entities->AddEntity(EntityType_PALADIN, { barracksPos.x + 30, barracksPos.y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_PALADIN), unitInfo, this);
			//	break;

			case EntityType_GRYPHON_RIDER:
				App->entities->AddEntity(EntityType_GRYPHON_RIDER, { gryphonAviary->GetPos().x + 30, gryphonAviary->GetPos().y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_GRYPHON_RIDER), unitInfo, this);
				gryphoSpawningListUI.front().entityIcon->isActive = false;
				gryphoSpawningListUI.front().entityLifeBar->isActive = false;
				gryphoSpawningListUI.front().owner = nullptr;
				break;

			default:
				break;
			}
			
			delete queue->front();
			queue->pop();

			/*if (!toSpawnUnitStats.empty())
			{
				lastElem = toSpawnUnitStats.begin();

				App->gui->RemoveElem((UIElement**)&lastElem->entityIcon);
				App->gui->RemoveElem((UIElement**)&lastElem->entityLifeBar);

				toSpawnUnitStats.erase(toSpawnUnitStats.begin());

				UpdateSpawnUnitsStats();
			}*/
		}
	}
	//Update LifeBar
	for (list<GroupSpawning>::iterator barrackIter = barracksSpawningListUI.begin(), gryphIter = gryphoSpawningListUI.begin(); barrackIter != barracksSpawningListUI.end(); ++barrackIter, ++gryphIter)
	{
		if((*barrackIter).entityLifeBar->isActive)
			(*barrackIter).entityLifeBar->SetLife((*barrackIter).owner->toSpawnTimer.ReadSec());
		if ((*gryphIter).entityLifeBar->isActive)
			(*gryphIter).entityLifeBar->SetLife((*gryphIter).owner->toSpawnTimer.ReadSec());
	}
}

void j1Player::SpawnUnit(fPoint spawningBuildingPos, ENTITY_TYPE spawningEntity, UnitInfo unitInfo)
{
	iPoint buildingTile = App->map->WorldToMap(spawningBuildingPos.x, spawningBuildingPos.y);
	buildingTile.x -= 1;

	// Make sure that there are no entities on the spawn tile and that the tile is walkable
	if (App->entities->IsEntityOnTile(buildingTile, EntityCategory_DYNAMIC_ENTITY) != nullptr
		|| App->entities->IsEntityOnTile(buildingTile, EntityCategory_STATIC_ENTITY) != nullptr
		|| !App->pathfinding->IsWalkable(buildingTile))

		buildingTile = App->movement->FindClosestValidTile(buildingTile);

	// Make sure that the spawn tile is valid
	if (buildingTile.x != -1 && buildingTile.y != -1) {

		iPoint buildingTilePos = App->map->MapToWorld(buildingTile.x, buildingTile.y);
		fPoint pos = { (float)buildingTilePos.x,(float)buildingTilePos.y };

		App->entities->AddEntity(spawningEntity, pos, (EntityInfo&)App->entities->GetUnitInfo(spawningEntity), unitInfo, this);
		isUnitSpawning = false;
	}
}

void j1Player::UpdateSpawnUnitsStats()
{
	int cont = 0;
	/*for (list<GroupSpawning>::iterator iterator = toSpawnUnitStats.begin(); iterator != toSpawnUnitStats.end(); ++iterator)
	{
		(*iterator).entityIcon->SetLocalPos({ 48 * cont + 72, 5 });
		(*iterator).entityLifeBar->SetLocalPos({ 48 * cont + 72, 40 });
		cont++;
	}*/
}

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;

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

	for (; !UIMenuInfoList.empty(); UIMenuInfoList.pop_back())
	{
		UIMenuInfoList.back()->toRemove = true;
	}
	UIMenuInfoList.clear();

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

	for (; !imagePrisonersVector.empty(); imagePrisonersVector.pop_back())
	{
		imagePrisonersVector.back()->toRemove = true;
	}
	imagePrisonersVector.clear();

	DeleteEntitiesMenu();

	DeleteGroupSelectionButtons();

	if (hoverInfo.background != nullptr) {
		DeleteHoverInfoMenu();
	}

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
		delete toSpawnUnitBarracks.front();
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

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						(*it)->SetGoldMine((GoldMine*)staticEntity);

						it++;
					}

					App->entities->CommandToUnits(units, UnitCommand_GatherGold);
				}
				else if (App->scene->terenasDialogEvent != TerenasDialog_GOLD_MINE) {
					App->scene->terenasDialogTimer.Start();
					App->scene->terenasDialogEvent = TerenasDialog_GOLD_MINE;
					App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
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

			else if (staticEntity->staticEntityType == EntityType_BARRACKS && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Barracks", { 546,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && keepUpgrade && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Castle", { 546,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && townHallUpgrade && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Keep", { 597,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowEntitySelectedInfo(ent->GetStringLife(), "Town Hall", { 597,160,50,41 }, ent);
			}
			else if (staticEntity->staticEntityType == EntityType_GOLD_MINE) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowMineOrRuneStoneSelectedInfo(EntityType_GOLD_MINE,  { 848, 112, 50, 41 }, "Gold Mine", ent);
			}
			else if (staticEntity->staticEntityType == EntityType_RUNESTONE) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				ShowMineOrRuneStoneSelectedInfo(EntityType_RUNESTONE, { 848, 67, 50, 41 }, "RuneStone", ent);
			}
			break;

		case EntitiesEvent_HOVER:

			if (staticEntity->staticEntityType == EntityType_GOLD_MINE) {

				GoldMine* goldMine = (GoldMine*)staticEntity;

				if (goldMine->GetGoldMineState() == GoldMineState_Untouched) {

					App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 });
					App->player->isMouseOnMine = true;
				}
			}
			if (staticEntity->staticEntityType == EntityType_RUNESTONE) {

				Runestone* runestone = (Runestone*)staticEntity;

				if (runestone->GetRunestoneState() == RunestoneState_Untouched) {

					App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 });
					App->player->isMouseOnMine = true;
				}
			}
			if ((staticEntity->staticEntityType == EntityType_TOWN_HALL || staticEntity->staticEntityType == EntityType_BARRACKS) && ent->GetCurrLife() == ent->GetMaxLife())
				hoverCheck = HoverCheck_Upgrate;
			else if (ent->GetCurrLife() < ent->GetMaxLife())
				hoverCheck = HoverCheck_Repair;
			else
				hoverCheck = HoverCheck_None;

			/*hoverButtonStruct.isCreated = true;
			hoverButtonStruct.prevEntity = hoverButtonStruct.currentEntity;
			hoverButtonStruct.nextEntity = staticEntity;
			*/
			//DestroyHoverButton(ent);

			break;
		case EntitiesEvent_LEAVE:
			//DestroyHoverButton(ent);
			App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
			App->player->isMouseOnMine = false;
			break;
		case EntitiesEvent_CREATED:
			if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM)
				ShowEntitySelectedInfo("Building...", "Chicken Farm", { 241,34,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY)
				ShowEntitySelectedInfo("Building...", "Gryphon Aviary", { 394,160,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER)
				ShowEntitySelectedInfo("Building...", "Mage Tower", { 394,202,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER)
				ShowEntitySelectedInfo("Building...", "Scout Tower", { 394,34,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_PLAYER_GUARD_TOWER)
				ShowEntitySelectedInfo("Building...", "Guard Tower", { 394,76,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_PLAYER_CANNON_TOWER)
				ShowEntitySelectedInfo("Building...", "Cannon Tower", { 394,118,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_STABLES)
				ShowEntitySelectedInfo("Building...", "Stables", { 241,160,50,41 }, ent);
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
		if (dynamicEntity->dynamicEntityType == EntityType_ALLERIA || dynamicEntity->dynamicEntityType == EntityType_TURALYON) {

			list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

			if (units.size() > 0) {

				list<DynamicEntity*>::const_iterator it = units.begin();

				while (it != units.end()) {

					(*it)->SetPrisoner(dynamicEntity);

					it++;
				}

				App->entities->CommandToUnits(units, UnitCommand_RescuePrisoner);
			}
			/*
			else if (App->scene->terenasDialogEvent != TerenasDialog_GOLD_MINE) {

				App->scene->terenasDialogTimer.Start();
				App->scene->terenasDialogEvent = TerenasDialog_GOLD_MINE;
				App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);

			}
			*/
		}
		break;
	case EntitiesEvent_LEFT_CLICK:
		break;
	case EntitiesEvent_HOVER:
		break;
	case EntitiesEvent_LEAVE:
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
		
		barracksSpawningListUI.push_back({ nullptr,image,lifeBar });

		image = CreateGroupIcon({ 48 * i + 72, 20 }, { 649,160,39,30 },false);
		lifeBar = CreateGroupLifeBar({ 48 * i + 72, 40 }, { 241, 362, 46, 7 }, { 243, 358, 42, 3 },false); //To spawn unit lifeBar timer
		gryphoSpawningListUI.push_back({ nullptr,image,lifeBar });

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
	CreateHoverInfoMenu();
}

void j1Player::CreateGroupSelectionButtons()
{
	CreateSimpleSelectionButton({ 241,244,50,41 }, { 496, 244, 50, 41 }, { 751,244,50,41 }, { 14, 540 }, groupSelectionButtons.selectFootmans);
	CreateSimpleSelectionButton({ 292,244,50,41 }, { 547, 244, 50, 41 }, { 802,244,50,41 }, { 65, 540 }, groupSelectionButtons.selectElvenArchers);
	CreateSimpleSelectionButton({ 649,286,50,41 }, { 700, 286, 50, 41 }, { 751,286,50,41 }, { 116, 540 }, groupSelectionButtons.selectGryphonRiders);
}

void j1Player::ShowEntitySelectedInfo(string HP_text, string entityName_text, SDL_Rect iconDim, Entity* currentEntity) 
{
	if (currentEntity->entityType == EntityCategory_STATIC_ENTITY) 
		App->entities->UnselectAllEntities();
	//Hide Last Entity info
	HideEntitySelectedInfo();

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


	if (entityName_text == "Barracks") 
		ShowEntitySelectedButt(EntityType_BARRACKS);
	
	else if (entityName_text == "Gryphon Aviary" && gryphonAviary->buildingState == BuildingState_Normal) 
		ShowEntitySelectedButt(EntityType_GRYPHON_AVIARY);
	
	//if (entityName_text == "Mage Tower" && mageTower->buildingState == BuildingState_Normal) 
	//	ShowEntitySelectedButt(EntityType_MAGE_TOWER);
	
	else if ((entityName_text == "Town Hall" || entityName_text == "Keep") && townHall->buildingState == BuildingState_Normal) {
		ShowEntitySelectedButt(EntityType_TOWN_HALL);
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
		upgradeTownHallButton->isActive = true;
		break;
	case EntityType_BARRACKS:
		produceElvenArcherButton->isActive = true;
		produceFootmanButton->isActive = true;
		break;
	//case EntityType_MAGE_TOWER:
	//	break;
	case EntityType_GRYPHON_AVIARY:
		produceGryphonRiderButton->isActive = true;
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
			(*iterator).entityLifeBar = false;
			(*iterator).owner = nullptr;
		}
	}

	else if (entitySelectedStats.entitySelected == townHall)
		upgradeTownHallButton->isActive = false;

	else if (entitySelectedStats.entitySelected == gryphonAviary && gryphonAviary != nullptr)
		produceGryphonRiderButton->isActive = false;

	//Hide Dynamic stats
	else if (App->scene->groupElementsList.front().owner != nullptr || (entitySelectedStats.entitySelected != nullptr && entitySelectedStats.entitySelected->entityType == EntityCategory_DYNAMIC_ENTITY))
		App->scene->HideUnselectedUnits();

	if (entitySelectedStats.entitySelected != nullptr) {
		entitySelectedStats.HP->isActive = false;
		entitySelectedStats.entityName->isActive = false;
		entitySelectedStats.entityIcon->isActive = false;
		entitySelectedStats.lifeBar->isActive = false;
		entitySelectedStats.entitySelected = nullptr;
	}
}


void j1Player::DeleteEntitiesMenu()
{
	//Barracs Butt
	App->gui->RemoveElem((UIElement**)&produceElvenArcherButton);
	App->gui->RemoveElem((UIElement**)&produceFootmanButton);
	App->gui->RemoveElem((UIElement**)&producePaladinButton);
	for (list<GroupSpawning>::iterator barrackIter = barracksSpawningListUI.begin(), gryphIter = gryphoSpawningListUI.begin(); barrackIter != barracksSpawningListUI.end(); ++barrackIter, ++gryphIter)
	{
		//Barracks Spawning
		App->gui->RemoveElem((UIElement**)&(*barrackIter).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*barrackIter).entityLifeBar);

		delete (*barrackIter).owner;
		(*barrackIter).owner = nullptr;
		//Gryphon Spawning
		App->gui->RemoveElem((UIElement**)&(*gryphIter).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*gryphIter).entityLifeBar);

		delete (*gryphIter).owner;
		(*gryphIter).owner = nullptr;
	}
	barracksSpawningListUI.clear();
	gryphoSpawningListUI.clear();

	//TownHall Butt
	App->gui->RemoveElem((UIElement**)&upgradeTownHallButton);

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

void j1Player::ShowHoverInfoMenu(string unitProduce, string gold) {

	hoverInfo.background->isActive = true;

	hoverInfo.info->SetText(unitProduce);
	hoverInfo.cost->SetText(gold);

	hoverInfo.info->isActive = true;
	hoverInfo.cost->isActive = true;
}
void j1Player::HideHoverInfoMenu()
{
	hoverInfo.background->isActive = false;
	hoverInfo.info->isActive = false;
	hoverInfo.cost->isActive = false;
}
void j1Player::DeleteHoverInfoMenu()
{
	App->gui->RemoveElem((UIElement**)&hoverInfo.background);
	App->gui->RemoveElem((UIElement**)&hoverInfo.cost);
	App->gui->RemoveElem((UIElement**)&hoverInfo.info);
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

/*void j1Player::CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity) 
{
	UIButton_Info InfoButton;
	if (hoverCheck == HoverCheck_Repair) {
		InfoButton.normalTexArea  = { 579,34,49,41 };
		InfoButton.hoverTexArea   = { 629,34,49,41 };
		InfoButton.pressedTexArea = { 679,34,49,41 };
	}
	else if (hoverCheck == HoverCheck_Upgrate) {
		InfoButton.normalTexArea  = { 579,118,49,41 };
		InfoButton.hoverTexArea   = { 629,118,49,41 };
		InfoButton.pressedTexArea = { 679,118,49,41 };
	}
	InfoButton.horizontalOrientation = HORIZONTAL_POS_CENTER;
	InfoButton.verticalOrientation = VERTICAL_POS_CENTER;



	if (hoverCheck != HoverCheck_None) {
		hoverButtonStruct.hoverButton = App->gui->CreateUIButton({ pos.x + pos.w / 2, pos.y + pos.h / 2 }, InfoButton, this, nullptr, true);
		hoverButtonStruct.currentEntity = staticEntity;
		hoverButtonStruct.hoverButton->SetPriorityDraw(PriorityDraw_BUTTONSINGAME);
	}
}
*/
/*void j1Player::DestroyHoverButton(Entity* ent) {
	if (hoverButtonStruct.currentEntity == ent || hoverButtonStruct.prevEntity == ent) {
		App->gui->DestroyElement((UIElement**)&hoverButtonStruct.hoverButton);
		hoverButtonStruct.currentEntity = nullptr;
	}
}
*/

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
void j1Player::CreateHoverInfoMenu() {

	UIImage_Info backgroundImageInfo;
	backgroundImageInfo.texArea = { 241, 384, 85, 38 };
	hoverInfo.background = App->gui->CreateUIImage({ 643, 481 }, backgroundImageInfo, nullptr);
	hoverInfo.background->isActive = false;

	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, labelInfo, nullptr, hoverInfo.background);
	hoverInfo.info->isActive = false;

	labelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, labelInfo, nullptr, hoverInfo.background);
	hoverInfo.cost->isActive = false;
}
void j1Player::HandleSpawningUnitsUIElem(ToSpawnUnit* toSpawnUnit, list<GroupSpawning> groupList)
{
	//Delete UI elements when not used
	for (list<GroupSpawning>::iterator iterator = groupList.begin(); iterator != groupList.end(); ++iterator)
	{
		if (!(*iterator).entityIcon->isActive) {

			switch ((toSpawnUnit)->entityType) {
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
				icon = { 649,160,39,30 };
				(*iterator).entityIcon->SetNewRect(icon);
				break;
			default:
				break;
			}

			(*iterator).entityLifeBar->SetLife(toSpawnUnit->toSpawnTimer.ReadSec());

			(*iterator).entityLifeBar->isActive = true;
			(*iterator).entityIcon->isActive = true;
			(*iterator).owner = toSpawnUnit;

			isUnitSpawning = true;
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

void j1Player::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) 
{
	if (App->scene->GetPauseMenuActions() == PauseMenuActions_NOT_EXIST)
		switch (UIevent)
		{
		case UI_EVENT_NONE:
			break;
		case UI_EVENT_MOUSE_ENTER:

			if (UIelem == produceFootmanButton) {
				ShowHoverInfoMenu("Produces footman", "Cost: 500 gold");
			}
			if (UIelem == produceElvenArcherButton) {
				ShowHoverInfoMenu("Produces archer", "Cost: 400 gold");
			}
			if (UIelem == produceMageButton && mageTower != nullptr) {
				ShowHoverInfoMenu("Produces mage", "Cost: 1200 gold");
			}
			if (UIelem == producePaladinButton) {
				ShowHoverInfoMenu("Produces paladin", "Cost: 800 gold");
			}
			if (UIelem == produceGryphonRiderButton) {
				ShowHoverInfoMenu("Produces gryphon", "Cost: 2500 gold");
			}
			break;
		case UI_EVENT_MOUSE_LEAVE:
			if (UIelem == produceFootmanButton || UIelem == produceElvenArcherButton || UIelem == producePaladinButton || UIelem == produceMageButton || UIelem == produceGryphonRiderButton) {
				HideHoverInfoMenu();
			}
			break;
		case UI_EVENT_MOUSE_RIGHT_CLICK:
			break;
		case UI_EVENT_MOUSE_LEFT_CLICK:

			if (UIelem == upgradeTownHallButton) {
				if (townHallUpgrade && currentGold >= 1500) {
					keepUpgrade = true;
					currentGold -= 1500;
					App->scene->hasGoldChanged = true;
					App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Constructuion sound
				}
				else if(currentGold >= 500) {
					townHallUpgrade = true;
					currentGold -= 500;
					App->scene->hasGoldChanged = true;
					App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
				}
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			/*if (hoverCheck == HoverCheck_Repair) {
				if (currentGold < 500) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
				else {
					App->audio->PlayFx(App->audio->GetFX().repairBuild, 0); //Repair building sound
					hoverButtonStruct.currentEntity->SetCurrLife(hoverButtonStruct.currentEntity->GetMaxLife());
					hoverButtonStruct.currentEntity->CheckBuildingState();
					entitySelectedStats.HP->SetText(hoverButtonStruct.currentEntity->GetStringLife());
					entitySelectedStats.lifeBar->SetLife(hoverButtonStruct.currentEntity->GetMaxLife());
					currentGold -= 500;
					App->scene->hasGoldChanged = true;
				}
			}
			else if (hoverCheck == HoverCheck_Upgrate)
			{
				if (hoverButtonStruct.currentEntity == barracks) {
					if (currentGold >= 1000) {
						barracksUpgrade = true;
						currentGold -= 1000;
						App->scene->hasGoldChanged = true;
						DestroyHoverButton(barracks);
						App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
					}
					else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
				if (hoverButtonStruct.currentEntity == townHall && townHallUpgrade) {
					if (currentGold >= 500) {
						keepUpgrade = true;
						currentGold -= 500;
						App->scene->hasGoldChanged = true;
						DestroyHoverButton(townHall);
						App->audio->PlayFx(App->audio->GetFX().buildingConstruction, 0); //Construction sound
					}
					else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
				if (hoverButtonStruct.currentEntity == townHall) {
					if (currentGold >= 1500) {
						townHallUpgrade = true;
						currentGold -= 1500;
						App->scene->hasGoldChanged = true;
						DestroyHoverButton(townHall);
					}
					else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}
			*/
			if (UIelem == produceFootmanButton) {
				if (currentGold >= footmanCost && GetGroupSpawningSize(barracksSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(gryphoSpawningListUI) + GetGroupSpawningSize(barracksSpawningListUI))) {
						App->audio->PlayFx(1, 0); //Button sound
						currentGold -= 500;
						App->scene->hasGoldChanged = true;
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_FOOTMAN);
						toSpawnUnitBarracks.push(toSpawnUnit);
						//newUnitsToSpawn.push_back(toSpawnUnit);
						toSpawnUnitBarracks.back()->toSpawnTimer.Start();
						if (App->scene->terenasDialogEvent == TerenasDialog_FOOD || App->scene->terenasDialogEvent == TerenasDialog_GOLD) {
							App->scene->HideTerenasDialog();
						}
						HandleSpawningUnitsUIElem(toSpawnUnit, barracksSpawningListUI);
					}
					else if (App->scene->terenasDialogEvent != TerenasDialog_FOOD){
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_FOOD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
						}
				}
				else if (currentGold < footmanCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->terenasDialogEvent != TerenasDialog_GOLD) {
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_GOLD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
			}
			if (UIelem == produceElvenArcherButton) {
				if (currentGold >= elvenArcherCost && GetGroupSpawningSize(barracksSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(barracksSpawningListUI) + GetGroupSpawningSize(gryphoSpawningListUI))) {
						App->audio->PlayFx(1, 0); //Button sound
						currentGold -= 400;
						App->scene->hasGoldChanged = true;
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_ELVEN_ARCHER);
						toSpawnUnitBarracks.push(toSpawnUnit);
						//newUnitsToSpawn.push_back(toSpawnUnit);
						toSpawnUnitBarracks.back()->toSpawnTimer.Start();
						if (App->scene->terenasDialogEvent == TerenasDialog_FOOD || App->scene->terenasDialogEvent == TerenasDialog_GOLD) {
							App->scene->HideTerenasDialog();
						}
						HandleSpawningUnitsUIElem(toSpawnUnit, barracksSpawningListUI);
					}
					else if (App->scene->terenasDialogEvent != TerenasDialog_FOOD) {
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_FOOD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
				else if (currentGold < elvenArcherCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->terenasDialogEvent != TerenasDialog_GOLD) {
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_GOLD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
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
			if (UIelem == produceGryphonRiderButton) {
				if (currentGold >= gryphonRiderCost && GetGroupSpawningSize(gryphoSpawningListUI) <= maxSpawnQueueSize) {

					if (currentFood > (App->entities->GetNumberOfPlayerUnits() + GetGroupSpawningSize(gryphoSpawningListUI) + GetGroupSpawningSize(barracksSpawningListUI))) {
						App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
						currentGold -= 2500;
						App->scene->hasGoldChanged = true;
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_GRYPHON_RIDER);
						toSpawnUnitGrypho.push(toSpawnUnit);
						//	newUnitsToSpawn.push_back(toSpawnUnit);
						toSpawnUnitGrypho.back()->toSpawnTimer.Start();
						if (App->scene->terenasDialogEvent == TerenasDialog_FOOD || App->scene->terenasDialogEvent == TerenasDialog_GOLD)
							App->scene->HideTerenasDialog();
						HandleSpawningUnitsUIElem(toSpawnUnit, gryphoSpawningListUI);
					}
					else if (App->scene->terenasDialogEvent != TerenasDialog_FOOD) {
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_FOOD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
				else if (currentGold < gryphonRiderCost) {
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
					if (App->scene->terenasDialogEvent != TerenasDialog_GOLD) {
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_GOLD;
						App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
			}
			if (UIelem == groupSelectionButtons.selectFootmans) {
				//TODO SANDRA (seleccion de ese tipo de tropas en camara)
				// Recuerda la condición "si no hay ninguna unidad de ese tipo en camara -> button error sound"
			}
			if (UIelem == groupSelectionButtons.selectElvenArchers) {
				//TODO SANDRA (seleccion de ese tipo de tropas en camara)
				// Recuerda la condición "si no hay ninguna unidad de ese tipo en camara -> button error sound"
			}
			if (UIelem == groupSelectionButtons.selectGryphonRiders) {
				//TODO SANDRA (seleccion de ese tipo de tropas en camara)
				// Recuerda la condición "si no hay ninguna unidad de ese tipo en camara -> button error sound"
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