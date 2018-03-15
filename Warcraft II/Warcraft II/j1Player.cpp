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

	// Mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	//TRANSPARENT BUILDING HOVERING (DEBUG)
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		App->entities->alphaChickenFarm = !App->entities->alphaChickenFarm;
	}

	//CREATING A STATIC ENTITY (DEBUG)
	if (App->entities->alphaChickenFarm && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
		float auxX = (int)mouseTilePos.x;
		float auxY = (int)mouseTilePos.y;
		fPoint buildingPos = { auxX, auxY };
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
		App->entities->AddStaticEntity(StaticEntityType_ChickenFarm, buildingPos, { 64,64 }, 30, (const EntityInfo&)App->entities->chickenFarmInfo);
		App->entities->alphaChickenFarm = !App->entities->alphaChickenFarm;
	}

	if(!App->entities->alphaChickenFarm)
		SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);

	return true;
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