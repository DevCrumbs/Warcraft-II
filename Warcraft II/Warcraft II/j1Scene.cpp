#include"Brofiler\Brofiler.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Input.h"
#include "j1Textures.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UICursor.h"


j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading scene");

	// Load maps
	pugi::xml_node maps = config.child("maps");

	orthogonalMap = maps.child("orthogonal").attribute("name").as_string();
	orthogonalActive = maps.child("orthogonal").attribute("active").as_bool();
	orthogonalTexName = maps.child("orthogonal").attribute("tex").as_string();

	isometricMap = maps.child("isometric").attribute("name").as_string();
	isometricActive = maps.child("isometric").attribute("active").as_bool();
	isometricTexName = maps.child("isometric").attribute("tex").as_string();

	warcraftMap = maps.child("warcraft").attribute("name").as_string();
	warcraftActive = maps.child("warcraft").attribute("active").as_bool();
	warcraftTexName = maps.child("warcraft").attribute("tex").as_string();

	LoadKeys(config.child("buttons"));

	// Load songs

	// Load FX
	/*
	for (pugi::xml_node node = config.child("audio").child("fx").child("fx"); node; node = node.next_sibling("fx"))
		App->audio->LoadFx(node.attribute("name").as_string());
	*/

	//Load camera attributes
	pugi::xml_node camera = config.child("camera");

	camSpeed = camera.attribute("speed").as_float();
	camMovMargin = camera.attribute("movMarginPcnt").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	// Save camera info
	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	// Load an orthogonal, isometric or warcraft-based map
	if (orthogonalActive) {
		ret = App->map->Load(orthogonalMap.data());
		debugTex = App->tex->Load(orthogonalTexName.data());
	}
	else if (isometricActive) {
		ret = App->map->Load(isometricMap.data());
		debugTex = App->tex->Load(isometricTexName.data());
	}
	else if (warcraftActive) {
		ret = App->map->CreateNewMap();

		debugTex = App->tex->Load(warcraftTexName.data());
	}

	// Create walkability map
	if (ret)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	//LoadInGameUI
	LoadInGameUI();

	//Calculate camera movement in pixels through the percentatge given
	camMovMargin = camMovMargin * ((width + height) / 2) / 100;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	bool ret = true;

	// debug pathfing ------------------
	static iPoint origin;
	static bool isSelected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (isSelected)
		{
			App->pathfinding->CreatePath(origin, p, DISTANCE_TO);
			isSelected = false;
		}
		else
		{
			origin = p;
			isSelected = true;
		}
	}

	// Player start position
	/*
	App->entities->playerData->startPos = App->map->data.GetObjectPosition("Player", "StartPos");
	App->entities->playerData->position = App->entities->playerData->startPos;
	*/

	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	bool ret = true;



	// Draw
	//App->map->Draw(); // map
	App->entities->Draw(); // entities

	// Debug pathfinding ------------------------------

	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debugTex, p.x, p.y);

	const vector<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->at(i).x, path->at(i).y);
		App->render->Blit(debugTex, pos.x, pos.y);
	}


	DebugKeys();
	CheckCameraMovement(dt);

	if (App->input->GetKey(buttonReloadMap) == KEY_REPEAT)
	{
		App->map->UnLoad();
		App->map->CreateNewMap();
	}

//	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
	//	App->win->scale += 0.05f;

//	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) 
//		App->win->scale -= 0.05f;



	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(buttonLeaveGame) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	bool ret = true;

	LOG("Freeing scene");

	App->audio->PauseMusic();
	App->map->UnLoad();
	App->tex->UnLoad(debugTex);

	// Set to nullptr the pointers to the UI elements

	return ret;
}

// Debug keys
void j1Scene::DebugKeys()
{
	// F1: start from the beginning of the first level
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		/*
		if (index == 0)
			App->entities->playerData->position = App->entities->playerData->startPos;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F2: start from the beginning of the current level
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		/*
		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F4: change between maps
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		/*
		if (index == 0)
			index = 1;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	}

	// F5: save the current state
	if (App->input->GetKey(buttonSaveGame) == KEY_DOWN) {
		App->SaveGame();
	}

	// F6: load the previous state
	if (App->input->GetKey(buttonLoadGame) == KEY_DOWN) {
		App->LoadGame();
	}

	// F7: fullscreen
	if (App->input->GetKey(buttonFullScreen) == KEY_DOWN)
		App->win->SetFullscreen();

	// F10: God mode
	if (App->input->GetKey(buttonGodMode) == KEY_DOWN)
		god = !god;

	// 1, 2, 3: camera blit
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->map->blitOffset < 15 && App->map->cameraBlit)
		App->map->blitOffset += 7;

	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && App->map->blitOffset > -135 && App->map->cameraBlit)
		App->map->blitOffset -= 7;

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		App->map->cameraBlit = !App->map->cameraBlit;
}

void j1Scene::CheckCameraMovement(float dt) {

	int downMargin = -(App->map->data.height * App->map->data.tileHeight) + height / scale;
	int rightMargin = -(App->map->data.width * App->map->data.tileWidth) + width / scale;

	downMargin = -10000;
	rightMargin = -10000;

	//Move with arrows
	//UP
	if (App->input->GetKey(buttonMoveUp) == KEY_REPEAT && App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	//DOWN
	if (App->input->GetKey(buttonMoveDown) == KEY_REPEAT && App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	//LEFT
	if (App->input->GetKey(buttonMoveLeft) == KEY_REPEAT && App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	//RIGHT
	if (App->input->GetKey(buttonMoveRight) == KEY_REPEAT && App->render->camera.x >= rightMargin)
		App->render->camera.x -= camSpeed * dt;

	//Move with mouse
	//App->input->GetMousePosition(mouse.x, mouse.y);

	////UP
	//if (mouse.y <= camMovMargin/scale && App->render->camera.y <= 0)
	//	App->render->camera.y += camSpeed * dt;
	////DOWN
	//if (mouse.y >= (height - camMovMargin) / scale && App->render->camera.y >= downMargin)
	//	App->render->camera.y -= camSpeed * dt;
	////LEFT
	//if (mouse.x <= camMovMargin / scale && App->render->camera.x <= 0)
	//	App->render->camera.x += camSpeed * dt;
	////RIGHT
	//if (mouse.x >= (width - camMovMargin) / scale && App->render->camera.x >= rightMargin)
	//	App->render->camera.x -= camSpeed * dt;

}

void j1Scene::LoadInGameUI()
{
	UIButton_Info buildingButtonInfo;
	buildingButtonInfo.normalTexArea = {0, 0, 129, 33};
	buildingButtonInfo.hoverTexArea = { 129, 0, 129, 33 };
	buildingButtonInfo.pressedTexArea = { 257, 0, 129, 33 };
	buildingButton = App->gui->CreateUIButton({ (int)App->render->camera.w - buildingButtonInfo.normalTexArea.w, 0 }, buildingButtonInfo, this, nullptr);

	UILabel_Info buildingLabelInfo;
	buildingLabelInfo.fontName = FONT_NAME_WARCRAFT;
	buildingLabelInfo.normalColor = White_;
	buildingLabelInfo.text = "Buildings";
	buildingLabel = App->gui->CreateUILabel({ 27,12 }, buildingLabelInfo, this, buildingButton);


	UIImage_Info entitiesInfo;
	entitiesInfo.texArea = { 0, 565, 371, 82 };
	entitiesStats = App->gui->CreateUIImage({ (int)App->render->camera.w - entitiesInfo.texArea.w,(int)App->render->camera.h - entitiesInfo.texArea.h }, entitiesInfo, this);

	UICursor_Info mouseInfo;
	mouseInfo.default = { 243, 525, 28, 33 };
	mouseInfo.onClick = { 243, 525, 28, 33 };
	mouseText = App->gui->CreateUICursor(mouseInfo, this);

}

void j1Scene::LoadBuildingMenu()
{
	UIImage_Info buildingMenuInfo;
	buildingMenuInfo.texArea = { 0,33,240,529 };
	buildingMenu = App->gui->CreateUIImage({ -112, 0 }, buildingMenuInfo, this, buildingButton);
	buildingMenuOn = true;

	UIButton_Info chickenFarmInfo;
	chickenFarmInfo.normalTexArea = { 241,34,50,41 };
	chickenFarmInfo.hoverTexArea = { 292,34,50,41 };
	chickenFarmInfo.pressedTexArea = { 343,34,50,41 };
	chickenFarmButton = App->gui->CreateUIButton({ 15, 55 }, chickenFarmInfo, this, buildingMenu);

	UILabel_Info chickenFarmLabelInfo;
	chickenFarmLabelInfo.fontName = FONT_NAME_WARCRAFT;
	chickenFarmLabelInfo.normalColor = White_;
	chickenFarmLabelInfo.text = "Chicken Farm";
	chickenFarmLabel = App->gui->CreateUILabel({ 75, 65 }, chickenFarmLabelInfo, this, buildingMenu);

	UIButton_Info elvenLumberInfo;
	elvenLumberInfo.normalTexArea = { 241,76,50,41 };
	elvenLumberInfo.hoverTexArea = { 292,76,50,41 };
	elvenLumberInfo.pressedTexArea = { 343,76,50,41 };
	elvenLumberButton = App->gui->CreateUIButton({ 15, 100 }, elvenLumberInfo, this, buildingMenu);

	UILabel_Info elvenLumberLabelInfo;
	elvenLumberLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	elvenLumberLabelInfo.normalColor = White_;
	elvenLumberLabelInfo.text = "Elven Lumber Mill";
	elvenLumberLabel = App->gui->CreateUILabel({ 75, 110}, elvenLumberLabelInfo, this, buildingMenu);

	UIButton_Info blackSmithInfo;
	blackSmithInfo.normalTexArea = { 241,118,50,41 };
	blackSmithInfo.hoverTexArea = { 292,118,50,41 };
	blackSmithInfo.pressedTexArea = { 343,118,50,41 };
	blackSmithButton = App->gui->CreateUIButton({ 15, 145 }, blackSmithInfo, this, buildingMenu);

	UILabel_Info blackSmithLabelInfo;
	blackSmithLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	blackSmithLabelInfo.normalColor = White_;
	blackSmithLabelInfo.text = "Blacksmith";
	blackSmithLabel = App->gui->CreateUILabel({ 75, 155 }, blackSmithLabelInfo, this, buildingMenu);

	UIButton_Info stablesInfo;
	stablesInfo.normalTexArea = { 241,160,50,41 };
	stablesInfo.hoverTexArea = { 292,160,50,41 };
	stablesInfo.pressedTexArea = { 343,160,50,41 };
	stablesButton = App->gui->CreateUIButton({ 15, 190 }, stablesInfo, this, buildingMenu);

	UILabel_Info stablesLabelInfo;
	stablesLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	stablesLabelInfo.normalColor = White_;
	stablesLabelInfo.text = "Stables";
	stablesLabel = App->gui->CreateUILabel({ 75, 200 }, stablesLabelInfo, this, buildingMenu);

	UIButton_Info churchInfo;
	churchInfo.normalTexArea = { 241,202,50,41 };
	churchInfo.hoverTexArea = { 292,202,50,41 };
	churchInfo.pressedTexArea = { 343,202,50,41 };
	churchButton = App->gui->CreateUIButton({ 15, 235 }, churchInfo, this, buildingMenu);

	UILabel_Info churchLabelInfo;
	churchLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	churchLabelInfo.normalColor = White_;
	churchLabelInfo.text = "Church";
	churchLabel = App->gui->CreateUILabel({ 75, 245 }, churchLabelInfo, this, buildingMenu);

	UIButton_Info gryphonAviaryInfo;
	gryphonAviaryInfo.normalTexArea = { 394,160,50,41 };
	gryphonAviaryInfo.hoverTexArea = { 445,160,50,41 };
	gryphonAviaryInfo.pressedTexArea = { 496,160,50,41 };
	gryphonAviaryButton = App->gui->CreateUIButton({ 15, 280 }, gryphonAviaryInfo, this, buildingMenu);

	UILabel_Info gryphonAviaryLabelInfo;
	gryphonAviaryLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	gryphonAviaryLabelInfo.normalColor = White_;
	gryphonAviaryLabelInfo.text = "Gryphon Aviary";
	gryphonAviaryLabel = App->gui->CreateUILabel({ 75, 290 }, gryphonAviaryLabelInfo, this, buildingMenu);

	UIButton_Info mageTowerInfo;
	mageTowerInfo.normalTexArea = { 394,202,50,41 };
	mageTowerInfo.hoverTexArea = { 445,202,50,41 };
	mageTowerInfo.pressedTexArea = { 496,202,50,41 };
	mageTowerButton = App->gui->CreateUIButton({ 15, 325 }, mageTowerInfo, this, buildingMenu);

	UILabel_Info mageTowerLabelInfo;
	mageTowerLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	mageTowerLabelInfo.normalColor = White_;
	mageTowerLabelInfo.text = "Mage Tower";
	mageTowerLabel = App->gui->CreateUILabel({ 75, 335 }, mageTowerLabelInfo, this, buildingMenu);

	UIButton_Info scoutTowerInfo;
	scoutTowerInfo.normalTexArea = { 394,34,50,41 };
	scoutTowerInfo.hoverTexArea = { 445,34,50,41 };
	scoutTowerInfo.pressedTexArea = { 496,34,50,41 };
	scoutTowerButton = App->gui->CreateUIButton({ 15, 370 }, scoutTowerInfo, this, buildingMenu);

	UILabel_Info scoutTowerLabelInfo;
	scoutTowerLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	scoutTowerLabelInfo.normalColor = White_;
	scoutTowerLabelInfo.text = "Scout Tower";
	scoutTowerLabel = App->gui->CreateUILabel({ 75, 380 }, scoutTowerLabelInfo, this, buildingMenu);

	UIButton_Info guardTowerInfo;
	guardTowerInfo.normalTexArea = { 394,76,50,41 };
	guardTowerInfo.hoverTexArea = { 445,76,50,41 };
	guardTowerInfo.pressedTexArea = { 496,76,50,41 };
	guardTowerButton = App->gui->CreateUIButton({ 15, 415 }, guardTowerInfo, this, buildingMenu);

	UILabel_Info guardTowerLabelInfo;
	guardTowerLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	guardTowerLabelInfo.normalColor = White_;
	guardTowerLabelInfo.text = "Guard Tower";
	guardTowerLabel = App->gui->CreateUILabel({ 75, 425 }, guardTowerLabelInfo, this, buildingMenu);

	UIButton_Info cannonTowerInfo;
	cannonTowerInfo.normalTexArea = { 394,118,50,41 };
	cannonTowerInfo.hoverTexArea = { 445,118,50,41 };
	cannonTowerInfo.pressedTexArea = { 496,118,50,41 };
	cannonTowerButton = App->gui->CreateUIButton({ 15, 460 }, cannonTowerInfo, this, buildingMenu);

	UILabel_Info cannonTowerLabelInfo;
	cannonTowerLabelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	cannonTowerLabelInfo.normalColor = White_;
	cannonTowerLabelInfo.text = "Cannon Tower";
	cannonTowerLabel = App->gui->CreateUILabel({ 75, 470 }, cannonTowerLabelInfo, this, buildingMenu);
}

void j1Scene::UnLoadBuildingMenu()
{
	App->gui->DestroyElement(buildingMenu);
	App->gui->DestroyElement(chickenFarmButton);
	App->gui->DestroyElement(chickenFarmLabel);
	App->gui->DestroyElement(elvenLumberButton);
	App->gui->DestroyElement(elvenLumberLabel);
	App->gui->DestroyElement(blackSmithButton);
	App->gui->DestroyElement(blackSmithLabel);
	App->gui->DestroyElement(stablesButton);
	App->gui->DestroyElement(stablesLabel);
	App->gui->DestroyElement(churchButton);
	App->gui->DestroyElement(churchLabel);
	App->gui->DestroyElement(gryphonAviaryButton);
	App->gui->DestroyElement(gryphonAviaryLabel);
	App->gui->DestroyElement(mageTowerButton);
	App->gui->DestroyElement(mageTowerLabel);
	App->gui->DestroyElement(scoutTowerButton);
	App->gui->DestroyElement(scoutTowerLabel);
	App->gui->DestroyElement(guardTowerButton);
	App->gui->DestroyElement(guardTowerLabel);
	App->gui->DestroyElement(cannonTowerButton);
	App->gui->DestroyElement(cannonTowerLabel);
	buildingMenuOn = false;

}

void j1Scene::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent)
{
	switch (UIevent)
	{
	case UI_EVENT_MOUSE_ENTER:

		break;

	case UI_EVENT_MOUSE_LEAVE:

		break;

	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (UIelem == buildingButton) {
			if (!buildingMenuOn)
				LoadBuildingMenu();

			else
				UnLoadBuildingMenu();

		}

		break;

	case UI_EVENT_MOUSE_LEFT_UP:

		break;
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

// Save
bool j1Scene::Save(pugi::xml_node& save) const
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
bool j1Scene::Load(pugi::xml_node& save)
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

bool j1Scene::LoadKeys(pugi::xml_node& buttons)
{
	bool ret = true;

	if ((buttonSaveGame = (SDL_Scancode)buttons.attribute("buttonSaveGame").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load SaveGame button");
		ret = false;
	}

	if ((buttonLoadGame = (SDL_Scancode)buttons.attribute("buttonLoadGame").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load SaveGame button");
		ret = false;
	}

	if ((buttonFullScreen = (SDL_Scancode)buttons.attribute("buttonFullScreen").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load FullScreen button");
		ret = false;
	}

	if ((buttonGodMode = (SDL_Scancode)buttons.attribute("buttonGodMode").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load GodMode button");
		ret = false;
	}

	if ((buttonMoveUp = (SDL_Scancode)buttons.attribute("buttonMoveUp").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load MoveUp button");
		ret = false;
	}

	if ((buttonMoveDown = (SDL_Scancode)buttons.attribute("buttonMoveDown").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load MoveDown button");
		ret = false;
	}

	if ((buttonMoveLeft = (SDL_Scancode)buttons.attribute("buttonMoveLeft").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load MoveLeft button");
		ret = false;
	}

	if ((buttonMoveRight = (SDL_Scancode)buttons.attribute("buttonMoveRight").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load MoveRight button");
		ret = false;
	}

	if ((buttonLeaveGame = (SDL_Scancode)buttons.attribute("buttonLeaveGame").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load LeaveGame button");
		ret = false;
	}

	if ((buttonReloadMap = (SDL_Scancode)buttons.attribute("buttonReloadMap").as_int()) == SDL_SCANCODE_UNKNOWN)
	{
		LOG("Could not load ReloadMap button");
		ret = false;
	}
	


	return ret;
}