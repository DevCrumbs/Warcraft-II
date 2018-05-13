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
#include "j1Printer.h"

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

	// Music
	pugi::xml_node audio = config.child("audioPaths");

	mainMenuMusicName = audio.child("mainTheme").attribute("path").as_string();

	return ret;
}

// Called before the first frame
bool j1Menu::Start()
{
	active = true;
	
	App->audio->PlayMusic(mainMenuMusicName.data(), 0.0f);

	App->render->camera.x = App->render->camera.y = 0;

	//Background
	UIImage_Info imageInfo;
	imageInfo.texArea = { 1722, 950, 800, 600 };
	settingsBackground = App->gui->CreateUIImage({ 0, 0 }, imageInfo, this, nullptr);
	settingsBackground->SetPriorityDraw(PriorityDraw_FRAMEWORK);

	CreateMenu();

	if (!isMouseTextCreated) {

		UICursor_Info mouseInfo;
		mouseInfo.default = { 243, 525, 28, 33 };
		mouseInfo.onClick = { 275, 525, 28, 33 };
		mouseInfo.onMine = { 310, 525, 28, 33 };
		mouseInfo.onMineClick = { 338, 525, 28, 33 };
		mouseInfo.onEnemies = { 374, 527, 28, 33 };
		mouseInfo.onEnemiesClick = { 402, 527, 28, 33 };
		mouseText = App->gui->CreateUICursor(mouseInfo, this);

		mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });

		isMouseTextCreated = true;
	}

	return true;
}

// Called each loop iteration
bool j1Menu::PreUpdate()
{
	switch (menuActions)
	{
	case MenuActions_SLIDERFX:
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
bool j1Menu::Update(float dt)
{

	switch (menuActions)
	{
	case MenuActions_NONE:
		break;
	case MenuActions_PLAY_EASYONE:
	case MenuActions_PLAY_EASYTWO:
	case MenuActions_PLAY_MEDIUMONE:
	case MenuActions_PLAY_MEDIUMTWO:
	case MenuActions_PLAY_HARD:
		DestroyNewGame();
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		isFadetoScene = true;
		App->scene->mapDifficulty = menuActions;
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_SETTINGS:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeteleMenu();
		CreateSettings();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_RETURN:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeleteSettings();
		DestroyNewGame();
		CreateMenu();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_NEWGAME:
		DeteleMenu();
		CreateNewGame();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_EXIT:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		isExit = true;
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

	// Blit the background
	//App->render->DrawQuad({ 0,0,(int)App->render->camera.w, (int)App->render->camera.h }, 70, 70, 70, 255);
	//SDL_Color brown = { 70,70,70,255 };  150-113   1920 - 1080   175 - 
	//App->printer->PrintQuad({ 0,0,(int)App->render->camera.w, (int)App->render->camera.h }, brown, true, false, Layers_PreviewBuildingsQuad);

	if (isFadetoScene) {
		App->fade->FadeToBlack(this, App->scene);
		isFadetoScene = false;
	}
	if (isExit)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Menu::CleanUp()
{
	bool ret = true;

	if (!App->gui->isGuiCleanUp) {
		DeteleMenu();
		App->gui->RemoveElem((UIElement**)&settingsBackground);
	}

	active = false;

	return ret;
}

void j1Menu::CreateMenu() 
{
	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	playButt = App->gui->CreateUIButton({ 590, 307 }, buttonInfo, this, nullptr);
	settingsButt = App->gui->CreateUIButton({ 590, 382 }, buttonInfo, this, nullptr);
	exitButt = App->gui->CreateUIButton({ 590, 457 }, buttonInfo, this, nullptr);

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "New Game";
	playLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w/2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, playButt);

	labelInfo.text = "Quit Game";
	exitLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, exitButt);

	labelInfo.text = "Settings";
	settingsLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, settingsButt);

	artifacts.push_back(AddArtifact({ 50, 475 }, App->gui->bookText, App->gui->bookAnim));
	artifacts.push_back(AddArtifact({ 175,525 }, App->gui->skullText, App->gui->skullAnim));
	artifacts.push_back(AddArtifact({ 300,525 }, App->gui->eyeText, App->gui->eyeAnim));
	artifacts.push_back(AddArtifact({ 425,475 }, App->gui->scepterText, App->gui->scepterAnim));

	UIImage_Info imageInfo;
	imageInfo.texArea = { 1039,740,345,141 };
	logoImg = App->gui->CreateUIImage({ 95,370 }, imageInfo, this, nullptr);
	
	imageInfo.texArea = { 0,954,776,600 };
	mainMenuImg = App->gui->CreateUIImage({ 0,0 }, imageInfo, this, nullptr);
	menuImgAnim.speed = 5;
	menuImgAnim.PushBack({ 0,954,800,600 });
	menuImgAnim.PushBack({ 829,951,800,600 });
	menuImgAnim.PushBack({ 0,1564,800,600 });
	menuImgAnim.PushBack({ 876,1563,800,600 });
	mainMenuImg->StartAnimation(menuImgAnim);
	mainMenuImg->SetPriorityDraw(PriorityDraw_FRAMEWORK);
}

void j1Menu::CreateSettings() {

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	returnButt = App->gui->CreateUIButton({ 575, 550 }, buttonInfo, this, nullptr);

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

void j1Menu::CreateNewGame()
{
	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 20000, 0, 140, 33 };
	buttonInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	returnButt = App->gui->CreateUIButton({ 645, 550 }, buttonInfo, this, nullptr);


	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Return";
	returnLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, returnButt);

	easyOneButt = App->gui->CreateUIButton({ 170, 200 }, buttonInfo, this, nullptr);
	easyTwoButt = App->gui->CreateUIButton({ 170, 500 }, buttonInfo, this, nullptr);
	mediumOneButt = App->gui->CreateUIButton({ 420, 200 }, buttonInfo, this, nullptr);
	mediumTwoButt = App->gui->CreateUIButton({ 420, 500 }, buttonInfo, this, nullptr);
	hardButt = App->gui->CreateUIButton({ 645, 400 }, buttonInfo, this, nullptr);

	labelInfo.text = "Easy One";
	easyOneLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, easyOneButt);
	labelInfo.text = "Easy Two";
	easyTwoLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, easyTwoButt);
	labelInfo.text = "Medium One";
	mediumOneLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, mediumOneButt);
	labelInfo.text = "Hardddd";
	hardLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, hardButt);
	labelInfo.text = "Medium Two";
	mediumTwoLabel = App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, mediumTwoButt);

	UIImage_Info imageInfo;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	imageInfo.texArea = { 0, 50, 175, 131 };
	artifacts.push_back(App->gui->CreateUIImage({ 170,50 }, imageInfo));

	imageInfo.texArea = { 0, 50, 175, 131 };
	artifacts.push_back(App->gui->CreateUIImage({ 170,350 }, imageInfo));

	imageInfo.texArea = { 0, 50, 175, 131 };
	artifacts.push_back(App->gui->CreateUIImage({ 420,50 }, imageInfo));

	imageInfo.texArea = { 0, 50, 175, 131 };
	artifacts.push_back(App->gui->CreateUIImage({ 420,350 }, imageInfo));

	imageInfo.texArea = { 0, 50, 175, 131 };
	artifacts.push_back(App->gui->CreateUIImage({ 645,250 }, imageInfo));

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

UIImage* j1Menu::AddArtifact(iPoint pos, SDL_Rect textArea, Animation anim) 
{
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
		if (UIelem == playButt)
			playLabel->SetColor(ColorGreen,true);
		else if (UIelem == exitButt)
			exitLabel->SetColor(ColorGreen, true);

		else if (UIelem == settingsButt)
			settingsLabel->SetColor(ColorGreen, true);

		else if (UIelem == returnButt)
			returnLabel->SetColor(ColorGreen, true);

		else if (UIelem == easyOneButt)
			easyOneLabel->SetColor(ColorGreen, true);
		else if (UIelem == easyTwoButt)
			easyTwoLabel->SetColor(ColorGreen, true);
		else if (UIelem == mediumOneButt)
			mediumOneLabel->SetColor(ColorGreen, true);
		else if (UIelem == mediumTwoButt)
			mediumTwoLabel->SetColor(ColorGreen, true);
		else if (UIelem == hardButt)
			hardLabel->SetColor(ColorGreen, true);

		break;
	case UI_EVENT_MOUSE_LEAVE:
		if (UIelem == playButt)
			playLabel->SetColor(Black_, true);
		else if (UIelem == exitButt)
			exitLabel->SetColor(Black_, true);

		else if (UIelem == settingsButt)
			settingsLabel->SetColor(Black_, true);

		else if (UIelem == returnButt)
			returnLabel->SetColor(Black_, true);

		else if (UIelem == easyOneButt)
			easyOneLabel->SetColor(Black_, true);
		else if (UIelem == easyTwoButt)
			easyTwoLabel->SetColor(Black_, true);
		else if (UIelem == mediumOneButt)
			mediumOneLabel->SetColor(Black_, true);
		else if (UIelem == mediumTwoButt)
			mediumTwoLabel->SetColor(Black_, true);
		else if (UIelem == hardButt)
			hardLabel->SetColor(Black_, true);

		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (UIelem == playButt) 
			menuActions = MenuActions_NEWGAME;
		
		else if (UIelem == exitButt) 
			menuActions = MenuActions_EXIT;
		
		else if (UIelem == settingsButt) 
			menuActions = MenuActions_SETTINGS;

		else if(UIelem == returnButt)
			menuActions = MenuActions_RETURN;

		else if (UIelem == audioFX.slider) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			menuActions = MenuActions_SLIDERFX;
		}
		
		else if (UIelem == audioMusic.slider) 
			menuActions = MenuActions_SLIDERMUSIC;

		else if (UIelem == fullScreenButt)
		{
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			App->win->SetFullscreen();
		}

		else if (UIelem == easyOneButt)
		{
			menuActions = MenuActions_PLAY_EASYONE;
		}
		else if (UIelem == easyTwoButt)
		{
			menuActions = MenuActions_PLAY_EASYTWO;
		}
		else if (UIelem == mediumOneButt)
		{
			menuActions = MenuActions_PLAY_MEDIUMONE;
		}
		else if (UIelem == mediumTwoButt)
		{
			menuActions = MenuActions_PLAY_MEDIUMTWO;
		}
		else if (UIelem == hardButt)
		{
			menuActions = MenuActions_PLAY_HARD;
		}

		break;
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == audioFX.slider || UIelem == audioMusic.slider) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			menuActions = MenuActions_NONE;
		}
		break;
	case UI_EVENT_MAX_EVENTS:

		break;
	default:

		break;
	}
}
void j1Menu::DeteleMenu() 
{
	App->gui->RemoveElem((UIElement**)&mainMenuImg);
	App->gui->RemoveElem((UIElement**)&logoImg);
	App->gui->RemoveElem((UIElement**)&playLabel);
	App->gui->RemoveElem((UIElement**)&playButt);
	App->gui->RemoveElem((UIElement**)&exitLabel);
	App->gui->RemoveElem((UIElement**)&exitButt);
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	App->gui->RemoveElem((UIElement**)&settingsButt);

	
	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}
}

void j1Menu::DeleteSettings() {

	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&returnButt);
	App->gui->RemoveElem((UIElement**)&fullScreenLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenButt);
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

void j1Menu::DestroyNewGame()
{
	App->gui->RemoveElem((UIElement**)&easyTwoLabel);
	App->gui->RemoveElem((UIElement**)&easyOneLabel);
	App->gui->RemoveElem((UIElement**)&hardLabel);
	App->gui->RemoveElem((UIElement**)&mediumOneLabel);
	App->gui->RemoveElem((UIElement**)&mediumTwoLabel);
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&returnButt);
	App->gui->RemoveElem((UIElement**)&easyOneButt);
	App->gui->RemoveElem((UIElement**)&easyTwoButt);
	App->gui->RemoveElem((UIElement**)&mediumTwoButt);
	App->gui->RemoveElem((UIElement**)&mediumOneButt);
	App->gui->RemoveElem((UIElement**)&hardButt);

	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}
}
