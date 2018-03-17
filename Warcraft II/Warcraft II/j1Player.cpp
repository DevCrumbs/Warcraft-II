#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"
#include "j1Gui.h"
#include "j1Scene.h"


#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"

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

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		if (stables != nullptr) {
			Entity* ent = (Entity*)stables;
			ent->SetDamageLife(20);
			ent->SetStringLife(ent->GetCurrLife(), ent->GetMaxLife());
			if (entityName->GetText() == "Stables")
				HP->SetText(ent->GetStringLife());
		}


	return true;
}

void j1Player::CheckIfPlaceBuilding() {

	// Mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);
	float auxX = (int)mouseTilePos.x;
	float auxY = (int)mouseTilePos.y;
	fPoint buildingPos = { auxX, auxY };

	//Creates static entities (buildings)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		//Chicken Farm
		if (App->entities->alphaChickenFarm) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			chickenFarm = App->entities->AddStaticEntity(StaticEntityType_ChickenFarm, buildingPos, { 64,64 }, 400, App->entities->GetBuildingInfo(StaticEntityType_ChickenFarm), this);
			chickenFarm->SetStringLife(chickenFarm->GetCurrLife(), chickenFarm->GetMaxLife());
			App->entities->alphaChickenFarm = !App->entities->alphaChickenFarm;
		}
		//Blacksmith
		//if (App->entities->alphaBlacksmith) {}
		//Stables
		if (App->entities->alphaStables) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			stables = App->entities->AddStaticEntity(StaticEntityType_Stables, buildingPos, { 128,128 }, 500, App->entities->GetBuildingInfo(StaticEntityType_Stables), this);
			stables->SetStringLife(stables->GetCurrLife(), stables->GetMaxLife());
			App->entities->alphaStables = !App->entities->alphaStables;
		}
		//Chuch
		//if (App->entities->alphaChurch) {}

		//Gryphon Aviary
		if (App->entities->alphaGryphonAviary) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			gryphonAviary = App->entities->AddStaticEntity(StaticEntityType_GryphonAviary, buildingPos, { 128,128 }, 500, App->entities->GetBuildingInfo(StaticEntityType_GryphonAviary), this);
			gryphonAviary->SetStringLife(gryphonAviary->GetCurrLife(), gryphonAviary->GetMaxLife());

			App->entities->alphaGryphonAviary = !App->entities->alphaGryphonAviary;
		}

		//Mage Tower
		if (App->entities->alphaMageTower) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			mageTower = App->entities->AddStaticEntity(StaticEntityType_MageTower, buildingPos, { 128,128 }, 500, App->entities->GetBuildingInfo(StaticEntityType_MageTower), this);
			App->entities->alphaMageTower = !App->entities->alphaMageTower;
			mageTower->SetStringLife(mageTower->GetCurrLife(), mageTower->GetMaxLife());

		}

		//Scout Tower
		if (App->entities->alphaScoutTower) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			scoutTower = App->entities->AddStaticEntity(StaticEntityType_ScoutTower, buildingPos, { 64,64 }, 150, App->entities->GetBuildingInfo(StaticEntityType_ScoutTower), this);
			App->entities->alphaScoutTower = !App->entities->alphaScoutTower;
			scoutTower->SetStringLife(scoutTower->GetCurrLife(), scoutTower->GetMaxLife());

		}
	}

	if (!App->entities->alphaChickenFarm || !App->entities->alphaStables)
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

	//Vanish alpha building preview with mouse button right
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		App->entities->alphaChickenFarm = false;
		//App->entities->alphaElvenLumber = false;
		//App->entities->alphaBlacksmith = false;
		App->entities->alphaStables = false;
		//App->entities->alphaChurch = false;
		App->entities->alphaGryphonAviary = false;
		App->entities->alphaMageTower = false;
		App->entities->alphaScoutTower = false;
		//Guard Tower
		//Cannon Tower
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
	case EntitiesEvent_None:
		break;
	case EntitiesEvent_RightClick:
		DeleteEntitiesMenu();
		break;
	case EntitiesEvent_LeftClick:
		DeleteEntitiesMenu();
		if (staticEntity->staticEntityType == StaticEntityType_ChickenFarm)
			MakeEntitiesMenu(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_GryphonAviary)
			MakeEntitiesMenu(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_MageTower)
			MakeEntitiesMenu(ent->GetStringLife(), "Magic Tower", { 394,202,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_ScoutTower)
			MakeEntitiesMenu(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_Stables)
			MakeEntitiesMenu(ent->GetStringLife(), "Stables", { 241,160,50,41 });
	
		break;
	case EntitiesEvent_Hover:
		break;
	case EntitiesEvent_Leave:
		break;
	case EntitiesEvent_Created:
		DeleteEntitiesMenu();
		if (staticEntity->staticEntityType == StaticEntityType_ChickenFarm)
			MakeEntitiesMenu(ent->GetStringLife(), "Chicken Farm", { 241,34,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_GryphonAviary)
			MakeEntitiesMenu(ent->GetStringLife(), "Gryphon Aviary", { 394,160,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_MageTower)
			MakeEntitiesMenu(ent->GetStringLife(), "Magic Tower", { 394,202,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_ScoutTower)
			MakeEntitiesMenu(ent->GetStringLife(), "Scout Tower", { 394,34,50,41 });

		else if (staticEntity->staticEntityType == StaticEntityType_Stables)
			MakeEntitiesMenu(ent->GetStringLife(), "Stables", { 241,160,50,41 });


		break;
	default:
		break;
	}

}

void j1Player::MakeEntitiesMenu(string HP_text, string entityName_text, SDL_Rect iconDim) {
	

	UILabel_Info labelInfo;
	labelInfo.text = HP_text;
	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	HP = App->gui->CreateUILabel({ 5, App->scene->entitiesStats->GetLocalRect().h }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);


	labelInfo.text = entityName_text;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	entityName = App->gui->CreateUILabel({ 5,5 }, labelInfo, nullptr, (UIElement*)App->scene->entitiesStats);

	UIImage_Info iconInfo;
	iconInfo.texArea = iconDim;
	iconInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	iconInfo.verticalOrientation = VERTICAL_POS_CENTER;
	entityIcon = App->gui->CreateUIImage({ 5, App->scene->entitiesStats->GetLocalRect().h/2 }, iconInfo, nullptr, (UIElement*)App->scene->entitiesStats);

}

void j1Player::DeleteEntitiesMenu() {

	App->gui->DestroyElement(HP);
	App->gui->DestroyElement(entityName);
	App->gui->DestroyElement(entityIcon);

}