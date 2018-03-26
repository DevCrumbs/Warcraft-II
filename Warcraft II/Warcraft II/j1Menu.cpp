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

#include "j1Gui.h"
#include "UIImage.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UISlider.h"
#include "j1FadeToBlack.h"
#include "j1Menu.h"


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



	return ret;
}

// Called before the first frame
bool j1Menu::Start()
{

	CreateMenu();
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

	return true;
}

// Called each loop iteration
bool j1Menu::PostUpdate()
{
	bool ret = true;

	switch (menuActions)
	{
	case MenuActions_NONE:
		break;
	case MenuActions_EXIT:
		ret = false;
		break;
	case MenuActions_PLAY:
		App->fade->FadeToBlack(this, App->scene);
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_SETTINGS:
		DeteleMenu();
		CreateSettings();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_RETURN:
		DeleteSettings();
		CreateMenu();
		menuActions = MenuActions_NONE;
		break;
	default:
		break;
	}

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{
	DeteleMenu();

	App->map->active = true;
	App->scene->active = true;
	App->player->active = true;
	App->entities->active = true;
	App->collision->active = true;
	App->pathfinding->active = true;

	App->map->Start();
	App->player->Start();
	App->entities->Start();
	App->collision->Start();
	App->pathfinding->Start();

	active = false;

	return true;
}

void j1Menu::CreateMenu() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1000, 0, 129, 33 };
	PlayButt = App->gui->CreateUIButton({ 500, 275 }, buttonInfo, this, nullptr);
	SettingsButt = App->gui->CreateUIButton({ 500, 350 }, buttonInfo, this, nullptr);
	ExitButt = App->gui->CreateUIButton({ 500, 425 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.text = "Play";
	PlayLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w/2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, PlayButt);

	labelInfo.text = "Exit";
	ExitLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, ExitButt);

	labelInfo.text = "Settings";
	SettingsLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, SettingsButt);

	UISlider_Info sliderInfo;
	sliderInfo.button_slider_area = { 0,0,30,30 };
	sliderInfo.tex_area = { 0,130,400,30 };
	FPSString = App->gui->CreateUISlider({ 50,50 }, sliderInfo, this);
}

void j1Menu::CreateSettings() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1000, 0, 129, 33 };
	ReturnButt = App->gui->CreateUIButton({ 500, 425 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;

	labelInfo.text = "Return";
	ReturnLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, ReturnButt);

}

void j1Menu::DeleteSettings() {

	App->gui->DestroyElement(ReturnButt);
	App->gui->DestroyElement(ReturnLabel);
	App->gui->DestroyElement(AudioFXButt);
	App->gui->DestroyElement(AudioFXLabel);
	App->gui->DestroyElement(AudioMusicButt);
	App->gui->DestroyElement(AudioMusicLabel);
	App->gui->DestroyElement(FPSString);
	App->gui->DestroyElement(FPSLabel);
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

		if (UIelem == PlayButt) 
			menuActions = MenuActions_PLAY;
		
		else if (UIelem == ExitButt) 
			menuActions = MenuActions_EXIT;
		
		else if (UIelem == SettingsButt) 
			menuActions = MenuActions_SETTINGS;

		else if(UIelem == ReturnButt)
			menuActions = MenuActions_RETURN;

		break;
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		break;
	case UI_EVENT_MAX_EVENTS:
		break;
	default:
		break;
	}

}
void j1Menu::DeteleMenu() {

	App->gui->DestroyElement(PlayButt);
	App->gui->DestroyElement(PlayLabel);
	App->gui->DestroyElement(ExitButt);
	App->gui->DestroyElement(ExitLabel);
	App->gui->DestroyElement(SettingsButt);
	App->gui->DestroyElement(SettingsLabel);
	
}
