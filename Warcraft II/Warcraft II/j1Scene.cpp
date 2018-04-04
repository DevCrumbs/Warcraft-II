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
#include "j1Movement.h"
#include "j1Gui.h"
#include "j1Menu.h"
#include "j1Player.h"

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

	alphaBuilding = EntityType_NONE;
	pauseMenuActions = PauseMenuActions_NOT_EXIST;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	bool ret = true;

	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// ---------------------------------------------------------------------

	// Entities info
	/// Entity
	fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
	iPoint size = { 32,32 };
	uint maxLife = 10;
	int currLife = (int)maxLife;

	/// DynamicEntity
	UnitInfo unitInfo;
	unitInfo.maxSpeed = 50.0f;
	unitInfo.damage = 2;

	/// Footman
	FootmanInfo footmanInfo;

	/// Grunt
	GruntInfo gruntInfo;

	// Entities creation
	if (App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY) == nullptr && App->pathfinding->IsWalkable(mouseTile)) {

		// 1: spawn a Footman with priority 1
		unitInfo.sightRadius = 6;
		unitInfo.attackRadius = 3;

		if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
			App->entities->AddEntity(EntityType_FOOTMAN, pos, (EntityInfo&)footmanInfo, unitInfo);

		// 2: spawn a Grunt with priority 1
		unitInfo.sightRadius = 3;
		unitInfo.attackRadius = 2;

		if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
			App->entities->AddEntity(EntityType_GRUNT, pos, (EntityInfo&)gruntInfo, unitInfo);
	}

	return ret;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	bool ret = true;

	// Save mouse position (world and map coords)
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint mousePos = App->render->ScreenToWorld(x, y);
	iPoint mouseTile = App->map->WorldToMap(mousePos.x, mousePos.y);
	iPoint mouseTilePos = App->map->MapToWorld(mouseTile.x, mouseTile.y);

	// ---------------------------------------------------------------------

	// Draw
	App->map->Draw(); // map
	App->entities->Draw(); // entities
	App->render->Blit(debugTex, mouseTilePos.x, mouseTilePos.y); // tile under the mouse pointer

	// Movement															 // Select units by mouse click
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		startRectangle = mousePos;

		Entity* entity = App->entities->IsEntityOnTile(mouseTile);

		if (entity != nullptr)
			App->entities->SelectEntity(entity);
		else
			App->entities->UnselectAllEntities();
	}

	int width = mousePos.x - startRectangle.x;
	int height = mousePos.y - startRectangle.y;

	// Select units by rectangle drawing
	if (abs(width) >= RECTANGLE_MIN_AREA && abs(height) >= RECTANGLE_MIN_AREA && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {

		// Draw the rectangle
		SDL_Rect mouseRect = { startRectangle.x, startRectangle.y, width, height };
		App->render->DrawQuad(mouseRect, 255, 255, 255, 255, false);

		// Select units within the rectangle
		if (width < 0) {
			mouseRect.x = mousePos.x;
			mouseRect.w *= -1;
		}
		if (height < 0) {
			mouseRect.y = mousePos.y;
			mouseRect.h *= -1;
		}

		App->entities->SelectEntitiesWithinRectangle(mouseRect);
	}

	// Select a new goal for the selected units (single click or drag)
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {

		if (App->movement->GetGroupByUnits(App->entities->GetLastUnitsSelected()) == nullptr)

			// Selected units will now behave as a group
			App->movement->CreateGroupFromUnits(App->entities->GetLastUnitsSelected());

		App->movement->GetGroupByUnits(App->entities->GetLastUnitsSelected())->DrawShapedGoal(mouseTile);
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP) {

		if (App->movement->GetGroupByUnits(App->entities->GetLastUnitsSelected()) == nullptr)

			// Selected units will now behave as a group
			App->movement->CreateGroupFromUnits(App->entities->GetLastUnitsSelected());

		UnitGroup* group = App->movement->GetGroupByUnits(App->entities->GetLastUnitsSelected());

		if (!group->SetShapedGoal())
			group->SetGoal(mouseTile);
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

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {
	}

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) {
	}

	if(parchment != nullptr)
		if (parchment->anim.Finished() && pauseMenuActions == PauseMenuActions_NOT_EXIST) {
			pauseMenuActions = PauseMenuActions_CREATED;
		}

	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	switch (pauseMenuActions)
	{
	case PauseMenuActions_NONE:
		break;
	case PauseMenuActions_CREATED:
		CreatePauseMenu();
		pauseMenuActions = PauseMenuActions_NONE;
		break;
	case PauseMenuActions_DESTROY:
		if (parchment != nullptr) {
			parchment->isDeleted = true;
			parchment = nullptr;
		}
		DestroyPauseMenu();
		pauseMenuActions = PauseMenuActions_NOT_EXIST;
		break;
	case PauseMenuActions_RETURN_MENU:
		pauseMenuActions = PauseMenuActions_NONE;
		App->fade->FadeToBlack(this, App->menu);
			break;
	case PauseMenuActions_SETTINGS_MENU:
		DestroyPauseMenu();
		CreateSettingsMenu();
		pauseMenuActions = PauseMenuActions_NONE;		
		break;
	case PauseMenuActions_SLIDERFX:
		App->menu->UpdateSlider(AudioFXPause);
		break;
	case PauseMenuActions_SLIDERMUSIC:
		App->menu->UpdateSlider(AudioMusicPause);
	default:
		break;
	}
	if (App->input->GetKey(buttonLeaveGame) == KEY_DOWN) {
		ret = false;
		if (parchment != nullptr) {
			parchment->isDeleted = true;
			parchment = nullptr;
		}
	}

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


	DestroyAllUI();


	// Set to nullptr the pointers to the UI elements
	App->menu->active = true;
	App->map->active = false;
	App->player->active = false;
	App->entities->active = false;
	App->collision->active = false;
	App->pathfinding->active = false;

	App->map->CleanUp();
	App->player->CleanUp();
	App->entities->CleanUp();
	App->collision->CleanUp();
	App->pathfinding->CleanUp();

	active = false;

	return ret;
}

// Debug keys
void j1Scene::DebugKeys()
{
	// Movement
	/*
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
		isFrameByFrame = !isFrameByFrame;

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debugDrawMovement = !debugDrawMovement;

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		debugDrawPath = !debugDrawPath;

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		debugDrawMap = !debugDrawMap;
	*/

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
	//Buiding options
	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = {0, 0, 129, 33};
	buttonInfo.hoverTexArea = { 129, 0, 129, 33 };
	buttonInfo.pressedTexArea = { 257, 0, 129, 33 };
	buildingButton = App->gui->CreateUIButton({ (int)App->render->camera.w - buttonInfo.normalTexArea.w, 0 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.text = "Buildings";
	buildingLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w / 2, 12 }, labelInfo, this, buildingButton);


	//Pause menu 
	pauseMenuButt = App->gui->CreateUIButton({ 0,0 }, buttonInfo, this);

	labelInfo.text = "Menu";
	pauseMenuLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w/2, 12 }, labelInfo, this, pauseMenuButt);


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
	UIButton_Info buttonInfo;
	UILabel_Info labelInfo;

	UIImage_Info imageInfo;
	imageInfo.texArea = { 0,33,240,529 };
	buildingMenu = App->gui->CreateUIImage({ -112, 0 }, imageInfo, this, buildingButton);
	buildingMenuOn = true;

	buttonInfo.normalTexArea = { 241,34,50,41 };
	buttonInfo.hoverTexArea = { 292,34,50,41 };
	buttonInfo.pressedTexArea = { 343,34,50,41 };
	chickenFarmButton = App->gui->CreateUIButton({ 15, 55 }, buttonInfo, this, buildingMenu);
	
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.text = "Chicken Farm";
	chickenFarmLabel = App->gui->CreateUILabel({ 75, 65 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 241,76,50,41 };
	buttonInfo.hoverTexArea = { 292,76,50,41 };
	buttonInfo.pressedTexArea = { 343,76,50,41 };
	elvenLumberButton = App->gui->CreateUIButton({ 15, 100 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Elven Lumber Mill";
	elvenLumberLabel = App->gui->CreateUILabel({ 75, 110}, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 241,118,50,41 };
	buttonInfo.hoverTexArea = { 292,118,50,41 };
	buttonInfo.pressedTexArea = { 343,118,50,41 };
	blackSmithButton = App->gui->CreateUIButton({ 15, 145 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Blacksmith";
	blackSmithLabel = App->gui->CreateUILabel({ 75, 155 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 241,160,50,41 };
	buttonInfo.hoverTexArea = { 292,160,50,41 };
	buttonInfo.pressedTexArea = { 343,160,50,41 };
	stablesButton = App->gui->CreateUIButton({ 15, 190 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Stables";
	stablesLabel = App->gui->CreateUILabel({ 75, 200 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 241,202,50,41 };
	buttonInfo.hoverTexArea = { 292,202,50,41 };
	buttonInfo.pressedTexArea = { 343,202,50,41 };
	churchButton = App->gui->CreateUIButton({ 15, 235 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Church";
	churchLabel = App->gui->CreateUILabel({ 75, 245 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,160,50,41 };
	buttonInfo.hoverTexArea = { 445,160,50,41 };
	buttonInfo.pressedTexArea = { 496,160,50,41 };
	gryphonAviaryButton = App->gui->CreateUIButton({ 15, 280 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Gryphon Aviary";
	gryphonAviaryLabel = App->gui->CreateUILabel({ 75, 290 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,202,50,41 };
	buttonInfo.hoverTexArea = { 445,202,50,41 };	
	buttonInfo.pressedTexArea = { 496,202,50,41 };
	mageTowerButton = App->gui->CreateUIButton({ 15, 325 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Mage Tower";
	mageTowerLabel = App->gui->CreateUILabel({ 75, 335 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,34,50,41 };
	buttonInfo.hoverTexArea = { 445,34,50,41 };
	buttonInfo.pressedTexArea = { 496,34,50,41 };
	scoutTowerButton = App->gui->CreateUIButton({ 15, 370 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Scout Tower";
	scoutTowerLabel = App->gui->CreateUILabel({ 75, 380 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,76,50,41 };
	buttonInfo.hoverTexArea = { 445,76,50,41 };
	buttonInfo.pressedTexArea = { 496,76,50,41 };
	guardTowerButton = App->gui->CreateUIButton({ 15, 415 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Guard Tower";
	guardTowerLabel = App->gui->CreateUILabel({ 75, 425 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,118,50,41 };
	buttonInfo.hoverTexArea = { 445,118,50,41 };
	buttonInfo.pressedTexArea = { 496,118,50,41 };
	cannonTowerButton = App->gui->CreateUIButton({ 15, 460 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Cannon Tower";
	cannonTowerLabel = App->gui->CreateUILabel({ 75, 470 }, labelInfo, this, buildingMenu);
}

void j1Scene::UnLoadBuildingMenu()
{
	App->gui->DestroyElement((UIElement**)&buildingMenu);
	App->gui->DestroyElement((UIElement**)&chickenFarmButton);
	App->gui->DestroyElement((UIElement**)&chickenFarmLabel);
	App->gui->DestroyElement((UIElement**)&elvenLumberButton);
	App->gui->DestroyElement((UIElement**)&elvenLumberLabel);
	App->gui->DestroyElement((UIElement**)&blackSmithButton);
	App->gui->DestroyElement((UIElement**)&blackSmithLabel);
	App->gui->DestroyElement((UIElement**)&stablesButton);
	App->gui->DestroyElement((UIElement**)&stablesLabel);
	App->gui->DestroyElement((UIElement**)&churchButton);
	App->gui->DestroyElement((UIElement**)&churchLabel);
	App->gui->DestroyElement((UIElement**)&gryphonAviaryButton);
	App->gui->DestroyElement((UIElement**)&gryphonAviaryLabel);
	App->gui->DestroyElement((UIElement**)&mageTowerButton);
	App->gui->DestroyElement((UIElement**)&mageTowerLabel);
	App->gui->DestroyElement((UIElement**)&scoutTowerButton);
	App->gui->DestroyElement((UIElement**)&scoutTowerLabel);
	App->gui->DestroyElement((UIElement**)&guardTowerButton);
	App->gui->DestroyElement((UIElement**)&guardTowerLabel);
	App->gui->DestroyElement((UIElement**)&cannonTowerButton);
	App->gui->DestroyElement((UIElement**)&cannonTowerLabel);

	buildingMenuOn = false;	 

}

void j1Scene::CreatePauseMenu() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1000, 0, 129, 33 };
	buttonInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	int x = parchment->position.x + 100 + App->render->camera.x;
	int y = parchment->position.y + 60 + App->render->camera.y;
	settingsButt = App->gui->CreateUIButton	 ({ x, y }, buttonInfo, this);

	y = parchment->position.y + 110 + App->render->camera.y;
	continueButt = App->gui->CreateUIButton	 ({ x, y }, buttonInfo, this);

	y = parchment->position.y + 160 + App->render->camera.y;
	ReturnMenuButt = App->gui->CreateUIButton({ x, y}, buttonInfo, this);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "Settings";
	settingsLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2, 12 }, labelInfo, this, settingsButt);

	labelInfo.text = "Resume Game";
	continueLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2, 12 }, labelInfo, this, continueButt);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Return Main Menu";
	ReturnMenuLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2, 12 }, labelInfo, this, ReturnMenuButt);

}

void j1Scene::DestroyPauseMenu() {

	App->gui->DestroyElement((UIElement**)&settingsButt);
	App->gui->DestroyElement((UIElement**)&ReturnMenuButt);
	App->gui->DestroyElement((UIElement**)&continueButt);
	App->gui->DestroyElement((UIElement**)&settingsLabel);
	App->gui->DestroyElement((UIElement**)&continueLabel);
	App->gui->DestroyElement((UIElement**)&ReturnMenuLabel);

}

void j1Scene::CreateSettingsMenu() {
	UIButton_Info buttonInfo;
	UILabel_Info labelInfo;
	
	//Fullscreen
	if (!App->win->fullscreen) {
		buttonInfo.normalTexArea = { 635, 240, 20, 20 };
		//buttonInfo.hoverTexArea = { 129, 0, 20, 20 };
	}
	else {
		buttonInfo.normalTexArea = { 658, 240, 20, 20 };
		//buttonInfo.hoverTexArea = { 129, 0, 20, 20 };
	}
	buttonInfo.verticalOrientation = VERTICAL_POS_CENTER;
	int x = parchment->position.x + 130 + App->render->camera.x;
	int y = parchment->position.y + 150 + App->render->camera.y;
	fullScreenButt = App->gui->CreateUIButton({ x, y }, buttonInfo, this);

	x -= 100;
	labelInfo.text = "Fullscreen";
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = Black_;
	fullScreenLabel = App->gui->CreateUILabel({ x,y }, labelInfo, this);


	//Sliders
	x = parchment->position.x + 30 + App->render->camera.x;
	y = parchment->position.y + 60 + App->render->camera.y;
	float relativeVol = (float)App->audio->fxVolume / MAX_AUDIO_VOLUM;
	SDL_Rect butText = { 703, 224 , 8, 10 };
	SDL_Rect bgText = { 572, 224, 130, 10 };
	App->menu->AddSlider(AudioFXPause, {x,y}, "Audio FX", relativeVol, butText, bgText, this);
	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	y += 50;
	App->menu->AddSlider(AudioMusicPause, { x,y }, "Audio Music", relativeVol, butText, bgText, this);

	buttonInfo.normalTexArea = { 1000, 0, 30, 20 };
	buttonInfo.hoverTexArea = { 0, 0, 0, 0 };
	buttonInfo.pressedTexArea = { 0, 0, 0, 0 };
	x = parchment->position.x + 30 + App->render->camera.x;
	y = parchment->position.y + 185 + App->render->camera.y;
	returnButt = App->gui->CreateUIButton({ x, y }, buttonInfo, this);

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.pressedColor = White_;
	labelInfo.text = "<--";
	returnLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2, 5 }, labelInfo, this, returnButt);
}

void j1Scene::DestroySettingsMenu() {

	App->gui->DestroyElement((UIElement**)&returnButt);
	App->gui->DestroyElement((UIElement**)&returnLabel);
	App->gui->DestroyElement((UIElement**)&fullScreenButt);
	App->gui->DestroyElement((UIElement**)&fullScreenLabel);
	App->gui->DestroyElement((UIElement**)&AudioFXPause.slider);
	App->gui->DestroyElement((UIElement**)&AudioFXPause.name);
	App->gui->DestroyElement((UIElement**)&AudioFXPause.value);
	App->gui->DestroyElement((UIElement**)&AudioMusicPause.slider);
	App->gui->DestroyElement((UIElement**)&AudioMusicPause.name);
	App->gui->DestroyElement((UIElement**)&AudioMusicPause.value);

}

void j1Scene::DestroyAllUI() {
	if (parchment != nullptr) {
		parchment->isDeleted = true;
		parchment = nullptr;
	}
	DestroyPauseMenu();
	DestroySettingsMenu();
	UnLoadBuildingMenu();
	App->gui->DestroyElement((UIElement**)&pauseMenuButt);
	App->gui->DestroyElement((UIElement**)&pauseMenuLabel);
	App->gui->DestroyElement((UIElement**)&entitiesStats);
	App->gui->DestroyElement((UIElement**)&buildingButton);
	App->gui->DestroyElement((UIElement**)&buildingLabel);
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
		else if (UIelem == chickenFarmButton) {
			UnLoadBuildingMenu();
			alphaBuilding = EntityType_CHICKEN_FARM;
		}
		
		else if (UIelem == stablesButton) {
			UnLoadBuildingMenu();
			alphaBuilding = EntityType_STABLES;
		}
		
		else if (UIelem == gryphonAviaryButton) {
			UnLoadBuildingMenu();
			alphaBuilding = EntityType_GRYPHON_AVIARY;
		}

		else if (UIelem == mageTowerButton) {
			UnLoadBuildingMenu();
			alphaBuilding = EntityType_MAGE_TOWER;
		}

		else if (UIelem == scoutTowerButton) {
			UnLoadBuildingMenu();
			alphaBuilding = EntityType_SCOUT_TOWER;
		}

		else if (UIelem == pauseMenuButt) {
			if (parchment == nullptr)
				parchment = App->particles->AddParticle(App->particles->parchmentAnimation, App->render->GetMidCameraPos().x - 100, App->render->GetMidCameraPos().y - 125);
			else {
				pauseMenuActions = PauseMenuActions_NOT_EXIST;
				if (parchment != nullptr) {
					parchment->isDeleted = true;
					parchment = nullptr;
				}
				DestroyPauseMenu();
			}
		}

		else if (UIelem == continueButt) {
			pauseMenuActions = PauseMenuActions_DESTROY;
		}

		else if (UIelem == ReturnMenuButt) {
			pauseMenuActions = PauseMenuActions_RETURN_MENU;
		}

		else if (UIelem == settingsButt) {
			pauseMenuActions = PauseMenuActions_SETTINGS_MENU;
		}

		else if (UIelem == returnButt) {
			DestroySettingsMenu();
			pauseMenuActions = PauseMenuActions_CREATED;
		}

		else if (UIelem == (UIElement*)AudioFXPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERFX;

		else if (UIelem == (UIElement*)AudioMusicPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERMUSIC;

		else if (UIelem == fullScreenButt)
		{
			if (App->win->fullscreen) {
				App->win->fullscreen = false;
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_SHOWN);
				break;
			}
			else {
				App->win->fullscreen = true;
				SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				break;
			}
		}
		break;

	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == (UIElement*)AudioFXPause.slider || UIelem == (UIElement*)AudioMusicPause.slider)
			pauseMenuActions = PauseMenuActions_NONE;
		break;
	}
}

ENTITY_TYPE j1Scene::GetAlphaBuilding() {
	return alphaBuilding;
}

void j1Scene::SetAplphaBuilding(ENTITY_TYPE alphaBuilding) {
	this->alphaBuilding = alphaBuilding;
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