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
#include "j1Player.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Movement.h"
#include "j1Gui.h"
#include "j1Menu.h"
#include "j1Player.h"
#include "j1Fonts.h"

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

	//Music
	pugi::xml_node audio = config.child("audioPaths");

	mainThemeMusicName = audio.child("mainTheme").attribute("path").as_string();

	LoadKeys(config.child("buttons"));

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
	App->audio->active = true;

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

	if (terenasDialogEvent == TerenasDialog_NONE) {
		terenasDialogTimer.Start();
		terenasDialogEvent = TerenasDialog_START;
		LoadTerenasDialog(terenasDialogEvent);
	}

	//Calculate camera movement in pixels through the percentatge given
	camMovMargin = camMovMargin * ((width + height) / 2) / 100;

	alphaBuilding = EntityType_NONE;
	pauseMenuActions = PauseMenuActions_NOT_EXIST;

	App->audio->PlayMusic(mainThemeMusicName.data(), 2.0f);

	// The camera is in the player base
	App->render->camera.x = -2400;
	App->render->camera.y = -6720;

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
	iPoint size = { App->map->data.tileWidth,App->map->data.tileHeight };
	uint maxLife = 30;
	int currLife = (int)maxLife;

	/// DynamicEntity
	UnitInfo unitInfo;
	unitInfo.damage = 2;
	unitInfo.priority = 1; // TODO: change to 3 or so

						   /// Footman
	//FootmanInfo footmanInfo;

	/// Grunt
	GruntInfo gruntInfo;

	/// Sheep
	CritterSheepInfo critterSheepInfo;
	critterSheepInfo.restoredHealth = 5;

	/// Boar
	CritterBoarInfo critterBoarInfo;
	critterBoarInfo.restoredHealth = 10;

	// Entities creation

	// 1: spawn a Footman with priority 1
	unitInfo.sightRadius = 6;
	unitInfo.attackRadius = 2;
	unitInfo.maxSpeed = 80.0f;

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		// Make sure that there are no entities on the spawn tile and that the tile is walkable
		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->player->FindClosestValidTile(tile);

		// Make sure that the spawn tile is valid
		//if (tile.x != -1 && tile.y != -1) {  // TODO: uncomment this line

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
		//fPoint pos = { (float)tilePos.x,(float)tilePos.y }; // TODO: uncomment this line

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y }; // TODO: delete this debug
		App->entities->AddEntity(EntityType_FOOTMAN, pos, App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, this);
		//}
	}

	// 2: spawn a Grunt with priority 1
	unitInfo.sightRadius = 5;
	unitInfo.attackRadius = 2;
	unitInfo.maxSpeed = 50.0f;

	maxLife = 20;
	currLife = (int)maxLife;

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {

		iPoint tile = { 15,11 };

		// Make sure that there are no entities on the spawn tile and that the tile is walkable
		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->player->FindClosestValidTile(tile);

		// Make sure that the spawn tile is valid
		//if (tile.x != -1 && tile.y != -1) { // TODO: uncomment this line

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
		//fPoint pos = { (float)tilePos.x,(float)tilePos.y }; // TODO: uncomment this line

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y }; // TODO: delete this debug
		App->entities->AddEntity(EntityType_GRUNT, pos, (EntityInfo&)gruntInfo, unitInfo, this);
		//}
	}

	/*
	// 5: spawn a group of Footmans
	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) {

		list<DynamicEntity*> units;
		uint maxFootmans = 4;

		for (uint i = 0; i < maxFootmans; ++i) {

			iPoint tile = { rand() % App->map->data.width,rand() % App->map->data.height };

			// Make sure that there are no entities on the spawn tile and that the tile is walkable
			if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

				tile = FindClosestValidTile(tile);

			// Make sure that the spawn tile is valid
			if (tile.x != -1 && tile.y != -1) {

				iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
				fPoint pos = { (float)tilePos.x,(float)tilePos.y };

				DynamicEntity* dynEnt = (DynamicEntity*)App->entities->AddEntity(EntityType_FOOTMAN, pos, (EntityInfo&)footmanInfo, unitInfo, this);

				if (dynEnt != nullptr)
					units.push_back(dynEnt);
			}
		}

		if (units.size() > 0)
			App->movement->CreateGroupFromUnits(units);
	}

	// 6: spawn a group of Grunts
	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {

		list<DynamicEntity*> units;
		uint maxGrunts = 4;

		for (uint i = 0; i < maxGrunts; ++i) {

			iPoint tile = { rand() % App->map->data.width,rand() % App->map->data.height };

			// Make sure that there are no entities on the spawn tile and that the tile is walkable
			if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

				tile = FindClosestValidTile(tile);

			// Make sure that the spawn tile is valid
			if (tile.x != -1 && tile.y != -1) {

				iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
				fPoint pos = { (float)tilePos.x,(float)tilePos.y };

				DynamicEntity* dynEnt = (DynamicEntity*)App->entities->AddEntity(EntityType_GRUNT, pos, (EntityInfo&)gruntInfo, unitInfo, this);

				if (dynEnt != nullptr)
					units.push_back(dynEnt);
			}
		}

		if (units.size() > 0)
			App->movement->CreateGroupFromUnits(units);
	}
	*/

	fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };

	// 3: spawn a Sheep
	unitInfo.sightRadius = 0;
	unitInfo.attackRadius = 0;
	unitInfo.priority = 1;
	maxLife = 10;
	currLife = (int)maxLife;

	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		App->entities->AddEntity(EntityType_SHEEP, pos, (EntityInfo&)critterSheepInfo, unitInfo, this);

	// 4: spawn a Boar
	unitInfo.sightRadius = 0;
	unitInfo.attackRadius = 0;
	unitInfo.priority = 2;
	maxLife = 20;
	currLife = (int)maxLife;

	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
		App->entities->AddEntity(EntityType_BOAR, pos, (EntityInfo&)critterBoarInfo, unitInfo, this);

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
	App->particles->DrawPaws(); // paws particles
	App->entities->Draw(); // entities
	App->particles->Draw(); // the rest of the particles
	App->gui->Draw(); // gui

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		debugDrawAttack = !debugDrawAttack;

	if (debugDrawAttack)
		App->collision->DebugDraw(); // debug draw collisions

	if (debugDrawMovement)
		App->movement->DebugDraw(); // debug draw movement

	App->render->Blit(debugTex, mouseTilePos.x, mouseTilePos.y); // tile under the mouse pointer
	//App->collision->DebugDraw();

	// Units ---------------------------------------------------------------------------------
	
	// Select units by mouse click
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		startRectangle = mousePos;

		Entity* entity = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY); // TODO Sandra: only player side

		if (entity != nullptr)
			App->entities->SelectEntity(entity);
		//else
			//App->entities->UnselectAllEntities();
	}

	int width = mousePos.x - startRectangle.x;
	int height = mousePos.y - startRectangle.y;

	/// SELECT UNITS
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

		App->entities->SelectEntitiesWithinRectangle(mouseRect, EntityCategory_DYNAMIC_ENTITY); // TODO Sandra: add static entities, only player side
	}

	list<DynamicEntity*> units = App->entities->GetLastUnitsSelected();

	if (units.size() > 0) {

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			App->player->DeleteEntitiesMenu();
			App->player->MakeUnitsMenu(units);
		}

		UnitGroup* group = App->movement->GetGroupByUnits(units);

		if (group == nullptr)

			// Selected units will now behave as a group
			group = App->movement->CreateGroupFromUnits(units);

		if (group != nullptr) {

			/// COMMAND PATROL
			/*
			if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)

				App->entities->CommandToUnits(units, UnitCommand_Patrol);
			*/
			/// STOP UNIT (FROM WHATEVER THEY ARE DOING)
			/*
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)

				App->entities->CommandToUnits(units, UnitCommand_Stop);
			*/
			/// COMMAND ATTACK
			/// Enemy
			// TODO Sandra: ENTITY CATEGORY MUST BE ALSO STATIC ENTITIES (BUILDINGS)
			Entity* target = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);

			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && target != nullptr) {

				// All the group is issued to attack this enemy (and other enemies if seen when arrived at destination)
				list<DynamicEntity*>::const_iterator it = units.begin();

				bool isTarget = true;

				while (it != units.end()) {

					if (!(*it)->SetCurrTarget(target))
						isTarget = false;

					it++;
				}

				if (isTarget)

					App->entities->CommandToUnits(units, UnitCommand_AttackTarget);
			}

			/// Critter
			Entity* critter = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);

			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && critter != nullptr) {

				// All the group is issued to attack this enemy (and other enemies if seen when arrived at destination)
				list<DynamicEntity*>::const_iterator it = units.begin();

				bool isTarget = true;

				while (it != units.end()) {

					if (!(*it)->SetCurrTarget(critter))
						isTarget = false;

					it++;
				}

				if (isTarget)

					App->entities->CommandToUnits(units, UnitCommand_AttackTarget);
			}

			/// SET GOAL (COMMAND MOVE TO POSITION)
			// Draw a shaped goal
			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)

				group->DrawShapedGoal(mouseTile);

			// Set a normal or shaped goal
			if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP) {

				bool isGoal = false;

				if (group->GetShapedGoalSize() <= 1) {

					group->ClearShapedGoal();

					if (group->SetGoal(mouseTile)) /// normal goal

						isGoal = true;
				}
				else if (group->SetShapedGoal()) /// shaped goal

					isGoal = true;

				if (isGoal) {

					uint isPatrol = 0;

					list<DynamicEntity*>::const_iterator it = units.begin();

					while (it != units.end()) {

						if ((*it)->GetUnitCommand() == UnitCommand_Patrol)
							isPatrol++;

						it++;
					}

					/// If all units are in the Patrol command or the AttackTarget command, do not set the MoveToPosition command
					bool isFull = false;

					if (isPatrol == units.size() || target != nullptr || critter != nullptr)
						isFull = true;

					if (!isFull)
						App->entities->CommandToUnits(units, UnitCommand_MoveToPosition);
				}
			}
		}
	}
	
	// ---------------------------------------------------------------------------------

	DebugKeys();
	CheckCameraMovement(dt);

	//Checks if resources have changed to update building menu and gold label
	if (hasGoldChanged) {
		UnLoadResourcesLabels();
		LoadResourcesLabels();
		if (buildingMenuOn) {
			UnLoadBuildingMenu();
			LoadBuildingMenu();
		}
		hasGoldChanged = false;
	}
	if (hasFoodChanged) {
		UnLoadResourcesLabels();
		LoadResourcesLabels();
		hasFoodChanged = false;
	}

	if (terenasDialogTimer.Read() >= 30000 && terenasDialogEvent != TerenasDialog_NONE) {
		terenasDialogEvent = TerenasDialog_NONE;
		UnLoadTerenasDialog();
	}

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

	if (parchmentImg != nullptr)
		if (parchmentImg->GetAnimation()->Finished() && pauseMenuActions == PauseMenuActions_NOT_EXIST)
			pauseMenuActions = PauseMenuActions_CREATED;

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
		if (parchmentImg != nullptr) {
			App->gui->DestroyElement((UIElement**)&parchmentImg);
		}
		DestroyPauseMenu();
		DestroySettingsMenu();
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
		if (parchmentImg != nullptr) {
			App->gui->DestroyElement((UIElement**)&parchmentImg);
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
	//warcraftActive = false;

	// Set to nullptr the pointers to the UI elements
	App->menu->active = true;
	App->map->active = false;
	App->player->active = false;
	App->entities->active = false;
	App->collision->active = false;
	App->pathfinding->active = false;

	App->map->UnLoad();
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
	buttonInfo.normalTexArea = {0, 0, 126, 26};
	buttonInfo.hoverTexArea = { 129, 0, 126, 26 };
	buttonInfo.pressedTexArea = { 257, 0, 126, 26 };
	buildingButton = App->gui->CreateUIButton({ (int)App->render->camera.w - buttonInfo.normalTexArea.w - 15, 0 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.text = "Buildings";
	buildingLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w / 2, 8 }, labelInfo, this, buildingButton);


	//Pause menu 
	pauseMenuButt = App->gui->CreateUIButton({ 5,1 }, buttonInfo, this);

	labelInfo.text = "Menu";
	pauseMenuLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w/2, 8 }, labelInfo, this, pauseMenuButt);


	UIImage_Info entitiesInfo;
	entitiesInfo.texArea = { 0, 565, 371, 82 };
	entitiesStats = App->gui->CreateUIImage({ (int)App->render->camera.w - entitiesInfo.texArea.w,(int)App->render->camera.h - entitiesInfo.texArea.h }, entitiesInfo, this);
	entitiesStats->SetPriorityDraw(PriorityDraw_UIINGAME);

	entitiesInfo.texArea={ 1006,0,800,600 };
	inGameFrameImage = App->gui->CreateUIImage({ 0,0 }, entitiesInfo, this);
	inGameFrameImage->SetPriorityDraw(PriorityDraw_FRAMEWORK);

	LoadResourcesLabels();
}

void j1Scene::LoadBuildingMenu()
{
	UIButton_Info buttonInfo;
	UILabel_Info labelInfo;

	UIImage_Info imageInfo;
	imageInfo.texArea = { 0,33,240,529 };
	buildingMenu = App->gui->CreateUIImage({ -110, 0 }, imageInfo, this, buildingButton);
	buildingMenuOn = true;
	buildingMenu->SetPriorityDraw(PriorityDraw_UIINGAME);

	buttonInfo.normalTexArea = { 241,34,50,41 };
	buttonInfo.hoverTexArea = { 292,34,50,41 };
	buttonInfo.pressedTexArea = { 343,34,50,41 };
	if (App->player->currentGold < chickenFarmCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	chickenFarmButton = App->gui->CreateUIButton({ 15, 55 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Chicken Farm";
	labelInfo.normalColor = White_;
	chickenFarmLabel = App->gui->CreateUILabel({ 75, 65 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Cost: 500 gold";
	if (App->player->currentGold < chickenFarmCost) {
		labelInfo.normalColor = BloodyRed_;
		labelInfo.hoverColor = BloodyRed_;
		labelInfo.pressedColor = BloodyRed_;
	}
	chickenFarmCostLabel = App->gui->CreateUILabel({ 75, 82 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 343,76,50,41 };
	buttonInfo.hoverTexArea = { 343,76,50,41 };
	buttonInfo.pressedTexArea = { 343,76,50,41 };
	if (App->player->currentGold < elvenLumberCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	elvenLumberButton = App->gui->CreateUIButton({ 15, 100 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Elven Lumber Mill";
	labelInfo.normalColor = White_;
	elvenLumberLabel = App->gui->CreateUILabel({ 75, 110}, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	elvenLumberCostLabel = App->gui->CreateUILabel({ 75, 127 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 343,118,50,41 };
	buttonInfo.hoverTexArea = { 343,118,50,41 };
	buttonInfo.pressedTexArea = { 343,118,50,41 };
	if (App->player->currentGold < blacksmithCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	blackSmithButton = App->gui->CreateUIButton({ 15, 145 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Blacksmith";
	labelInfo.normalColor = White_;
	blackSmithLabel = App->gui->CreateUILabel({ 75, 155 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	
	blackSmithCostLabel = App->gui->CreateUILabel({ 75, 172 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 343,160,50,41 };
	buttonInfo.hoverTexArea = { 343,160,50,41 };
	buttonInfo.pressedTexArea = { 343,160,50,41 };
	if (App->player->currentGold < stablesCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	stablesButton = App->gui->CreateUIButton({ 15, 190 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Stables";
	labelInfo.normalColor = White_;
	stablesLabel = App->gui->CreateUILabel({ 75, 200 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	
	stablesCostLabel = App->gui->CreateUILabel({ 75, 217 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 343,202,50,41 };
	buttonInfo.hoverTexArea = { 343,202,50,41 };
	buttonInfo.pressedTexArea = { 343,202,50,41 };
	if (App->player->currentGold < churchCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	churchButton = App->gui->CreateUIButton({ 15, 235 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Church";
	labelInfo.normalColor = White_;
	churchLabel = App->gui->CreateUILabel({ 75, 245 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	
	churchCostLabel = App->gui->CreateUILabel({ 75, 262 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 496,160,50,41 };
	buttonInfo.hoverTexArea = { 496,160,50,41 };
	buttonInfo.pressedTexArea = { 496,160,50,41 };
	if (App->player->currentGold < gryphonAviaryCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	gryphonAviaryButton = App->gui->CreateUIButton({ 15, 280 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Gryphon Aviary";
	labelInfo.normalColor = White_;
	gryphonAviaryLabel = App->gui->CreateUILabel({ 75, 290 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	
	gryphonAviaryCostLabel = App->gui->CreateUILabel({ 75, 307 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 496,202,50,41 };
	buttonInfo.hoverTexArea = { 496,202,50,41 };
	buttonInfo.pressedTexArea = { 496,202,50,41 };
	if (App->player->currentGold < mageTowerCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	mageTowerButton = App->gui->CreateUIButton({ 15, 325 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Mage Tower";
	labelInfo.normalColor = White_;
	mageTowerLabel = App->gui->CreateUILabel({ 75, 335 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Coming Soon...";
	labelInfo.normalColor = BloodyRed_;
	labelInfo.hoverColor = BloodyRed_;
	labelInfo.pressedColor = BloodyRed_;
	
	mageTowerCostLabel = App->gui->CreateUILabel({ 75, 352 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,34,50,41 };
	buttonInfo.hoverTexArea = { 445,34,50,41 };
	buttonInfo.pressedTexArea = { 496,34,50,41 };
	if (App->player->currentGold < scoutTowerCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	scoutTowerButton = App->gui->CreateUIButton({ 15, 370 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Scout Tower";
	labelInfo.normalColor = White_;
	scoutTowerLabel = App->gui->CreateUILabel({ 75, 380 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Cost: 400 gold";
	if (App->player->currentGold < scoutTowerCost) {
		labelInfo.normalColor = BloodyRed_;
		labelInfo.hoverColor = BloodyRed_;
		labelInfo.pressedColor = BloodyRed_;
	}
	scoutTowerCostLabel = App->gui->CreateUILabel({ 75, 397 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,76,50,41 };
	buttonInfo.hoverTexArea = { 445,76,50,41 };
	buttonInfo.pressedTexArea = { 496,76,50,41 };
	if (App->player->currentGold < guardTowerCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	guardTowerButton = App->gui->CreateUIButton({ 15, 415 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Guard Tower";
	labelInfo.normalColor = White_;
	guardTowerLabel = App->gui->CreateUILabel({ 75, 425 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Cost: 600 gold";
	if (App->player->currentGold < guardTowerCost) {
		labelInfo.normalColor = BloodyRed_;
		labelInfo.hoverColor = BloodyRed_;
		labelInfo.pressedColor = BloodyRed_;
	}
	guardTowerCostLabel = App->gui->CreateUILabel({ 75, 442 }, labelInfo, this, buildingMenu);

	buttonInfo.normalTexArea = { 394,118,50,41 };
	buttonInfo.hoverTexArea = { 445,118,50,41 };
	buttonInfo.pressedTexArea = { 496,118,50,41 };
	if (App->player->currentGold < cannonTowerCost) {
		buttonInfo.hoverTexArea = buttonInfo.pressedTexArea;
		buttonInfo.normalTexArea = buttonInfo.pressedTexArea;
	}
	cannonTowerButton = App->gui->CreateUIButton({ 15, 460 }, buttonInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME::FONT_NAME_WARCRAFT;
	labelInfo.text = "Cannon Tower";
	labelInfo.normalColor = White_;
	cannonTowerLabel = App->gui->CreateUILabel({ 75, 470 }, labelInfo, this, buildingMenu);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Cost: 800 gold";
	if (App->player->currentGold < cannonTowerCost) {
		labelInfo.normalColor = BloodyRed_;
		labelInfo.hoverColor = BloodyRed_;
		labelInfo.pressedColor = BloodyRed_;
	}
	cannonTowerCostLabel = App->gui->CreateUILabel({ 75, 487 }, labelInfo, this, buildingMenu);
}

void j1Scene::UnLoadBuildingMenu()
{

	App->gui->DestroyElement((UIElement**)&buildingMenu);
	App->gui->DestroyElement((UIElement**)&chickenFarmButton);
	App->gui->DestroyElement((UIElement**)&chickenFarmLabel);
	App->gui->DestroyElement((UIElement**)&chickenFarmCostLabel);
	App->gui->DestroyElement((UIElement**)&elvenLumberButton);
	App->gui->DestroyElement((UIElement**)&elvenLumberLabel);
	App->gui->DestroyElement((UIElement**)&elvenLumberCostLabel);
	App->gui->DestroyElement((UIElement**)&blackSmithButton);
	App->gui->DestroyElement((UIElement**)&blackSmithLabel);
	App->gui->DestroyElement((UIElement**)&blackSmithCostLabel);
	App->gui->DestroyElement((UIElement**)&stablesButton);
	App->gui->DestroyElement((UIElement**)&stablesLabel);
	App->gui->DestroyElement((UIElement**)&stablesCostLabel);
	App->gui->DestroyElement((UIElement**)&churchButton);
	App->gui->DestroyElement((UIElement**)&churchLabel);
	App->gui->DestroyElement((UIElement**)&churchCostLabel);
	App->gui->DestroyElement((UIElement**)&gryphonAviaryButton);
	App->gui->DestroyElement((UIElement**)&gryphonAviaryLabel);
	App->gui->DestroyElement((UIElement**)&gryphonAviaryCostLabel);
	App->gui->DestroyElement((UIElement**)&mageTowerButton);
	App->gui->DestroyElement((UIElement**)&mageTowerLabel);
	App->gui->DestroyElement((UIElement**)&mageTowerCostLabel);
	App->gui->DestroyElement((UIElement**)&scoutTowerButton);
	App->gui->DestroyElement((UIElement**)&scoutTowerLabel);
	App->gui->DestroyElement((UIElement**)&scoutTowerCostLabel);
	App->gui->DestroyElement((UIElement**)&guardTowerButton);
	App->gui->DestroyElement((UIElement**)&guardTowerLabel);
	App->gui->DestroyElement((UIElement**)&guardTowerCostLabel);
	App->gui->DestroyElement((UIElement**)&cannonTowerButton);
	App->gui->DestroyElement((UIElement**)&cannonTowerLabel);
	App->gui->DestroyElement((UIElement**)&cannonTowerCostLabel);
	buildingMenuOn = false;
}

void j1Scene::LoadResourcesLabels()
{
	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = to_string(App->player->currentGold);
	goldLabel = App->gui->CreateUILabel({ 224, 0 }, labelInfo, this, inGameFrameImage);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;	
	labelInfo.text = to_string(App->player->currentFood);
	foodLabel = App->gui->CreateUILabel({ 334, 0 }, labelInfo, this, inGameFrameImage);
}

void j1Scene::UnLoadResourcesLabels()
{
	App->gui->DestroyElement((UIElement**)&goldLabel);
	App->gui->DestroyElement((UIElement**)&foodLabel);
}

void j1Scene::CreatePauseMenu() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	buttonInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	int x = parchmentImg->GetLocalPos().x + 100;
	int y = parchmentImg->GetLocalPos().y + 110;
	settingsButt = App->gui->CreateUIButton	 ({ x - 10, y }, buttonInfo, this);

	y = parchmentImg->GetLocalPos().y + 60;
	continueButt = App->gui->CreateUIButton	 ({ x - 8, y }, buttonInfo, this);

	y = parchmentImg->GetLocalPos().y + 160;
	buttonInfo.normalTexArea = { 2000, 0, 150, 33 };
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
	labelInfo.text = "Return to Main Menu";
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
		buttonInfo.normalTexArea = buttonInfo.hoverTexArea = { 498, 370, 20, 20 };
		buttonInfo.pressedTexArea = { 520, 370, 20, 20 };
	}
	else {
		buttonInfo.normalTexArea = buttonInfo.hoverTexArea = { 520, 370, 20, 20 };
		buttonInfo.pressedTexArea = { 498, 370, 20, 20 };
	}
	buttonInfo.verticalOrientation = VERTICAL_POS_CENTER;
	buttonInfo.checkbox = true;
	int x = parchmentImg->GetLocalPos().x + 130;
	int y = parchmentImg->GetLocalPos().y + 160;
	fullScreenButt = App->gui->CreateUIButton({ x, y }, buttonInfo, this);

	x -= 100;
	labelInfo.text = "Fullscreen";
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = Black_;
	fullScreenLabel = App->gui->CreateUILabel({ x,y }, labelInfo, this);


	//Sliders
	x = parchmentImg->GetLocalPos().x + 30;
	y = parchmentImg->GetLocalPos().y + 70;
	float relativeVol = (float)App->audio->fxVolume / MAX_AUDIO_VOLUM;
	SDL_Rect butText = { 565, 359 , 8, 10 };
	SDL_Rect bgText = { 434, 359, 130, 10 };
	App->menu->AddSlider(AudioFXPause, {x,y}, "Audio FX", relativeVol, butText, bgText, this);
	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	y += 50;
	App->menu->AddSlider(AudioMusicPause, { x,y }, "Audio Music", relativeVol, butText, bgText, this);

	buttonInfo.checkbox = false;
	buttonInfo.normalTexArea = { 2000, 0, 40, 20 };
	buttonInfo.hoverTexArea = { 0, 0, 0, 0 };
	buttonInfo.pressedTexArea = { 0, 0, 0, 0 };
	x = parchmentImg->GetLocalPos().x + 30;
	y = parchmentImg->GetLocalPos().y + 195;
	returnButt = App->gui->CreateUIButton({ x, y }, buttonInfo, this);

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.pressedColor = White_;
	labelInfo.text = "Back";
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
	if (parchmentImg != nullptr) {
		App->gui->DestroyElement((UIElement**)&parchmentImg);
	}
	DestroyPauseMenu();
	DestroySettingsMenu();
	UnLoadBuildingMenu();
	UnLoadResourcesLabels();
	App->gui->DestroyElement((UIElement**)&pauseMenuButt);
	App->gui->DestroyElement((UIElement**)&pauseMenuLabel);
	App->gui->DestroyElement((UIElement**)&entitiesStats);
	App->gui->DestroyElement((UIElement**)&buildingButton);
	App->gui->DestroyElement((UIElement**)&buildingLabel);
	App->gui->DestroyElement((UIElement**)&inGameFrameImage);
}

PauseMenuActions j1Scene::GetPauseMenuActions()
{
	return pauseMenuActions;
}

void j1Scene::LoadTerenasDialog(TerenasDialogEvents dialogEvent)
{
	UIImage_Info imageInfo;
	UILabel_Info labelInfo;
	if (dialogEvent == TerenasDialog_START) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 340;
		labelInfo.interactive = false;
		labelInfo.text = "Welcome adventurers of Azeroth's armies! You have been sent to Draenor to rescue the members from the legendary Alliance expedition and defeat Ner'zhul to reclaim the artifacts from Azeroth and avoid caos. FOR THE ALLIANCE!";
		terenasAdvices.text = App->gui->CreateUILabel({ 305,37 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_RESCUE_ALLERIA) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 350;
		labelInfo.interactive = false;
		labelInfo.text = "Congratulations! You have freed Alleria. I thank you in the name of Azeroth. For the alliance!";
		terenasAdvices.text = App->gui->CreateUILabel({ 305,37 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_RESCUE_KHADGAR) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 350;
		labelInfo.interactive = false;
		labelInfo.text = "Congratulations! You have freed Khadgar. I thank you in the name of Azeroth. For the alliance!";
		terenasAdvices.text = App->gui->CreateUILabel({ 305,37 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_RESCUE_TURALYON) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 350;
		labelInfo.interactive = false;
		labelInfo.text = "Congratulations! You have freed Turalyon. I thank you in the name of Azeroth. For the alliance!";
		terenasAdvices.text = App->gui->CreateUILabel({ 305,37 }, labelInfo, this);
	}

}

void j1Scene::UnLoadTerenasDialog()
{
	App->gui->DestroyElement((UIElement**)&terenasAdvices.text);
	App->gui->DestroyElement((UIElement**)&terenasAdvices.terenasImage);
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
			if (parchmentImg == nullptr) {
				App->audio->PlayFx(1, 0); //Button sound

				if (!buildingMenuOn)
					LoadBuildingMenu();

				else
					UnLoadBuildingMenu();
			}
		}

		if (UIelem == chickenFarmButton) {
			if (App->player->currentGold >= chickenFarmCost) {
				App->audio->PlayFx(1, 0); //Button sound
				UnLoadBuildingMenu();
				alphaBuilding = EntityType_CHICKEN_FARM;
			}
			else if(App->player->currentGold < chickenFarmCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}
		
		if (UIelem == stablesButton) {
			if (App->player->currentGold >= stablesCost) {
				App->audio->PlayFx(1, 0); //Button sound
				UnLoadBuildingMenu();
				alphaBuilding = EntityType_STABLES;
			}
			else if(App->player->currentGold < stablesCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}
		
		if (UIelem == gryphonAviaryButton) {
			if (App->player->currentGold >= gryphonAviaryCost) {
				App->audio->PlayFx(1, 0); //Button sound
				UnLoadBuildingMenu();
				alphaBuilding = EntityType_GRYPHON_AVIARY;
			}
			else if(App->player->currentGold < gryphonAviaryCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == mageTowerButton) {
			if (App->player->currentGold >= mageTowerCost) {
				App->audio->PlayFx(1, 0); //Button sound
				UnLoadBuildingMenu();
				alphaBuilding = EntityType_MAGE_TOWER;
			}
			else if(App->player->currentGold < mageTowerCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == scoutTowerButton) {
			if (App->player->currentGold >= scoutTowerCost) {
				App->audio->PlayFx(1, 0); //Button sound
				UnLoadBuildingMenu();
				alphaBuilding = EntityType_SCOUT_TOWER;
			}
			else if(App->player->currentGold < scoutTowerCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		else if (UIelem == pauseMenuButt) {
			App->audio->PlayFx(1, 0); //Button sound
			if (parchmentImg == nullptr) {
				UIImage_Info parchmentInfo;
				parchmentInfo.texArea = App->gui->parchmentArea;
				parchmentImg = App->gui->CreateUIImage({ 260, 145 }, parchmentInfo, this);
				parchmentImg->StartAnimation(App->gui->parchmentAnim);
				parchmentImg->SetPriorityDraw(PriorityDraw_PAUSEMENU);
			}
			else {
				pauseMenuActions = PauseMenuActions_DESTROY;
			}
		}

		else if (UIelem == continueButt) {
			App->audio->PlayFx(1, 0); //Button sound
			pauseMenuActions = PauseMenuActions_DESTROY;
		}

		else if (UIelem == ReturnMenuButt) {
			App->audio->PlayFx(1, 0); //Button sound
			pauseMenuActions = PauseMenuActions_RETURN_MENU;
		}

		else if (UIelem == settingsButt) {
			App->audio->PlayFx(1, 0); //Button sound
			pauseMenuActions = PauseMenuActions_SETTINGS_MENU;
		}

		else if (UIelem == returnButt) {
			App->audio->PlayFx(1, 0); //Button sound
			DestroySettingsMenu();
			pauseMenuActions = PauseMenuActions_CREATED;
		}

		else if (UIelem == (UIElement*)AudioFXPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERFX;

		else if (UIelem == (UIElement*)AudioMusicPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERMUSIC;

		else if (UIelem == fullScreenButt)
		{
			App->audio->PlayFx(1, 0); //Button sound
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