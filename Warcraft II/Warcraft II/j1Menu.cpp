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

#ifdef _WIN32	
#include <shellapi.h>
void open_url(const string& url)
{
	ShellExecute(GetActiveWindow(), "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
#else
void open_url(const string& url) {}
#endif


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
	LoadKeysVector();
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
		DeleteNewGame();
		App->audio->PlayFx(App->audio->GetFX().gameStart, 0); //Button sound
		isFadetoScene = true;
		App->scene->mapDifficulty = menuActions;
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_LOADGAME:
		//Todo OSCAR LoadGame
		break;
	case MenuActions_SETTINGS:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeteleMenu();
		DeleteChangingButtons();
		CreateSettings();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_RETURN:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeleteSettings();
		DeleteNewGame();
		DeleteCredits();
		CreateMenu();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_CREDITS:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeteleMenu();
		CreateCredits();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_NEWGAME:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeteleMenu();
		CreateNewGame();
		menuActions = MenuActions_NONE;
		break;
	case MenuActions_EXIT:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		isExit = true;
		break;
	case MenuActions_CHANGE_BUTTONS:
		App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		DeleteSettings();
		CreateChangingButtons();
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

	if (changeButt.changeLabel != nullptr)
	{
		if (!changeButtonTimer.IsStarted())
			changeButtonTimer.Start();

		if (changeButtonTimer.Read() > 250)
		{
			SDL_Color color{ 0,0,0,0 };

			if (textColor)
				color = changeButt.changeLabel->GetInfo()->hoverColor;
			else
				color = changeButt.changeLabel->GetInfo()->normalColor;

			changeButt.changeLabel->SetColor(color);
			textColor = !textColor;
			changeButtonTimer.Stop();
		}

		if (App->input->scancode != SDL_SCANCODE_UNKNOWN && App->input->GetKey(App->input->scancode) == KEY_DOWN)
		{
			bool isChanged = false;
			if (App->input->scancode < keysName.size())
			{
				if (CanChangeButt(App->input->scancode) && CheckCorrectButt(App->input->scancode))
				{
					changeButt.changeLabel->SetText(keysName[App->input->scancode]);
					*changeButt.currentButton = App->input->scancode;
					isChanged = true;
				}
				else if (CanSwapButt(App->input->scancode))
				{
					isChanged = true;
				}
			}

			if (isChanged) {
				changeButt.changeLabel->SetColor(changeButt.changeLabel->GetInfo()->normalColor);
				changeButt.changeLabel = nullptr;
				App->input->scancode = SDL_SCANCODE_UNKNOWN;
				App->audio->PlayFx(App->audio->GetFX().changeKey);
			}
			else
				App->audio->PlayFx(App->audio->GetFX().errorButt, 0); //Button error sound

		}
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
	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;
	labelInfo.text = "New Game";
	int y = 290;
	playLabel = App->gui->CreateUILabel({ 660, y }, labelInfo, this);

	y += 50;
	labelInfo.text = "Load Game";
	loadLabel = App->gui->CreateUILabel({ 660, y }, labelInfo, this);

	y += 50;
	labelInfo.text = "Settings";
	settingsLabel = App->gui->CreateUILabel({ 660, y }, labelInfo, this);

	y += 50;
	labelInfo.text = "Credits";
	creditsLabel = App->gui->CreateUILabel({ 660, y }, labelInfo, this);

	y += 50;
	labelInfo.text = "Quit Game";
	exitLabel = App->gui->CreateUILabel({ 660, y }, labelInfo, this);

	artifacts.push_back(AddArtifact({ 50, 475 }, App->gui->bookText, App->gui->bookAnim, 5));
	artifacts.push_back(AddArtifact({ 175,525 }, App->gui->skullText, App->gui->skullAnim, 5));
	artifacts.push_back(AddArtifact({ 300,525 }, App->gui->eyeText, App->gui->eyeAnim, 5));
	artifacts.push_back(AddArtifact({ 425,475 }, App->gui->scepterText, App->gui->scepterAnim, 5));

	//Logo
	UIImage_Info imageInfo;
	imageInfo.texArea = { 1039,740,345,141 };
	logoImg = App->gui->CreateUIImage({ 95,370 }, imageInfo, this, nullptr);
	
	//Background
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

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Return";
	returnLabel = App->gui->CreateUILabel({ 650, 550 }, labelInfo, this);

	float relativeVol = (float)App->audio->fxVolume / MAX_AUDIO_VOLUM;
	SDL_Rect butText = { 834,328,26,30 };
	SDL_Rect bgText = { 434,328,400,30 };
	AddSlider(audioFX, { 175,200 }, "Audio FX", relativeVol, butText, bgText, this);

	relativeVol = (float)App->audio->musicVolume / MAX_AUDIO_VOLUM;
	AddSlider(audioMusic, { 175,300 }, "Audio Music", relativeVol, butText, bgText, this);

	labelInfo.text = "Controls";
	buttonsLabel = App->gui->CreateUILabel({ 375, 550 }, labelInfo, this);

	UIButton_Info buttonInfo;
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
	labelInfo.interactive = false;
	fullScreenLabel = App->gui->CreateUILabel({ 250, 450 }, labelInfo, this);

	artifacts.push_back(AddArtifact({ 100,125 }, App->gui->bookText, App->gui->bookAnim, 5));
	artifacts.push_back(AddArtifact({ 275, 50 }, App->gui->skullText, App->gui->skullAnim, 5));
	artifacts.push_back(AddArtifact({ 450, 50 }, App->gui->eyeText, App->gui->eyeAnim, 5));
	artifacts.push_back(AddArtifact({ 625,125 }, App->gui->scepterText, App->gui->scepterAnim, 5));


}

void j1Menu::CreateChangingButtons() {

	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Back";
	returnSettings = App->gui->CreateUILabel({ 450, 575 }, labelInfo, this);


	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.interactive = false;

	labelInfo.text = "Select all Footman on screen";
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 100 }, labelInfo, this));
	labelInfo.text = "Select all Archer on screen";	  
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 180 }, labelInfo, this));
	labelInfo.text = "Select all Gryphon on screen";  
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 260 }, labelInfo, this));
	labelInfo.text = "Select all Units on screen";
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 340 }, labelInfo, this));
	labelInfo.text = "Go to base";					 
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 420 }, labelInfo, this));
	labelInfo.text = "Go to unitis selected";
	staticLabels.push_back(App->gui->CreateUILabel({ 175, 500 }, labelInfo, this));
	labelInfo.text = "Change minimap zoom";
	staticLabels.push_back(App->gui->CreateUILabel({ 575, 180 }, labelInfo, this));
	labelInfo.text = "Open building menu";
	staticLabels.push_back(App->gui->CreateUILabel({ 575, 260 }, labelInfo, this));
	labelInfo.text = "Open pause menu";
	staticLabels.push_back(App->gui->CreateUILabel({ 575, 340 }, labelInfo, this));
	labelInfo.text = "Patrol units";
	staticLabels.push_back(App->gui->CreateUILabel({ 575, 420 }, labelInfo, this));
	labelInfo.text = "Stop units";
	staticLabels.push_back(App->gui->CreateUILabel({ 575, 500 }, labelInfo, this));

	labelInfo.normalColor = White_;
	labelInfo.interactive = true;						 
	labelInfo.text = keysName[*App->player->buttonSelectFootman];		//"Select all Footman on screen";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 100 }, labelInfo, this), App->player->buttonSelectFootman });
	labelInfo.text = keysName[*App->player->buttonSelectArcher];	// "Select all Archer on screen
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 180 }, labelInfo, this), App->player->buttonSelectArcher });
	labelInfo.text = keysName[*App->player->buttonSelectGryphon];	// "Select all Gryphon on screen;
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 260 }, labelInfo, this), App->player->buttonSelectGryphon });
	labelInfo.text = keysName[*App->player->buttonSelectAll];	// "Select all Units on scree
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 340 }, labelInfo, this), App->player->buttonSelectAll });
	labelInfo.text = keysName[*App->scene->buttonGoToBase];	// "Go to base";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 420 }, labelInfo, this), App->scene->buttonGoToBase });
	labelInfo.text = keysName[*App->scene->buttonGoToUnits];	// "Go to unities selected";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 350, 500 }, labelInfo, this), App->scene->buttonGoToUnits });
	labelInfo.text = keysName[*App->scene->buttonMinimap];	// "Change minimap zoom";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 700, 180 }, labelInfo, this), App->scene->buttonMinimap });
	labelInfo.text = keysName[*App->scene->buttonBuildingMenu];	//	  "Open building menu";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 700, 260 }, labelInfo, this), App->scene->buttonBuildingMenu });
	labelInfo.text = keysName[*App->scene->buttonPauseMenu];	//	  "Open pause menu";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 700, 340 }, labelInfo, this), App->scene->buttonPauseMenu });
	labelInfo.text = keysName[*App->scene->buttonPatrolUnits];	//	  "Patrol units";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 700, 420 }, labelInfo, this), App->scene->buttonPatrolUnits });
	labelInfo.text = keysName[*App->scene->buttonStopUnits];	//	  "Stop units";
	interactiveLabels.push_back({ App->gui->CreateUILabel({ 700, 500 }, labelInfo, this), App->scene->buttonStopUnits });

}


void j1Menu::CreateNewGame()
{
	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.normalColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Back";
	returnLabel = App->gui->CreateUILabel({ 650, 550 }, labelInfo, this);


	//Choose levels butt
	CreateSimpleButt({ 1807,   0, 175, 135 }, { 1983,   0, 175, 135 }, { 2159,   0, 175, 135 }, { 170,175 }, easyOneButt, HORIZONTAL_POS_CENTER);
	CreateSimpleButt({ 1807, 136, 175, 135 }, { 1983, 136, 175, 135 }, { 2159, 136, 175, 135 }, { 170,350 }, easyTwoButt, HORIZONTAL_POS_CENTER);
	CreateSimpleButt({ 1807, 272, 175, 135 }, { 1983, 272, 175, 135 }, { 2159, 272, 175, 135 }, { 420,175 }, mediumOneButt, HORIZONTAL_POS_CENTER);
	CreateSimpleButt({ 1807, 408, 175, 135 }, { 1983, 408, 175, 135 }, { 2159, 408, 175, 135 }, { 420,350 }, mediumTwoButt, HORIZONTAL_POS_CENTER);
	CreateSimpleButt({ 1807, 544, 175, 135 }, { 1983, 544, 175, 135 }, { 2159, 544, 175, 135 }, { 675,275 }, hardButt, HORIZONTAL_POS_CENTER);

	labelInfo.interactive = false;

	labelInfo.text = "Choose your level";
	staticLabels.push_back(App->gui->CreateUILabel({ 375, 50 }, labelInfo, this));

	labelInfo.text = "Easy";
	labelInfo.normalColor = ColorGreen;
	staticLabels.push_back(App->gui->CreateUILabel({ 170, 150 }, labelInfo, this));
	labelInfo.text = "Medium";
	labelInfo.normalColor = ColorYellow;
	staticLabels.push_back(App->gui->CreateUILabel({ 420, 150 }, labelInfo, this));
	labelInfo.text = "Hard";
	labelInfo.normalColor = ColorRed;
	staticLabels.push_back(App->gui->CreateUILabel({ 675, 250 }, labelInfo, this));

	labelInfo.normalColor = White_;
	labelInfo.text = "-";
	bookArtifact = App->gui->CreateUILabel({ 125, 555 }, labelInfo, this);
	skullArtifact = App->gui->CreateUILabel({ 250, 555 }, labelInfo, this);
	eyeArtifact = App->gui->CreateUILabel({ 375, 555 }, labelInfo, this);
	scepterArtifact = App->gui->CreateUILabel({ 500, 555 }, labelInfo, this);

	artifacts.push_back(AddArtifact({ 50, 525 }, App->gui->bookText, App->gui->bookAnim, 5));
	artifacts.push_back(AddArtifact({ 175,525 }, App->gui->skullText, App->gui->skullAnim, 5));
	artifacts.push_back(AddArtifact({ 300,525 }, App->gui->eyeText, App->gui->eyeAnim, 5));
	artifacts.push_back(AddArtifact({ 425,525 }, App->gui->scepterText, App->gui->scepterAnim, 5));
}

void j1Menu::CreateCredits()
{
	UILabel_Info labelInfo;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;
	labelInfo.pressedColor = Black_;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Sandra Alvarez";
	sandraLead = App->gui->CreateUILabel({ 500, 100 }, labelInfo, this);
	labelInfo.text = "Óscar Hernández";
	oscarCode = App->gui->CreateUILabel({ 500, 175 }, labelInfo, this);
	labelInfo.text = "Manav Lakhwani";
	manavManagment = App->gui->CreateUILabel({ 500, 250 }, labelInfo, this);
	labelInfo.text = "Joan Valiente";
	joanDesigner = App->gui->CreateUILabel({ 500, 325 }, labelInfo, this);
	labelInfo.text = "David Valdivia";
	davidQA = App->gui->CreateUILabel({ 500, 400 }, labelInfo, this);
	labelInfo.text = "David Varela";
	davidArt = App->gui->CreateUILabel({ 500, 475 }, labelInfo, this);
	labelInfo.text = "Aleix Gabarró";
	aleixUI = App->gui->CreateUILabel({ 500, 550 }, labelInfo, this);

	labelInfo.pressedColor = White_;
	labelInfo.normalColor = Black_;
	labelInfo.text = "Return";
	returnLabel = App->gui->CreateUILabel({ 650, 550 }, labelInfo, this);

	labelInfo.interactive = false;
	labelInfo.text = "Lead";
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 100 }, labelInfo, this));
	labelInfo.text = "Code";						 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 175 }, labelInfo, this));
	labelInfo.text = "Management";					 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 250 }, labelInfo, this));
	labelInfo.text = "Game Designer";							 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 325 }, labelInfo, this));
	labelInfo.text = "QA";				 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 400 }, labelInfo, this));
	labelInfo.text = "Art";							 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 475 }, labelInfo, this));
	labelInfo.text = "UI";							 
	staticLabels.push_back(App->gui->CreateUILabel({ 200, 550 }, labelInfo, this));

	labelInfo.fontName = FONT_NAME_WARCRAFT30;
	labelInfo.text = "Contributors";
	staticLabels.push_back(App->gui->CreateUILabel({ 375, 25 }, labelInfo, this));

}

void j1Menu::CreateSimpleButt(SDL_Rect normal, SDL_Rect hover, SDL_Rect click, iPoint pos, UIButton* &butt, UIE_HORIZONTAL_POS hPos, UIE_VERTICAL_POS vPos)
{

	UIButton_Info infoButton;
	infoButton.horizontalOrientation = hPos;
	infoButton.verticalOrientation = vPos;
	infoButton.normalTexArea = normal;
	infoButton.hoverTexArea = hover;
	infoButton.pressedTexArea = click;
	butt = App->gui->CreateUIButton(pos, infoButton, this);

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

UIImage* j1Menu::AddArtifact(iPoint pos, SDL_Rect textArea, Animation anim, int speed) 
{
	UIImage* retImage;

	UIImage_Info imageInfo;
	imageInfo.texArea = textArea;
	retImage = App->gui->CreateUIImage(pos, imageInfo);
	anim.speed = speed;
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

bool j1Menu::CanSwapButt(SDL_Scancode button)
{
	bool ret = false;

	for (list<ChangeButtons>::iterator iterator = interactiveLabels.begin(); iterator != interactiveLabels.end(); ++iterator)
	{
		if (*(*iterator).currentButton == App->input->scancode)
		{
			SwapButt((*iterator), changeButt);
			ret = true;
			break;
		}
	}
	return ret;
}

void j1Menu::SwapButt(ChangeButtons &buttonA, ChangeButtons &buttonB)
{
	SDL_Scancode aux = *buttonA.currentButton;
	*buttonA.currentButton = *buttonB.currentButton;
	*buttonB.currentButton = aux;

	string aText = buttonA.changeLabel->GetText();
	buttonA.changeLabel->SetText(buttonB.changeLabel->GetText());
	buttonB.changeLabel->SetText(aText);
}

bool j1Menu::CanChangeButt(SDL_Scancode button)
{
	bool ret = true;

	if (keysName[button] == "?")
		ret = false;

	return ret;
}

bool j1Menu::CheckCorrectButt(SDL_Scancode button)
{
	bool ret = true;

	for (list<SDL_Scancode*>::iterator iterator = App->input->inGameKeys.begin(); iterator != App->input->inGameKeys.end(); ++iterator)
	{
		if (*(*iterator) == button) {
			ret = false;
			break;
		}
	}
	return ret;
}

void j1Menu::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {

	switch (UIevent)
	{
	case UI_EVENT_NONE:
		break;
	case UI_EVENT_MOUSE_ENTER:

		if (UIelem == easyOneButt)
		{
			bookArtifact->SetText(to_string(artifactsEasyOne.book));
			skullArtifact->SetText(to_string(artifactsEasyOne.skull));
			eyeArtifact->SetText(to_string(artifactsEasyOne.eye));
			scepterArtifact->SetText(to_string(artifactsEasyOne.scepter));
		}
		else if (UIelem == easyTwoButt)
		{
			bookArtifact->SetText(to_string(artifactsEasyTwo.book));
			skullArtifact->SetText(to_string(artifactsEasyTwo.skull));
			eyeArtifact->SetText(to_string(artifactsEasyTwo.eye));
			scepterArtifact->SetText(to_string(artifactsEasyTwo.scepter));
		}
		else if (UIelem == mediumOneButt)
		{
			bookArtifact->SetText(to_string(artifactsMediumOne.book));
			skullArtifact->SetText(to_string(artifactsMediumOne.skull));
			eyeArtifact->SetText(to_string(artifactsMediumOne.eye));
			scepterArtifact->SetText(to_string(artifactsMediumOne.scepter));
		}
		else if (UIelem == mediumTwoButt)
		{
			bookArtifact->SetText(to_string(artifactsMediumTwo.book));
			skullArtifact->SetText(to_string(artifactsMediumTwo.skull));
			eyeArtifact->SetText(to_string(artifactsMediumTwo.eye));
			scepterArtifact->SetText(to_string(artifactsMediumTwo.scepter));
		}
		else if (UIelem == hardButt)
		{
			bookArtifact->SetText(to_string(artifactsHard.book));
			skullArtifact->SetText(to_string(artifactsHard.skull));
			eyeArtifact->SetText(to_string(artifactsHard.eye));
			scepterArtifact->SetText(to_string(artifactsHard.scepter));
		}
		break;
	case UI_EVENT_MOUSE_LEAVE:
		if (UIelem == easyOneButt || UIelem == easyTwoButt || UIelem == mediumOneButt || UIelem == mediumTwoButt || UIelem == hardButt)
		{
			bookArtifact->SetText("-");
			skullArtifact->SetText("-");
			eyeArtifact->SetText("-");
			scepterArtifact->SetText("-");
		}
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (UIelem == playLabel){
			menuActions = MenuActions_NEWGAME;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		else if (UIelem == exitLabel){
			menuActions = MenuActions_EXIT;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		else if (UIelem == settingsLabel || UIelem == returnSettings){
			menuActions = MenuActions_SETTINGS;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		else if (UIelem == creditsLabel) {
			menuActions = MenuActions_CREDITS;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		else if (UIelem == loadLabel) {
			menuActions = MenuActions_LOADGAME;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		
		else if (UIelem == returnLabel) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			menuActions = MenuActions_RETURN;
		}

		else if (UIelem == audioFX.slider) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			menuActions = MenuActions_SLIDERFX;
		}

		else if (UIelem == audioMusic.slider){
			menuActions = MenuActions_SLIDERMUSIC;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		else if (UIelem == fullScreenButt)
		{
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			App->win->SetFullscreen();
		}

		//NewGame
		else if (UIelem == easyOneButt)
		{
			menuActions = MenuActions_PLAY_EASYONE;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == easyTwoButt)
		{
			menuActions = MenuActions_PLAY_EASYTWO;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == mediumOneButt)
		{
			menuActions = MenuActions_PLAY_MEDIUMONE;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == mediumTwoButt)
		{
			menuActions = MenuActions_PLAY_MEDIUMTWO;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == hardButt)
		{
			menuActions = MenuActions_PLAY_HARD;
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		//Credits

		else if (UIelem == sandraLead)
		{
			open_url("https://github.com/Sandruski");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == oscarCode)
		{
			open_url("https://github.com/OscarHernandezG");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == joanDesigner)
		{
			open_url("https://github.com/JoanValiente");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == davidQA)
		{
			open_url("https://github.com/ValdiviaDev");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == manavManagment)
		{
			open_url("https://github.com/manavld");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == davidArt)
		{
			open_url("https://github.com/lFreecss");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}
		else if (UIelem == aleixUI)
		{
			open_url("https://github.com/aleixgab");
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
		}

		//ChangeButtons

		else if (UIelem == buttonsLabel)
		{
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			menuActions = MenuActions_CHANGE_BUTTONS;
		}
		for (list<ChangeButtons>::iterator iterator = interactiveLabels.begin(); iterator != interactiveLabels.end(); ++iterator)
		{
			if (UIelem == (*iterator).changeLabel) {
				changeButt.changeLabel = (UILabel*)UIelem;
				changeButt.currentButton = (*iterator).currentButton;
				App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
				break;
			}
		}

		break;
	case UI_EVENT_MOUSE_RIGHT_UP:
		break;
	case UI_EVENT_MOUSE_LEFT_UP:
		if (UIelem == audioFX.slider || UIelem == audioMusic.slider) {
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
	App->gui->RemoveElem((UIElement**)&exitLabel);
	App->gui->RemoveElem((UIElement**)&settingsLabel);
	App->gui->RemoveElem((UIElement**)&creditsLabel);
	App->gui->RemoveElem((UIElement**)&loadLabel);

	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}
}

void j1Menu::DeleteSettings() {

	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenLabel);
	App->gui->RemoveElem((UIElement**)&fullScreenButt);
	App->gui->RemoveElem((UIElement**)&audioFX.name);
	App->gui->RemoveElem((UIElement**)&audioFX.value);
	App->gui->RemoveElem((UIElement**)&audioFX.slider);
	App->gui->RemoveElem((UIElement**)&audioMusic.name);
	App->gui->RemoveElem((UIElement**)&audioMusic.value);
	App->gui->RemoveElem((UIElement**)&audioMusic.slider); 
	App->gui->RemoveElem((UIElement**)&buttonsLabel);

	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}
}

void j1Menu::DeleteChangingButtons()
{

	App->gui->RemoveElem((UIElement**)&returnSettings);
	App->gui->RemoveElem((UIElement**)&changeButt);

	for (; !interactiveLabels.empty(); interactiveLabels.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&interactiveLabels.back().changeLabel);
	}
	for (; !staticLabels.empty(); staticLabels.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&staticLabels.back());
	}

}


void j1Menu::DeleteNewGame()
{
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&easyOneButt);
	App->gui->RemoveElem((UIElement**)&easyTwoButt);
	App->gui->RemoveElem((UIElement**)&mediumTwoButt);
	App->gui->RemoveElem((UIElement**)&mediumOneButt);
	App->gui->RemoveElem((UIElement**)&hardButt);
	App->gui->RemoveElem((UIElement**)&bookArtifact);
	App->gui->RemoveElem((UIElement**)&skullArtifact);
	App->gui->RemoveElem((UIElement**)&eyeArtifact);
	App->gui->RemoveElem((UIElement**)&scepterArtifact);

	for (; !staticLabels.empty(); staticLabels.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&staticLabels.back());
	}
	for (; !artifacts.empty(); artifacts.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&artifacts.back());
	}
}

void j1Menu::DeleteCredits()
{
	App->gui->RemoveElem((UIElement**)&returnLabel);
	App->gui->RemoveElem((UIElement**)&sandraLead);
	App->gui->RemoveElem((UIElement**)&oscarCode);
	App->gui->RemoveElem((UIElement**)&manavManagment);
	App->gui->RemoveElem((UIElement**)&joanDesigner);
	App->gui->RemoveElem((UIElement**)&davidQA);
	App->gui->RemoveElem((UIElement**)&davidArt);
	App->gui->RemoveElem((UIElement**)&aleixUI);

	for (; !staticLabels.empty(); staticLabels.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&staticLabels.back());
	}

}


void j1Menu::LoadKeysVector()
{
	if (keysName.empty())
		keysName.clear();

	for (int i = 0; i < 4; ++i)
		keysName.push_back("?");

	keysName.push_back(" A ");
	keysName.push_back(" B ");
	keysName.push_back(" C ");
	keysName.push_back(" D ");
	keysName.push_back(" E ");
	keysName.push_back(" F ");
	keysName.push_back(" G ");
	keysName.push_back(" H ");
	keysName.push_back(" I ");
	keysName.push_back(" J ");
	keysName.push_back(" K ");
	keysName.push_back(" L ");
	keysName.push_back(" M ");
	keysName.push_back(" N ");
	keysName.push_back(" O ");
	keysName.push_back(" P ");
	keysName.push_back(" Q ");
	keysName.push_back(" R ");
	keysName.push_back(" S ");
	keysName.push_back(" T ");
	keysName.push_back(" U ");
	keysName.push_back(" V ");
	keysName.push_back(" W ");
	keysName.push_back(" X ");
	keysName.push_back(" Y ");
	keysName.push_back(" Z ");
						 
	keysName.push_back(" 1 ");
	keysName.push_back(" 2 ");
	keysName.push_back(" 3 ");
	keysName.push_back(" 4 ");
	keysName.push_back(" 5 ");
	keysName.push_back(" 6 ");
	keysName.push_back(" 7 ");
	keysName.push_back(" 8 ");
	keysName.push_back(" 9 ");
	keysName.push_back(" 0 ");

	keysName.push_back("ENTER");
	keysName.push_back("ESC");
	keysName.push_back("BACKSPACE");
	keysName.push_back("TAB");
	keysName.push_back("SPACE");

	//NOT 100% REAL DEPENDS KEYBOARD
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");

	keysName.push_back("GRAVE");

	keysName.push_back("?");
	keysName.push_back("?");
	keysName.push_back("?");

	keysName.push_back("CAPSLOCK");

	keysName.push_back("F1");
	keysName.push_back("F2");
	keysName.push_back("F3");
	keysName.push_back("F4");
	keysName.push_back("F5");
	keysName.push_back("F6");
	keysName.push_back("F7");
	keysName.push_back("F8");
	keysName.push_back("F9");
	keysName.push_back("F10");
	keysName.push_back("F11");
	keysName.push_back("F12");

	keysName.push_back("PRTSC");
	keysName.push_back("SCRLK");
	keysName.push_back("PAUSE");
	keysName.push_back("INS");
	keysName.push_back("HOME");
	keysName.push_back("PGUP");
	keysName.push_back("DEL");
	keysName.push_back("END");
	keysName.push_back("PGDN");

	for (int i = 0; i < 4; ++i)
		keysName.push_back("?");

	//KeyPad
	keysName.push_back("NUMLOCK");
	keysName.push_back("KP /");
	keysName.push_back("KP *");
	keysName.push_back("KP -");
	keysName.push_back("KP +");	
	keysName.push_back("KP ENTER");
	keysName.push_back("KP 1");
	keysName.push_back("KP 2");
	keysName.push_back("KP 3");
	keysName.push_back("KP 4");
	keysName.push_back("KP 5");
	keysName.push_back("KP 6");
	keysName.push_back("KP 7");
	keysName.push_back("KP 8");
	keysName.push_back("KP 9");
	keysName.push_back("KP 0");
	keysName.push_back("KP .");
	keysName.push_back(" < ");

	for (int i = 0; i <	16; ++i)
		keysName.push_back("?");

	// Useless media keys
	for (int i = 0; i < 29; ++i)
		keysName.push_back("?");
	

	// Useless chinese keys
	for (int i = 0; i < 78; ++i)
		keysName.push_back("?");
	

	keysName.push_back("LCTRL");
	keysName.push_back("LSHIFT");
	keysName.push_back("LALT");
	keysName.push_back("LWIN");
	keysName.push_back("RCTRL");
	keysName.push_back("RSHIFT");
	keysName.push_back("RALT");
	keysName.push_back("RWIN");
}