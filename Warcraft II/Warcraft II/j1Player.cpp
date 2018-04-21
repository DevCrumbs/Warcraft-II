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

bool j1Player::Update(float dt) 
{
	for (list<GroupSelectedElements>::iterator iterator = groupElementsList.begin(); iterator != groupElementsList.end(); ++iterator) {
		(*iterator).entityLifeBar->SetLife((*iterator).owner->GetCurrLife());
	}
	//Check if a building needs to be placed
	if(App->scene->GetAlphaBuilding() != EntityType_NONE)
		CheckIfPlaceBuilding();

	//Check if the units need to spawn
	if (!toSpawnUnitQueue.empty())
		CheckUnitSpawning();

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
						DeleteEntitiesMenu();
					chickenFarm.pop_back();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		
	if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
		App->audio->PlayFx(6, 0); //Gold mine sound
		AddGold(500);
		App->scene->hasGoldChanged = true;
	}

	//Life Bar on building 
	if (entitySelectedStats.entitySelected != nullptr) {
		if (entitySelectedStats.entitySelected->entityType == EntityCategory_STATIC_ENTITY) {
			if (!((StaticEntity*)entitySelectedStats.entitySelected)->GetIsFinishedBuilt()) {
				entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
			}
			else if (((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() == ((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTime()) {
				entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
				entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
				entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17 });
				if (entitySelectedStats.entitySelected == barracks) {
					if (barracksUpgrade && stables != nullptr && producePaladinButton == nullptr) {
						UIButton_Info produceButtonInfo;
						produceButtonInfo.normalTexArea = { 444,244,50,41 };
						produceButtonInfo.hoverTexArea = { 699,244,50,41 };
						produceButtonInfo.pressedTexArea = { 954,244,50,41 };
						producePaladinButton = App->gui->CreateUIButton({ 319, 2 }, produceButtonInfo, this, (UIElement*)App->scene->entitiesStats);
					}
				}
			}
		}
	}
	//Handle the apparence and disapparence of to spawn units UI elements
	
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

				// Set unwalkable tiles (SMALL)
				/*
				App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
				App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);
				*/
				// ----
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(4, 0); //Placement building error button sound
			break;

		case EntityType_STABLES:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				stables = (StaticEntity*)App->entities->AddEntity(EntityType_STABLES, buildingPos, App->entities->GetBuildingInfo(EntityType_STABLES), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->stablesCost); //Discount gold
				App->scene->hasGoldChanged = true;

				// Set unwalkable tiles (MEDIUM)
				/*
				App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y) + (buildingTile.x + 2)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 2) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 2) + (buildingTile.x + 2)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 2)] = 0u;
				App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);
				*/
				// -----
			}
			else if(App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(4, 0); //Placement building error button sound
			break;

		case EntityType_GRYPHON_AVIARY:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				gryphonAviary = (StaticEntity*)App->entities->AddEntity(EntityType_GRYPHON_AVIARY, buildingPos, App->entities->GetBuildingInfo(EntityType_GRYPHON_AVIARY), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->gryphonAviaryCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(4, 0); //Placement building error button sound
			break;

		case EntityType_MAGE_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				mageTower = (StaticEntity*)App->entities->AddEntity(EntityType_MAGE_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_MAGE_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->mageTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium))
				App->audio->PlayFx(4, 0); //Placement building error button sound
			break;

		case EntityType_SCOUT_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				StaticEntity* s;
				s = (StaticEntity*)App->entities->AddEntity(EntityType_SCOUT_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_SCOUT_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				scoutTower.push_back(s);
				AddGold(-App->scene->scoutTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;

				// Set unwalkable tiles (SMALL)
				/*
				App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
				App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);
				*/
				// ----
			}
			else if (App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small))
				App->audio->PlayFx(4, 0); //Placement building error button sound
			break;

		case EntityType_PLAYER_GUARD_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				guardTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_GUARD_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_GUARD_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->guardTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;

				// Set unwalkable tiles (SMALL)
				/*
				App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
				App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);
				*/
				// ----
			}
			break;
		case EntityType_PLAYER_CANNON_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				cannonTower.push_back((StaticEntity*)App->entities->AddEntity(EntityType_PLAYER_CANNON_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_PLAYER_CANNON_TOWER), unitInfo, this));
				App->scene->SetAplphaBuilding(EntityType_NONE);
				AddGold(-App->scene->cannonTowerCost); //Discount gold
				App->scene->hasGoldChanged = true;

				// Set unwalkable tiles (SMALL)
				/*
				App->scene->data[App->scene->w * buildingTile.y + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * buildingTile.y + (buildingTile.x + 1)] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + buildingTile.x] = 0u;
				App->scene->data[App->scene->w * (buildingTile.y + 1) + (buildingTile.x + 1)] = 0u;
				App->pathfinding->SetMap(App->scene->w, App->scene->h, App->scene->data);
				*/
				// ----
			}
		case EntityType_NONE:
			break;

		default:
			break;
		}
	}


	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		App->scene->SetAplphaBuilding(EntityType_NONE);

	//This makes that buildings on the scene aren't printed on alpha

	if (alphaBuilding != EntityType_MAX)
	{
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
		SDL_SetTextureAlphaMod(App->entities->GetNeutralBuildingTexture(), 255);
	}
}

//This method checks for the spawning queue of the units and if they're ready to spawn or not
void j1Player::CheckUnitSpawning()
{
	fPoint barracksPos = { -1,-1 };

	if (barracks != nullptr)
		barracksPos = barracks->GetPos();
	else
		return;

	UnitInfo unitInfo;
	fPoint mageTowerPos = { 0.0f,0.0f };
	fPoint gryphonAviaryPos = { 0.0f,0.0f };

	if (mageTower != nullptr) {
		mageTowerPos = mageTower->GetPos();
	}
	if (gryphonAviary != nullptr) {
		gryphonAviaryPos = gryphonAviary->GetPos();
	}

	list<GroupSpawning>::iterator lastElem;

	if (!toSpawnUnitQueue.empty()) {

		if (toSpawnUnitQueue.front()->toSpawnTimer.Read() > (spawningTime * 1000)) {
			ENTITY_TYPE toSpawnEntity = toSpawnUnitQueue.front()->entityType;

			switch (toSpawnEntity) {

			case EntityType_FOOTMAN:
			{
				iPoint barracksTile = App->map->WorldToMap(barracksPos.x, barracksPos.y);
				barracksTile.x -= 1;

				// Make sure that there are no entities on the spawn tile and that the tile is walkable
				if (App->entities->IsEntityOnTile(barracksTile, EntityCategory_DYNAMIC_ENTITY) != nullptr 
					|| App->entities->IsEntityOnTile(barracksTile, EntityCategory_STATIC_ENTITY) != nullptr 
					|| !App->pathfinding->IsWalkable(barracksTile))

					barracksTile = App->movement->FindClosestValidTile(barracksTile);

				// Make sure that the spawn tile is valid
				if (barracksTile.x != -1 && barracksTile.y != -1) {

					iPoint barracksTilePos = App->map->MapToWorld(barracksTile.x, barracksTile.y);
					fPoint pos = { (float)barracksTilePos.x,(float)barracksTilePos.y };

					App->entities->AddEntity(EntityType_FOOTMAN, pos, (EntityInfo&)App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, this);
					isUnitSpawning = false;
					App->audio->PlayFx(21, 0);
				}
			}
				
			break;

			case EntityType_ELVEN_ARCHER:
			{
				iPoint barracksTile = App->map->WorldToMap(barracksPos.x, barracksPos.y);
				barracksTile.x -= 1;

				// Make sure that there are no entities on the spawn tile and that the tile is walkable
				if (App->entities->IsEntityOnTile(barracksTile, EntityCategory_DYNAMIC_ENTITY) != nullptr
					|| App->entities->IsEntityOnTile(barracksTile, EntityCategory_STATIC_ENTITY) != nullptr
					|| !App->pathfinding->IsWalkable(barracksTile))

					barracksTile = App->movement->FindClosestValidTile(barracksTile);

				// Make sure that the spawn tile is valid
				if (barracksTile.x != -1 && barracksTile.y != -1) {

					iPoint barracksTilePos = App->map->MapToWorld(barracksTile.x, barracksTile.y);
					fPoint pos = { (float)barracksTilePos.x,(float)barracksTilePos.y };

					App->entities->AddEntity(EntityType_ELVEN_ARCHER, pos, (EntityInfo&)App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER), unitInfo, this);
					isUnitSpawning = false;
					App->audio->PlayFx(18, 0);
				}
			}
				break;

			case EntityType_MAGE:
				App->entities->AddEntity(EntityType_MAGE, { mageTowerPos.x + 30, mageTowerPos.y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_MAGE), unitInfo, this);
				break;

			case EntityType_PALADIN:
				App->entities->AddEntity(EntityType_PALADIN, { barracksPos.x + 30, barracksPos.y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_PALADIN), unitInfo, this);
				break;

			case EntityType_GRYPHON_RIDER:
				App->entities->AddEntity(EntityType_GRYPHON_RIDER, { gryphonAviaryPos.x + 30, gryphonAviaryPos.y - 50 }, (EntityInfo&)App->entities->GetUnitInfo(EntityType_GRYPHON_RIDER), unitInfo, this);
				break;

			default:
				break;
			}
			
			delete toSpawnUnitQueue.front();
			toSpawnUnitQueue.pop();

			if (!toSpawnUnitStats.empty())
			{
				lastElem = toSpawnUnitStats.begin();
				LOG("Size before erase: %i", toSpawnUnitStats.size());

				App->gui->RemoveElem((UIElement**)&lastElem->entityIcon);
				App->gui->RemoveElem((UIElement**)&lastElem->entityLifeBar);

				toSpawnUnitStats.erase(toSpawnUnitStats.begin());
				LOG("Size after erase: %i", toSpawnUnitStats.size());

				UpdateSpawnUnitsStats();
			}
		}
	}
	for (list<GroupSpawning>::iterator iterator = toSpawnUnitStats.begin(); iterator != toSpawnUnitStats.end(); ++iterator) 
	{
		(*iterator).entityLifeBar->SetLife((*iterator).owner->toSpawnTimer.ReadSec());
	}
}

void j1Player::UpdateSpawnUnitsStats()
{
	int cont = 0;
	for (list<GroupSpawning>::iterator iterator = toSpawnUnitStats.begin(); iterator != toSpawnUnitStats.end(); ++iterator)
	{
		(*iterator).entityIcon->SetLocalPos({ 48 * cont + 72, 5 });
		(*iterator).entityLifeBar->SetLocalPos({ 48 * cont + 72, 40 });
		cont++;
	}
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
	if(App->scene->GetPauseMenuActions() == PauseMenuActions_NOT_EXIST)
		switch (entitiesEvent)
		{
		case EntitiesEvent_NONE:
			break;
		case EntitiesEvent_RIGHT_CLICK:
			break;
		case EntitiesEvent_LEFT_CLICK:

			DeleteEntitiesMenu();

			if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM) {
				App->audio->PlayFx(5, 0); //Chicken farm sound
				MakeEntitiesMenu(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY) {
				App->audio->PlayFx(7, 0); //Gryphon aviary sound
				MakeEntitiesMenu(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER) {
				App->audio->PlayFx(8, 0); //Mage tower sound
				MakeEntitiesMenu(ent->GetStringLife(), "Mage Tower", { 394,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER) {
				App->audio->PlayFx(1, 0); //Button sound
				MakeEntitiesMenu(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_PLAYER_GUARD_TOWER) {
				App->audio->PlayFx(5, 0); //Chicken farm sound
				MakeEntitiesMenu(ent->GetStringLife(), "Guard Tower", { 394,76,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_PLAYER_CANNON_TOWER) {
				App->audio->PlayFx(5, 0); //Chicken farm sound
				MakeEntitiesMenu(ent->GetStringLife(), "Cannon Tower", { 394,118,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_STABLES) {
				App->audio->PlayFx(9, 0); //Stables sound
				MakeEntitiesMenu(ent->GetStringLife(), "Stables", { 241,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_BARRACKS && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(1, 0); //Button sound
				MakeEntitiesMenu(ent->GetStringLife(), "Barracks", { 546,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && keepUpgrade && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(1, 0); //Button sound
				MakeEntitiesMenu(ent->GetStringLife(), "Castle", { 546,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && townHallUpgrade && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(1, 0); //Button sound
				MakeEntitiesMenu(ent->GetStringLife(), "Keep", { 597,202,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && staticEntity->buildingState == BuildingState_Normal) {
				App->audio->PlayFx(1, 0); //Button sound
				MakeEntitiesMenu(ent->GetStringLife(), "Town Hall", { 597,160,50,41 }, ent);
			}

			else if (staticEntity->staticEntityType == EntityType_GOLD_MINE && staticEntity->buildingState == BuildingState_Normal) {

				iPoint pos = App->map->WorldToMap((int)staticEntity->GetPos().x, (int)staticEntity->GetPos().y);
				if (App->entities->IsNearSoldiers(pos, 7)) {

					App->audio->PlayFx(6, 0); //Gold mine sound
					/*list<DynamicEntity*> pene = App->entities->GetLastUnitsSelected();
					if (pene.size() > 0) {
						pene.front()->SetBlitState(false);
					}*/
					int random = rand() % 4;
					switch (random) {
					case 0:
						App->player->AddGold(550);
						break;
					case 1:
						App->player->AddGold(600);
						break;
					case 2:
						App->player->AddGold(650);
						break;
					case 3:
						App->player->AddGold(700);
						break;
					}

					App->scene->hasGoldChanged = true;
					App->scene->UnLoadTerenasDialog();
					staticEntity->buildingState = BuildingState_Destroyed;
				}
				else if(App->scene->terenasDialogEvent != TerenasDialog_GOLD_MINE){
					App->scene->UnLoadTerenasDialog();
					App->scene->terenasDialogTimer.Start();
					App->scene->terenasDialogEvent = TerenasDialog_GOLD_MINE;
					App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
				}
			}
			else if (staticEntity->staticEntityType == EntityType_RUNESTONE && staticEntity->buildingState == BuildingState_Normal) {

				iPoint pos = App->map->WorldToMap((int)staticEntity->GetPos().x, (int)staticEntity->GetPos().y);
				if (App->entities->IsNearSoldiers(pos, 7)) {
					list<DynamicEntity*>::const_iterator it = App->entities->activeDynamicEntities.begin();
					while (it != App->entities->activeDynamicEntities.end()) {
						if ((*it)->entitySide == EntitySide_Player) 
							(*it)->ApplyHealth((*it)->GetMaxLife() / 2);
						it++;
					}
					App->scene->UnLoadTerenasDialog();
					staticEntity->buildingState = BuildingState_Destroyed;
				}
				else if (App->scene->terenasDialogEvent != TerenasDialog_RUNESTONE) {
					App->scene->UnLoadTerenasDialog();
					App->scene->terenasDialogTimer.Start();
					App->scene->terenasDialogEvent = TerenasDialog_RUNESTONE;
					App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
				}
			}
				
				break;
		case EntitiesEvent_HOVER:
			if (staticEntity->staticEntityType == EntityType_GOLD_MINE) {
				App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 });
				App->player->isMouseOnMine = true;
			}
			if (staticEntity->staticEntityType == EntityType_RUNESTONE) {
				App->menu->mouseText->SetTexArea({ 310, 525, 28, 33 }, { 338, 525, 28, 33 });
				App->player->isMouseOnMine = true;
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
			DeleteEntitiesMenu();
			if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM)
				MakeEntitiesMenu("NO_HP_TEXT", "Chicken Farm", { 241,34,50,41 }, ent);
		
			else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY)
				MakeEntitiesMenu("NO_HP_TEXT", "Gryphon Aviary", { 394,160,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER)
				MakeEntitiesMenu("NO_HP_TEXT", "Mage Tower", { 394,202,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER)
				MakeEntitiesMenu("NO_HP_TEXT", "Scout Tower", { 394,34,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_PLAYER_GUARD_TOWER)
				MakeEntitiesMenu("NO_HP_TEXT", "Guard Tower", { 394,76,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_PLAYER_CANNON_TOWER)
				MakeEntitiesMenu("NO_HP_TEXT", "Cannon Tower", { 394,118,50,41 }, ent);

			else if (staticEntity->staticEntityType == EntityType_STABLES)
				MakeEntitiesMenu("NO_HP_TEXT", "Stables", { 241,160,50,41 },ent);
			break;
		
		default:
			break;
		}

}

void j1Player::OnDynamicEntitiesEvent(DynamicEntity* dynamicEntity, EntitiesEvent entitiesEvent) {

	switch (entitiesEvent)
	{
	case EntitiesEvent_NONE:
		break;
	case EntitiesEvent_RIGHT_CLICK:
		break;
	case EntitiesEvent_LEFT_CLICK:
		if (dynamicEntity->dynamicEntityType == EntityType_ALLERIA) {
			iPoint pos = App->map->WorldToMap((int)dynamicEntity->GetPos().x, (int)dynamicEntity->GetPos().y);
			if (App->entities->IsNearSoldiers(pos, 5)) {
				dynamicEntity->isRemove = true;
				RescuePrisoner(TerenasDialog_RESCUE_ALLERIA, { 848,159,52,42 }, { 8, 245 });
				App->audio->PlayFx(14, 0);
			}
		}
		else if (dynamicEntity->dynamicEntityType == EntityType_KHADGAR) {
			iPoint pos = App->map->WorldToMap((int)dynamicEntity->GetPos().x, (int)dynamicEntity->GetPos().y);
			if (App->entities->IsNearSoldiers(pos, 5)) {
				dynamicEntity->isRemove = true;
				RescuePrisoner(TerenasDialog_RESCUE_KHADGAR, { 796,159,52,42 }, { 8, 200 });
				App->audio->PlayFx(14, 0);
			}
		}
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
void j1Player::RescuePrisoner(TerenasDialogEvents dialogEvent, SDL_Rect iconText, iPoint iconPos) {

	if (App->scene->terenasDialogEvent != dialogEvent) {
		App->scene->UnLoadTerenasDialog();
		App->scene->terenasDialogTimer.Start();
		App->scene->terenasDialogEvent = dialogEvent;
		App->scene->LoadTerenasDialog(dialogEvent);
	}

	UIImage_Info imageInfo;
	imageInfo.texArea = iconText;
	
	imagePrisonersVector.push_back(App->gui->CreateUIImage(iconPos,imageInfo));
}


void j1Player::MakeEntitiesMenu(string HP_text, string entityName_text, SDL_Rect iconDim, Entity* currentEntity) 
{
	App->entities->UnselectAllEntities();

	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.text = entityName_text;
	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT14;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	labelInfo.text = HP_text;
	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT14;
	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	if (HP_text != "NO_HP_TEXT")
		entitySelectedStats.HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	
	else if (HP_text == "NO_HP_TEXT") {
		labelInfo.text = "Building...";
		entitySelectedStats.HP = App->gui->CreateUILabel({ 80, App->scene->entitiesStats->GetLocalRect().h - 30 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	}

	UIImage_Info imageInfo;
	imageInfo.texArea = iconDim;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h/2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	UILifeBar_Info lifeInfo;
	lifeInfo.background = { 289,346,145,23 };
	lifeInfo.bar = { 300,373,128,8 };
	lifeInfo.maxLife = currentEntity->GetMaxLife();
	lifeInfo.life = ((StaticEntity*)currentEntity)->GetConstructionTimer() * currentEntity->GetMaxLife() / 10;

	//Show the complete bar in certain circumstances
	if (lifeInfo.life > currentEntity->GetMaxLife() || ((StaticEntity*)currentEntity)->GetIsFinishedBuilt())
		lifeInfo.life = currentEntity->GetCurrLife();
	if(((StaticEntity*)currentEntity)->staticEntityType == EntityType_TOWN_HALL || ((StaticEntity*)currentEntity)->staticEntityType == EntityType_BARRACKS)
		lifeInfo.life = currentEntity->GetCurrLife();

	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 12, 10 };

	entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50}, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	if (entityName_text == "Barracks") {
		CreateBarracksButtons();
	}
	if (entityName_text == "Gryphon Aviary" && gryphonAviary->buildingState == BuildingState_Normal) {
		CreateGryphonAviaryButtons();
	}
	if (entityName_text == "Mage Tower" && mageTower->buildingState == BuildingState_Normal) {
		CreateMageTowerButtons();
	}

	entitySelectedStats.entitySelected = currentEntity;
}

void j1Player::MakeUnitMenu(Entity* entity)
{
	if (entity == nullptr)
		return;

	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_FOOTMAN) 
	{
		UIImage_Info imageInfo;
		imageInfo.texArea = { 240,244, 50, 41 };
		imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
		imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
		entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h / 2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILifeBar_Info lifeInfo;
		lifeInfo.background = { 289,346,145,23 };
		lifeInfo.bar = { 300,373,128,8 };
		lifeInfo.maxLife = entity->GetMaxLife();
		lifeInfo.life = entity->GetCurrLife();
		lifeInfo.maxWidth = lifeInfo.bar.w;
		lifeInfo.lifeBarPosition = { 12, 10 };
		entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50 }, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILabel_Info labelInfo;
		labelInfo.interactive = false;
		labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT14;
		labelInfo.text = "Footman";
		labelInfo.verticalOrientation = VERTICAL_POS_TOP;
		entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		entity->SetStringLife(entity->GetCurrLife(), entity->GetMaxLife());
		labelInfo.text = entity->GetStringLife();
		labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
		entitySelectedStats.HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT9;
		labelInfo.text = "Damage: 6";
		entitySelectedStats.entityDamage = App->gui->CreateUILabel({ 75, 25 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Mana: -";
		entitySelectedStats.entityMana = App->gui->CreateUILabel({ 145, 25 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Sight: 4";
		entitySelectedStats.entitySight = App->gui->CreateUILabel({ 75, 37 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Range: 9";
		entitySelectedStats.entityRange = App->gui->CreateUILabel({ 145, 37 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Movement Speed: 10";
		entitySelectedStats.entityMovementSpeed = App->gui->CreateUILabel({ 75, 50 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	}

	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_ELVEN_ARCHER) 
	{
		UIImage_Info imageInfo;
		imageInfo.texArea = { 291,244, 50, 41 };
		imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
		imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
		entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h / 2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILifeBar_Info lifeInfo;
		lifeInfo.background = { 289,346,145,23 };
		lifeInfo.bar = { 300,373,128,8 };
		lifeInfo.maxLife = entity->GetMaxLife();
		lifeInfo.life = entity->GetCurrLife();
		lifeInfo.maxWidth = lifeInfo.bar.w;
		lifeInfo.lifeBarPosition = { 12, 10 };
		entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50 }, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILabel_Info labelInfo;
		labelInfo.interactive = false;
		labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT14;
		labelInfo.text = "Elven Archer";
		labelInfo.verticalOrientation = VERTICAL_POS_TOP;
		entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		entity->SetStringLife(entity->GetCurrLife(), entity->GetMaxLife());
		labelInfo.text = entity->GetStringLife();
		labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
		entitySelectedStats.HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT9;
		labelInfo.text = "Damage: 5";
		entitySelectedStats.entityDamage = App->gui->CreateUILabel({ 75, 25 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Mana: -";
		entitySelectedStats.entityMana = App->gui->CreateUILabel({ 145, 25 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Sight: 9";
		entitySelectedStats.entitySight = App->gui->CreateUILabel({ 75, 37 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Range: 4";
		entitySelectedStats.entityRange = App->gui->CreateUILabel({ 145, 37 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "Movement Speed: 10";
		entitySelectedStats.entityMovementSpeed = App->gui->CreateUILabel({ 75, 50 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);
	}	

	entitySelectedStats.entitySelected = entity;	
}

void j1Player::MakeUnitsMenu(list<DynamicEntity*> units)
{
	list<DynamicEntity*>::iterator it;
	it = units.begin();
	int cont = 0;
	while (it != units.end()) {
		UIImage* image = nullptr;
		UILifeBar* lifeBar = nullptr;
		if (units.size() == 1) {
			MakeUnitMenu((*it));
		}
		else {
				
			if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
				image = CreateGroupIcon({ 55 * (cont % 4) + 2, 39 * (cont / 4) + 18 }, { 649, 160, 46, 30 });
			}
			else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
				image = CreateGroupIcon({ 55 * (cont % 4) + 2, 39 * (cont / 4) + 18 }, { 696, 160, 46, 30 });
			}
			lifeBar = CreateGroupLifeBar({ 55 * (cont % 4) + 1, 39 * (cont / 4) + 33 }, { 240,362,47,7 }, { 242,358,42,3 }, (Entity*)(*it));
			
			groupElementsList.push_back({(*it), image, lifeBar});
		}		
		it++;
		cont++;
	}

	CreateAbilitiesButtons();
}

void j1Player::DeleteEntitiesMenu()
{
	if (entitySelectedStats.entitySelected == barracks) {
		App->gui->RemoveElem((UIElement**)&produceElvenArcherButton);
		App->gui->RemoveElem((UIElement**)&produceFootmanButton);
		App->gui->RemoveElem((UIElement**)&producePaladinButton);
		for (list<GroupSpawning>::iterator iterator = toSpawnUnitStats.begin(); iterator != toSpawnUnitStats.end(); ++iterator) {
			App->gui->RemoveElem((UIElement**)&(*iterator).entityIcon);
			App->gui->RemoveElem((UIElement**)&(*iterator).entityLifeBar);
		}
		toSpawnUnitStats.clear();
	}

	if (entitySelectedStats.entitySelected != nullptr) {
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.HP);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityName);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityIcon);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.lifeBar);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityDamage);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityMana);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityMovementSpeed);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entityRange);
		App->gui->RemoveElem((UIElement**)&entitySelectedStats.entitySight);
		App->gui->RemoveElem((UIElement**)&commandPatrolButton);
		App->gui->RemoveElem((UIElement**)&commandStopButton);
		entitySelectedStats.entitySelected = nullptr;
	}

	for (list<GroupSelectedElements>::iterator iterator = groupElementsList.begin(); iterator != groupElementsList.end(); ++iterator) {
		App->gui->RemoveElem((UIElement**)&(*iterator).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*iterator).entityLifeBar);
	}

	if (!groupElementsList.empty()) {
		if (commandPatrolButton != nullptr)
			App->gui->RemoveElem((UIElement**)&commandPatrolButton);
		if (commandStopButton != nullptr)
			App->gui->RemoveElem((UIElement**)&commandStopButton);
	}
	groupElementsList.clear();
}

void j1Player::MakeHoverInfoMenu(string unitProduce, string gold) {

	UIImage_Info backgroundImageInfo;
	backgroundImageInfo.texArea = { 241, 384, 85, 38 };
	hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.text = unitProduce;
	labelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, labelInfo, nullptr, hoverInfo.background);

	labelInfo.text = gold;
	labelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, labelInfo, nullptr, hoverInfo.background);

}
void j1Player::DeleteHoverInfoMenu()
{
	App->gui->RemoveElem((UIElement**)&hoverInfo.background);
	App->gui->RemoveElem((UIElement**)&hoverInfo.cost);
	App->gui->RemoveElem((UIElement**)&hoverInfo.info);
}

UIImage* j1Player::CreateGroupIcon(iPoint iconPos, SDL_Rect texArea)
{
	UIImage_Info imageInfo;
	imageInfo.texArea = texArea;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	return App->gui->CreateUIImage(iconPos, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);
}
UILifeBar* j1Player::CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, Entity * entity)
{
	UILifeBar_Info lifeInfo;
	lifeInfo.background = backgroundTexArea;
	lifeInfo.bar = barTexArea;
	lifeInfo.maxLife = entity->GetMaxLife();
	lifeInfo.life = entity->GetCurrLife();
	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 2, 2 };
	return App->gui->CreateUILifeBar(lifeBarPos, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);	
}

UILifeBar* j1Player::CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea)
{
	UILifeBar_Info barInfo;
	//barInfo.background = { 241,362,46,7 };
	//barInfo.bar =			{ 243,358,42,3 };
	barInfo.background = backgroundTexArea;
	barInfo.bar = barTexArea;
	barInfo.maxLife = spawningTime;
	barInfo.life = (barInfo.maxLife);
	barInfo.maxWidth = barInfo.bar.w;
	barInfo.lifeBarPosition = { 2, 2 };
	return App->gui->CreateUILifeBar({ lifeBarPos.x, lifeBarPos.y }, barInfo, nullptr, (UIElement*)App->scene->entitiesStats);
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
	if (barracksUpgrade && stables != nullptr && stables->buildingState == BuildingState_Normal)
		CreateSimpleButton({ 444,244,50,41 }, { 699, 244, 50, 41 }, { 954,244,50,41 }, { 319, 2 }, producePaladinButton);
}

void j1Player::HandleBarracksUIElem()
{
	//Delete UI elements when not used
	bool isVul = true;
	int cont = toSpawnUnitStats.size();
	LOG("Original cont = %i", cont);
	if (cont <= 3)
	{
		for (list<ToSpawnUnit*>::iterator newUnit = newUnitsToSpawn.begin(); newUnit != newUnitsToSpawn.end(); ++newUnit)
		{ //Iterates every element in the queue
			if (!(cont <= 3))
				break;

			UIImage_Info info;
			UIImage* image = nullptr;
			UILifeBar* lifeBar = nullptr;

			switch ((*newUnit)->entityType) {
			case EntityType_FOOTMAN:
				image = CreateGroupIcon({ 48 * cont + 72, 20 }, { 649,160,39,30 });
				break;
			case EntityType_ELVEN_ARCHER:
				image = CreateGroupIcon({ 48 * cont + 72, 20 }, { 696,160,39,30 });
				break;
			default:
				break;
			}
			lifeBar = CreateGroupLifeBar({ 48 * cont + 72, 40 }, { 241, 362, 46, 7 }, { 243, 358, 42, 3 }); //To spawn unit lifeBar timer
			lifeBar->SetLife((*newUnit)->toSpawnTimer.ReadSec());


			isUnitSpawning = true;

			toSpawnUnitStats.push_back({ (*newUnit), image, lifeBar });

			LOG("Cont = %i", cont);


			cont++;
		}
	}
	else {
		LOG("WTF is going on");
	}
	newUnitsToSpawn.clear();
}

void j1Player::CreateGryphonAviaryButtons()
{
	CreateSimpleButton({ 648,286,50,41 }, { 699, 286, 50, 41 }, { 750,286,50,41 }, { 217, 2 }, produceGryphonRiderButton);
}

void j1Player::CreateMageTowerButtons()
{
	CreateSimpleButton({ 342,244,50,41 }, { 597, 244, 50, 41 }, { 852,244,50,41 }, { 217, 2 }, produceMageButton);
}

void j1Player::CreateAbilitiesButtons()
{
	CreateSimpleButton({ 802,202,50,41 }, { 904, 202, 50, 41 }, { 853,202,50,41 }, { 217, 2 }, commandStopButton);
	CreateSimpleButton({ 649,202,50,41 }, { 751, 202, 50, 41 }, { 700,202,50,41 }, { 268, 2 }, commandPatrolButton);
}

void j1Player::CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button) {

	UIButton_Info infoButton;

	infoButton.normalTexArea = normal;
	infoButton.hoverTexArea = hover;
	infoButton.pressedTexArea = pressed;
	button = App->gui->CreateUIButton(pos, infoButton, this, (UIElement*)App->scene->entitiesStats);

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
				DeleteHoverInfoMenu();
				MakeHoverInfoMenu("Produces footman", "Cost: 500 gold");
			}
			if (UIelem == produceElvenArcherButton) {
				DeleteHoverInfoMenu();
				MakeHoverInfoMenu("Produces archer", "Cost: 400 gold");
			}
			if (UIelem == produceMageButton && mageTower != nullptr) {
				DeleteHoverInfoMenu();
				MakeHoverInfoMenu("Produces mage", "Cost: 1200 gold");
			}
			if (UIelem == producePaladinButton) {
				DeleteHoverInfoMenu();
				MakeHoverInfoMenu("Produces paladin", "Cost: 800 gold");
			}
			if (UIelem == produceGryphonRiderButton) {
				DeleteHoverInfoMenu();
				MakeHoverInfoMenu("Produces gryphon", "Cost: 2500 gold");
			}
			break;
		case UI_EVENT_MOUSE_LEAVE:
			if (UIelem == produceFootmanButton || UIelem == produceElvenArcherButton || UIelem == producePaladinButton || UIelem == produceMageButton || UIelem == produceGryphonRiderButton) {
				DeleteHoverInfoMenu();
			}
			break;
		case UI_EVENT_MOUSE_RIGHT_CLICK:
			break;
		case UI_EVENT_MOUSE_LEFT_CLICK:

			if (UIelem == commandPatrolButton) {
				// Command Patrol (SANDRA)
				if (App->scene->units.size() > 0)
					App->entities->CommandToUnits(App->scene->units, UnitCommand_Patrol);
			}
			if (UIelem == commandStopButton) {
				// Command Stop (SANDRA)
				if (App->scene->units.size() > 0)
					App->entities->CommandToUnits(App->scene->units, UnitCommand_Stop);
			}

			/*if (hoverCheck == HoverCheck_Repair) {
				if (currentGold < 500) {
					App->audio->PlayFx(3, 0); //Button error sound
				}
				else {
					App->audio->PlayFx(10, 0); //Repair building sound
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
						App->audio->PlayFx(2, 0); //Construction sound
					}
					else
						App->audio->PlayFx(3, 0); //Button error sound
				}
				if (hoverButtonStruct.currentEntity == townHall && townHallUpgrade) {
					if (currentGold >= 500) {
						keepUpgrade = true;
						currentGold -= 500;
						App->scene->hasGoldChanged = true;
						DestroyHoverButton(townHall);
						App->audio->PlayFx(2, 0); //Construction sound
					}
					else
						App->audio->PlayFx(3, 0); //Button error sound
				}
				if (hoverButtonStruct.currentEntity == townHall) {
					if (currentGold >= 1500) {
						townHallUpgrade = true;
						currentGold -= 1500;
						App->scene->hasGoldChanged = true;
						DestroyHoverButton(townHall);
					}
					else
						App->audio->PlayFx(3, 0); //Button error sound
				}
			}
			*/
			if (UIelem == produceFootmanButton) {
				if (currentGold >= footmanCost && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					if (currentFood > (App->entities->GetPlayerSoldiers() + toSpawnUnitQueue.size())) {
						App->audio->PlayFx(1, 0); //Button sound
						currentGold -= 500;
						App->scene->hasGoldChanged = true;
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_FOOTMAN);
						toSpawnUnitQueue.push(toSpawnUnit);
						newUnitsToSpawn.push_back(toSpawnUnit);
						toSpawnUnitQueue.back()->toSpawnTimer.Start();
						if (App->scene->terenasDialogEvent == TerenasDialog_FOOD || App->scene->terenasDialogEvent == TerenasDialog_GOLD) {
							App->scene->UnLoadTerenasDialog();
						}
						HandleBarracksUIElem();
					}
					else if (App->scene->terenasDialogEvent != TerenasDialog_FOOD){
						App->scene->UnLoadTerenasDialog();
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_FOOD;
						App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
						}
				}
				else if (currentGold < footmanCost) {
					App->audio->PlayFx(3, 0); //Button error sound
					if (App->scene->terenasDialogEvent != TerenasDialog_GOLD) {
						App->scene->UnLoadTerenasDialog();
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_GOLD;
						App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
			}
			if (UIelem == produceElvenArcherButton) {
				if (currentGold >= elvenArcherCost && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					if (currentFood > (App->entities->GetPlayerSoldiers() + toSpawnUnitQueue.size())) {
						App->audio->PlayFx(1, 0); //Button sound
						currentGold -= 400;
						App->scene->hasGoldChanged = true;
						//Timer for the spawning
						j1Timer spawnTimer;
						ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_ELVEN_ARCHER);
						toSpawnUnitQueue.push(toSpawnUnit);
						newUnitsToSpawn.push_back(toSpawnUnit);
						toSpawnUnitQueue.back()->toSpawnTimer.Start();
						if (App->scene->terenasDialogEvent == TerenasDialog_FOOD || App->scene->terenasDialogEvent == TerenasDialog_GOLD) {
							App->scene->UnLoadTerenasDialog();
						}
						HandleBarracksUIElem();
					}
					else if (App->scene->terenasDialogEvent != TerenasDialog_FOOD) {
						App->scene->UnLoadTerenasDialog();
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_FOOD;
						App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
				else if (currentGold < elvenArcherCost) {
					App->audio->PlayFx(3, 0); //Button error sound
					if (App->scene->terenasDialogEvent != TerenasDialog_GOLD) {
						App->scene->UnLoadTerenasDialog();
						App->scene->terenasDialogTimer.Start();
						App->scene->terenasDialogEvent = TerenasDialog_GOLD;
						App->scene->LoadTerenasDialog(App->scene->terenasDialogEvent);
					}
				}
			}
			if (UIelem == produceMageButton && mageTower != nullptr) {
				if (currentGold >= mageCost  && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					App->audio->PlayFx(1, 0); //Button sound
					currentGold -= 1200;
					//Timer for the spawning
					j1Timer spawnTimer;
					ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_MAGE);
					toSpawnUnitQueue.push(toSpawnUnit);
					newUnitsToSpawn.push_back(toSpawnUnit);
					toSpawnUnitQueue.back()->toSpawnTimer.Start();
				}
				else if (currentGold < mageCost)
					App->audio->PlayFx(3, 0); //Button error sound
			}
			if (UIelem == producePaladinButton) {
				if (currentGold >= paladinCost && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					App->audio->PlayFx(1, 0); //Button sound
					currentGold -= 800;
					//Timer for the spawning
					j1Timer spawnTimer;
					ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_PALADIN);
					toSpawnUnitQueue.push(toSpawnUnit);
					newUnitsToSpawn.push_back(toSpawnUnit);
					toSpawnUnitQueue.back()->toSpawnTimer.Start();
				}
				else if (currentGold < paladinCost)
					App->audio->PlayFx(3, 0); //Button error sound
			}
			if (UIelem == produceGryphonRiderButton) {
				if (currentGold >= gryphonRiderCost && toSpawnUnitQueue.size() <= maxSpawnQueueSize) {
					App->audio->PlayFx(1, 0); //Button sound
					currentGold -= 2500;
					//Timer for the spawning
					j1Timer spawnTimer;
					ToSpawnUnit* toSpawnUnit = new ToSpawnUnit(spawnTimer, EntityType_GRYPHON_RIDER);
					toSpawnUnitQueue.push(toSpawnUnit);
					newUnitsToSpawn.push_back(toSpawnUnit);
					toSpawnUnitQueue.back()->toSpawnTimer.Start();
				}
				else if (currentGold < gryphonRiderCost)
					App->audio->PlayFx(3, 0); //Button error sound
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