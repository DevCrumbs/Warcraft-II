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
#include "j1EnemyWave.h"
#include "j1FogOfWar.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UICursor.h"
#include "UISlider.h"
#include "UILifeBar.h"


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

	levelTheme1 = audio.child("levelTheme1").attribute("path").as_string();
	levelTheme2 = audio.child("levelTheme2").attribute("path").as_string();
	levelTheme3 = audio.child("levelTheme3").attribute("path").as_string();
	levelTheme4 = audio.child("levelTheme4").attribute("path").as_string();

	LoadKeys(config.child("buttons"));

	//Load camera attributes
	pugi::xml_node camera = config.child("camera");

	camSpeed = camera.attribute("speed").as_float();
	camMovement = camera.attribute("movement").as_int();
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
	App->fow->active = true;
	App->wave->active = true;

	App->player->Start();
	App->collision->Start();
	App->particles->Start();
	App->entities->Start();
	App->wave->Start();
	App->fow->Start();

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
		ret = LoadNewMap(mapDifficulty);
	//	ret = App->map->Load("verticalSliceMap.tmx");
		debugTex = App->tex->Load(warcraftTexName.data());
	}

	// Load FoW map
	App->fow->LoadFoW();

	// Create walkability map
	if (ret)
		App->map->CreateWalkabilityMap(w, h, &data);

	//LoadInGameUI
	LoadInGameUI();

	if (terenasDialogEvent == TerenasDialog_NONE) {
		terenasDialogTimer.Start();
		terenasDialogEvent = TerenasDialog_START;
		ShowTerenasDialog(terenasDialogEvent);
	}
	
	//Calculate camera movement in pixels through the percentatge given
	if (!isCamMovMarginCharged) {
		camMovMargin = camMovMargin * ((width + height) / 2) / 100;
		isCamMovMarginCharged = true;
	}

	alphaBuilding = EntityType_NONE;
	pauseMenuActions = PauseMenuActions_NOT_EXIST;

	////Play music
	string musicToPlay;
	musicToPlay = ChooseMusicToPlay();
	App->audio->PlayMusic(musicToPlay.data(), 2.0f);

	App->map->LoadLogic();

	isStartedFinalTransition = false;

	// Create the groups of the enemies
	list<list<Entity*>>::const_iterator enIt = App->map->entityGroups.begin();

	while (enIt != App->map->entityGroups.end()) {

		list<Entity*>::const_iterator groupIt = (*enIt).begin();
		list<DynamicEntity*> units;

		while (groupIt != (*enIt).end()) {

			units.push_back((DynamicEntity*)(*groupIt));
			groupIt++;
		}

		if (units.size() > 0)
			App->movement->CreateGroupFromUnits(units);

		enIt++;
	}

	App->isDebug = false;

	return ret;
}

bool j1Scene::LoadNewMap(int map) 
{
	bool ret = true;

	if (map == -1) 
	{
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

	// Entities creation
	UnitInfo unitInfo;
	fPoint pos = { (float)mouseTilePos.x,(float)mouseTilePos.y };

	if (App->isDebug)
	{
		// SDL_SCANCODE_KP_1
		if (App->input->GetKey(buttonSpawnFootman) == KEY_DOWN)
			App->entities->AddEntity(EntityType_FOOTMAN, pos, App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, App->player);
		
		// SDL_SCANCODE_KP_2
		else if (App->input->GetKey(buttonSpawnArcher) == KEY_DOWN)
			App->entities->AddEntity(EntityType_ELVEN_ARCHER, pos, App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER), unitInfo, App->player);

		// SDL_SCANCODE_KP_3
		else if (App->input->GetKey(buttonSpawnGryphon) == KEY_DOWN)
			App->entities->AddEntity(EntityType_GRYPHON_RIDER, pos, App->entities->GetUnitInfo(EntityType_GRYPHON_RIDER), unitInfo, App->player);

		// SDL_SCANCODE_KP_4
		else if (App->input->GetKey(buttonSpawnGrunt) == KEY_DOWN)
			App->entities->AddEntity(EntityType_GRUNT, pos, App->entities->GetUnitInfo(EntityType_GRUNT), unitInfo, App->player);

		// SDL_SCANCODE_KP_5
		else if (App->input->GetKey(buttonSpawnTroll) == KEY_DOWN)
			App->entities->AddEntity(EntityType_TROLL_AXETHROWER, pos, App->entities->GetUnitInfo(EntityType_TROLL_AXETHROWER), unitInfo, App->player);

		// SDL_SCANCODE_KP_6
		else if (App->input->GetKey(buttonSpawnDragon) == KEY_DOWN)
			App->entities->AddEntity(EntityType_DRAGON, pos, App->entities->GetUnitInfo(EntityType_DRAGON), unitInfo, App->player);

		// SDL_SCANCODE_KP_7
		else if (App->input->GetKey(buttonSpawnSheep) == KEY_DOWN)
			App->entities->AddEntity(EntityType_SHEEP, pos, App->entities->GetUnitInfo(EntityType_SHEEP), unitInfo, App->player);

		// SDL_SCANCODE_KP_8
		else if (App->input->GetKey(buttonSpawnBoar) == KEY_DOWN)
			App->entities->AddEntity(EntityType_BOAR, pos, App->entities->GetUnitInfo(EntityType_BOAR), unitInfo, App->player);

		// SDL_SCANCODE_KP_9
		else if (App->input->GetKey(buttonSpawnAlleria) == KEY_DOWN)
			App->entities->AddEntity(EntityType_ALLERIA, pos, App->entities->GetUnitInfo(EntityType_ALLERIA), unitInfo, App->player);

		// SDL_SCANCODE_KP_0
		else if (App->input->GetKey(buttonSpawnTauralyon) == KEY_DOWN)
			App->entities->AddEntity(EntityType_TURALYON, pos, App->entities->GetUnitInfo(EntityType_TURALYON), unitInfo, App->player);
	}
	//_Entities_creation

	if (hasGoldChanged != GoldChange_NoChange && hasGoldChanged != GoldChange_ChangeColor) {
		UpdateGoldLabel(hasGoldChanged);
		if (buildingMenu->isActive)
		{
			UpdateLabelsMenu();
			UpdateIconsMenu();
		}
		hasGoldChanged = GoldChange_ChangeColor;
	}
	if (hasFoodChanged == true) {
		UpdateFoodLabel();
		hasFoodChanged = false;
	}

	// Change to wite Gold Label Color before 2 sec
	SDL_Color white = { 255,255,255,255 };
	if (goldLabelColorTime.Read() > 1200 && hasGoldChanged == GoldChange_ChangeColor) {
		goldLabel->SetColor(White_, true);
		hasGoldChanged = GoldChange_NoChange;
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

	// SDL_SCANCODE_F8
	if (App->input->GetKey(buttonTogleDebug) == KEY_DOWN)
		App->isDebug = !App->isDebug;
	
	// SDL_SCANCODE_F5
	if (App->input->GetKey(buttonTogleDebugAttack) == KEY_DOWN && App->isDebug)
		debugDrawAttack = !debugDrawAttack;

	if (debugDrawAttack)
		App->collision->DebugDraw(); // debug draw collisions

	// SDL_SCANCODE_F9
	if (App->input->GetKey(buttonTogleDebugMovement) == KEY_DOWN && App->isDebug)
		debugDrawMovement = !debugDrawMovement;

	if (debugDrawMovement)
		App->movement->DebugDraw(); // debug draw movement

	if (App->pathfinding->IsWalkable(mouseTile) && App->isDebug)
		App->printer->PrintSprite(mouseTilePos, debugTex, { 0,0,32,32 }); // tile under the mouse pointer

	// Units ---------------------------------------------------------------------------------

	// Update units selected life bars
	for (list<GroupSelectedElements>::iterator iterator = groupElementsList.begin(); iterator != groupElementsList.end(); ++iterator) {

		if ((*iterator).owner != nullptr) {
			if ((*iterator).owner->GetCurrLife() <= 0) {
				ShowSelectedUnits(units);
			}
			else
				(*iterator).entityLifeBar->SetLife((*iterator).owner->GetCurrLife());
		}
	}

	if (App->gui->IsMouseOnUI()) {

		SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
		if (r.x != 243)
			App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
	}

	// *****UNITS*****
	/// Units cannot be clicked if a building is being placed or Pause Menu is active
	if (GetAlphaBuilding() == EntityType_NONE && pauseMenuActions == PauseMenuActions_NOT_EXIST) {

		// Select units by mouse click
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !App->gui->IsMouseOnUI()) {

			startRectangle = mousePos;

			Entity* entity = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);
			if (entity != nullptr)
				App->entities->SelectEntity(entity);
			//else
				//App->entities->UnselectAllEntities();

			/// **Debug purposes**
			Entity* enemy = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);
			if (enemy != nullptr)
				enemy->isSelected = true;
			///_**Debug_purposes**

			Entity* playerBuilding = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_STATIC_ENTITY, EntitySide_Player);
			if (playerBuilding != nullptr)
				App->entities->SelectBuilding((StaticEntity*)playerBuilding);

			Entity* neutralBuilding = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_STATIC_ENTITY, EntitySide_Neutral);
			if (neutralBuilding != nullptr)
				App->entities->SelectBuilding((StaticEntity*)neutralBuilding);

			if (playerBuilding == nullptr && neutralBuilding == nullptr)
				App->entities->UnselectAllBuildings();

			Entity* prisonerUnit = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_NoSide);
			if (prisonerUnit != nullptr)
				prisonerUnit->isSelected = true;
			else
				App->entities->UnselectAllPrisoners();
		}

		int width = mousePos.x - startRectangle.x;
		int height = mousePos.y - startRectangle.y;

		/// SELECT UNITS

		// b) Select a group of units
		// SDL_SCANCODE_LSHIFT									// SDL_SCANCODE_RSHIFT
		if (App->input->GetKey(buttonSelectGroup) == KEY_REPEAT || App->input->GetKey(buttonSelectGroupb) == KEY_REPEAT) {

			bool isSelectedGroup = false;
			uint numGroup = 0;

			// SDL_SCANCODE_1
			if (App->input->GetKey(buttonSelectGroup1) == KEY_DOWN) {

				numGroup = 0;
				isSelectedGroup = true;
			}
			// SDL_SCANCODE_2
			else if (App->input->GetKey(buttonSelectGroup2) == KEY_DOWN) {

				numGroup = 1;
				isSelectedGroup = true;
			}
			// SDL_SCANCODE_3
			else if (App->input->GetKey(buttonSelectGroup3) == KEY_DOWN) {

				numGroup = 2;
				isSelectedGroup = true;
			}

			if (isSelectedGroup) {

				list<DynamicEntity*> savedGroup = App->entities->GetSavedEntityGroup(numGroup);

				if (savedGroup.size() > 0) {

					// Select the group
					App->entities->SelectEntitiesGroup(savedGroup);

					// Blit the selection of the units (just a few seconds) from alpha 255 to 0
					list<DynamicEntity*>::const_iterator sg = savedGroup.begin();

					while (sg != savedGroup.end()) {

						(*sg)->BlitSelectedGroupSelection();

						sg++;
					}
				}
				else {

					if (adviceMessage != AdviceMessage_EMPTY_GROUP) {
						adviceMessageTimer.Start();
						adviceMessage = AdviceMessage_EMPTY_GROUP;
						ShowAdviceMessage(adviceMessage);
					}
				}
			}
		}

		// Manage units selection
		/// Remove certain units from units selected
		// SDL_SCANCODE_LSHIFT									// SDL_SCANCODE_RSHIFT
		if (App->input->GetKey(buttonSelectGroup) == KEY_DOWN || App->input->GetKey(buttonSelectGroupb) == KEY_DOWN) {
			App->entities->auxUnitsSelected = App->entities->unitsSelected;
			isShift = true;
			isCtrl = false;
		}
		// SDL_SCANCODE_LSHIFT									// SDL_SCANCODE_RSHIFT
		else if (App->input->GetKey(buttonSelectGroup) == KEY_UP || App->input->GetKey(buttonSelectGroupb) == KEY_UP) {
			isShift = false;
			isCtrl = false;
		}
		/// Select more than one group of units
		// SDL_SCANCODE_LCTRL										// SDL_SCANCODE_RCTRL
		if (App->input->GetKey(buttonSaveGroup) == KEY_DOWN || App->input->GetKey(buttonSaveGroupb) == KEY_DOWN) {
			App->entities->auxUnitsSelected.clear();
			isCtrl = true;
			isShift = false;
		}
		// SDL_SCANCODE_LCTRL										// SDL_SCANCODE_RCTRL
		else if (App->input->GetKey(buttonSaveGroup) == KEY_UP || App->input->GetKey(buttonSaveGroupb) == KEY_UP) {
			isCtrl = false;
			isShift = false;
		}

		// Select units by rectangle drawing
		if (abs(width) >= RECTANGLE_MIN_AREA && abs(height) >= RECTANGLE_MIN_AREA && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {

			Entity* playerBuilding = App->entities->IsEntityUnderMouse(startRectangle, EntityCategory_STATIC_ENTITY, EntitySide_Player);
			Entity* neutralBuilding = App->entities->IsEntityUnderMouse(startRectangle, EntityCategory_STATIC_ENTITY, EntitySide_Neutral);

			if (startRectangle.x != -1 && startRectangle.y != -1
				&& playerBuilding == nullptr && neutralBuilding == nullptr) {

				// Draw the rectangle
				SDL_Rect mouseRect = { startRectangle.x, startRectangle.y, width, height };
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

				App->entities->SelectEntitiesWithinRectangle(mouseRect, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player, isCtrl, isShift);
			}
		}

		//LOG("%i", App->entities->unitsSelected.size());

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

			startRectangle = { -1,-1 };

			if (isShift)

				App->entities->auxUnitsSelected.clear();

			if (isCtrl) {
			
				list<DynamicEntity*>::const_iterator it = App->entities->auxUnitsSelected.begin();

				while (it != App->entities->auxUnitsSelected.end()) {
				
					if (find(App->entities->unitsSelected.begin(), App->entities->unitsSelected.end(), *it) == App->entities->unitsSelected.end())
						App->entities->unitsSelected.push_back(*it);

					it++;
				}

				App->entities->auxUnitsSelected.clear();
			}
		}

		units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0) {

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

				if (!App->gui->IsMouseOnUI()) {

					if (!CompareSelectedUnitsLists(units)) {
						ShowSelectedUnits(units);
						PlayUnitSound(units, true); // Units selected sound
					}
				}
			}

			UnitGroup* group = App->movement->GetGroupByUnits(units);

			if (group == nullptr)

				// Selected units will now behave as a group
				group = App->movement->CreateGroupFromUnits(units);

			if (group != nullptr) {

				// a) Save group of units
				// SDL_SCANCODE_LCTRL										// SDL_SCANCODE_RCTRL
				if (App->input->GetKey(buttonSaveGroup) == KEY_REPEAT || App->input->GetKey(buttonSaveGroup) == KEY_REPEAT) {

					bool isSavedGroup = false;

					// SDL_SCANCODE_1
					if (App->input->GetKey(buttonSelectGroup1) == KEY_DOWN) {
					
						App->entities->SaveEntityGroup(units, 0);
						isSavedGroup = true;
						App->entities->UpdateGroupIcons(0);
					}
					// SDL_SCANCODE_2
					else if (App->input->GetKey(buttonSelectGroup2) == KEY_DOWN) {

						App->entities->SaveEntityGroup(units, 1);
						isSavedGroup = true;
						App->entities->UpdateGroupIcons(1);
					}
					// SDL_SCANCODE_3
					else if (App->input->GetKey(buttonSelectGroup3) == KEY_DOWN) {

						App->entities->SaveEntityGroup(units, 2);
						isSavedGroup = true;
						App->entities->UpdateGroupIcons(2);
					}

					if (isSavedGroup) {

						// Blit the selection of the units (just a few seconds) from alpha 255 to 0
						list<DynamicEntity*>::const_iterator sg = units.begin();

						while (sg != units.end()) {

							(*sg)->BlitSavedGroupSelection();

							sg++;
						}
					}
				}

				// Move the camera to the group of units
				// SDL_SCANCODE_Q
				if (App->input->GetKey(buttonGoToUnits) == KEY_DOWN) {

					iPoint centroid = App->entities->CalculateCentroidEntities(units);
					iPoint cameraPos = App->render->FindCameraPosFromCenterPos(centroid);

					/// Check if there is an entity from the group on the centroid calculated
					list<DynamicEntity*>::const_iterator u = units.begin();

					bool isInScreen = false;
					const SDL_Rect cameraRect{ -cameraPos.x, -cameraPos.y, App->render->camera.w, App->render->camera.h };

					while (u != units.end()) {

						const SDL_Rect entityRect = { (*u)->GetPos().x, (*u)->GetPos().y, (*u)->GetSize().x, (*u)->GetSize().y };

						if (App->render->IsInRectangle(cameraRect, entityRect)) {

							isInScreen = true;
							break;
						}

						u++;
					}

					/// If there isn't, move the camera to the position of a random entity from the group
					if (!isInScreen) {

						centroid = { (int)units.front()->GetPos().x, (int)units.front()->GetPos().y };
						cameraPos = App->render->FindCameraPosFromCenterPos(centroid);
					}

					// Move the camera to the resulting position
					App->render->camera.x = cameraPos.x;
					App->render->camera.y = cameraPos.y;
				}

				// Command a group of units
				/// COMMAND PATROL
				// SDL_SCANCODE_N
				if (App->input->GetKey(buttonPatrolUnits) == KEY_DOWN)
					App->entities->CommandToUnits(units, UnitCommand_Patrol);

				/// STOP UNIT (FROM WHATEVER THEY ARE DOING)
				// SDL_SCANCODE_M
				if (App->input->GetKey(buttonStopUnits) == KEY_DOWN)

					App->entities->CommandToUnits(units, UnitCommand_Stop);

				/// COMMAND ATTACK
				/// Enemy
				Entity* target = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_Enemy);

				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && target != nullptr && !App->gui->IsMouseOnUI()) {

					list<DynamicEntity*>::const_iterator it = units.begin();

					bool isTarget = true;

					while (it != units.end()) {

						if (!(*it)->SetCurrTarget(target))
							isTarget = false;

						it++;
					}

					if (isTarget) {
						// All the group is issued to attack this enemy (DYNAMIC ENTITY: enemy)
						App->entities->CommandToUnits(units, UnitCommand_AttackTarget);

						SDL_Rect targetRect = { target->GetPos().x, target->GetPos().y, target->GetSize().x, target->GetSize().y };
						App->printer->PrintQuad(targetRect, ColorRed);
					}
				}

				/// Critter
				Entity* critter = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_Neutral);

				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && critter != nullptr && !App->gui->IsMouseOnUI()) {

					list<DynamicEntity*>::const_iterator it = units.begin();

					bool isTarget = true;

					while (it != units.end()) {

						if (!(*it)->SetCurrTarget(critter))
							isTarget = false;

						it++;
					}

					if (isTarget) {
						// All the group is issued to attack this enemy (DYNAMIC ENTITY: neutral)
						App->entities->CommandToUnits(units, UnitCommand_AttackTarget);

						SDL_Rect targetRect = { critter->GetPos().x, critter->GetPos().y, critter->GetSize().x, critter->GetSize().y };
						App->printer->PrintQuad(targetRect, ColorRed);
					}
				}

				/// Buildings
				Entity* building = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_STATIC_ENTITY, EntitySide_Enemy);

				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && building != nullptr && !App->gui->IsMouseOnUI()) {

					list<DynamicEntity*>::const_iterator it = units.begin();

					bool isTarget = true;

					while (it != units.end()) {

						if (!(*it)->SetCurrTarget(building))
							isTarget = false;

						it++;
					}

					if (isTarget) {
						// All the group is issued to attack this enemy (STATIC ENTITY: enemy)
						App->entities->CommandToUnits(units, UnitCommand_AttackTarget);

						SDL_Rect targetRect = { building->GetPos().x, building->GetPos().y, building->GetSize().x, building->GetSize().y };
						App->printer->PrintQuad(targetRect, ColorRed);
					}
				}

				// Entities to take in account when the mouse sprite changes
				Entity* playerUnit = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);
				Entity* playerBuilding = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_STATIC_ENTITY, EntitySide_Player);
				Entity* prisoner = App->entities->IsEntityUnderMouse(mousePos, EntityCategory_DYNAMIC_ENTITY, EntitySide_NoSide);

				// Set the cursor texture (SWORD)
				if (target != nullptr  && App->fow->IsOnSight(App->player->GetMousePos())
					|| critter != nullptr && App->fow->IsOnSight(App->player->GetMousePos())
					|| building != nullptr && App->fow->IsOnSight(App->player->GetMousePos())) {
					SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
					if (r.x != 374 && !App->gui->IsMouseOnUI())
						App->menu->mouseText->SetTexArea({ 374, 527, 28, 33 }, { 402, 527, 28, 33 });
				}
				//Lens cursor
				else if (playerUnit != nullptr || playerBuilding != nullptr 
					|| prisoner != nullptr && App->fow->IsOnSight(App->player->GetMousePos())) {
					SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
					if (r.x != 503 && !App->gui->IsMouseOnUI())
						App->menu->mouseText->SetTexArea({ 503, 524, 30, 32 }, { 503, 524, 30, 32 });
				}
				else {
					SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
					if (r.x != 243)
						if (!App->player->isMouseOnMine)
							App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
				}

				/// SET GOAL (COMMAND MOVE TO POSITION)
				bool isOnlyGryphonRider = App->entities->IsOnlyThisTypeOfUnits(units, EntityType_GRYPHON_RIDER);
				bool isGryphonRiderRunestone = false;

				if (isOnlyGryphonRider)
					isGryphonRiderRunestone = App->entities->AreAllUnitsDoingSomething(units, UnitState_HealRunestone);

				// Draw a shaped goal
				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT && !App->gui->IsMouseOnUI())

					if (isOnlyGryphonRider && !isGryphonRiderRunestone)
						group->DrawShapedGoal(mouseTile, false);
					else
						group->DrawShapedGoal(mouseTile);

				// Set a normal or shaped goal
				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP) {

					bool isGoal = false;

					// Cross particle where the mouse is
					if (!App->gui->IsMouseOnUI()) {

						App->particles->AddParticle(App->particles->cross, App->player->GetMousePos());

						if (group->GetShapedGoalSize() <= 1) {

							group->ClearShapedGoal();

							if (isOnlyGryphonRider && isGryphonRiderRunestone) {

								if (group->SetGoal(mouseTile, true)) /// normal goal
									isGoal = true;
							}
							else {
							
								if (group->SetGoal(mouseTile)) /// normal goal
									isGoal = true;
							}
						}
						else if (group->SetShapedGoal()) /// shaped goal

							isGoal = true;
					}

					if (isGoal || isGoalFromMinimap) {

						// Reset the goal from the minimap
						if (isGoalFromMinimap)

							isGoalFromMinimap = false;
						
						PlayUnitSound(units, false); // Unit command sound

						uint isPatrol = 0;

						list<DynamicEntity*>::const_iterator it = units.begin();

						while (it != units.end()) {

							if ((*it)->GetUnitCommand() == UnitCommand_Patrol)
								isPatrol++;

							it++;
						}

						/// If all units are in the Patrol command or the AttackTarget command, do not set the MoveToPosition command
						bool isFull = false;

						if (isPatrol == units.size() || target != nullptr || critter != nullptr || building != nullptr || prisoner != nullptr)
							isFull = true;

						if (!isFull)
							App->entities->CommandToUnits(units, UnitCommand_MoveToPosition);
					}
				}
			}
		}
	}
	//_*****UNITS*****
	
	if (isRoomCleared)
		BlitRoomClearedFloor(dt);

	// ---------------------------------------------------------------------------------

	DebugKeys();

	if (pauseMenuActions == PauseMenuActions_NOT_EXIST)
		CheckCameraMovement(dt);

	// Checks if resources have changed to update building menu and gold label
	if (terenasDialogTimer.Read() >= 25000 && terenasDialogEvent == TerenasDialog_START) {
		if (App->scene->terenasDialogEvent != TerenasDialog_WAVES) {
			App->scene->terenasDialogTimer.Start();
			App->scene->terenasDialogEvent = TerenasDialog_WAVES;
			App->scene->ShowTerenasDialog(App->scene->terenasDialogEvent);
		}
	}
	if (terenasDialogTimer.Read() >= 20000 && terenasDialogEvent == TerenasDialog_WAVES) {
		HideTerenasDialog();
	}
	if (terenasDialogTimer.Read() >= 5000 && terenasDialogEvent != TerenasDialog_NONE && terenasDialogEvent != TerenasDialog_START) {
		HideTerenasDialog();
	}
	if (adviceMessageTimer.Read() >= 2500 && adviceMessage != AdviceMessage_NONE && adviceMessage != AdviceMessage_UNDER_ATTACK) {
		HideAdviceMessage();
	}
	if (adviceMessageTimer.Read() >= 3500 && adviceMessage == AdviceMessage_UNDER_ATTACK) {
		HideAdviceMessage();
	}

	if (parchmentImg != nullptr) {
		if (parchmentImg->GetAnimation()->Finished() && pauseMenuActions == PauseMenuActions_NOT_EXIST) {
			pauseMenuActions = PauseMenuActions_CREATED;
			alphaCont = 0;
		}

		else if (parchmentImg->GetAnimation()->speed > 0) {
			SDL_Rect rect = { -(int)App->render->camera.x, -(int)App->render->camera.y, (int)App->render->camera.w, (int)App->render->camera.h };
			
			if (dt > 0)
				alphaCont += 100 * dt;
			else
				alphaCont += 100 * App->auxiliarDt;

			App->printer->PrintQuad(rect, { 0,0,0, (Uint8)alphaCont }, true, true, Layers_QuadsPrinters);
		}

		if (pauseMenuActions != PauseMenuActions_NOT_EXIST) {
			SDL_Rect rect = { -(int)App->render->camera.x, -(int)App->render->camera.y, (int)App->render->camera.w, (int)App->render->camera.h };
			App->printer->PrintQuad(rect, { 0,0,0,100 }, true, true, Layers_QuadsPrinters);
		}
	}
	switch (pauseMenuActions)
	{
	case PauseMenuActions_NONE:
		break;
	case PauseMenuActions_NOT_EXIST:
		break;
	case PauseMenuActions_CREATED:
		CreatePauseMenu();

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
	case PauseMenuActions_SLIDERFX:
		App->menu->UpdateSlider(AudioFXPause);
		break;
	case PauseMenuActions_SLIDERMUSIC:
		App->menu->UpdateSlider(AudioMusicPause);
		break;
	default:
		break;
	}
	// SDL_SCANCODE_ESCAPE
	if (App->input->GetKey(buttonPauseMenu) == KEY_DOWN) {
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		if (parchmentImg == nullptr) {
			UIImage_Info parchmentInfo;
			parchmentInfo.texArea = App->gui->parchmentArea;
			parchmentImg = App->gui->CreateUIImage({ 260, 145 }, parchmentInfo, this);
			parchmentImg->StartAnimation(App->gui->parchmentAnim);
			parchmentImg->SetPriorityDraw(PriorityDraw_WINDOW);
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

	if (!isStartedFinalTransition) {

		//if (App->input->GetKey(buttonLeaveGame) == KEY_DOWN) {

		//	App->gui->RemoveElem((UIElement**)&parchmentImg);
		//	return false;
		//}

		if (App->player->imagePrisonersVector.size() >= 2) {

			App->player->isWin = true;
			finalTransition.Start();
			isStartedFinalTransition = true;
		}
		// SDL_SCANCODE_F1
		else if (App->input->GetKey(buttonWinGame) == KEY_DOWN && App->isDebug)
		{
			App->player->isWin = true;
			App->fade->FadeToBlack(this, App->finish);
			App->finish->active = true;
		}
	}

	// Final transition timer (when win is achieved)
	if (App->player->isWin && finalTransition.ReadSec() >= 5.0f) {

		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}

	if (!isStartedFinalTransition) {

		//LoseConditions
			//We have no more units in game
		if ((App->entities->GetNumberOfPlayerUnits() <= 0 && isStarted) &&
			//Not enogh gold to create Archer (cheeper unit) and we have no units spawning
			((App->player->GetCurrentGold() < App->player->elvenArcherCost && App->player->toSpawnUnitBarracks.empty() && App->player->toSpawnUnitGrypho.empty() && !App->player->isUnitSpawning)
				//Have not barracks and gryphos and have not enogh gold for build it and create the cheeper option (grypho)
				|| (App->player->GetCurrentGold() < (App->player->gryphonRiderCost + gryphonAviaryCost) && App->player->gryphonAviary == nullptr && App->player->barracks == nullptr)
				//Not enogh gold to create Gryphos and have not barracks and we and have no units spawning
				|| (App->player->GetCurrentGold() < App->player->gryphonRiderCost  && App->player->barracks == nullptr && App->player->toSpawnUnitGrypho.empty() && !App->player->isUnitSpawning))
			//Instant Lose with F2
			// SDL_SCANCODE_F2
			|| (App->input->GetKey(buttonLoseGame) == KEY_DOWN && App->isDebug)
			//Orde destroy townhall
			|| App->player->townHall == nullptr) {

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
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	bool ret = true;

	LOG("Freeing scene");

	App->audio->PauseMusic();
	App->tex->UnLoad(debugTex);

	pauseMenuActions = PauseMenuActions_NOT_EXIST;

	if (!App->gui->isGuiCleanUp)
		DestroyAllUI();
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
	App->wave->active = false;
	App->fow->active = false;

	App->map->UnLoad();
	App->player->CleanUp();
	App->entities->CleanUp();
	App->particles->CleanUp();
	App->movement->CleanUp();
	App->pathmanager->CleanUp();
	App->pathfinding->CleanUp();
	App->collision->CleanUp();
	App->wave->CleanUp();
	App->fow->CleanUp();

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
	//if (App->input->GetKey(*buttonSaveGame) == KEY_DOWN) {
	//	App->SaveGame();
	//}

	// F6: load the previous state
	//if (App->input->GetKey(*buttonLoadGame) == KEY_DOWN) {
	//	App->LoadGame();
	//}

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

	// SDL_SCANCODE_SPACE
	if (App->input->GetKey(buttonGoToBase) == KEY_DOWN)
	{
		App->render->camera.x = -basePos.x;
		App->render->camera.y = -basePos.y;
	}

	// SDL_SCANCODE_B
	if (App->input->GetKey(buttonBuildingMenu) == KEY_DOWN)

		ChangeBuildingMenuState(&buildingMenuButtons);
}

void j1Scene::CheckCameraMovement(float dt) 
{
	int x = 0, y = 0;
	App->input->GetMousePosition(x, y);

	mouse = App->player->GetMousePos();
	int downMargin = -(App->map->data.height * App->map->data.tileHeight) + height / scale;
	int rightMargin = -(App->map->data.width * App->map->data.tileWidth) + width / scale;

	//NOT MOVING WITH App->input->GetKey(buttonMoveUp) == KEY_REPEAT
	//Move with arrows
	//UP
	// SDL_SCANCODE_UP										// SDL_SCANCODE_W
	if ((App->input->GetKey(buttonMoveUp) == KEY_REPEAT || App->input->GetKey(buttonMoveUpb) == KEY_REPEAT)
		&& App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	//DOWN
	// SDL_SCANCODE_DOWN										// SDL_SCANCODE_S
	if ((App->input->GetKey(buttonMoveDown) == KEY_REPEAT || App->input->GetKey(buttonMoveDownb) == KEY_REPEAT)
		&& App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	//LEFT
	// SDL_SCANCODE_LEFT										// SDL_SCANCODE_A
	if ((App->input->GetKey(buttonMoveLeft) == KEY_REPEAT || App->input->GetKey(buttonMoveLeftb) == KEY_REPEAT)
		&& App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	//RIGHT
	// SDL_SCANCODE_RIGHT										// SDL_SCANCODE_D
	if ((App->input->GetKey(buttonMoveRight) == KEY_REPEAT || App->input->GetKey(buttonMoveRightb) == KEY_REPEAT)
		&& App->render->camera.x >= rightMargin)
		App->render->camera.x -= camSpeed * dt;

	//Move with mouse
	////UP
	if (mouse.y <= (camMovMargin - App->render->camera.y) / scale && App->render->camera.y <= 0)
	{
		App->render->camera.y += GetCamSpeed(y) * dt;
	}
	////DOWN
	if (mouse.y >= (height - (camMovMargin + 30) - App->render->camera.y) / scale && App->render->camera.y >= downMargin)
	{
		int newY = App->win->height - y;
		App->render->camera.y -= GetCamSpeed(newY) * dt;
	}
	////LEFT
	if (mouse.x <= (camMovMargin - App->render->camera.x) / scale && App->render->camera.x <= 0)
	{
		App->render->camera.x += GetCamSpeed(x) * dt;
	}
	////RIGHT
	if (mouse.x >= (width - (camMovMargin + 30) - App->render->camera.x) / scale && App->render->camera.x >= rightMargin)
	{
		 int newX = App->win->width - x;
		App->render->camera.x -= GetCamSpeed(newX) * dt;
	}
}

int j1Scene::GetCamSpeed(int pos)
{
	int speed = 0;

	int distanceTo = -(pos - camMovMargin);

	if (distanceTo < 0)
		distanceTo = 0;
	else if (distanceTo > camMovMargin)
	{
		distanceTo = camMovMargin;
	}

	float proximity = pow((distanceTo / camMovMargin), camMovement);

	speed = camSpeed * proximity;

	return speed;
}


void j1Scene::LoadInGameUI()
{
	//Buiding options
	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 0, 0, 126, 26 };
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
	pauseMenuLabel = App->gui->CreateUILabel({ buttonInfo.hoverTexArea.w / 2, 8 }, labelInfo, this, pauseMenuButt);

	UIImage_Info entitiesInfo;
	entitiesInfo.draggable = false;
	entitiesInfo.texArea = { 0, 565, 371, 82 };
	entitiesStats = App->gui->CreateUIImage({ (int)App->render->camera.w - entitiesInfo.texArea.w,(int)App->render->camera.h - entitiesInfo.texArea.h }, entitiesInfo, this);
	entitiesStats->SetPriorityDraw(PriorityDraw_WINDOW);

	entitiesInfo.texArea = { 1006,0,800,600 };
	inGameFrameImage = App->gui->CreateUIImage({ 0,0 }, entitiesInfo, this);
	inGameFrameImage->SetPriorityDraw(PriorityDraw_FRAMEWORK);

	//changing map button
	buttonInfo.normalTexArea = { 241, 453, 30, 21 };
	buttonInfo.hoverTexArea = { 272, 453, 30, 21 };
	buttonInfo.pressedTexArea = { 303, 453, 30, 21 };
	changeMinimapButt = App->gui->CreateUIButton({ 200,27 }, buttonInfo, this);

	LoadResourcesLabels();
	LoadBuildingMenu();
	LoadUnitsMenuInfo();
	LoadTerenasDialog();
	LoadAdviceMessage();
	//create this before entitiesInfo (Parent)
	App->player->CreateEntitiesStatsUI();
	App->player->CreateGroupSelectionButtons();
	App->player->CreatePlayerGroupsButtons();
	App->player->CreateUISpawningUnits();
}

void j1Scene::LoadUnitsMenuInfo()
{
	int cont = 0;
	while (groupElementsList.size() < 8) {
		UIImage* image = nullptr;
		UILifeBar* lifeBar = nullptr;


		image = App->player->CreateGroupIcon({ 54 * (cont % 4) + 3, 39 * (cont / 4) + 4 }, { 0, 0, 0, 0 });
		image->isActive = false;
		lifeBar = CreateGroupLifeBar({ 54 * (cont % 4) + 2, 39 * (cont / 4) + 33 }, { 240,362,47,7 }, { 242,358,42,3 });
		lifeBar->isActive = false;

		groupElementsList.push_back({ nullptr, image, lifeBar });
		cont++;
	}
	CreateAbilitiesButtons();
}

UILifeBar* j1Scene::CreateGroupLifeBar(iPoint lifeBarPos, SDL_Rect backgroundTexArea, SDL_Rect barTexArea)
{
	UILifeBar_Info lifeInfo;
	lifeInfo.background = backgroundTexArea;
	lifeInfo.bar = barTexArea;
	lifeInfo.maxWidth = lifeInfo.bar.w;
	lifeInfo.lifeBarPosition = { 2, 2 };
	return App->gui->CreateUILifeBar(lifeBarPos, lifeInfo, nullptr, (UIElement*)App->scene->entitiesStats);
}

void j1Scene::CreateAbilitiesButtons()
{
	UIButton_Info infoButton;

	infoButton.normalTexArea = { 802,202,50,41 };
	infoButton.hoverTexArea = { 904, 202, 50, 41 };
	infoButton.pressedTexArea = { 853,202,50,41 };
	commandStopButton = App->gui->CreateUIButton({ 217, 2 }, infoButton, this, (UIElement*)App->scene->entitiesStats);
	commandStopButton->isActive = false;

	infoButton.normalTexArea = { 649,202,50,41 };
	infoButton.hoverTexArea = { 751, 202, 50, 41 };
	infoButton.pressedTexArea = { 700,202,50,41 };
	commandPatrolButton = App->gui->CreateUIButton({ 268, 2 }, infoButton, this, (UIElement*)App->scene->entitiesStats);
	commandPatrolButton->isActive = false;
}

void j1Scene::LoadTerenasDialog()
{
	UIImage_Info imageInfo;
	imageInfo.texArea = { 734,34,70,100 };
	terenasAdvices.terenasImage = App->gui->CreateUIImage({ 695,32 }, imageInfo, this);
	terenasAdvices.terenasImage->isActive = false;

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.interactive = false;
	terenasAdvices.text = App->gui->CreateUILabel({ 355,47 }, labelInfo, this);
	terenasAdvices.text->isActive = false;
}

void j1Scene::LoadAdviceMessage()
{
	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.interactive = false;
	adviceLabel = App->gui->CreateUILabel({ 300,235 }, labelInfo, this);
	adviceLabel->isActive = false;
}

string j1Scene::ChooseMusicToPlay()
{
	string ret = levelTheme1;
	uint rng = rand() % 4 + 1;
	if (rng == 1)
		ret = levelTheme1;
	else if(rng == 2)
		ret = levelTheme2;
	else if (rng == 3)
		ret = levelTheme3;
	else if (rng == 4)
		ret = levelTheme4;

	return ret;
}

void j1Scene::ShowSelectedUnits(list<DynamicEntity*> units)
{
	HideUnselectedUnits();
	App->player->HideEntitySelectedInfo();
	list<DynamicEntity*>::iterator iterator = units.begin();
	while (iterator != units.end()) {
		UIImage* image = nullptr;
		UILifeBar* lifeBar = nullptr;
		if (units.size() == 1) {
			App->player->MakeUnitMenu((*iterator));
		}
		else {
			for (list<GroupSelectedElements>::iterator iteratorInfo = groupElementsList.begin(); iteratorInfo != groupElementsList.end(); ++iteratorInfo)
			{
				if (!(*iteratorInfo).entityIcon->isActive)
				{
					SDL_Rect text;
					if ((*iterator)->dynamicEntityType == EntityType_FOOTMAN) {
						text = { 649, 160, 46, 30 };
						(*iteratorInfo).entityIcon->SetNewRect(text);
					}
					else if ((*iterator)->dynamicEntityType == EntityType_ELVEN_ARCHER) {
						text = { 696, 160, 46, 30 };
						(*iteratorInfo).entityIcon->SetNewRect(text);
					}
					else if ((*iterator)->dynamicEntityType == EntityType_GRYPHON_RIDER) {
						text = { 702, 288, 46, 30 };
						(*iteratorInfo).entityIcon->SetNewRect(text);
					}
					(*iteratorInfo).entityIcon->isActive = true;
					(*iteratorInfo).entityLifeBar->SetLife((*iterator)->GetCurrLife());
					(*iteratorInfo).entityLifeBar->SetMaxLife((*iterator)->GetMaxLife());
					(*iteratorInfo).entityLifeBar->isActive = true;
					(*iteratorInfo).owner = *iterator;
					break;
				}
			}
		}
		iterator++;
	}
	if (units.size() > 0) {
		commandPatrolButton->isActive = true;
		commandStopButton->isActive = true;
	}
}

void j1Scene::HideUnselectedUnits()
{
	//One Selection
	if (App->player->entitySelectedStats.entitySelected != nullptr)
	{
		if (App->player->entitySelectedStats.entitySelected->entityType == EntityCategory_DYNAMIC_ENTITY) {
			App->player->entitySelectedStats.entitySight->isActive = false;
			App->player->entitySelectedStats.entityRange->isActive = false;
			App->player->entitySelectedStats.entityDamage->isActive = false;
			App->player->entitySelectedStats.entitySpeed->isActive = false;
			App->player->entitySelectedStats.HP->isActive = false;
			App->player->entitySelectedStats.entityName->isActive = false;
			App->player->entitySelectedStats.entityIcon->isActive = false;
			App->player->entitySelectedStats.lifeBar->isActive = false;
			App->player->entitySelectedStats.entitySelected = nullptr;
		}
	}
	//Group Selection
	else {
		for (list<GroupSelectedElements>::iterator iteratorInfo = groupElementsList.begin(); iteratorInfo != groupElementsList.end(); ++iteratorInfo)
		{

			if ((*iteratorInfo).owner != nullptr)
			{
				(*iteratorInfo).entityLifeBar->isActive = false;
				(*iteratorInfo).entityIcon->isActive = false;
				(*iteratorInfo).owner = nullptr;
			}
			else {
				break;
			}
		}
	}
	commandPatrolButton->isActive = false;
	commandStopButton->isActive = false;
}

//This function executes a unit sound depending on if it is a command sound or selection, and on which entity and 
//numer of entities are on the selection, depending on the type
void j1Scene::PlayUnitSound(list<DynamicEntity*> units, bool isSelect)
{
	uint footmanNum = 0, archerNum = 0, gryphonNum = 0;

	list<DynamicEntity*>::iterator iterator = units.begin();

	while (iterator != units.end()) {
		switch ((*iterator)->dynamicEntityType) {
		case EntityType_FOOTMAN:
			footmanNum++;
			break;
		case EntityType_ELVEN_ARCHER:
			archerNum++;
			break;
		case EntityType_GRYPHON_RIDER:
			gryphonNum++;
			break;
		default:
			break;
	}
		iterator++;
	}

	//Chooses a random sound to make for the unit
	FX footmanSound = ChooseRandomUnitSound(EntityType_FOOTMAN, isSelect);
	FX archerSound = ChooseRandomUnitSound(EntityType_ELVEN_ARCHER, isSelect);
	FX gryphonSound = ChooseRandomUnitSound(EntityType_GRYPHON_RIDER, isSelect);

	//Selects which sound to play depending on the number of entities that compose the unit selection
	if (footmanNum > archerNum + gryphonNum) {
		App->audio->PlayFx(footmanSound, 0);
	}

	else if (archerNum > footmanNum + gryphonNum) {
		App->audio->PlayFx(archerSound, 0);
	}

	else if(gryphonNum > archerNum + footmanNum)
		App->audio->PlayFx(gryphonSound, 0);

	else if (footmanNum == archerNum && footmanNum != gryphonNum) {
		uint rng = rand() % 2 + 1;

		if (rng == 1) 
			App->audio->PlayFx(footmanSound, 0);

		else if (rng == 2)
			App->audio->PlayFx(archerSound, 0);
	}
	else if (footmanNum == gryphonNum && footmanNum != archerNum) {
		uint rng = rand() % 2 + 1;

		if (rng == 1)
			App->audio->PlayFx(footmanSound, 0);
		else if (rng == 2)
			App->audio->PlayFx(gryphonSound, 0);
	}
	else if (archerNum == gryphonNum && archerNum != footmanNum) {
		uint rng = rand() % 2 + 1;

		if (rng == 1)
			App->audio->PlayFx(archerSound, 0);
		else if (rng == 2)
			App->audio->PlayFx(gryphonSound, 0);
	}
	else if (footmanNum == archerNum && footmanNum == gryphonNum) {
		uint rng = rand() % 2 + 1;

		if (rng == 1)
			App->audio->PlayFx(footmanSound, 0);
		else if (rng == 2)
			App->audio->PlayFx(archerSound, 0);
		else if (rng == 3)
			App->audio->PlayFx(gryphonSound, 0);
	}
	else
		App->audio->PlayFx(footmanSound, 0); //default
}

//This function chooses a random sound for the unit to make when selected or being commanded somwhere
FX j1Scene::ChooseRandomUnitSound(ENTITY_TYPE unitType, bool isSelect)
{
	FX unitSound = 0;
	list<FX> posibleFX;
	uint rng = rand() % 4 + 1; //Random number for the sound choosing

	//Looks for which sound it has to choose depending on the entity type and if it is a selection or a command
	switch (unitType) {
	case EntityType_FOOTMAN:
		if (isSelect) {
			rng = rand() % 5 + 1; //These are five sounds to choose instead of four
			posibleFX.push_back(App->audio->GetFX().footmanSelected1);
			posibleFX.push_back(App->audio->GetFX().footmanSelected2);
			posibleFX.push_back(App->audio->GetFX().footmanSelected3);
			posibleFX.push_back(App->audio->GetFX().footmanSelected4);
			posibleFX.push_back(App->audio->GetFX().footmanSelected5);
		}
		else if (!isSelect) {
			posibleFX.push_back(App->audio->GetFX().footmanCommand1);
			posibleFX.push_back(App->audio->GetFX().footmanCommand2);
			posibleFX.push_back(App->audio->GetFX().footmanCommand3);
			posibleFX.push_back(App->audio->GetFX().footmanCommand4);
		}
		break;
	case EntityType_ELVEN_ARCHER:
		if (isSelect) {
			posibleFX.push_back(App->audio->GetFX().archerSelected1);
			posibleFX.push_back(App->audio->GetFX().archerSelected2);
			posibleFX.push_back(App->audio->GetFX().archerSelected3);
			posibleFX.push_back(App->audio->GetFX().archerSelected4);
		}
		else if (!isSelect) {
			posibleFX.push_back(App->audio->GetFX().archerCommand1);
			posibleFX.push_back(App->audio->GetFX().archerCommand2);
			posibleFX.push_back(App->audio->GetFX().archerCommand3);
			posibleFX.push_back(App->audio->GetFX().archerCommand4);
		}
		break;
	case EntityType_GRYPHON_RIDER:
		if (isSelect) {
			return App->audio->GetFX().griffonSelected;
		}
		else if (!isSelect) {
			return App->audio->GetFX().griffonCommand;
		}
		break;
	default:
		break;
	}
	
	uint auxNum = 1;
	list<FX>::const_iterator it = posibleFX.begin();
	//Chooses the random sound depending on the random number generated (rng)
	while (it != posibleFX.end()) {
		if (rng == auxNum)
			unitSound = (*it);
		auxNum++;
		it++;
	}

	return unitSound;
}
	
void j1Scene::ChangeBuildingButtState(MenuBuildingButton* elem, bool isForced)
{
	if (!isForced) {
		elem->cost->isActive = !elem->cost->isActive;
		elem->icon->isActive = !elem->icon->isActive;
		elem->name->isActive = !elem->name->isActive;
	}
	else
	{
		elem->cost->isActive = false;
		elem->icon->isActive = false;
		elem->name->isActive = false;
	}
}
void j1Scene::ChangeBuildingMenuState(BuildingMenu * elem, bool isForced)
{
	if (!isForced)
		buildingMenu->isActive = !buildingMenu->isActive;
	else
		buildingMenu->isActive = false;
	ChangeBuildingButtState(&elem->cannonTower, isForced);
	ChangeBuildingButtState(&elem->chickenFarm, isForced);
	ChangeBuildingButtState(&elem->gryphonAviary, isForced);
	ChangeBuildingButtState(&elem->guardTower, isForced);
	ChangeBuildingButtState(&elem->scoutTower, isForced);
	ChangeBuildingButtState(&elem->barracks, isForced);
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
	ChangeMenuIconsText(buildingMenuButtons.guardTower.icon, guardTowerCost, { 394,76,50,41 }, { 445,76,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.scoutTower.icon, scoutTowerCost, { 394,34,50,41 }, { 445,34,50,41 });
	//Only one construction for each one
	ChangeMenuIconsText(buildingMenuButtons.barracks.icon, barracksCost, { 547,160,50,41 }, { 802,286,50,41 }, true, App->player->barracks);
	ChangeMenuIconsText(buildingMenuButtons.gryphonAviary.icon, gryphonAviaryCost, { 394,160,50,41 }, { 445,160,50,41 }, true, App->player->gryphonAviary);
}
void j1Scene::ChangeMenuIconsText(UIButton * butt, int cost, SDL_Rect normalText, SDL_Rect hoverText, bool isSingle, StaticEntity* stcEntity)
{
	if (isSingle) {

		if (stcEntity == nullptr && App->player->GetCurrentGold() >= cost) {
			if (stcEntity == App->player->gryphonAviary && !App->player->townHallUpgrade)
				butt->ChangesTextsAreas(false);
			else
				butt->ChangesTextsAreas(true, normalText, hoverText);
		}
		else
			butt->ChangesTextsAreas(false);
	}
	else {
		if (App->player->GetCurrentGold() >= cost)
			butt->ChangesTextsAreas(true, normalText, hoverText);
		else 
			butt->ChangesTextsAreas(false);
	}
}

void j1Scene::UpdateLabelsMenu()
{

	ChangeMenuLabelInfo(buildingMenuButtons.cannonTower.cost, cannonTowerCost);
	ChangeMenuLabelInfo(buildingMenuButtons.chickenFarm.cost, chickenFarmCost);
	ChangeMenuLabelInfo(buildingMenuButtons.guardTower.cost, guardTowerCost);
	ChangeMenuLabelInfo(buildingMenuButtons.scoutTower.cost, scoutTowerCost);
	//Only one construction for each one
	ChangeMenuLabelInfo(buildingMenuButtons.barracks.cost, barracksCost, true, App->player->barracks);
	ChangeMenuLabelInfo(buildingMenuButtons.gryphonAviary.cost, gryphonAviaryCost, true, App->player->gryphonAviary);
}

void j1Scene::ChangeMenuLabelInfo(UILabel * Label, int cost, bool isSingle, StaticEntity* stcEntity)
{
	//Text if you only can have one building like barracks
	if (isSingle) {
		if (stcEntity == nullptr) {

			if (App->player->GetCurrentGold() >= cost)
				Label->SetColor(White_, true);

			else
				Label->SetColor(BloodyRed_, true);

			if (stcEntity == App->player->gryphonAviary && !App->player->townHallUpgrade) {
				Label->SetText("Requires Keep");
				Label->SetColor(BloodyRed_, true);
			}
			else
				Label->SetText("Cost: " + to_string(cost) + " gold");
		}
		else {
			Label->SetColor(BloodyRed_, true);
			Label->SetText("You already have it");
		}
	}
	else {

		if (App->player->GetCurrentGold() >= cost)
			Label->SetColor(White_, true);
		else
			Label->SetColor(BloodyRed_, true);
	}
}

void j1Scene::LoadBuildingMenu()
{

	UIImage_Info imageInfo;
	imageInfo.draggable = false;
	imageInfo.texArea = { 0,33,240,345 };
	imageInfo.horizontalOrientation = HORIZONTAL_POS_RIGHT;
	buildingMenu = App->gui->CreateUIImage({ (int)App->win->width, 0 }, imageInfo, this, nullptr);
	buildingMenuOn = true;
	buildingMenu->SetPriorityDraw(PriorityDraw_UNDER_FRAMEWORK);
	buildingMenu->isActive = false;

	if (buildingMenu->type != UIE_TYPE_NO_TYPE)
	{

		CreateBuildingElements({ 343,34,50,41 }, { 585, 55 }, "Chicken Farm",
			"Cost: 250 gold", { 645, 65 }, { 645, 82 }, chickenFarmCost, &buildingMenuButtons.chickenFarm);

		CreateBuildingElements( { 496,160,50,41 }, { 585, 100 }, "Gryphon Aviary",
			"Requires Keep", { 645, 110 }, { 645, 127 }, gryphonAviaryCost, &buildingMenuButtons.gryphonAviary);
		
		CreateBuildingElements( { 496,34,50,41 }, { 585, 145 }, "Scout Tower",
			"Cost: 400 gold", { 645, 155 }, { 645, 172 }, scoutTowerCost, &buildingMenuButtons.scoutTower);
		
		CreateBuildingElements({ 496,76,50,41 }, { 585, 190 }, "Guard Tower",
			"Cost: 600 gold", { 645, 200 }, { 645, 217 }, guardTowerCost, &buildingMenuButtons.guardTower);
		
		CreateBuildingElements({ 496,118,50,41 }, { 585, 235 }, "Cannon Tower",
			"Cost: 600 gold", { 645, 245 }, { 645, 262 }, cannonTowerCost, &buildingMenuButtons.cannonTower);

		CreateBuildingElements({ 547,160,50,41 }, { 585, 280 }, "Barracks",
			"Cost: 1000 gold", { 645, 290 }, { 645, 307 }, barracksCost, &buildingMenuButtons.barracks);
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
	App->gui->RemoveElem((UIElement**)&elem->name);
	App->gui->RemoveElem((UIElement**)&elem->cost);
	App->gui->RemoveElem((UIElement**)&elem->icon);
}

void j1Scene::UnLoadBuildingMenu()
{	
	DeleteBuildingElements(&buildingMenuButtons.chickenFarm);
	DeleteBuildingElements(&buildingMenuButtons.gryphonAviary);
	DeleteBuildingElements(&buildingMenuButtons.scoutTower);
	DeleteBuildingElements(&buildingMenuButtons.guardTower);
	DeleteBuildingElements(&buildingMenuButtons.cannonTower);
	DeleteBuildingElements(&buildingMenuButtons.barracks);
	App->gui->RemoveElem((UIElement**)&buildingMenu);

	buildingMenuOn = false;
}

void j1Scene::LoadResourcesLabels()
{
	UILabel_Info labelInfo;
	labelInfo.interactive = false;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = to_string(App->player->GetCurrentGold());
	goldLabel = App->gui->CreateUILabel({ 250, 0 }, labelInfo, this, inGameFrameImage);

	labelInfo.fontName = FONT_NAME_WARCRAFT14;	
	labelInfo.text = to_string(App->player->currentFood);
	foodLabel = App->gui->CreateUILabel({ 334, 0 }, labelInfo, this, inGameFrameImage);
}

void j1Scene::UpdateGoldLabel(GoldChange state)
{
	if (state == GoldChange_Win)
		goldLabel->SetColor(ColorYellow);
	else if (state == GoldChange_Lose)
		goldLabel->SetColor(ColorRed);

	string label = to_string(App->player->GetCurrentGold());
	goldLabel->SetText(label);

	goldLabelColorTime.Start();
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

void j1Scene::CreatePauseMenu() 
{

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "Settings";

	int x = parchmentImg->GetLocalPos().x + 100;
	int y = parchmentImg->GetLocalPos().y + 60;
	settingsLabel = App->gui->CreateUILabel({x, y}, labelInfo, this);

	y += 40;
	labelInfo.text = "Save Game";
	saveGameLabel = App->gui->CreateUILabel({ x, y }, labelInfo, this);

	y += 40;
	labelInfo.text = "Resume Game";
	continueLabel = App->gui->CreateUILabel({ x, y }, labelInfo, this);

	y += 40;
	labelInfo.fontName = FONT_NAME_WARCRAFT14;
	labelInfo.text = "Return to Main Menu";
	ReturnMenuLabel = App->gui->CreateUILabel({ x, y }, labelInfo, this);

	// Mouse texture
	SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
	if (r.x != 243)
		App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });
}

void j1Scene::DestroyPauseMenu() 
{
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	App->gui->RemoveElem((UIElement**)&continueLabel);
	App->gui->RemoveElem((UIElement**)&ReturnMenuLabel);
	App->gui->RemoveElem((UIElement**)&saveGameLabel);
}

void j1Scene::CreateSettingsMenu() 
{
	UIButton_Info buttonInfo;
	UILabel_Info labelInfo;
	
	// Fullscreen
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

	y += 25;
	x -= 25;
	labelInfo.text = "Controls";
	labelInfo.hoverColor = ColorGreen;
	labelInfo.normalColor = Black_;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	buttonsLabel = App->gui->CreateUILabel({ x,y }, labelInfo, this);

	y -= 25;
	x -= 75;
	labelInfo.text = "Fullscreen";
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.interactive = false;
	fullScreenLabel = App->gui->CreateUILabel({ x,y }, labelInfo, this);


	// Sliders
	x = parchmentImg->GetLocalPos().x + 30;
	y = parchmentImg->GetLocalPos().y + 70;
	float relativeVol = (float)App->audio->fxVolume / MAX_AUDIO_VOLUM;
	SDL_Rect butText = { 565, 359 , 8, 10 };
	SDL_Rect bgText = { 434, 359, 130, 10 };
	App->menu->AddSlider(AudioFXPause, {x,y}, "Audio FX", relativeVol, butText, bgText, this);
	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	y += 50;
	App->menu->AddSlider(AudioMusicPause, { x,y }, "Audio Music", relativeVol, butText, bgText, this);
	labelInfo.interactive = true;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_TOP;
	labelInfo.text = "Back";
	x = parchmentImg->GetLocalPos().x + 50;
	y = parchmentImg->GetLocalPos().y + 185;
	returnLabel = App->gui->CreateUILabel({x, y}, labelInfo, this);
}

void j1Scene::DestroySettingsMenu() 
{
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenButt);
	App->gui->RemoveElem((UIElement**)&fullScreenLabel);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.slider);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.name);
	App->gui->RemoveElem((UIElement**)&AudioFXPause.value);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.slider);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.name);
	App->gui->RemoveElem((UIElement**)&AudioMusicPause.value);
	App->gui->RemoveElem((UIElement**)&buttonsLabel);
}

void j1Scene::DestroyAllUI()
{
	if (parchmentImg != nullptr) {
		App->gui->RemoveElem((UIElement**)&parchmentImg);
	}

	DestroyPauseMenu();
	DestroySettingsMenu();
	UnLoadBuildingMenu();
	UnLoadResourcesLabels();
	UnLoadTerenasDialog();

	App->gui->RemoveElem((UIElement**)&adviceLabel);
	App->gui->RemoveElem((UIElement**)&pauseMenuButt);
	App->gui->RemoveElem((UIElement**)&pauseMenuLabel);
	App->gui->RemoveElem((UIElement**)&entitiesStats);
	App->gui->RemoveElem((UIElement**)&buildingLabel);
	App->gui->RemoveElem((UIElement**)&buildingButton);
	App->gui->RemoveElem((UIElement**)&inGameFrameImage);
	App->gui->RemoveElem((UIElement**)&changeMinimapButt);

	for (list<GroupSelectedElements>::iterator iterator = groupElementsList.begin(); iterator != groupElementsList.end(); ++iterator) {
		App->gui->RemoveElem((UIElement**)&(*iterator).entityIcon);
		App->gui->RemoveElem((UIElement**)&(*iterator).entityLifeBar);
		(*iterator).owner = nullptr;
	}
	groupElementsList.clear();

	App->gui->RemoveElem((UIElement**)&commandPatrolButton);
	App->gui->RemoveElem((UIElement**)&commandStopButton);
}

PauseMenuActions j1Scene::GetPauseMenuActions()
{
	return pauseMenuActions;
}

bool j1Scene::CompareSelectedUnitsLists(list<DynamicEntity*> units)
{
	bool ret = false;
	int sizeList = 0;//Want to know how many active icons are


	if (App->player->entitySelectedStats.entitySelected != nullptr && units.size() == 1)
		if (units.front() == App->player->entitySelectedStats.entitySelected)
			ret = true;
	if (!ret) {
		if (units.size() == GetGroupElementSize()) {
			for (list<DynamicEntity*>::iterator unitsIterator = units.begin(); unitsIterator != units.end(); ++unitsIterator)
			{
				for (list<GroupSelectedElements>::iterator playerIterator = groupElementsList.begin(); playerIterator != groupElementsList.end(); ++playerIterator)
				{
					if (*unitsIterator == playerIterator->owner)
					{
						ret = true;
						sizeList++;
						break;
					}
					//if owner is nullptr, next ones will be nullptr
					else if (playerIterator->owner == nullptr) {
						ret = false;
						break;
					}
					else
						ret = false;
				}
				if (!ret)
					break;
				//If size are diferent have to redo information
			}
		}
	}
	return ret;
}

uint j1Scene::GetGroupElementSize() 
{
	uint size = 0u;
	for (list<GroupSelectedElements>::iterator playerIterator = groupElementsList.begin(); playerIterator != groupElementsList.end(); ++playerIterator)
	{
		if (playerIterator->owner != nullptr) {
			size++;
		}
		else
			break;
	}
	return size;
}
void j1Scene::ShowTerenasDialog(TerenasDialogEvents dialogEvent)
{
	// TODO: Search the same pos and length
	string text;
	switch (dialogEvent)
	{
	case TerenasDialog_START:
		text = "Welcome adventurers of Azeroth's armies! You have been sent to Draenor to rescue the members from the legendary Alliance expedition and defeat Ner'zhul to reclaim the artifacts from Azeroth and avoid caos. FOR THE ALLIANCE!";
		terenasAdvices.text->SetText(text, 340);
		terenasAdvices.text->SetLocalPos({ 355,47 });
		break;
	case TerenasDialog_WAVES:
		text = "BY THE WAY, THE SEAS SURROUNDING YOUR BASE ARE FULL OF ENEMIES TOO. BE AWARE OF YOUR SURROUNDINGS AND PROTECT YOUR TOWN HALL AT ANY COST!";
		terenasAdvices.text->SetText(text, 340);
		terenasAdvices.text->SetLocalPos({ 355,47 });
		break;
	case TerenasDialog_RESCUE_ALLERIA:
		text = "Congratulations! You have freed Alleria. I thank you in the name of Azeroth. For the Alliance!";
		terenasAdvices.text->SetText(text, 350);
		terenasAdvices.text->SetLocalPos({ 355,37 });
		break;
	case TerenasDialog_RESCUE_TURALYON:
		text = "Congratulations! You have freed Turalyon. I thank you in the name of Azeroth. For the Alliance!";
		terenasAdvices.text->SetText(text, 350);
		terenasAdvices.text->SetLocalPos({ 355,37 });
		break;
	case TerenasDialog_GOLD_MINE:
		text = "To get gold from the mine, first select the units that will gather it.";
		terenasAdvices.text->SetText(text, 320);
		terenasAdvices.text->SetLocalPos({ 355,47 });
		break;
	case TerenasDialog_RUNESTONE:
		text = "To heal a group of units, first select the units.";
		terenasAdvices.text->SetText(text, 320);
		terenasAdvices.text->SetLocalPos({ 355,47 });
		break;
	case TerenasDialog_NONE:
		break;
	default:
		break;
	}
	terenasAdvices.text->isActive = true;
	terenasAdvices.terenasImage->isActive = true;
}

void j1Scene::HideTerenasDialog()
{
	terenasDialogEvent = TerenasDialog_NONE;
	terenasAdvices.text->isActive = false;
	terenasAdvices.terenasImage->isActive = false;
}

void j1Scene::ShowAdviceMessage(AdviceMessages adviceMessage)
{
	string text;
	switch (adviceMessage)
	{
	case AdviceMessage_FOOD:
		text = "Not enough food. Build more farms.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 245,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_GOLD:
		text = "Not enough gold.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 320,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_SELECT_FOOTMANS:
		text = "No footman on screen.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 290,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_SELECT_ARCHERS:
		text = "No elven archer on screen.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 285,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_SELECT_GRYPHS:
		text = "No gryphon rider on screen.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_SELECT_ALL_UNITS:
		text = "No units on screen.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_EMPTY_GROUP:
		text = "No units in the group.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_ROOM_CLEAR:
		text = "ROOM CLEARED!";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 285,265 });
		adviceLabel->SetColor(WarmYellow_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT30);
		break;

	case AdviceMessage_BASE_DEFENDED:
		text = "BASE DEFENDED!";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 285,265 });
		adviceLabel->SetColor(WarmYellow_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT30);
		break;

	case AdviceMessage_UNDER_ATTACK:
		text = "YOUR BASE IS UNDER ATTACK";;
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 205,265 });
		adviceLabel->SetColor(BloodyRed_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT30);
		break;

	case AdviceMessage_MINE:
		text = "Select units to gather gold";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_GRYPH_MINE:
		text = "Gryphon riders cannot gather gold.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 245,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_GRYPH_PRISONER:
		text = "Gryphon riders cannot rescue prisoners.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 225,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_PRISONER:
		text = "Select units to rescue prisoners.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_BUILDING_IS_FULL_LIFE:
		text = "This building has full life.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 275,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_TOWNHALL_IS_NOT_UPGRADE:
		text = "You need to upgrade the Townhall.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 235,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	case AdviceMessage_ONLY_ONE_BUILDING:
		text = "You already have this building.";
		adviceLabel->SetText(text, 340);
		adviceLabel->SetLocalPos({ 255,265 });
		adviceLabel->SetColor(White_);
		adviceLabel->SetFontName(FONT_NAME_WARCRAFT20);
		break;

	}


	adviceLabel->isActive = true;
}

void j1Scene::HideAdviceMessage()
{
	adviceMessage = AdviceMessage_NONE;
	adviceLabel->isActive = false;
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
		if (UIelem == commandPatrolButton) {
			App->player->ShowHoverInfoMenu("Patrol units", "", &App->player->firstHoverInfo);
		}
		else if (UIelem == commandStopButton) {
			App->player->ShowHoverInfoMenu("Stop units", "", &App->player->secondHoverInfo);
		}

		break;

	case UI_EVENT_MOUSE_LEAVE:
		if (UIelem == commandPatrolButton)
			App->player->HideHoverInfoMenu(&App->player->firstHoverInfo);
		else if (UIelem == commandStopButton)
			App->player->HideHoverInfoMenu(&App->player->secondHoverInfo);

		break;

	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (parchmentImg == nullptr) {

			if (UIelem == buildingButton) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				HideTerenasDialog();
				ChangeBuildingMenuState(&buildingMenuButtons);
			}

			if (UIelem == changeMinimapButt) {
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				isMinimapChanged = !isMinimapChanged;
			}

			else if (UIelem == buildingMenuButtons.chickenFarm.icon) {
				if (App->player->GetCurrentGold() >= chickenFarmCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_CHICKEN_FARM;
				}
				else if (App->player->GetCurrentGold() < chickenFarmCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}

			else if (UIelem == buildingMenuButtons.gryphonAviary.icon && App->player->gryphonAviary == nullptr && App->player->townHallUpgrade && App->player->townHall->buildingState == BuildingState_Normal) {
				if (App->player->GetCurrentGold() >= gryphonAviaryCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_GRYPHON_AVIARY;
				}
				else if (App->player->GetCurrentGold() < gryphonAviaryCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}
			else if (UIelem == buildingMenuButtons.gryphonAviary.icon && !App->player->townHallUpgrade) { //Theres no townhall upgrade
					if (App->scene->adviceMessage != AdviceMessage_TOWNHALL_IS_NOT_UPGRADE) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_TOWNHALL_IS_NOT_UPGRADE;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			else if (UIelem == buildingMenuButtons.gryphonAviary.icon && App->player->townHallUpgrade) { //There's a aviary already on map
					if (App->scene->adviceMessage != AdviceMessage_ONLY_ONE_BUILDING) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_ONLY_ONE_BUILDING;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}


			else if (UIelem == buildingMenuButtons.scoutTower.icon) {
				if (App->player->GetCurrentGold() >= scoutTowerCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_SCOUT_TOWER;
				}
				else if (App->player->GetCurrentGold() < scoutTowerCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}

			else if (UIelem == buildingMenuButtons.guardTower.icon) {
				if (App->player->GetCurrentGold() >= guardTowerCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_PLAYER_GUARD_TOWER;
				}
				else if (App->player->GetCurrentGold() < guardTowerCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}

			else if (UIelem == buildingMenuButtons.cannonTower.icon) {
				if (App->player->GetCurrentGold() >= cannonTowerCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_PLAYER_CANNON_TOWER;
				}
				else if (App->player->GetCurrentGold() < cannonTowerCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}

			else if (UIelem == buildingMenuButtons.barracks.icon && App->player->barracks == nullptr) {
				if (App->player->GetCurrentGold() >= barracksCost) {
					//Unselect entities and hide their info
					//---------------------------------------
					App->entities->UnselectAllEntities();
					//---------------------------------------
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_BARRACKS;
				}
				else if (App->player->GetCurrentGold() < barracksCost) {
					if (App->scene->adviceMessage != AdviceMessage_GOLD) {
						App->audio->PlayFx(App->audio->GetFX().errorButt);
						App->scene->adviceMessageTimer.Start();
						App->scene->adviceMessage = AdviceMessage_GOLD;
						App->scene->ShowAdviceMessage(App->scene->adviceMessage);
					}
					else
						App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
			}
			else if (UIelem == buildingMenuButtons.barracks.icon) //There's a barrack already on map

				if (App->scene->adviceMessage != AdviceMessage_ONLY_ONE_BUILDING) {
					App->audio->PlayFx(App->audio->GetFX().errorButt);
					App->scene->adviceMessageTimer.Start();
					App->scene->adviceMessage = AdviceMessage_ONLY_ONE_BUILDING;
					App->scene->ShowAdviceMessage(App->scene->adviceMessage);
				}
				else
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
		}

		if (UIelem == pauseMenuButt) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			if (parchmentImg == nullptr) {
				ChangeBuildingMenuState(&buildingMenuButtons, true);
				UIImage_Info parchmentInfo;
				parchmentInfo.texArea = App->gui->parchmentArea;
				parchmentImg = App->gui->CreateUIImage({ 260, 145 }, parchmentInfo, this);
				parchmentImg->StartAnimation(App->gui->parchmentAnim);
				parchmentImg->SetPriorityDraw(PriorityDraw_WINDOW);
			}
			else {
				pauseMenuActions = PauseMenuActions_DESTROY;
			}
		}

		else if (UIelem == continueLabel) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_DESTROY;
		}

		else if (UIelem == ReturnMenuLabel) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_RETURN_MENU;
		}

		else if (UIelem == settingsLabel) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_SETTINGS_MENU;
		}

		else if (UIelem == saveGameLabel)
		{
			//TODO OSCAR SAVE
		}

		else if (UIelem == returnLabel) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			DestroySettingsMenu();
			pauseMenuActions = PauseMenuActions_CREATED;
		}

		else if (UIelem == (UIElement*)AudioFXPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERFX;

		else if (UIelem == (UIElement*)AudioMusicPause.slider)
			pauseMenuActions = PauseMenuActions_SLIDERMUSIC;

		else if (UIelem == fullScreenButt)
		{
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			App->win->SetFullscreen();

			break;
		}

		else if (UIelem == commandPatrolButton && commandPatrolButton->isActive) {
			// Command Patrol (SANDRA)
			if (App->scene->units.size() > 0)
				App->entities->CommandToUnits(App->scene->units, UnitCommand_Patrol);
		}
		else if (UIelem == commandStopButton && commandStopButton->isActive) {
			// Command Stop (SANDRA)
			if (App->scene->units.size() > 0)
				App->entities->CommandToUnits(App->scene->units, UnitCommand_Stop);
		}

		break;

	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == (UIElement*)AudioFXPause.slider || UIelem == (UIElement*)AudioMusicPause.slider)
			pauseMenuActions = PauseMenuActions_NONE;
		break;
	}
}

ENTITY_TYPE j1Scene::GetAlphaBuilding() 
{
	return alphaBuilding;
}

void j1Scene::SetAplphaBuilding(ENTITY_TYPE alphaBuilding) 
{
	this->alphaBuilding = alphaBuilding;
}

void j1Scene::BlitRoomClearedFloor(float dt)
{
	alpha -= 100.0f * dt;

	if (alpha < 0)
		alpha = 0;

	if (alpha == 0)
		isRoomCleared = false;

	App->printer->PrintQuad(roomCleared, { 255, 255, 255, (Uint8)alpha }, true, true, Layers_FloorColliders);
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


	ret = LoadKey(&buttonGoToBase, "buttonGoToBase", buttons);
	ret = LoadKey(&buttonGoToUnits, "buttonGoToUnits", buttons);
	ret = LoadKey(&buttonMinimap, "buttonMinimap", buttons);
	ret = LoadKey(&buttonBuildingMenu, "buttonBuildingMenu", buttons);
	ret = LoadKey(&buttonPauseMenu, "buttonPauseMenu", buttons);
	ret = LoadKey(&buttonPatrolUnits, "buttonPatrolUnits", buttons);
	ret = LoadKey(&buttonStopUnits, "buttonStopUnits", buttons);
	ret = LoadKey(&buttonDrawFow, "buttonStopUnits", buttons);

	ret = LoadKey(&buttonSpawnFootman, "buttonSpawnFootman", buttons);
	ret = LoadKey(&buttonSpawnArcher, "buttonSpawnArcher", buttons);
	ret = LoadKey(&buttonSpawnGryphon, "buttonSpawnGryphon", buttons);
	ret = LoadKey(&buttonSpawnGrunt, "buttonSpawnGrunt", buttons);
	ret = LoadKey(&buttonSpawnTroll, "buttonSpawnTroll", buttons);
	ret = LoadKey(&buttonSpawnDragon, "buttonSpawnDragon", buttons);
	ret = LoadKey(&buttonSpawnSheep, "buttonSpawnSheep", buttons);
	ret = LoadKey(&buttonSpawnBoar, "buttonSpawnBoar", buttons);
	ret = LoadKey(&buttonSpawnAlleria, "buttonSpawnAlleria", buttons);
	ret = LoadKey(&buttonSpawnTauralyon, "buttonSpawnTauralyon", buttons);

	ret = LoadKey(&buttonTogleDebug, "buttonTogleDebug", buttons);
	ret = LoadKey(&buttonTogleDebugAttack, "buttonTogleDebugAttack", buttons);
	ret = LoadKey(&buttonTogleDebugMovement, "buttonTogleDebugMovement", buttons);


	ret = LoadKey(&buttonSelectGroup, "buttonSelectGroup", buttons);
	ret = LoadKey(&buttonSelectGroupb, "buttonSelectGroupb", buttons);

	ret = LoadKey(&buttonSelectGroup1, "buttonSelectGroup1", buttons);
	ret = LoadKey(&buttonSelectGroup2, "buttonSelectGroup2", buttons);
	ret = LoadKey(&buttonSelectGroup3, "buttonSelectGroup3", buttons);

	ret = LoadKey(&buttonSaveGroup, "buttonSaveGroup", buttons);
	ret = LoadKey(&buttonSaveGroupb, "buttonSaveGroupb", buttons);
	ret = LoadKey(&buttonWinGame, "buttonWinGame", buttons);
	ret = LoadKey(&buttonLoseGame, "buttonLoseGame", buttons);
	ret = LoadKey(&buttonMoveUp, "buttonMoveUp", buttons);
	ret = LoadKey(&buttonMoveUpb, "buttonMoveUpb", buttons);
	ret = LoadKey(&buttonMoveDown, "buttonMoveDown", buttons);
	ret = LoadKey(&buttonMoveDownb, "buttonMoveDownb", buttons);
	ret = LoadKey(&buttonMoveLeft, "buttonMoveLeft", buttons);
	ret = LoadKey(&buttonMoveLeftb, "buttonMoveLeftb", buttons);
	ret = LoadKey(&buttonMoveRight, "buttonMoveRight", buttons);
	ret = LoadKey(&buttonMoveRightb, "buttonMoveRightb", buttons);

	return ret;
}