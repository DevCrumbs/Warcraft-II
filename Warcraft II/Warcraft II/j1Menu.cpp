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
	
	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		parchment = App->particles->AddParticle(App->particles->parchmentAnimation, (App->render->camera.w / 2) - 100, (App->render->camera.h / 2) - 125);
	
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
		UpdateSlider(AudioFX);
		break;
	case MenuActions_SLIDERMUSIC:
		UpdateSlider(AudioMusic);
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

	AddSlider(AudioFX, { 50,100 }, "Audio FX", (float)App->audio->fxVolume / MAX_AUDIO_VOLUM);
	AddSlider(AudioMusic, { 50,200 }, "Audio Music", (float)App->audio->musicVolume / MAX_AUDIO_VOLUM);

}

void j1Menu::AddSlider(SliderStruct &sliderStruct, iPoint pos, string nameText, float relativeNumberValue) {

	UILabel_Info labelInfo;
	UISlider_Info sliderInfo;
	sliderInfo.button_slider_area = { 0,0,30,30 };
	sliderInfo.tex_area = { 0,130,400,30 };
	sliderStruct.slider = App->gui->CreateUISlider(pos, sliderInfo, this);
	sliderStruct.slider->SetRelativePos(relativeNumberValue);

	labelInfo.text = nameText;
	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.verticalOrientation = VERTICAL_POS_BOTTOM;
	int x = (sliderInfo.tex_area.w / 2) + sliderStruct.slider->GetLocalPos().x;
	int y = sliderStruct.slider->GetLocalPos().y;
	sliderStruct.name = App->gui->CreateUILabel({ x, y }, labelInfo, this);

	static char fpsText[5];
	sprintf_s(fpsText, 5, "%.0f", relativeNumberValue * 100);
	labelInfo.text = fpsText;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	x = sliderInfo.tex_area.w + sliderStruct.slider->GetLocalPos().x + 10;
	y = sliderStruct.slider->GetLocalPos().y + (sliderInfo.tex_area.h / 2);
	sliderStruct.value = App->gui->CreateUILabel({ x, y }, labelInfo, this);


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

		if (UIelem == PlayButt) 
			menuActions = MenuActions_PLAY;
		
		else if (UIelem == ExitButt) 
			menuActions = MenuActions_EXIT;
		
		else if (UIelem == SettingsButt) 
			menuActions = MenuActions_SETTINGS;

		else if(UIelem == ReturnButt)
			menuActions = MenuActions_RETURN;

		else if (UIelem == AudioFX.slider) 
			menuActions = MenuActions_SLIDERFX;
		
		else if (UIelem == AudioMusic.slider) 
			menuActions = MenuActions_SLIDERMUSIC;

		break;
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == AudioFX.slider || UIelem == AudioMusic.slider)
			menuActions = MenuActions_NONE;
		break;
	case UI_EVENT_MAX_EVENTS:

		break;
	default:

		break;
	}

}
void j1Menu::DeteleMenu() {

	App->gui->DestroyElement((UIElement**)&PlayButt);
	App->gui->DestroyElement((UIElement**)&PlayLabel);
	App->gui->DestroyElement((UIElement**)&ExitButt);
	App->gui->DestroyElement((UIElement**)&ExitLabel);
	App->gui->DestroyElement((UIElement**)&SettingsButt);
	App->gui->DestroyElement((UIElement**)&SettingsLabel);
	
}

void j1Menu::DeleteSettings() {

	App->gui->DestroyElement((UIElement**)&ReturnButt);
	App->gui->DestroyElement((UIElement**)&ReturnLabel);
	App->gui->DestroyElement((UIElement**)&AudioFX.name);
	App->gui->DestroyElement((UIElement**)&AudioFX.value);
	App->gui->DestroyElement((UIElement**)&AudioFX.slider);
	App->gui->DestroyElement((UIElement**)&AudioMusic.name);
	App->gui->DestroyElement((UIElement**)&AudioMusic.value);
	App->gui->DestroyElement((UIElement**)&AudioMusic.slider);
}

