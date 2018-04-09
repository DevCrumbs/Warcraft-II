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
		
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
		AddGold(500);
	//Life Bar on building 
	if (entitySelectedStats.entitySelected != nullptr) {
		if (!((StaticEntity*)entitySelectedStats.entitySelected)->GetIsFinishedBuilt()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
		}
		else if (((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() == ((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTime()) {
			entitySelectedStats.lifeBar->SetLife(((StaticEntity*)entitySelectedStats.entitySelected)->GetConstructionTimer() * entitySelectedStats.entitySelected->GetMaxLife() / 10);
			entitySelectedStats.HP->SetText(entitySelectedStats.entitySelected->GetStringLife());
			entitySelectedStats.HP->SetLocalPos({ 5, App->scene->entitiesStats->GetLocalRect().h - 17});
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

void j1Player::AddGold(int sumGold)
{
	currentGold += sumGold;
}

int j1Player::GetCurrentGold()
{
	return currentGold;
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

		else if (staticEntity->staticEntityType == EntityType_BARRACKS && staticEntity->buildingState == BuildingState_Normal)
			MakeEntitiesMenu(ent->GetStringLife(), "Barracks", { 546,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && keepUpgrade && staticEntity->buildingState == BuildingState_Normal)
			MakeEntitiesMenu(ent->GetStringLife(), "Castle", { 546,202,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && townHallUpgrade && staticEntity->buildingState == BuildingState_Normal)
			MakeEntitiesMenu(ent->GetStringLife(), "Keep", { 597,202,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_TOWN_HALL && staticEntity->buildingState == BuildingState_Normal)
			MakeEntitiesMenu(ent->GetStringLife(), "Town Hall", { 597,160,50,41 }, ent);

		else if (staticEntity->staticEntityType == EntityType_GOLD_MINE && staticEntity->buildingState == BuildingState_Normal) {
			list<DynamicEntity*> pene = App->entities->GetLastUnitsSelected();
			if (pene.size() != 0) {
					pene.front()->SetBlitState(false);
				}
			staticEntity->buildingState = BuildingState_Destroyed;
		}

		else if (staticEntity->staticEntityType == EntityType_RUNESTONE)
			staticEntity->buildingState = BuildingState_Destroyed;
	
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
	if (entityName_text == "Gryphon Aviary" && gryphonAviary->buildingState == BuildingState_Normal) {
		CreateGryphonAviaryButtons();
	}
	if (entityName_text == "Mage Tower" && mageTower->buildingState == BuildingState_Normal) {
		CreateMageTowerButtons();
	}

	entitySelectedStats.entitySelected = currentEntity;
}

void j1Player::DeleteEntitiesMenu() {

	if (entitySelectedStats.entitySelected == barracks) {
		DestroyUIElem(produceElvenArcherButton);
		DestroyUIElem(produceFootmanButton);
		DestroyUIElem(producePaladinButton);
	}

	else if (entitySelectedStats.entitySelected == gryphonAviary)
		DestroyUIElem(produceGryphonRiderButton);
	else if (entitySelectedStats.entitySelected == mageTower)
		DestroyUIElem(produceMageButton);

	if (entitySelectedStats.entitySelected != nullptr) {
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.HP);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityName);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.entityIcon);
		App->gui->DestroyElement((UIElement**)&entitySelectedStats.lifeBar);
		entitySelectedStats.entitySelected = nullptr;
	}
}

void j1Player::DeleteHoverInfoMenu()
{
	App->gui->DestroyElement((UIElement**)&hoverInfo.background);
	App->gui->DestroyElement((UIElement**)&hoverInfo.cost);
	App->gui->DestroyElement((UIElement**)&hoverInfo.info);
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
		App->gui->DestroyElement((UIElement**)&hoverButtonStruct.hoverButton);
		hoverButtonStruct.currentEntity = nullptr;
	}
}

void j1Player::CreateBarracksButtons()
{
	CreateSimpleButton({ 241,244,50,41 }, { 496, 244, 50, 41 }, { 751,244,50,41 }, { 217, 2 }, produceFootmanButton);
	CreateSimpleButton({ 292,244,50,41 }, { 547, 244, 50, 41 }, { 802,244,50,41 }, { 268, 2 }, produceElvenArcherButton);
	if (barracksUpgrade && stables != nullptr && stables->buildingState == BuildingState_Normal)
		CreateSimpleButton({ 444,244,50,41 }, { 699, 244, 50, 41 }, { 954,244,50,41 }, { 319, 2 }, producePaladinButton);
}

void j1Player::CreateGryphonAviaryButtons()
{
	CreateSimpleButton({ 648,286,50,41 }, { 699, 286, 50, 41 }, { 750,286,50,41 }, { 217, 2 }, produceGryphonRiderButton);
}

void j1Player::CreateMageTowerButtons()
{
<<<<<<< HEAD
	App->gui->DestroyElement((UIElement**)&produceFootmanButton);
	App->gui->DestroyElement((UIElement**)&produceElvenArcherButton);
=======
	CreateSimpleButton({ 342,244,50,41 }, { 597, 244, 50, 41 }, { 852,244,50,41 }, { 217, 2 }, produceMageButton);
}

void j1Player::CreateSimpleButton(SDL_Rect normal, SDL_Rect hover, SDL_Rect pressed, iPoint pos, UIButton* &button) {

	UIButton_Info infoButton;

	infoButton.normalTexArea = normal;
	infoButton.hoverTexArea = hover;
	infoButton.pressedTexArea = pressed;
	button = App->gui->CreateUIButton(pos, infoButton, this, (UIElement*)App->scene->entitiesStats);

}

void j1Player::DestroyUIElem(UIElement* elem) {
	if (elem != nullptr) {
		App->gui->DestroyElement(elem);
		elem = nullptr;
	}
>>>>>>> Develompent
}


void j1Player::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) 
{
	UnitInfo unitInfo;
	FootmanInfo footmanInfo;
	ElvenArcherInfo elvenArcherInfo;
	MageInfo mageInfo;
	PaladinInfo paladinInfo;
	GryphonRiderInfo gryphonRiderInfo;
	fPoint barracksPos = barracks->GetPos();
	fPoint mageTowerPos = { 0.0,0.0 };
	fPoint gryphonAviaryPos = { 0.0,0.0 };
	if (mageTower != nullptr) {
		mageTowerPos = mageTower->GetPos();
	}
	if (gryphonAviary != nullptr) {
		gryphonAviaryPos = gryphonAviary->GetPos();
	}

	switch (UIevent)
	{
	case UI_EVENT_NONE:
		break;
	case UI_EVENT_MOUSE_ENTER:
		if (UIelem == produceFootmanButton) {
			DeleteHoverInfoMenu();
			UIImage_Info backgroundImageInfo;
			backgroundImageInfo.texArea = { 241, 384, 85, 38 };
			hoverInfo.background = App->gui->CreateUIImage({ -2, -40}, backgroundImageInfo, nullptr, produceFootmanButton);
			UILabel_Info infoLabelInfo;
			infoLabelInfo.text = "Produces footman";
			infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
			UILabel_Info costLabelInfo;
			costLabelInfo.text = "Cost: 500 gold";
			costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);
		}
		if (UIelem == produceElvenArcherButton) {
			DeleteHoverInfoMenu();
			UIImage_Info backgroundImageInfo;
			backgroundImageInfo.texArea = { 241, 384, 85, 38 };
			hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
			UILabel_Info infoLabelInfo;
			infoLabelInfo.text = "Produces archer";
			infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
			UILabel_Info costLabelInfo;
			costLabelInfo.text = "Cost: 400 gold";
			costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);
		}
		if (UIelem == produceMageButton && mageTower != nullptr) {
			DeleteHoverInfoMenu();
			UIImage_Info backgroundImageInfo;
			backgroundImageInfo.texArea = { 241, 384, 85, 38 };
			hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
			UILabel_Info infoLabelInfo;
			infoLabelInfo.text = "Produces mage";
			infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
			UILabel_Info costLabelInfo;
			costLabelInfo.text = "Cost: 1200 gold";
			costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);
		}
		if (UIelem == producePaladinButton) {
			DeleteHoverInfoMenu();
			UIImage_Info backgroundImageInfo;
			backgroundImageInfo.texArea = { 241, 384, 85, 38 };
			hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
			UILabel_Info infoLabelInfo;
			infoLabelInfo.text = "Produces paladin";
			infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
			UILabel_Info costLabelInfo;
			costLabelInfo.text = "Cost: 800 gold";
			costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);
		}
		if (UIelem == produceGryphonRiderButton) {
			DeleteHoverInfoMenu();
			UIImage_Info backgroundImageInfo;
			backgroundImageInfo.texArea = { 241, 384, 85, 38 };
			hoverInfo.background = App->gui->CreateUIImage({ -2, -40 }, backgroundImageInfo, nullptr, produceFootmanButton);
			UILabel_Info infoLabelInfo;
			infoLabelInfo.text = "Produces gryphon";
			infoLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.info = App->gui->CreateUILabel({ 5,8 }, infoLabelInfo, nullptr, hoverInfo.background);
			UILabel_Info costLabelInfo;
			costLabelInfo.text = "Cost: 2500 gold";
			costLabelInfo.fontName = FONT_NAME_WARCRAFT9;
			hoverInfo.cost = App->gui->CreateUILabel({ 5, 25 }, costLabelInfo, nullptr, hoverInfo.background);
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
			hoverButtonStruct.currentEntity->SetCurrLife(hoverButtonStruct.currentEntity->GetMaxLife());
			hoverButtonStruct.currentEntity->CheckBuildingState();
			entitySelectedStats.HP->SetText(hoverButtonStruct.currentEntity->GetStringLife());
			entitySelectedStats.lifeBar->SetLife(hoverButtonStruct.currentEntity->GetMaxLife());

		}
		else if (hoverCheck == HoverCheck_Upgrate)
		{
			if (hoverButtonStruct.currentEntity == barracks) {
				barracksUpgrade = true;
				currentGold -= 1000;
			}
			if (hoverButtonStruct.currentEntity == townHall && townHallUpgrade) {
				keepUpgrade = true;
				currentGold -= 500;
			}
			if (hoverButtonStruct.currentEntity == townHall) {
				townHallUpgrade = true;
				currentGold -= 1500;
			}
		}

		if (UIelem == produceFootmanButton && currentGold >= footmanCost) {
			App->entities->AddEntity(EntityType_FOOTMAN, { barracksPos.x + 30, barracksPos.y - 50 }, (EntityInfo&)footmanInfo, unitInfo);
			currentGold -= 500;
		}
		if (UIelem == produceElvenArcherButton && currentGold >= elvenArcherCost) {
			App->entities->AddEntity(EntityType_ELVEN_ARCHER, { barracksPos.x + 30, barracksPos.y - 50 }, (EntityInfo&)elvenArcherInfo, unitInfo);
			currentGold -= 400;
		}
		if (UIelem == produceMageButton && mageTower != nullptr && currentGold >= mageCost) {
			App->entities->AddEntity(EntityType_MAGE, { mageTowerPos.x + 30, mageTowerPos.y - 50 }, (EntityInfo&)mageInfo, unitInfo);
			currentGold -= 1200;
		}
		if (UIelem == producePaladinButton && currentGold >= paladinCost) {
			App->entities->AddEntity(EntityType_PALADIN, { barracksPos.x + 30, barracksPos.y - 50 }, (EntityInfo&)paladinInfo, unitInfo);
			currentGold -= 800;
		}
		if (UIelem == produceGryphonRiderButton && currentGold >= gryphonRiderCost) {
			App->entities->AddEntity(EntityType_GRYPHON_RIDER, { gryphonAviaryPos.x + 30, gryphonAviaryPos.y - 50 }, (EntityInfo&)gryphonRiderInfo, unitInfo);
			currentGold -= 2500;
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

void j1Player::DeleteStaticEntity(StaticEntity* &staticEntity) {

	if (entitySelectedStats.entitySelected == staticEntity)
		DeleteEntitiesMenu();
	App->entities->DestroyStaticEntity(staticEntity);
		staticEntity = nullptr;
}