#include "p2Log.h"

#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Collision.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Particles.h"
#include "j1FadeToBlack.h"
#include "j1Menu.h"
#include "j1PathManager.h"
#include "j1Movement.h"

#include "j1Gui.h"
#include "UIImage.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UICursor.h"


#include <time.h>
#include <chrono>
#include <ctime>
#include <iostream>




j1Menu::j1Menu() : j1Module()
{
	name.assign("menu");
}

// Destructor
j1Menu::~j1Menu()
{}

// Called before render is available
bool j1Menu::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//Music
	pugi::xml_node audio = config.child("audioPaths");

	mainMenuMusicName = audio.child("mainTheme").attribute("path").as_string();

	//Sounds
	pugi::xml_node sounds = audio.child("sounds");

	pugi::xml_node uIButtonsSounds = sounds.child("buttonPaths");
	mainButtonSound = uIButtonsSounds.attribute("menuButton").as_string();
	errorButtonSound = uIButtonsSounds.attribute("errorBttn").as_string();

	pugi::xml_node buildingSounds = sounds.child("buildingPaths");
	buildingConstructionSound = buildingSounds.attribute("buildingConstruction").as_string();
	buildingErrorButtonSound = buildingSounds.attribute("errorBttn").as_string();
	chickenFarmSound = buildingSounds.attribute("chickenFarm").as_string();
	goldMineSound = buildingSounds.attribute("goldMine").as_string();
	gryphonAviarySound = buildingSounds.attribute("gryphAviar").as_string();
	mageTowerSound = buildingSounds.attribute("mageTower").as_string();
	stablesSound = buildingSounds.attribute("stables").as_string();
	repairBuildingSound = buildingSounds.attribute("repair").as_string();
	destroyBuildingSound = buildingSounds.attribute("buildingDestroy").as_string(); 

	pugi::xml_node unitsSounds = sounds.child("unitsPaths");
	humanDeadSound = unitsSounds.attribute("humanDeadSound").as_string();
	orcDeadSound = unitsSounds.attribute("orcDeadSound").as_string();
	prisonerRescueSound = unitsSounds.attribute("prisonerRescue").as_string();

	pugi::xml_node crittersSounds = sounds.child("crittersPaths");
	crittersBoarDead = crittersSounds.attribute("boarDead").as_string();
	crittersSheepDead = crittersSounds.attribute("sheepDead").as_string();

	pugi::xml_node archerSounds = sounds.child("archerPaths");
	archerGoToPlaceSound = archerSounds.attribute("goToPlace").as_string();
	archerReadySound = archerSounds.attribute("ready").as_string();
	archerSelectedSound = archerSounds.attribute("selected").as_string();

	pugi::xml_node footmanSounds = sounds.child("footmanPaths");
	footmanGoToPlaceSound = footmanSounds.attribute("goToPlace").as_string();
	footmanReadySound = footmanSounds.attribute("ready").as_string();
	footmanSelectedSound = footmanSounds.attribute("selected").as_string();

	pugi::xml_node attackSounds = sounds.child("attackPaths");
	axeThrowSound = attackSounds.attribute("axeThrow").as_string();
	bowFireSound = attackSounds.attribute("bowFire").as_string();
	swordSound = attackSounds.attribute("sword").as_string();

	return ret;
}

// Called before the first frame
bool j1Menu::Start()
{
	App->audio->PlayMusic(mainMenuMusicName.data(), 0.0f);

	//If it is the first code iteration, change all the sounds
	if(!App->isSoundCharged)
	ChargeGameSounds();

	App->render->camera.x = App->render->camera.y = 0;

	CreateMenu();

	UICursor_Info mouseInfo;
	mouseInfo.default = { 243, 525, 28, 33 };
	mouseInfo.onClick = { 275, 525, 28, 33 };
	mouseInfo.onMine = { 310, 525, 28, 33 };
	mouseInfo.onMineClick = { 338, 525, 28, 33 };
	mouseInfo.onEnemies = { 374, 527, 28, 33 };
	mouseInfo.onEnemiesClick = { 402, 527, 28, 33 };
	mouseText = App->gui->CreateUICursor(mouseInfo, this);
	return true;
}

// Called each loop iteration
bool j1Menu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Menu::Update(float dt)
{
	App->render->DrawQuad({ 0,0,(int)App->render->camera.w, (int)App->render->camera.h }, 100, 100, 100, 255);

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) {
		if (parchment != nullptr) {
			parchment->isRemove = true;
			parchment = nullptr;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		App->audio->PlayFx(14, 0);
	}

	switch (menuActions)
	{
	case MenuActions_NONE:
		break;
	case MenuActions_EXIT:
		App->audio->PlayFx(1, 0); //Button sound
		isExit = true;
		break;
	case MenuActions_PLAY:
		App->audio->PlayFx(1, 0); //Button sound
		isFadetoScene = true;
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_SETTINGS:
		App->audio->PlayFx(1, 0); //Button sound
		DeteleMenu();
		CreateSettings();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_RETURN:
		App->audio->PlayFx(1, 0); //Button sound
		DeleteSettings();
		CreateMenu();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_SLIDERFX:
		App->audio->PlayFx(1, 0); //Button sound
		UpdateSlider(audioFX);
		break;
	case MenuActions_SLIDERMUSIC:
		UpdateSlider(audioMusic);
		break;
	default:
		break;
	}
	return true;
}

// Called each loop iteration
bool j1Menu::PostUpdate()
{
	bool ret = true;

	if (isFadetoScene) {
		App->fade->FadeToBlack(this, App->scene);
		isFadetoScene = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || isExit)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{
	if (active) {

		DeteleMenu();

		App->map->active = true;
		App->scene->active = true;
		App->player->active = true;
		App->entities->active = true;
		App->collision->active = true;
		App->pathfinding->active = true;
		App->pathmanager->active = true;
		App->movement->active = true;

		App->player->Start();
		App->entities->Start();
		App->collision->Start();
		App->pathfinding->Start();
	}

	active = false;

	return true;
}

void j1Menu::CreateMenu() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	playButt = App->gui->CreateUIButton({ 600, 350 }, buttonInfo, this, nullptr);
	settingsButt = App->gui->CreateUIButton({ 600, 425 }, buttonInfo, this, nullptr);
	exitButt = App->gui->CreateUIButton({ 600, 500 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "Start Demo";
	playLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w/2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, playButt);

	labelInfo.text = "Quit Game";
	exitLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, exitButt);

	labelInfo.text = "Settings";
	settingsLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, settingsButt);

	artifacts.push_back(AddArtifact({ 50,450 }, App->gui->bookText, App->gui->bookAnim));
	artifacts.push_back(AddArtifact({ 175,500 }, App->gui->skullText, App->gui->skullAnim));
	artifacts.push_back(AddArtifact({ 300,500 }, App->gui->eyeText, App->gui->eyeAnim));
	artifacts.push_back(AddArtifact({ 425,450 }, App->gui->scepterText, App->gui->scepterAnim));

}

void j1Menu::CreateSettings() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	returnButt = App->gui->CreateUIButton({ 600, 500 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Return";
	returnLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, returnButt);

	float relativeVol = (float)App->audio->fxVolume / MAX_AUDIO_VOLUM;
	SDL_Rect butText = { 834,328,26,30 };
	SDL_Rect bgText = { 434,328,400,30 };
	AddSlider(audioFX, { 175,200 }, "Audio FX", relativeVol, butText, bgText, this);

	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	AddSlider(audioMusic, { 175,300 }, "Audio Music", relativeVol, butText, bgText, this);


	//Fullscreen
	if (!App->win->fullscreen) {
		buttonInfo.normalTexArea = buttonInfo.hoverTexArea = { 434, 370, 30, 30 };
		buttonInfo.pressedTexArea = { 466, 370, 30, 30 };
	}
	else {
		buttonInfo.normalTexArea = buttonInfo.hoverTexArea = { 466, 370, 30, 30 };
		buttonInfo.pressedTexArea = { 434, 370, 30, 30 };
	}
	buttonInfo.checkbox = true;
	buttonInfo.verticalOrientation = VERTICAL_POS_CENTER;
	fullScreenButt = App->gui->CreateUIButton({ 450, 450 }, buttonInfo, this);

	labelInfo.text = "Fullscreen";
	labelInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;

	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = Black_;
	fullScreenLabel = App->gui->CreateUILabel({ 250, 450 }, labelInfo, this);

	artifacts.push_back(AddArtifact({ 100,125 }, App->gui->bookText, App->gui->bookAnim));
	artifacts.push_back(AddArtifact({ 275, 50 }, App->gui->skullText, App->gui->skullAnim));
	artifacts.push_back(AddArtifact({ 450, 50 }, App->gui->eyeText, App->gui->eyeAnim));
	artifacts.push_back(AddArtifact({ 625,125 }, App->gui->scepterText, App->gui->scepterAnim));
}

void j1Menu::AddSlider(SliderStruct &sliderStruct, iPoint pos, string nameText, float relativeNumberValue, SDL_Rect buttText, SDL_Rect bgText, j1Module* listener) {

	UILabel_Info labelInfo;
	UISlider_Info sliderInfo;
	sliderInfo.button_slider_area = buttText;
	sliderInfo.tex_area = bgText;
	sliderStruct.slider = App->gui->CreateUISlider(pos, sliderInfo, listener);
	sliderStruct.slider->SetRelativePos(relativeNumberValue);

	labelInfo.text = nameText;
	if(active)
		labelInfo.fontName = FONT_NAME_WARCRAFT20;
	else
		labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.hoverColor = labelInfo.normalColor = labelInfo.pressedColor = Black_;
	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	int x = (sliderInfo.tex_area.w / 2) + sliderStruct.slider->GetLocalPos().x;
	int y = sliderStruct.slider->GetLocalPos().y;
	sliderStruct.name = App->gui->CreateUILabel({ x, y }, labelInfo, listener);

	static char fpsText[5];
	sprintf_s(fpsText, 5, "%.0f", relativeNumberValue * 100);
	labelInfo.text = fpsText;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	x = sliderInfo.tex_area.w + sliderStruct.slider->GetLocalPos().x + 5;
	y = sliderStruct.slider->GetLocalPos().y + (sliderInfo.tex_area.h / 2);
	sliderStruct.value = App->gui->CreateUILabel({ x, y }, labelInfo, listener);
}

UIImage* j1Menu::AddArtifact(iPoint pos, SDL_Rect textArea, Animation anim) {
	UIImage* retImage;

	UIImage_Info imageInfo;
	imageInfo.texArea = textArea;
	retImage = App->gui->CreateUIImage(pos, imageInfo);
	retImage->StartAnimation(anim);

	return retImage;
}


void j1Menu::UpdateSlider(SliderStruct &sliderStruct) {
	float volume = sliderStruct.slider->GetRelativePosition();
	if(sliderStruct.name->GetText() == "Audio FX")
		App->audio->SetFxVolume(volume * MAX_AUDIO_VOLUM);
	else
		App->audio->SetMusicVolume(volume * MAX_AUDIO_VOLUM);
	static char vol_text[4];
	sprintf_s(vol_text, 4, "%.0f", volume * 100);
	sliderStruct.value->SetText(vol_text);
	//LOG("%f", volume);
}

void j1Menu::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {

	switch (UIevent)
	{
	case UI_EVENT_NONE:
		break;
	case UI_EVENT_MOUSE_ENTER:
		break;
	case UI_EVENT_MOUSE_LEAVE:
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (UIelem == playButt) 
			menuActions = MenuActions_PLAY;
		
		else if (UIelem == exitButt) 
			menuActions = MenuActions_EXIT;
		
		else if (UIelem == settingsButt) 
			menuActions = MenuActions_SETTINGS;

		else if(UIelem == returnButt)
			menuActions = MenuActions_RETURN;

		else if (UIelem == audioFX.slider) 
			menuActions = MenuActions_SLIDERFX;
		
		else if (UIelem == audioMusic.slider) 
			menuActions = MenuActions_SLIDERMUSIC;

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
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == audioFX.slider || UIelem == audioMusic.slider)
			menuActions = MenuActions_NONE;
		break;
	case UI_EVENT_MAX_EVENTS:

		break;
	default:

		break;
	}

}
void j1Menu::DeteleMenu() {

	App->gui->RemoveElem((UIElement**)&playButt);
	App->gui->RemoveElem((UIElement**)&playLabel);
	App->gui->RemoveElem((UIElement**)&exitButt);
	App->gui->RemoveElem((UIElement**)&exitLabel);
	App->gui->RemoveElem((UIElement**)&settingsButt);
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	
	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}

}

void j1Menu::DeleteSettings() {

	App->gui->RemoveElem((UIElement**)&returnButt);
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenButt);
	App->gui->RemoveElem((UIElement**)&fullScreenLabel);
	App->gui->RemoveElem((UIElement**)&audioFX.name);
	App->gui->RemoveElem((UIElement**)&audioFX.value);
	App->gui->RemoveElem((UIElement**)&audioFX.slider);
	App->gui->RemoveElem((UIElement**)&audioMusic.name);
	App->gui->RemoveElem((UIElement**)&audioMusic.value);
	App->gui->RemoveElem((UIElement**)&audioMusic.slider);

	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}

}

void j1Menu::ChargeGameSounds()
{
	App->audio->LoadFx(mainButtonSound.data()); //1 Normal bttn sound
	App->audio->LoadFx(buildingConstructionSound.data()); //2 Construction building
	App->audio->LoadFx(errorButtonSound.data()); //3 Normal error bttn sound
	App->audio->LoadFx(buildingErrorButtonSound.data()); //4 Building placement error sound
	App->audio->LoadFx(chickenFarmSound.data()); //5 chicken farm sound
	App->audio->LoadFx(goldMineSound.data()); //6 gold mine sound
	App->audio->LoadFx(gryphonAviarySound.data()); //7 gryphon aviary sound
	App->audio->LoadFx(mageTowerSound.data()); //8 mage tower sound
	App->audio->LoadFx(stablesSound.data()); //9 stables sound
	App->audio->LoadFx(repairBuildingSound.data()); //10 repair building sound
	App->audio->LoadFx(destroyBuildingSound.data()); //11 destroy building sound

	App->audio->LoadFx(humanDeadSound.data()); //12
	App->audio->LoadFx(orcDeadSound.data()); //13
	App->audio->LoadFx(prisonerRescueSound.data()); //14
	App->audio->LoadFx(crittersBoarDead.data()); //15
	App->audio->LoadFx(crittersSheepDead.data()); //16
	App->audio->LoadFx(archerGoToPlaceSound.data()); //17
	App->audio->LoadFx(archerReadySound.data()); //18
	App->audio->LoadFx(archerSelectedSound.data()); //19
	App->audio->LoadFx(footmanGoToPlaceSound.data()); //20
	App->audio->LoadFx(footmanReadySound.data()); //21
	App->audio->LoadFx(footmanSelectedSound.data()); //22
	App->audio->LoadFx(axeThrowSound.data()); //23
	App->audio->LoadFx(bowFireSound.data()); //24
	App->audio->LoadFx(swordSound.data()); //25

	App->isSoundCharged = true;
}

