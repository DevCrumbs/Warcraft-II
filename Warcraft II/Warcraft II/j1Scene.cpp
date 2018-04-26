#include <time.h>

#include"Brofiler\Brofiler.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Input.h"
#include "j1Textures.h"
#include "j1FadeToBlack.h"
#include "j1FinishGame.h"
#include "j1Audio.h"
#include "j1Collision.h"
#include "j1Particles.h"
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
#include "j1PathManager.h"
#include "j1Printer.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UICursor.h"
#include "UISlider.h"
#include "UIMinimap.h"


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
	numMaps = maps.child("warcraft").attribute("numMaps").as_int();
	//Music
	pugi::xml_node audio = config.child("audioPaths");

	mainThemeMusicName = audio.child("mainTheme").attribute("path").as_string();

	//LoadKeys(config.child("buttons"));

	//Load camera attributes
	pugi::xml_node camera = config.child("camera");

	camSpeed = camera.attribute("speed").as_float();
	camMovMargin = camera.attribute("movMarginPcnt").as_float();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	active = true;

	App->map->active = true;
	App->player->active = true;
	App->entities->active = true;
	App->collision->active = true;
	App->pathfinding->active = true;
	App->pathmanager->active = true;
	App->movement->active = true;
	App->particles->active = true;

	App->player->Start();
	App->collision->Start();
	App->particles->Start();
	App->entities->Start();

	isStarted = false;

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
		ret = LoadNewMap(0);
	//	ret = App->map->Load("verticalSliceMap.tmx");
		debugTex = App->tex->Load(warcraftTexName.data());
	}

	// Create walkability map
	if (ret)
		App->map->CreateWalkabilityMap(w, h, &data);

	//LoadInGameUI
	LoadInGameUI();

	if (terenasDialogEvent == TerenasDialog_NONE) {
		UnLoadTerenasDialog();
		terenasDialogTimer.Start();
		terenasDialogEvent = TerenasDialog_START;
		LoadTerenasDialog(terenasDialogEvent);
	}

	//Calculate camera movement in pixels through the percentatge given
	if (!isCamMovMarginCharged) {
		camMovMargin = camMovMargin * ((width + height) / 2) / 100;
		isCamMovMarginCharged = true;
	}

	alphaBuilding = EntityType_NONE;
	pauseMenuActions = PauseMenuActions_NOT_EXIST;

	App->audio->PlayMusic(mainThemeMusicName.data(), 2.0f);

	App->map->LoadLogic();

	isStartedFinalTransition = false;

	return ret;
}

bool j1Scene::LoadNewMap(int map) 
{
	bool ret = true;

	if (map == -1) 
	{
		srand(time(NULL));
		map = rand() % numMaps;


		static char path[25];
		sprintf_s(path, 25, "alphaMap%i.tmx", map);

		LOG(path);

		ret = App->map->Load(path);
	}
	else
	{
		static char path[25];
		sprintf_s(path, 25, "alphaMap%i.tmx", map);

		ret = App->map->Load(path);
	}

	if (ret)
	{
		iPoint cameraPos{ 0,0 };
		switch (map)
		{
		case 0:
			cameraPos = App->map->MapToWorld(13, 78);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos.y;

			basePos = App->map->MapToWorld(5, 70);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		case 1:
			cameraPos = App->map->MapToWorld(13, 128);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos	.y;

			basePos = App->map->MapToWorld(5, 120);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		case 2:
			cameraPos = App->map->MapToWorld(63, 78);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos.y;

			basePos = App->map->MapToWorld(55, 70);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		case 3:
			cameraPos = App->map->MapToWorld(63, 78);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos.y;

			basePos = App->map->MapToWorld(55, 70);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		case 4:
			cameraPos = App->map->MapToWorld(13, 128);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos.y;

			basePos = App->map->MapToWorld(5, 120);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		case 5:
			cameraPos = App->map->MapToWorld(13, 78);
			App->render->camera.x = -cameraPos.x;
			App->render->camera.y = -cameraPos.y;

			basePos = App->map->MapToWorld(5, 70);
			App->map->playerBase = { basePos.x, basePos.y, 40 * 32,40 * 32 };
			break;
		default:
			break;
		}
		basePos = cameraPos;
	}

	UIMinimap_Info info;

	info.entityHeight = 32;
	info.entityHeight = 32;
	info.minimapInfo = { 30,31,160,161 };

	minimap = App->gui->CreateUIMinimap(info);
//	minimap->SetMinimap({ 30,31,160,161 }, 32, 32);

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
//	LOG("MouseTile: %i, %i", mouseTile.x, mouseTile.y);

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
	FootmanInfo footmanInfo;
	GruntInfo gruntInfo;
	ElvenArcherInfo elvenArcherInfo;
	TrollAxethrowerInfo trollAxethrowerInfo;

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

	if (isDebug && App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		// Make sure that there are no entities on the spawn tile and that the tile is walkable
		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		// Make sure that the spawn tile is valid
		//if (tile.x != -1 && tile.y != -1) {  // TODO: uncomment this line

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
		//fPoint pos = { (float)tilePos.x,(float)tilePos.y }; // TODO: uncomment this line

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y }; // TODO: delete this debug
		App->entities->AddEntity(EntityType_FOOTMAN, pos, App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, this);
		//}
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		// Make sure that there are no entities on the spawn tile and that the tile is walkable
		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		// Make sure that the spawn tile is valid
		//if (tile.x != -1 && tile.y != -1) {  // TODO: uncomment this line

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
		//fPoint pos = { (float)tilePos.x,(float)tilePos.y }; // TODO: uncomment this line

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y }; // TODO: delete this debug
		App->entities->AddEntity(EntityType_ELVEN_ARCHER, pos, App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER), unitInfo, this);
		//}
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		// Make sure that there are no entities on the spawn tile and that the tile is walkable
		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		// Make sure that the spawn tile is valid
		//if (tile.x != -1 && tile.y != -1) {  // TODO: uncomment this line

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);
		//fPoint pos = { (float)tilePos.x,(float)tilePos.y }; // TODO: uncomment this line

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y }; // TODO: delete this debug
		App->entities->AddEntity(EntityType_GRUNT, pos, App->entities->GetUnitInfo(EntityType_GRUNT), unitInfo, this);
		//}
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
		App->entities->AddEntity(EntityType_TROLL_AXETHROWER, pos, App->entities->GetUnitInfo(EntityType_TROLL_AXETHROWER), unitInfo, App->player);
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
		App->entities->AddEntity(EntityType_SHEEP, pos, App->entities->GetUnitInfo(EntityType_SHEEP), unitInfo, App->player);
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
		App->entities->AddEntity(EntityType_BOAR, pos, App->entities->GetUnitInfo(EntityType_BOAR), unitInfo, App->player);
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
		App->entities->AddEntity(EntityType_ALLERIA, pos, App->entities->GetUnitInfo(EntityType_ALLERIA), unitInfo, App->player);
	}
	if (isDebug && App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) {

		iPoint tile = { 10,10 };

		if (App->entities->IsEntityOnTile(tile) != nullptr || !App->pathfinding->IsWalkable(tile))

			tile = App->movement->FindClosestValidTile(tile);

		iPoint tilePos = App->map->MapToWorld(tile.x, tile.y);

		fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };
		App->entities->AddEntity(EntityType_KHADGAR, pos, App->entities->GetUnitInfo(EntityType_KHADGAR), unitInfo, App->player);
	}

	if (hasGoldChanged) {
		UpdateGoldLabel();
		if (buildingMenu->isActive)
		{
			UpdateLabelsMenu();
			UpdateIconsMenu();
		}
		hasGoldChanged = false;
	}
	if (hasFoodChanged == true) {
		UpdateFoodLabel();
		hasFoodChanged = false;
	}

	switch (pauseMenuActions) 
	{
	case PauseMenuActions_SLIDERFX:
		App->menu->UpdateSlider(AudioFXPause);
		break;
	case PauseMenuActions_SLIDERMUSIC:
		App->menu->UpdateSlider(AudioMusicPause);
		break;
	default:
		break;
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

	units = App->entities->GetLastUnitsSelected();

	if (units.size() > 0) {

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

			if (!CompareSelectedUnitsLists(units)) {

				App->player->DeleteEntitiesMenu();
				App->player->MakeUnitsMenu(units);
			}
		}
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

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		isDebug = !isDebug;
	
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		debugDrawAttack = !debugDrawAttack;

	if (debugDrawAttack)
		App->collision->DebugDraw(); // debug draw collisions

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debugDrawMovement = !debugDrawMovement;

	if (debugDrawMovement)
		App->movement->DebugDraw(); // debug draw movement


	App->render->Blit(debugTex, mouseTilePos.x, mouseTilePos.y); // tile under the mouse pointer

	// Units ---------------------------------------------------------------------------------

	Entity* isEnemyOnTile = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy); // TODO Sandra: only player side
	Entity* isCritterOnTile = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);

	if (isEnemyOnTile != nullptr || isCritterOnTile != nullptr) {
		SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
		if (r.x != 374)
			App->menu->mouseText->SetTexArea({ 374, 527, 28, 33 }, { 402, 527, 28, 33 });
	}
	else {
		SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
		if (r.x != 243)
			if(!App->player->isMouseOnMine)
				App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
	}

	// *****UNITS*****
	/// Units cannot be clicked if a building is being placed
	if (GetAlphaBuilding() == EntityType_NONE) {

		// Select units by mouse click
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			startRectangle = mousePos;

			Entity* entity = App->entities->IsEntityOnTile(mouseTile, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player); // TODO Sandra: only player side

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
			//App->render->DrawQuad(mouseRect, 255, 255, 255, 255, false);
			App->printer->PrintQuad(mouseRect, { 255,255,255,255 });

			// Select units within the rectangle
			if (width < 0) {
				mouseRect.x = mousePos.x;
				mouseRect.w *= -1;
			}
			if (height < 0) {
				mouseRect.y = mousePos.y;
				mouseRect.h *= -1;
			}

			App->entities->SelectEntitiesWithinRectangle(mouseRect, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player); // TODO Sandra: add static entities, only player side
		}

		units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0) {

			UnitGroup* group = App->movement->GetGroupByUnits(units);

			if (group == nullptr)

				// Selected units will now behave as a group
				group = App->movement->CreateGroupFromUnits(units);

			if (group != nullptr) {

				/// COMMAND PATROL
				if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)

					App->entities->CommandToUnits(units, UnitCommand_Patrol);

				/// STOP UNIT (FROM WHATEVER THEY ARE DOING)
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)

					App->entities->CommandToUnits(units, UnitCommand_Stop);

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

				/// Buildings
				Entity* building = App->entities->IsEntityOnTile(mouseTile, EntityCategory_STATIC_ENTITY, EntitySide_Enemy);

				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && building != nullptr) {

					// All the group is issued to attack this enemy (and other enemies if seen when arrived at destination)
					list<DynamicEntity*>::const_iterator it = units.begin();

					bool isTarget = true;

					while (it != units.end()) {

						if (!(*it)->SetCurrTarget(building))
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
	}
	//_*****UNITS*****
	
	// ---------------------------------------------------------------------------------

	DebugKeys();
	CheckCameraMovement(dt);

	//Checks if resources have changed to update building menu and gold label

	if (terenasDialogTimer.Read() >= 25000 && terenasDialogEvent == TerenasDialog_START) {
		terenasDialogEvent = TerenasDialog_NONE;
		UnLoadTerenasDialog();
	}
	if (terenasDialogTimer.Read() >= 5000 && terenasDialogEvent != TerenasDialog_NONE && terenasDialogEvent != TerenasDialog_START) {
		terenasDialogEvent = TerenasDialog_NONE;
		UnLoadTerenasDialog();
	}

	if (App->input->GetKey(buttonReloadMap) == KEY_REPEAT)
	{
		App->map->UnLoad();
		//App->map->CreateNewMap();
	}

	if (parchmentImg != nullptr)
		if (parchmentImg->GetAnimation()->Finished() && pauseMenuActions == PauseMenuActions_NOT_EXIST)
			pauseMenuActions = PauseMenuActions_CREATED;


	switch (pauseMenuActions)
	{
	case PauseMenuActions_NONE:
		break;
	case PauseMenuActions_NOT_EXIST:
		break;
	case PauseMenuActions_CREATED:
		CreatePauseMenu();

		/*if (buildingMenuOn)
		{
			ChangeBuildingMenuState(&buildingMenuButtons);
		}*/

		if (alphaBuilding != EntityType_NONE) {
			alphaBuilding = EntityType_NONE;
			if (alphaBuilding != EntityType_MAX)
			{
				SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
				SDL_SetTextureAlphaMod(App->entities->GetNeutralBuildingTexture(), 255);
			}
		}

		pauseMenuActions = PauseMenuActions_NONE;
		break;
	case PauseMenuActions_DESTROY:
		if (parchmentImg != nullptr) {
			parchmentImg->toRemove = true;
			parchmentImg = nullptr;
		}
		DestroyPauseMenu();
		DestroySettingsMenu();
		pauseMenuActions = PauseMenuActions_NOT_EXIST;
		break;
	case PauseMenuActions_RETURN_MENU:
		pauseMenuActions = PauseMenuActions_NONE;
		isFadeToMenu = true;
		break;
	case PauseMenuActions_SETTINGS_MENU:
		DestroyPauseMenu();
		CreateSettingsMenu();
		pauseMenuActions = PauseMenuActions_NONE;
		break;
	
	default:
		break;
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
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
	return ret;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(buttonLeaveGame) == KEY_DOWN) {

		App->gui->RemoveElem((UIElement**)&parchmentImg);
		return false;
	}

	if (App->player->imagePrisonersVector.size() >= 2) {

		App->player->isWin = true;

		if (!isStartedFinalTransition) {
		
			finalTransition.Start();
			isStartedFinalTransition = true;
		}
	}
	else if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
	
		App->player->isWin = true;
		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}

	// Final transition timer (when win is achieved)
	if (App->player->isWin && finalTransition.ReadSec() >= 5.0f) {

		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}
	
	if (((App->player->currentGold < 400 && App->entities->GetPlayerSoldiers() <= 0 && isStarted) && !App->player->isUnitSpawning) || (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)) {

		App->player->isWin = false;
		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}
	else
		isStarted = true;

	if (isFadeToMenu) {
		App->fade->FadeToBlack(this, App->menu);
		App->menu->active = true;
		isFadeToMenu = false;
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	bool ret = true;

	LOG("Freeing scene");

	App->audio->PauseMusic();
	//App->tex->UnLoad(debugTex);

	// Removes all UI from Scene
	DestroyAllUI();
	// Removes all UI (also life bars, for example)
	App->gui->DestroyAllUI();
	//warcraftActive = false;

	// Set to nullptr the pointers to the UI elements
	active = false;
	App->map->active = false;
	App->player->active = false;
	App->entities->active = false;
	App->collision->active = false;
	App->pathfinding->active = false;
	App->pathmanager->active = false;
	App->movement->active = false;
	App->particles->active = false;

	App->map->UnLoad();
	App->player->CleanUp();
	App->particles->CleanUp();
	App->entities->CleanUp();
	App->movement->CleanUp();
	App->pathmanager->CleanUp();
	App->pathfinding->CleanUp();
	App->collision->CleanUp();

	RELEASE_ARRAY(data);

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
	//if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		/*
		if (index == 0)
			App->entities->playerData->position = App->entities->playerData->startPos;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	//}

	// F2: start from the beginning of the current level
	//if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		/*
		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	//}

	// F4: change between maps
	//if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		/*
		if (index == 0)
			index = 1;
		else
			index = 0;

		App->fade->FadeToBlack(this, this, FADE_LESS_SECONDS, FADE_TYPE::FADE_TYPE_SLIDE);
		*/
	//}

	// F5: save the current state
	//if (App->input->GetKey(buttonSaveGame) == KEY_DOWN) {
	//	App->SaveGame();
	//}

	// F6: load the previous state
	//if (App->input->GetKey(buttonLoadGame) == KEY_DOWN) {
	//	App->LoadGame();
	//}

	// F7: fullscreen
	if (App->input->GetKey(buttonFullScreen) == KEY_DOWN)
		App->win->SetFullscreen();

	// F10: God mode
	//if (App->input->GetKey(buttonGodMode) == KEY_DOWN)
		//god = !god;

	// 1, 2, 3: camera blit
	/*
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && App->map->blitOffset < 15 && App->map->cameraBlit)
		App->map->blitOffset += 7;

	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && App->map->blitOffset > -135 && App->map->cameraBlit)
		App->map->blitOffset -= 7;

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		App->map->cameraBlit = !App->map->cameraBlit;
		*/

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		App->render->camera.x = -basePos.x;
		App->render->camera.y = -basePos.y;
	}

	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		ChangeBuildingMenuState(&buildingMenuButtons);
	}

}

void j1Scene::CheckCameraMovement(float dt) {

	mouse = App->player->GetMousePos();
	int downMargin = -(App->map->data.height * App->map->data.tileHeight) + height / scale;
	int rightMargin = -(App->map->data.width * App->map->data.tileWidth) + width / scale;

	//NOT MOVING WITH App->input->GetKey(buttonMoveUp) == KEY_REPEAT
	//Move with arrows
	//UP
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	//DOWN
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	//LEFT
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	//RIGHT
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && App->render->camera.x >= rightMargin)
		App->render->camera.x -= camSpeed * dt;

	//Move with mouse
	////UP
	if (mouse.y <= (camMovMargin - App->render->camera.y) /scale && App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	////DOWN
	if (mouse.y >= (height - (camMovMargin + 30) - App->render->camera.y) / scale && App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	////LEFT
	if (mouse.x <= (camMovMargin - App->render->camera.x) / scale && App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	////RIGHT
	if (mouse.x >= (width - (camMovMargin + 30) - App->render->camera.x) / scale && App->render->camera.x >= rightMargin)
		App->render->camera.x -= camSpeed * dt;

}

void j1Scene::LoadInGameUI()
{
	//Buiding options
	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = {0, 0, 126, 26};
	buttonInfo.hoverTexArea = { 129, 0, 126, 26 };
	buttonInfo.pressedTexArea = { 257, 0, 126, 26 };
	buildingButton = App->gui->CreateUIButton({ (int)App->render->camera.w - buttonInfo.normalTexArea.w - 15, 0 }, buttonInfo, this);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.text = "Buildings";
	labelInfo.interactive = false;
	buildingLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w / 2, 8 }, labelInfo, this, buildingButton);

	//Pause menu 
	pauseMenuButt = App->gui->CreateUIButton({ 5,1 }, buttonInfo, this);

	labelInfo.text = "Menu";
	pauseMenuLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w/2, 8 }, labelInfo, this, pauseMenuButt);


	UIImage_Info entitiesInfo;
	entitiesInfo.draggable = false;
	entitiesInfo.texArea = { 0, 565, 371, 82 };
	entitiesStats = App->gui->CreateUIImage({ (int)App->render->camera.w - entitiesInfo.texArea.w,(int)App->render->camera.h - entitiesInfo.texArea.h }, entitiesInfo, this);
	entitiesStats->SetPriorityDraw(PriorityDraw_UIINGAME);

	entitiesInfo.texArea={ 1006,0,800,600 };
	inGameFrameImage = App->gui->CreateUIImage({ 0,0 }, entitiesInfo, this);
	inGameFrameImage->SetPriorityDraw(PriorityDraw_FRAMEWORK);

	LoadResourcesLabels();
	LoadBuildingMenu();
}

void j1Scene::ChangeBuildingButtState(MenuBuildingButton* elem)
{
	elem->cost->isActive = !elem->cost->isActive;
	elem->icon->isActive = !elem->icon->isActive;
	elem->name->isActive = !elem->name->isActive;
}
void j1Scene::ChangeBuildingMenuState(BuildingMenu * elem)
{
	buildingMenu->isActive = !buildingMenu->isActive;
	ChangeBuildingButtState(&elem->cannonTower);
	ChangeBuildingButtState(&elem->chickenFarm);
	ChangeBuildingButtState(&elem->gryphonAviary);
	ChangeBuildingButtState(&elem->guardTower);
	ChangeBuildingButtState(&elem->mageTower);
	ChangeBuildingButtState(&elem->scoutTower);
	ChangeBuildingButtState(&elem->stables);
	if (buildingMenu->isActive)
	{
		UpdateLabelsMenu();
		UpdateIconsMenu();
	}
}
void j1Scene::UpdateIconsMenu()
{
	ChangeMenuIconsText(buildingMenuButtons.chickenFarm.icon, chickenFarmCost, { 241,34,50,41 }, { 292,34,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.cannonTower.icon, cannonTowerCost, { 394,118,50,41 }, { 445,118,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.gryphonAviary.icon, gryphonAviaryCost, { 496,160,50,41 }, { 496,160,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.guardTower.icon, guardTowerCost, { 394,76,50,41 }, { 445,76,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.mageTower.icon, mageTowerCost, { 496,202,50,41 }, { 496,202,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.stables.icon, stablesCost, { 343,160,50,41 }, { 343,160,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.scoutTower.icon, scoutTowerCost, { 394,34,50,41 }, { 445,34,50,41 });


}
void j1Scene::ChangeMenuIconsText(UIButton * butt, int cost, SDL_Rect normalText, SDL_Rect hoverText)
{
	if (App->player->currentGold >= cost)
		butt->ChangesTextsAreas(true, normalText, hoverText);
	else {
		butt->ChangesTextsAreas(false);
	}
}
void j1Scene::UpdateLabelsMenu()
{
	ChangeMenuLabelColor(buildingMenuButtons.cannonTower.cost, cannonTowerCost);
	ChangeMenuLabelColor(buildingMenuButtons.chickenFarm.cost, chickenFarmCost);
	ChangeMenuLabelColor(buildingMenuButtons.gryphonAviary.cost, gryphonAviaryCost);
	ChangeMenuLabelColor(buildingMenuButtons.guardTower.cost, guardTowerCost);
	ChangeMenuLabelColor(buildingMenuButtons.mageTower.cost, mageTowerCost);
	ChangeMenuLabelColor(buildingMenuButtons.stables.cost, stablesCost);
	ChangeMenuLabelColor(buildingMenuButtons.scoutTower.cost, scoutTowerCost);
}


void j1Scene::ChangeMenuLabelColor(UILabel * Label, int cost)
{
	if (App->player->currentGold >= cost)
		Label->SetColor(White_, true);
	else
		Label->SetColor(BloodyRed_, true);

}

void j1Scene::LoadBuildingMenu()
{
	UIImage_Info imageInfo;
	imageInfo.draggable = false;
	imageInfo.texArea = { 0,33,240,529 };
	imageInfo.horizontalOrientation = HORIZONTAL_POS_RIGHT;
	buildingMenu = App->gui->CreateUIImage({ (int)App->win->width, 0 }, imageInfo, this, nullptr);
	buildingMenuOn = true;
	buildingMenu->SetPriorityDraw(PriorityDraw_UNDER_FRAMEWORK);
	buildingMenu->isActive = false;

	if (buildingMenu->type != UIE_TYPE_NO_TYPE)
	{

		CreateBuildingElements({ 343,34,50,41 }, { 585, 55 }, "Chicken Farm",
			"Cost: 250 gold", { 645, 65 }, { 645, 82 }, chickenFarmCost, &buildingMenuButtons.chickenFarm);

		CreateBuildingElements( { 343,160,50,41 }, { 585, 100 }, "Stables",
			"Cost: 900 gold", { 645, 110 }, { 645, 127 }, stablesCost, &buildingMenuButtons.stables);

		CreateBuildingElements( { 496,160,50,41 }, { 585, 145 }, "Gryphon Aviary",
			"Cost: 400 gold", { 645, 155 }, { 645, 172 }, gryphonAviaryCost, &buildingMenuButtons.gryphonAviary);

		CreateBuildingElements( { 496,202,50,41 }, { 585, 190 }, "Mage Tower",
			"Cost: 1000 gold", { 645, 200 }, { 645, 217 }, mageTowerCost, &buildingMenuButtons.mageTower);

		CreateBuildingElements( { 496,34,50,41 }, { 585, 235 }, "Scout Tower",
			"Cost: 400 gold", { 645, 245 }, { 645, 262 }, scoutTowerCost, &buildingMenuButtons.scoutTower);

		CreateBuildingElements({ 496,76,50,41 }, { 585, 280 }, "Guard Tower",
			"Cost: 600 gold", { 645, 290 }, { 645, 307 }, guardTowerCost, &buildingMenuButtons.guardTower);

		CreateBuildingElements({ 496,118,50,41 }, { 585, 325 }, "Cannon Tower",
			"Cost: 600 gold", { 645, 335 }, { 645, 352 }, cannonTowerCost, &buildingMenuButtons.cannonTower);
	}
	else
	{
		LOG("WTF");
	}
}

void j1Scene::CreateBuildingElements(SDL_Rect TexArea, iPoint buttonPos, string buildingName, string buildingCost,  
									 iPoint namePos, iPoint costPos, int cost, MenuBuildingButton* elem)
{
	UIButton_Info buttonInfo;
	UILabel_Info labelInfo;

	buttonInfo.normalTexArea = buttonInfo.hoverTexArea = buttonInfo.pressedTexArea = TexArea;

	elem->icon = App->gui->CreateUIButton(buttonPos, buttonInfo, this);
	elem->icon->isActive = false;

	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.text = buildingName;
	labelInfo.normalColor = White_;

	elem->name = App->gui->CreateUILabel(namePos, labelInfo, this);
	elem->name->isActive = false;

	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = buildingCost;
	labelInfo.normalColor = BloodyRed_;

	elem->cost = App->gui->CreateUILabel(costPos, labelInfo, this);
	elem->cost->isActive = false;
}

void j1Scene::DeleteBuildingElements(MenuBuildingButton* elem)
{
	App->gui->RemoveElem((UIElement**)&elem->icon);
	App->gui->RemoveElem((UIElement**)&elem->name);
	App->gui->RemoveElem((UIElement**)&elem->cost);
}

void j1Scene::UnLoadBuildingMenu()
{	
	App->gui->RemoveElem((UIElement**)&buildingMenu);
	DeleteBuildingElements(&buildingMenuButtons.chickenFarm);
	DeleteBuildingElements(&buildingMenuButtons.stables);
	DeleteBuildingElements(&buildingMenuButtons.gryphonAviary);
	DeleteBuildingElements(&buildingMenuButtons.mageTower);
	DeleteBuildingElements(&buildingMenuButtons.scoutTower);
	DeleteBuildingElements(&buildingMenuButtons.guardTower);
	DeleteBuildingElements(&buildingMenuButtons.cannonTower);

	buildingMenuOn = false;
}

void j1Scene::LoadResourcesLabels()
{
	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = to_string(App->player->currentGold);
	goldLabel = App->gui->CreateUILabel({ 224, 0 }, labelInfo, this, inGameFrameImage);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;	
	labelInfo.text = to_string(App->player->currentFood);
	foodLabel = App->gui->CreateUILabel({ 334, 0 }, labelInfo, this, inGameFrameImage);
}

void j1Scene::UpdateGoldLabel()
{
	goldLabel->SetText(to_string(App->player->currentGold));
}
void j1Scene::UpdateFoodLabel()
{
	foodLabel->SetText(to_string(App->player->currentFood));
}
void j1Scene::UnLoadResourcesLabels()
{
	App->gui->RemoveElem((UIElement**)&goldLabel);
	App->gui->RemoveElem((UIElement**)&foodLabel);
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

void j1Scene::DestroyPauseMenu() 
{
	App->gui->RemoveElem((UIElement**)&settingsButt);
	App->gui->RemoveElem((UIElement**)&ReturnMenuButt);
	App->gui->RemoveElem((UIElement**)&continueButt);
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	App->gui->RemoveElem((UIElement**)&continueLabel);
	App->gui->RemoveElem((UIElement**)&ReturnMenuLabel);
}

void j1Scene::CreateSettingsMenu() 
{
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

void j1Scene::DestroySettingsMenu()
{
	App->gui->RemoveElem((UIElement**)&returnButt);
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenButt);
	App->gui->RemoveElem((UIElement**)&fullScreenLabel);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.slider);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.name);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.value);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.slider);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.name);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.value);
}

void j1Scene::DestroyAllUI()
{
	App->gui->RemoveElem((UIElement**)&parchmentImg);

	DestroyPauseMenu();
	DestroySettingsMenu();
	UnLoadBuildingMenu();
	UnLoadResourcesLabels();
	UnLoadTerenasDialog();

	App->gui->RemoveElem((UIElement**)&pauseMenuLabel);
	App->gui->RemoveElem((UIElement**)&pauseMenuButt);
	App->gui->RemoveElem((UIElement**)&buildingLabel);
	App->gui->RemoveElem((UIElement**)&buildingButton);
	App->gui->RemoveElem((UIElement**)&entitiesStats);
	App->gui->RemoveElem((UIElement**)&inGameFrameImage);
	App->gui->RemoveElem((UIElement**)&minimap);
}

PauseMenuActions j1Scene::GetPauseMenuActions()
{
	return pauseMenuActions;
}

bool j1Scene::CompareSelectedUnitsLists(list<DynamicEntity*> units)
{
	bool ret = false;

	// 1. Only 1 unit
	if (units.size() == 1 && App->player->entitySelectedStats.entitySelected != nullptr) {

		if (units.front() == App->player->entitySelectedStats.entitySelected)
			ret = true;
	}
	// 2. More than just 1 unit
	else if (units.size() == App->player->groupElementsList.size()) {

		for (list<DynamicEntity*>::iterator unitsIterator = units.begin(); unitsIterator != units.end(); ++unitsIterator)
		{
			for (list<GroupSelectedElements*>::iterator playerIterator = App->player->groupElementsList.begin(); playerIterator != App->player->groupElementsList.end(); ++playerIterator)
			{
				if (*unitsIterator == (*playerIterator)->owner)
				{
					ret = true;
					break;
				}
				else
					ret = false;
			}

			if (!ret)
				break;
		}
	}

	return ret;
}

void j1Scene::LoadTerenasDialog(TerenasDialogEvents dialogEvent)
{
	UIImage_Info imageInfo;
	imageInfo.texArea = {734,34,70,100};
	terenasAdvices.terenasImage = App->gui->CreateUIImage({ 695,32 }, imageInfo, this);
	UILabel_Info labelInfo;
	if (dialogEvent == TerenasDialog_START) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 340;
		labelInfo.interactive = false;
		labelInfo.text = "Welcome adventurers of Azeroth's armies! You have been sent to Draenor to rescue the members from the legendary Alliance expedition and defeat Ner'zhul to reclaim the artifacts from Azeroth and avoid caos. FOR THE ALLIANCE!";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,47 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_RESCUE_ALLERIA) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 350;
		labelInfo.interactive = false;
		labelInfo.text = "Congratulations! You have freed Alleria. I thank you in the name of Azeroth. For the alliance!";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,37 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_RESCUE_KHADGAR) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 350;
		labelInfo.interactive = false;
		labelInfo.text = "Congratulations! You have freed Khadgar. I thank you in the name of Azeroth. For the alliance!";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,37 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_GOLD_MINE) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 320;
		labelInfo.interactive = false;
		labelInfo.text = "To get gold from the mine you have to select units and they will gather it.";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,47 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_FOOD) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 320;
		labelInfo.interactive = false;
		labelInfo.text = "To produce units you need to have enough food to feed them. Build more farms.";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,47 }, labelInfo, this);
	}
	else if (dialogEvent == TerenasDialog_GOLD) {
		labelInfo.fontName = FONT_NAME_WARCRAFT14;
		labelInfo.textWrapLength = 320;
		labelInfo.interactive = false;
		labelInfo.text = "To produce units you need to have enough gold. Get more from mines.";
		terenasAdvices.text = App->gui->CreateUILabel({ 355,47 }, labelInfo, this);
	}

}

void j1Scene::UnLoadTerenasDialog()
{
	App->gui->RemoveElem((UIElement**)&terenasAdvices.text);
	App->gui->RemoveElem((UIElement**)&terenasAdvices.terenasImage);
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
				UnLoadTerenasDialog();
				ChangeBuildingMenuState(&buildingMenuButtons);
			}
		}

		if (UIelem == buildingMenuButtons.chickenFarm.icon) {
			if (App->player->currentGold >= chickenFarmCost) {
				App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				alphaBuilding = EntityType_CHICKEN_FARM;
			}
			else if(App->player->currentGold < chickenFarmCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}
		
		if (UIelem == buildingMenuButtons.stables.icon) {
			if (App->player->currentGold >= stablesCost) {
				//App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				//alphaBuilding = EntityType_STABLES;
				App->audio->PlayFx(3, 0); //Button error sound
			}
			else if(App->player->currentGold < stablesCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}
		
		if (UIelem == buildingMenuButtons.gryphonAviary.icon) {
			if (App->player->currentGold >= gryphonAviaryCost) {
				//App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				//alphaBuilding = EntityType_GRYPHON_AVIARY;
				App->audio->PlayFx(3, 0); //Button error sound
			}
			else if(App->player->currentGold < gryphonAviaryCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == buildingMenuButtons.mageTower.icon) {
			if (App->player->currentGold >= mageTowerCost) {
				//App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				//alphaBuilding = EntityType_MAGE_TOWER;
				App->audio->PlayFx(3, 0); //Button error sound
			}
			else if(App->player->currentGold < mageTowerCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == buildingMenuButtons.scoutTower.icon) {
			if (App->player->currentGold >= scoutTowerCost) {
				App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				alphaBuilding = EntityType_SCOUT_TOWER;
			}
			else if(App->player->currentGold < scoutTowerCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == buildingMenuButtons.guardTower.icon) {
			if (App->player->currentGold >= guardTowerCost) {
				App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				alphaBuilding = EntityType_PLAYER_GUARD_TOWER;
			}
			else if (App->player->currentGold < guardTowerCost)
				App->audio->PlayFx(3, 0); //Button error sound
		}

		if (UIelem == buildingMenuButtons.cannonTower.icon) {
			if (App->player->currentGold >= cannonTowerCost) {
				App->audio->PlayFx(1, 0); //Button sound
				ChangeBuildingMenuState(&buildingMenuButtons);
				alphaBuilding = EntityType_PLAYER_CANNON_TOWER;
			}
			else if (App->player->currentGold < cannonTowerCost)
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