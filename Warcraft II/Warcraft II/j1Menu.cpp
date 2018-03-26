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

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {

		App->map->active = true;
		App->scene->active = true;
		App->player->active = true;
		App->entities->active = true;
		App->collision->active = true;
		App->pathfinding->active = true;

		App->map->Start();
		App->scene->Start();
		App->player->Start();
		App->entities->Start();	  
		App->collision->Start();
		App->pathfinding->Start();

		active = false;

	}
	return true;
}

// Called each loop iteration
bool j1Menu::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{

	return true;
}

void j1Menu::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {


}


void j1Menu::CreateMenu() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1000, 0, 129, 33 };
	//buttonInfo.hoverTexArea = { 129, 0, 129, 33 };
	buttonInfo.pressedTexArea = { 257, 0, 129, 33 };
	PlayButt = App->gui->CreateUIButton({ 500, 350 }, buttonInfo, this, nullptr);
	ExitButt = App->gui->CreateUIButton({ 500, 450 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	///labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.text = "Play";
	PlayLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w/2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, PlayButt);

	labelInfo.text = "Exit";
	ExitLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, ExitButt);

	
}

void j1Menu::CreateSettings() {

	
}

void j1Menu::CreateLoading() {

}

void j1Menu::CleanUI() {


}