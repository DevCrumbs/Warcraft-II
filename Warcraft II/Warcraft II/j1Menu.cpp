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
	App->render->camera.x = App->render->camera.y = 0;
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
	
	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) {
		if (parchment != nullptr) {
			parchment->isDeleted = true;
			parchment = nullptr;
		}
	}
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
	case MenuActions_SLIDERFX:
		UpdateSlider(audioFX);
		break;
	case MenuActions_SLIDERMUSIC:
		UpdateSlider(audioMusic);
		break;
	default:
		break;
	}
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

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
	playButt = App->gui->CreateUIButton({ 600, 350 }, buttonInfo, this, nullptr);
	settingsButt = App->gui->CreateUIButton({ 600, 425 }, buttonInfo, this, nullptr);
	exitButt = App->gui->CreateUIButton({ 600, 500 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "Play";
	playLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w/2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, playButt);

	labelInfo.text = "Exit";
	exitLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, exitButt);

	labelInfo.text = "Settings";
	settingsLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, settingsButt);

}

void j1Menu::CreateSettings() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1000, 0, 129, 33 };
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
	AddSlider(audioFX, { 175,100 }, "Audio FX", relativeVol, butText, bgText, this);

	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	AddSlider(audioMusic, { 175,200 }, "Audio Music", relativeVol, butText, bgText, this);


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
	fullScreenButt = App->gui->CreateUIButton({ 450, 350 }, buttonInfo, this);

	labelInfo.text = "Fullscreen";
	labelInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;

	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = Black_;
	fullScreenLabel = App->gui->CreateUILabel({ 250, 350 }, labelInfo, this);
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

void j1Menu::UpdateSlider(SliderStruct &sliderStruct) {
	float volume = sliderStruct.slider->GetRelativePosition();
	App->audio->SetMusicVolume(volume * MAX_AUDIO_VOLUM);
	static char vol_text[4];
	sprintf_s(vol_text, 4, "%.0f", volume * 100);
	sliderStruct.value->SetText(vol_text);
	LOG("%f", volume);
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

	App->gui->DestroyElement((UIElement**)&playButt);
	App->gui->DestroyElement((UIElement**)&playLabel);
	App->gui->DestroyElement((UIElement**)&exitButt);
	App->gui->DestroyElement((UIElement**)&exitLabel);
	App->gui->DestroyElement((UIElement**)&settingsButt);
	App->gui->DestroyElement((UIElement**)&settingsLabel);
	
}

void j1Menu::DeleteSettings() {

	App->gui->DestroyElement((UIElement**)&returnButt);
	App->gui->DestroyElement((UIElement**)&returnLabel);
	App->gui->DestroyElement((UIElement**)&fullScreenButt);
	App->gui->DestroyElement((UIElement**)&fullScreenLabel);
	App->gui->DestroyElement((UIElement**)&audioFX.name);
	App->gui->DestroyElement((UIElement**)&audioFX.value);
	App->gui->DestroyElement((UIElement**)&audioFX.slider);
	App->gui->DestroyElement((UIElement**)&audioMusic.name);
	App->gui->DestroyElement((UIElement**)&audioMusic.value);
	App->gui->DestroyElement((UIElement**)&audioMusic.slider);
}

