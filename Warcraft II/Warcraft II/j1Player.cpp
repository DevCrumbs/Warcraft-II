#include "j1Player.h"

#include "Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1EntityFactory.h"

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

	//Creates static entities (buildings)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		//Chicken Farm
		if (App->entities->alphaChickenFarm) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ChickenFarm, buildingPos, { 64,64 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ChickenFarm));
			App->entities->alphaChickenFarm = !App->entities->alphaChickenFarm;
		}
		//Elven Lumber Mill
		if(App->entities->alphaElvenLumber){
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ElvenLumberMill, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ElvenLumberMill));
			App->entities->alphaElvenLumber = !App->entities->alphaElvenLumber;
		}
		//Blacksmith
		//if (App->entities->alphaBlacksmith) {}
		//Stables
		if (App->entities->alphaStables) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_Stables, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_Stables));
			App->entities->alphaStables = !App->entities->alphaStables;
		}
		//Chuch
		//if (App->entities->alphaChurch) {}

		//Gryphon Aviary
		if (App->entities->alphaGryphonAviary) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_GryphonAviary, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_GryphonAviary));
			App->entities->alphaGryphonAviary = !App->entities->alphaGryphonAviary;
		}

		//Mage Tower
		if (App->entities->alphaMageTower) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_MageTower, buildingPos, { 128,128 }, 30, App->entities->GetBuildingInfo(StaticEntityType_MageTower));
			App->entities->alphaMageTower = !App->entities->alphaMageTower;
		}

		//Scout Tower
		if (App->entities->alphaScoutTower) {
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
			App->entities->AddStaticEntity(StaticEntityType_ScoutTower, buildingPos, { 64,64 }, 30, App->entities->GetBuildingInfo(StaticEntityType_ScoutTower));
			App->entities->alphaScoutTower = !App->entities->alphaScoutTower;
		}
	}

	if (!App->entities->alphaChickenFarm || !App->entities->alphaStables)
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

	//Vanish alpha building preview with mouse button right
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		App->entities->alphaChickenFarm = false;
		App->entities->alphaElvenLumber = false;
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