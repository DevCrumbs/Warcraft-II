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

	mainThemeMusicName = audio.child("mainTheme").attribute("path").as_string();

	//LoadKeys(config.child("buttons"));

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

	App->audio->PlayMusic(mainThemeMusicName.data(), 2.0f);

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

	if (isDebug && App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)

		App->entities->AddEntity(EntityType_FOOTMAN, pos, App->entities->GetUnitInfo(EntityType_FOOTMAN), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)

		App->entities->AddEntity(EntityType_ELVEN_ARCHER, pos, App->entities->GetUnitInfo(EntityType_ELVEN_ARCHER), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)

		App->entities->AddEntity(EntityType_GRYPHON_RIDER, pos, App->entities->GetUnitInfo(EntityType_GRYPHON_RIDER), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)

		App->entities->AddEntity(EntityType_GRUNT, pos, App->entities->GetUnitInfo(EntityType_GRUNT), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)

		App->entities->AddEntity(EntityType_TROLL_AXETHROWER, pos, App->entities->GetUnitInfo(EntityType_TROLL_AXETHROWER), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)

		App->entities->AddEntity(EntityType_DRAGON, pos, App->entities->GetUnitInfo(EntityType_DRAGON), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)

		App->entities->AddEntity(EntityType_SHEEP, pos, App->entities->GetUnitInfo(EntityType_SHEEP), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)

		App->entities->AddEntity(EntityType_BOAR, pos, App->entities->GetUnitInfo(EntityType_BOAR), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)

		App->entities->AddEntity(EntityType_ALLERIA, pos, App->entities->GetUnitInfo(EntityType_ALLERIA), unitInfo, App->player);

	else if (isDebug && App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)

		App->entities->AddEntity(EntityType_TURALYON, pos, App->entities->GetUnitInfo(EntityType_TURALYON), unitInfo, App->player);
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


	//Change to wite Gold Label Color before 2 sec

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

	if (App->pathfinding->IsWalkable(mouseTile))
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
		// Select units by rectangle drawing
		if (abs(width) >= RECTANGLE_MIN_AREA && abs(height) >= RECTANGLE_MIN_AREA && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {

			if (startRectangle.x != -1 && startRectangle.y != -1) {

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

				App->entities->SelectEntitiesWithinRectangle(mouseRect, EntityCategory_DYNAMIC_ENTITY, EntitySide_Player);
			}
		}

		units = App->entities->GetLastUnitsSelected();

		if (units.size() > 0) {

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {

				startRectangle = { -1,-1 };

				if (!App->gui->IsMouseOnUI()) {

					if (!CompareSelectedUnitsLists(units)) {
						ShowSelectedUnits(units);
						PlayUnitSound(units, true); //Unit selected sound
					}
				}
			}

			UnitGroup* group = App->movement->GetGroupByUnits(units);

			if (group == nullptr)

				// Selected units will now behave as a group
				group = App->movement->CreateGroupFromUnits(units);

			if (group != nullptr) {

				/// COMMAND PATROL
				if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)

					App->entities->CommandToUnits(units, UnitCommand_Patrol);

				/// STOP UNIT (FROM WHATEVER THEY ARE DOING)
				if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)

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

				// Set the cursor texture
				if (target != nullptr || critter != nullptr || building != nullptr) {
					SDL_Rect r = App->menu->mouseText->GetDefaultTexArea();
					if (r.x != 374 && !App->gui->IsMouseOnUI())
						App->menu->mouseText->SetTexArea({ 374, 527, 28, 33 }, { 402, 527, 28, 33 });
				}
				else if (playerUnit != nullptr || playerBuilding != nullptr || prisoner != nullptr) {
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
				bool isGryphonRider = App->entities->IsOnlyThisTypeOfUnits(units, EntityType_GRYPHON_RIDER);
				bool isGryphonRiderRunestone = false;

				if (isGryphonRider)			
					isGryphonRiderRunestone = App->entities->AreAllUnitsDoingSomething(units, UnitState_HealRunestone);

				// Draw a shaped goal
				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT && !App->gui->IsMouseOnUI())

					if (isGryphonRider && !isGryphonRiderRunestone)
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

							if (isGryphonRider && !isGryphonRiderRunestone) {
								if (group->SetGoal(mouseTile, false)) /// normal goal
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
	CheckCameraMovement(dt);

	//Checks if resources have changed to update building menu and gold label

	if (terenasDialogTimer.Read() >= 25000 && terenasDialogEvent == TerenasDialog_START) {
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

	if (App->input->GetKey(buttonReloadMap) == KEY_REPEAT)
	{
		App->map->UnLoad();
		//App->map->CreateNewMap();
	}

	if (parchmentImg != nullptr) {
		if (parchmentImg->GetAnimation()->Finished() && pauseMenuActions == PauseMenuActions_NOT_EXIST) {
			pauseMenuActions = PauseMenuActions_CREATED;
			alphaCont = 0;
		}

		else if (parchmentImg->GetAnimation()->speed > 0) {
			SDL_Rect rect = { -(int)App->render->camera.x, -(int)App->render->camera.y, (int)App->render->camera.w, (int)App->render->camera.h };
			alphaCont += 100*dt;
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
	
	default:
		break;
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
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
	else if (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
	
		App->player->isWin = true;
		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}

	// Final transition timer (when win is achieved)
	if (App->player->isWin && finalTransition.ReadSec() >= 5.0f) {

		App->fade->FadeToBlack(this, App->finish);
		App->finish->active = true;
	}
	
	if (((App->player->GetCurrentGold() < 400 && App->entities->GetNumberOfPlayerUnits() <= 0 && isStarted) && !App->player->isUnitSpawning) 
		|| (App->scene->isDebug && App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) || App->player->townHall == nullptr) {

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
	App->tex->UnLoad(debugTex);

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

	App->map->UnLoad();
	App->player->CleanUp();
	App->entities->CleanUp();
	App->particles->CleanUp();
	App->movement->CleanUp();
	App->pathmanager->CleanUp();
	App->pathfinding->CleanUp();
	App->collision->CleanUp();
	App->wave->CleanUp();

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
	if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) 
		&& App->render->camera.y <= 0)
		App->render->camera.y += camSpeed * dt;
	//DOWN
	if ((App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		&& App->render->camera.y >= downMargin)
		App->render->camera.y -= camSpeed * dt;
	//LEFT
	if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		&& App->render->camera.x <= 0)
		App->render->camera.x += camSpeed * dt;
	//RIGHT
	if ((App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
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

void j1Scene::ShowSelectedUnits(list<DynamicEntity*> units)
{
	App->player->HideEntitySelectedInfo();
	HideUnselectedUnits();
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
	commandPatrolButton->isActive = true;
	commandStopButton->isActive = true;
}


void j1Scene::HideUnselectedUnits()
{
	//One Selection
	if (App->player->entitySelectedStats.entitySelected != nullptr)
	{
		App->player->entitySelectedStats.entitySight->isActive = false;
		App->player->entitySelectedStats.entityRange->isActive = false;
		App->player->entitySelectedStats.entityDamage->isActive = false;
		App->player->entitySelectedStats.entitySpeed->isActive = false;
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
	ChangeBuildingButtState(&elem->barracks);
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
	ChangeMenuIconsText(buildingMenuButtons.mageTower.icon, mageTowerCost, { 496,202,50,41 }, { 496,202,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.stables.icon, stablesCost, { 343,160,50,41 }, { 343,160,50,41 });
	ChangeMenuIconsText(buildingMenuButtons.scoutTower.icon, scoutTowerCost, { 394,34,50,41 }, { 445,34,50,41 });
	//Only one construction for each one
	ChangeMenuIconsText(buildingMenuButtons.barracks.icon, barracksCost, { 394,34,50,41 }, { 445,34,50,41 }, true, App->player->barracks);
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
	ChangeMenuLabelInfo(buildingMenuButtons.mageTower.cost, mageTowerCost);
	ChangeMenuLabelInfo(buildingMenuButtons.stables.cost, stablesCost);
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
			"Requires Keep", { 645, 155 }, { 645, 172 }, gryphonAviaryCost, &buildingMenuButtons.gryphonAviary);
		
		CreateBuildingElements( { 496,202,50,41 }, { 585, 190 }, "Mage Tower",
			"Cost: 1000 gold", { 645, 200 }, { 645, 217 }, mageTowerCost, &buildingMenuButtons.mageTower);
		
		CreateBuildingElements( { 496,34,50,41 }, { 585, 235 }, "Scout Tower",
			"Cost: 400 gold", { 645, 245 }, { 645, 262 }, scoutTowerCost, &buildingMenuButtons.scoutTower);
		
		CreateBuildingElements({ 496,76,50,41 }, { 585, 280 }, "Guard Tower",
			"Cost: 600 gold", { 645, 290 }, { 645, 307 }, guardTowerCost, &buildingMenuButtons.guardTower);
		
		CreateBuildingElements({ 496,118,50,41 }, { 585, 325 }, "Cannon Tower",
			"Cost: 600 gold", { 645, 335 }, { 645, 352 }, cannonTowerCost, &buildingMenuButtons.cannonTower);

		CreateBuildingElements({ 547,160,50,41 }, { 585, 370 }, "Barracks",
			"Cost: 1000 gold", { 645, 380 }, { 645, 397 }, barracksCost, &buildingMenuButtons.barracks);
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
	DeleteBuildingElements(&buildingMenuButtons.stables);
	DeleteBuildingElements(&buildingMenuButtons.gryphonAviary);
	DeleteBuildingElements(&buildingMenuButtons.mageTower);
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

	App->gui->RemoveElem((UIElement**)&settingsButt);
	App->gui->RemoveElem((UIElement**)&ReturnMenuButt);
	App->gui->RemoveElem((UIElement**)&continueButt);
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	App->gui->RemoveElem((UIElement**)&continueLabel);
	App->gui->RemoveElem((UIElement**)&ReturnMenuLabel);
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
	//TODO: Search the same pos and lenght
	string text;
	switch (dialogEvent)
	{
	case TerenasDialog_START:
		text = "Welcome adventurers of Azeroth's armies! You have been sent to Draenor to rescue the members from the legendary Alliance expedition and defeat Ner'zhul to reclaim the artifacts from Azeroth and avoid caos. FOR THE ALLIANCE!";
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
		text = "To get gold from the mine you have to select units and they will gather it.";
		terenasAdvices.text->SetText(text, 320);
		terenasAdvices.text->SetLocalPos({ 355,47 });
		break;
	case TerenasDialog_RUNESTONE:
		//TODO ??
		//text = "To get gold from the mine you have to select units and they will gather it.";
		//terenasAdvices.text->SetText(text, 320);
		//terenasAdvices.text->SetLocalPos({ 355,47 });
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

	case AdviceMessage_ROOM_CLEAR:
		text = "ROOM CLEARED!";
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
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_CHICKEN_FARM;
				}
				else if (App->player->GetCurrentGold() < chickenFarmCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			else if (UIelem == buildingMenuButtons.stables.icon) {
				if (App->player->GetCurrentGold() >= stablesCost) {
					//App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					//alphaBuilding = EntityType_STABLES;
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
				else if (App->player->GetCurrentGold() < stablesCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}


			else if (UIelem == buildingMenuButtons.gryphonAviary.icon && App->player->gryphonAviary == nullptr && App->player->townHallUpgrade && App->player->townHall->buildingState == BuildingState_Normal) {
				if (App->player->GetCurrentGold() >= gryphonAviaryCost) {

					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_GRYPHON_AVIARY;
				}
				else if (App->player->GetCurrentGold() < gryphonAviaryCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}
			else if (UIelem == buildingMenuButtons.gryphonAviary.icon)
				App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound

			else if (UIelem == buildingMenuButtons.mageTower.icon) {
				if (App->player->GetCurrentGold() >= mageTowerCost) {
					//App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					//alphaBuilding = EntityType_MAGE_TOWER;
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
				}
				else if (App->player->GetCurrentGold() < mageTowerCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			else if (UIelem == buildingMenuButtons.scoutTower.icon) {
				if (App->player->GetCurrentGold() >= scoutTowerCost) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_SCOUT_TOWER;
				}
				else if (App->player->GetCurrentGold() < scoutTowerCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			else if (UIelem == buildingMenuButtons.guardTower.icon) {
				if (App->player->GetCurrentGold() >= guardTowerCost) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_PLAYER_GUARD_TOWER;
				}
				else if (App->player->GetCurrentGold() < guardTowerCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			else if (UIelem == buildingMenuButtons.cannonTower.icon) {
				if (App->player->GetCurrentGold() >= cannonTowerCost) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_PLAYER_CANNON_TOWER;
				}
				else if (App->player->GetCurrentGold() < cannonTowerCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}

			else if (UIelem == buildingMenuButtons.barracks.icon && App->player->barracks == nullptr) {
				if (App->player->GetCurrentGold() >= barracksCost) {
					App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
					ChangeBuildingMenuState(&buildingMenuButtons);
					alphaBuilding = EntityType_BARRACKS;
				}
				else if (App->player->GetCurrentGold() < barracksCost)
					App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound
			}
			else if (UIelem == buildingMenuButtons.barracks.icon)
				App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound

		}

		if (UIelem == pauseMenuButt) {
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

		else if (UIelem == continueButt) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_DESTROY;
		}

		else if (UIelem == ReturnMenuButt) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_RETURN_MENU;
		}

		else if (UIelem == settingsButt) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			pauseMenuActions = PauseMenuActions_SETTINGS_MENU;
		}

		else if (UIelem == returnButt) {
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

	if ((buttonSaveGame = (SDL_Scancode)buttons.attribute("buttonSaveGame").as_int()) ==  SDL_SCANCODE_UNKNOWN)
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