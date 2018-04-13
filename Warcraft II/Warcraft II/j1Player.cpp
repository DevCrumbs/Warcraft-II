#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1Particles.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILifeBar.h"

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

	return ret;
}

bool j1Player::Update(float dt) {

	CheckIfPlaceBuilding();

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		if (stables != nullptr) {
			if (stables->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)stables;
				ent->ApplyDamage(20);
				if (!stables->CheckBuildingState()) {
					DeleteStaticEntity(stables);
				}
				if (entitySelectedStats.entitySelected == ent) {
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
				if (!mageTower->CheckBuildingState()) {
					DeleteStaticEntity(mageTower);
				}
				if (entitySelectedStats.entitySelected == ent) {
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
					DeleteStaticEntity(scoutTower.back());
					scoutTower.pop_back();
				}
				if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
	

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		if (gryphonAviary != nullptr)
			if (gryphonAviary->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)gryphonAviary;
				ent->ApplyDamage(20);
				if (!gryphonAviary->CheckBuildingState()) {
					DeleteStaticEntity(gryphonAviary);
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		if (!chickenFarm.empty()) 
			if (chickenFarm.back()->GetIsFinishedBuilt()) {
				Entity* ent = (Entity*)chickenFarm.back();
				ent->ApplyDamage(20);
				if (!chickenFarm.back()->CheckBuildingState()) {
					DeleteStaticEntity(chickenFarm.back());
					chickenFarm.pop_back();
				}
				else if (entitySelectedStats.entitySelected == ent) {
					entitySelectedStats.HP->SetText(ent->GetStringLife());
					entitySelectedStats.lifeBar->DecreaseLife(20);
				}
			}
		

	//Life Bar on building 
	if (entitySelectedStats.entitySelected != nullptr) {
		if (!((StaticEntity*)entitySelectedStats.entitySelected)->GetIsFinishedBuilt()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
		}
		else if (((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() == ((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTime()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
			entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
			entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17});
		}
	}


	return true;
}

bool j1Player::PostUpdate() {

	if (hoverButtonStruct.isCreated) {
		SDL_Rect r;
		r.x = (int)hoverButtonStruct.nextEntity->GetPos().x;
		r.y = (int)hoverButtonStruct.nextEntity->GetPos().y;
		r.w = hoverButtonStruct.nextEntity->GetSize().x;
		r.h = hoverButtonStruct.nextEntity->GetSize().y;

		CreateHoverButton(hoverCheck, r, (StaticEntity*)hoverButtonStruct.nextEntity);
		hoverButtonStruct.isCreated = false;                                              
	}


	return true;
}

iPoint j1Player::GetMouseTilePos() {

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);

	return mouseTile;
}

iPoint j1Player::GetMousePos() {

	iPoint mouseTilePos = App->map->MapToWorld(GetMouseTilePos().x, GetMouseTilePos().y);

	return mouseTilePos;
}

void j1Player::CheckIfPlaceBuilding()
{

	// Mouse position (world and map coords)
	float auxX = (int)GetMousePos().x;
	float auxY = (int)GetMousePos().y;
	fPoint buildingPos = { auxX, auxY };

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
			}
			break;

		case EntityType_STABLES:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				stables = (StaticEntity*)App->entities->AddEntity(EntityType_STABLES, buildingPos, App->entities->GetBuildingInfo(EntityType_STABLES), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
			}
			break;

		case EntityType_GRYPHON_AVIARY:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				gryphonAviary = (StaticEntity*)App->entities->AddEntity(EntityType_GRYPHON_AVIARY, buildingPos, App->entities->GetBuildingInfo(EntityType_GRYPHON_AVIARY), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
			}
			break;

		case EntityType_MAGE_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Medium)) {
				mageTower = (StaticEntity*)App->entities->AddEntity(EntityType_MAGE_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_MAGE_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
			}
			break;

		case EntityType_SCOUT_TOWER:
			if (!App->entities->IsPreviewBuildingOnEntity(GetMouseTilePos(), Small)) {
				StaticEntity* s;
				s = (StaticEntity*)App->entities->AddEntity(EntityType_SCOUT_TOWER, buildingPos, App->entities->GetBuildingInfo(EntityType_SCOUT_TOWER), unitInfo, this);
				App->scene->SetAplphaBuilding(EntityType_NONE);
				scoutTower.push_back(s);
			}
			break;

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

// Called before quitting
bool j1Player::CleanUp()
{
	bool ret = true;

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


void j1Player::OnStaticEntitiesEvent(StaticEntity* staticEntity, EntitiesEvent entitiesEvent) {

	Entity* ent = (Entity*)staticEntity;

	switch (entitiesEvent)
	{
	case EntitiesEvent_NONE:
		break;
	case EntitiesEvent_RIGHT_CLICK:
		DeleteEntitiesMenu();
		break;
	case EntitiesEvent_LEFT_CLICK:
		DeleteEntitiesMenu();

		if (staticEntity->staticEntityType == EntityType_CHICKEN_FARM)
			MakeEntitiesMenu(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_GRYPHON_AVIARY)
			MakeEntitiesMenu(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_MAGE_TOWER)
			MakeEntitiesMenu(ent->GetStringLife(), "Mage Tower", { 394,202,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_SCOUT_TOWER)
			MakeEntitiesMenu(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_STABLES)
			MakeEntitiesMenu(ent->GetStringLife(), "Stables", { 241,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_BARRACKS)
			MakeEntitiesMenu(ent->GetStringLife(), "Barracks", { 241,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_TOWN_HALL)
			MakeEntitiesMenu(ent->GetStringLife(), "Town Hall", { 241,160,50,41 }, ent);
	
		break;
	case EntitiesEvent_HOVER:
		if ((staticEntity->staticEntityType == EntityType_TOWN_HALL || staticEntity->staticEntityType == EntityType_BARRACKS) && ent->GetCurrLife() == ent->GetMaxLife())
			hoverCheck = HoverCheck_Upgrate;
		else if (ent->GetCurrLife() < ent->GetMaxLife())
			hoverCheck = HoverCheck_Repair;
		else
			hoverCheck = HoverCheck_None;

		hoverButtonStruct.isCreated = true;
		hoverButtonStruct.prevEntity = hoverButtonStruct.currentEntity;
		hoverButtonStruct.nextEntity = staticEntity;

		DestroyHoverButton(ent);

		break;
	case EntitiesEvent_LEAVE:
		DestroyHoverButton(ent);

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

		else if (staticEntity->staticEntityType == EntityType_STABLES)
			MakeEntitiesMenu("NO_HP_TEXT", "Stables", { 241,160,50,41 },ent);
		break;
	
	default:
		break;
	}

}

void j1Player::MakeEntitiesMenu(string HP_text, string entityName_text, SDL_Rect iconDim, Entity* currentEntity) {

	UILabel_Info labelInfo;
	labelInfo.text = entityName_text;
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

	entitySelectedStats.entitySelected = currentEntity;
}

void j1Player::MakeUnitMenu(Entity* entity)
{
	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_FOOTMAN) {
		UIImage_Info imageInfo;
		imageInfo.texArea = { 240,244, 50, 41 };
		imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
		imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
		entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h / 2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILifeBar_Info lifeInfo;
		lifeInfo.background = { 289,346,145,23 };
		lifeInfo.bar = { 300,373,128,8 };
		lifeInfo.maxLife = lifeInfo.life = entity->GetMaxLife();
		lifeInfo.maxWidth = lifeInfo.bar.w;
		lifeInfo.lifeBarPosition = { 12, 10 };
		entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50 }, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILabel_Info labelInfo;
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

	if (((DynamicEntity*)entity)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
		UIImage_Info imageInfo;
		imageInfo.texArea = { 291,244, 50, 41 };
		imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
		imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
		entitySelectedStats.entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h / 2 }, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILifeBar_Info lifeInfo;
		lifeInfo.background = { 289,346,145,23 };
		lifeInfo.bar = { 300,373,128,8 };
		lifeInfo.maxLife = lifeInfo.life = entity->GetMaxLife();
		lifeInfo.maxWidth = lifeInfo.bar.w;
		lifeInfo.lifeBarPosition = { 12, 10 };
		entitySelectedStats.lifeBar = App->gui->CreateUILifeBar({ 65, 50 }, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		UILabel_Info labelInfo;
		labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT14;
		labelInfo.text = "Elven Archer";
		labelInfo.verticalOrientation = VERTICAL_POS_TOP;
		entitySelectedStats.entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

		labelInfo.text = "50 HP";
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
	int i = 0;
	while (it != units.end()) {
		if (units.size() == 1) {
			MakeUnitMenu((*it));

		}
		else {
			switch (i)
			{
			case 0:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 2,18 }, { 649, 160, 46, 30 }, groupSelectedStats.entity1Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 2,18 }, { 696, 160, 46, 30 }, groupSelectedStats.entity1Icon);
				}
				CreateGroupLifeBar({ 1,33 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar1, (Entity*)(*it));
				break;
			case 1:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 57,18 }, { 649, 160, 46, 30 }, groupSelectedStats.entity2Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 57,18 }, { 696, 160, 46, 30 }, groupSelectedStats.entity2Icon);
				}
				CreateGroupLifeBar({ 56,33 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar2, (Entity*)(*it));
				break;
			case 2:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 111,18 }, { 649, 160, 46, 30 }, groupSelectedStats.entity3Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 111,18 }, { 696, 160, 46, 30 }, groupSelectedStats.entity3Icon);
				}
				CreateGroupLifeBar({ 110,33 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar3, (Entity*)(*it));
				break;
			case 3:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 166,18 }, { 649, 160, 46, 30 }, groupSelectedStats.entity4Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 166,18 }, { 696, 160, 46, 30 }, groupSelectedStats.entity4Icon);
				}
				CreateGroupLifeBar({ 165,33 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar4, (Entity*)(*it));
				break;
			case 4:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 2, 57 }, { 649, 160, 46, 30 }, groupSelectedStats.entity5Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 2, 57 }, { 696, 160, 46, 30 }, groupSelectedStats.entity5Icon);
				}
				CreateGroupLifeBar({ 1,72 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar5, (Entity*)(*it));
				break;
			case 5:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 57,57 }, { 649, 160, 46, 30 }, groupSelectedStats.entity6Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 57,57 }, { 696, 160, 46, 30 }, groupSelectedStats.entity6Icon);
				}
				CreateGroupLifeBar({ 56,72 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar6, (Entity*)(*it));
				break;
			case 6:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 111,57 }, { 649, 160, 46, 30 }, groupSelectedStats.entity7Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 111,57 }, { 696, 160, 46, 30 }, groupSelectedStats.entity7Icon);
				}
				CreateGroupLifeBar({ 110,72 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar7, (Entity*)(*it));
				break;
			case 7:
				if ((*it)->dynamicEntityType == EntityType_FOOTMAN) {
					CreateGroupIcon({ 166,57 }, { 649, 160, 46, 30 }, groupSelectedStats.entity8Icon);
				}
				else if ((*it)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
					CreateGroupIcon({ 166,57 }, { 696, 160, 46, 30 }, groupSelectedStats.entity8Icon);
				}
				CreateGroupLifeBar({ 165,72 }, { 240,362,47,7 }, { 242,358,42,3 }, groupSelectedStats.lifeBar8, (Entity*)(*it));
				break;
			default:
				break;
			}
		}
		
		it++;
		i++;
	}
	i = 0;
	groupSelectedStats.units = units;
	CreateAbilitiesButtons();
	
}

void j1Player::DeleteEntitiesMenu() {

	if (entitySelectedStats.entitySelected != nullptr) {

		App->gui->DestroyElement((UIElement**)&entitySelectedStats.HP);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityName);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityIcon);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.lifeBar);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityDamage);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityMana);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityMovementSpeed);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityRange);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entitySight);
		App->gui->DestroyElement((UIElement**)&commandPatrolButton);
		App->gui->DestroyElement((UIElement**)&commandStopButton);
	}

	if (!groupSelectedStats.units.empty()) {
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity1Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity2Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity3Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity4Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity5Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity6Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity7Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.entity8Icon);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar1);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar2);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar3);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar4);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar5);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar6);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar7);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar8);
		App->gui->DestroyElement((UIElement**)&commandPatrolButton);
		App->gui->DestroyElement((UIElement**)&commandStopButton);
		App->gui->DestroyElement((UIElement**)&groupSelectedStats.lifeBar8);
		groupSelectedStats.units.clear();
	}
	
}

void j1Player::MakeHoverInfoMenu(string unitProduce, string gold) {

	UIImage_Info backgroundImageInfo;
	backgroundImageInfo.texArea = { 241, 384, 85, 38 };
	hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
	UILabel_Info infoLabelInfo;
	infoLabelInfo.text = unitProduce;
	infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
	UILabel_Info costLabelInfo;
	costLabelInfo.text = gold;
	costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
	hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);

}
void j1Player::DeleteHoverInfoMenu()
{
	App->gui->DestroyElement((UIElement**)&hoverInfo.background);
	App->gui->DestroyElement((UIElement**)&hoverInfo.cost);
	App->gui->DestroyElement((UIElement**)&hoverInfo.info);
}

void j1Player::CreateGroupIcon(iPoint iconPos, SDL_Rect texArea, UIImage* &image)
{
	UIImage_Info imageInfo;
	imageInfo.texArea = texArea;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	image = App->gui->CreateUIImage(iconPos, imageInfo, nullptr, (UIElement*)App->scene->entitiesStats);
}
void j1Player::CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea, UILifeBar* &lifeBar, Entity * entity)
{
	UILifeBar_Info lifeInfo;
	lifeInfo.background = backgroundTexArea;
	lifeInfo.bar = barTexArea;
	lifeInfo.maxLife = entity->GetMaxLife();
	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 12, 10 };
	lifeBar = App->gui->CreateUILifeBar(lifeBarPos, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);
}

void j1Player::CreateToSpawnUnitLifeBar(iPoint lifeBarPos, UILifeBar* &lifeBar)
{
	UILifeBar_Info barInfo;
	barInfo.background = { 241,362,46,7 };
	barInfo.bar = { 243,358,42,3 };
	barInfo.maxLife = spawningTime;
	barInfo.life = (barInfo.maxLife);
	barInfo.maxWidth = barInfo.bar.w;
	barInfo.lifeBarPosition = { 2, 2 };
	lifeBar = App->gui->CreateUILifeBar({ lifeBarPos.x, lifeBarPos.y }, barInfo, nullptr, (UIElement*)App->scene->entitiesStats);

}

void j1Player::CreateHoverButton(HoverCheck hoverCheck, SDL_Rect pos, StaticEntity* staticEntity) {

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
	}
}

void j1Player::DestroyHoverButton(Entity* ent) {
	if (hoverButtonStruct.currentEntity == ent || hoverButtonStruct.prevEntity == ent) {
		App->gui->DestroyElement(hoverButtonStruct.hoverButton);
		hoverButtonStruct.currentEntity = nullptr;
	}
}

void j1Player::CreateBarracksButtons()
{
	UIButton_Info produceFootmanButtonInfo;
	produceFootmanButtonInfo.normalTexArea = { 241,244,50,41 };
	produceFootmanButtonInfo.hoverTexArea = { 496,244,50,41 };
	produceFootmanButtonInfo.pressedTexArea = { 751,244,50,41 };
	produceFootmanButton = App->gui->CreateUIButton({ 217, 2 }, produceFootmanButtonInfo, this, (UIElement*)App->scene->entitiesStats);

	UIButton_Info produceElvenArcherButtonInfo;
	produceElvenArcherButtonInfo.normalTexArea = { 292,244,50,41 };
	produceElvenArcherButtonInfo.hoverTexArea = { 547,244,50,41 };
	produceElvenArcherButtonInfo.pressedTexArea = { 802,244,50,41 };
	produceElvenArcherButton = App->gui->CreateUIButton({ 269, 2 }, produceElvenArcherButtonInfo, this, (UIElement*)App->scene->entitiesStats);
}

void j1Player::DestroyBarracksButtons()
{
	//Delete UI elements when not used
	if (toSpawnUnitStats.frstInQueueIcon != nullptr) {
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.frstInQueueIcon);
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.sndInQueueIcon);
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.trdInQueueIcon);
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.frstInQueueBar);
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.sndInQueueBar);
		App->gui->DestroyElement((UIElement**)&toSpawnUnitStats.trdInQueueBar);
	}

	uint unitInQueue = 1;
	for each (ToSpawnUnit unit in toSpawnUnitQueue._Get_container()) { //Iterates every element in the queue
		UIImage_Info info;
		switch (unitInQueue) {
		case 1:
			switch (unit.entityType) {
			case EntityType_FOOTMAN:
				CreateGroupIcon({ 72, 20 }, { 649,160,39,30 }, toSpawnUnitStats.frstInQueueIcon);
				break;
			case EntityType_ELVEN_ARCHER:
				CreateGroupIcon({ 72, 20 }, { 696,160,39,30 }, toSpawnUnitStats.frstInQueueIcon);
				break;
			default:
				CreateGroupIcon({ 72, 20 }, { 649,160,39,30 }, toSpawnUnitStats.frstInQueueIcon); //Footman
				break;
			}
			CreateToSpawnUnitLifeBar({ 72, 40 }, toSpawnUnitStats.frstInQueueBar); //To spawn unit lifeBar timer
			toSpawnUnitStats.frstInQueueBar->SetLife(unit.toSpawnTimer.ReadSec());
			break;
		case 2:
			switch (unit.entityType) {
			case EntityType_FOOTMAN:
				CreateGroupIcon({ 120, 20 }, { 649,160,39,30 }, toSpawnUnitStats.sndInQueueIcon);
				break;
			case EntityType_ELVEN_ARCHER:
				CreateGroupIcon({ 120, 20 }, { 696,160,39,30 }, toSpawnUnitStats.sndInQueueIcon);
				break;
			default:
				CreateGroupIcon({ 120, 20 }, { 649,160,39,30 }, toSpawnUnitStats.sndInQueueIcon); //Footman
				break;
			}
			CreateToSpawnUnitLifeBar({ 120, 40 }, toSpawnUnitStats.sndInQueueBar); //To spawn unit lifeBar timer
			toSpawnUnitStats.sndInQueueBar->SetLife(unit.toSpawnTimer.ReadSec());
			break;
		case 3:
			switch (unit.entityType) {
			case EntityType_FOOTMAN:
				CreateGroupIcon({ 168, 20 }, { 649,160,39,30 }, toSpawnUnitStats.trdInQueueIcon);
				break;
			case EntityType_ELVEN_ARCHER:
				CreateGroupIcon({ 168, 20 }, { 696,160,39,30 }, toSpawnUnitStats.trdInQueueIcon);
				break;
			default:
				CreateGroupIcon({ 168, 20 }, { 649,160,39,30 }, toSpawnUnitStats.trdInQueueIcon); //Footman
				break;
			}
			CreateToSpawnUnitLifeBar({ 168, 40 }, toSpawnUnitStats.trdInQueueBar); //To spawn unit lifeBar timer
			toSpawnUnitStats.trdInQueueBar->SetLife(unit.toSpawnTimer.ReadSec());
			break;
		default:
			break;
		}
		unitInQueue++;
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
	UnitInfo unitInfo;

	switch (UIevent)
	{
	case UI_EVENT_NONE:
		break;
	case UI_EVENT_MOUSE_ENTER:
		break;
	case UI_EVENT_MOUSE_LEAVE:
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (hoverCheck == HoverCheck_Repair) {
			hoverButtonStruct.currentEntity->SetCurrLife(hoverButtonStruct.currentEntity->GetMaxLife());
			hoverButtonStruct.currentEntity->CheckBuildingState();
			entitySelectedStats.HP->SetText(hoverButtonStruct.currentEntity->GetStringLife());
			entitySelectedStats.lifeBar->SetLife(hoverButtonStruct.currentEntity->GetMaxLife());

		}
		else if (hoverCheck == HoverCheck_Upgrate)
		{
			//Use hoverButtonStruct
			//TODO JOAN
		}
		if (UIelem == produceFootmanButton) {			
			App->entities->AddEntity(EntityType_FOOTMAN, { 100, 100 }, App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, this);
		}
		if (UIelem == produceElvenArcherButton) {
			App->entities->AddEntity(EntityType_ELVEN_ARCHER, { 100, 100 }, App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER), unitInfo, this);
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

void j1Player::DeleteStaticEntity(StaticEntity* &staticEntity)
{
	if(App->scene->GetPauseMenuActions() == PauseMenuActions_NOT_EXIST)
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
			if (UIelem == commandPatrolButton) {
				// Command Patrol (SANDRA)
			}
			if (UIelem == commandStopButton) {
				// Command Stop (SANDRA)
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

			if (hoverCheck == HoverCheck_Repair) {
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

	if (entitySelectedStats.entitySelected == staticEntity)
		DeleteEntitiesMenu();
	App->entities->DestroyStaticEntity(staticEntity);
		staticEntity = nullptr;
}