#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"
#include "j1Scene.h"

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

	StaticEntityType alphaBuilding = App->scene->GetAlphaBuilding();

	//Creates static entities (buildings)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		switch (alphaBuilding) {

		case StaticEntityType_ChickenFarm:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ChickenFarm, buildingPos, { 64,64 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ChickenFarm));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_ElvenLumberMill:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ElvenLumberMill, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ElvenLumberMill));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_Stables:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_Stables, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_Stables));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_GryphonAviary:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_GryphonAviary, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_GryphonAviary));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_MageTower:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_MageTower, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_MageTower));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_ScoutTower:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ScoutTower, buildingPos, { 64,64 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ScoutTower));
			App->scene->SetAplphaBuilding(StaticEntityType_NoType);
			break;

		case StaticEntityType_NoType:
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			break;

		default:
			break;
		}
	}

	//Vanish alpha building preview with mouse button right
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		App->scene->SetAplphaBuilding(StaticEntityType_NoType);
	}

	if (alphaBuilding == StaticEntityType_NoType)
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
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