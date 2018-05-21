#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1FinishGame.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"

#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UICursor.h"
#include "j1Menu.h"

j1FinishGame::j1FinishGame()
{
	name.assign("finish");
}


j1FinishGame::~j1FinishGame()
{}

bool j1FinishGame::Awake(pugi::xml_node& config) {

	//Music paths
	victoryMusicPath = config.child("audioPaths").child("victoryScreen").attribute("path").as_string();
	defeatMusicPath = config.child("audioPaths").child("defeatScreen").attribute("path").as_string();
	
	//Win Backgorund
	winBG = { config.child("winBackground").child("sprite").attribute("x").as_int(), config.child("winBackground").child("sprite").attribute("y").as_int(),
		      config.child("winBackground").child("sprite").attribute("w").as_int(), config.child("winBackground").child("sprite").attribute("h").as_int() };
	//Lose background
	loseBG = { config.child("loseBackground").child("sprite").attribute("x").as_int(), config.child("loseBackground").child("sprite").attribute("y").as_int(),
		      config.child("loseBackground").child("sprite").attribute("w").as_int(), config.child("loseBackground").child("sprite").attribute("h").as_int() };

	//Win flag
	pugi::xml_node winFlag = config.child("winFlag");
	winFlagAnimation.speed = winFlag.attribute("speed").as_float();
	winFlagAnimation.loop = winFlag.attribute("loop").as_bool();
	for (winFlag = winFlag.child("frame"); winFlag; winFlag = winFlag.next_sibling("frame")) {
		winFlagAnimation.PushBack({ winFlag.attribute("x").as_int(), winFlag.attribute("y").as_int(), winFlag.attribute("w").as_int(), winFlag.attribute("h").as_int() });
	}

	//Lose flag
	pugi::xml_node loseFlag = config.child("loseFlag");
	loseFlagAnimation.speed = loseFlag.attribute("speed").as_float();
	loseFlagAnimation.loop = loseFlag.attribute("loop").as_bool();
	for (loseFlag = loseFlag.child("frame"); loseFlag; loseFlag = loseFlag.next_sibling("frame")) {
		loseFlagAnimation.PushBack({ loseFlag.attribute("x").as_int(), loseFlag.attribute("y").as_int(), loseFlag.attribute("w").as_int(), loseFlag.attribute("h").as_int() });
	}


	return true;
}

// Load assets
bool j1FinishGame::Start()
{
	App->finish->active = true;

	// Get screen size
	App->render->camera.x = App->render->camera.y = 0;
	uint width = 0, height = 0, scale = 0;
	
	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	screen = { 0, 0, static_cast<int>(width * scale), static_cast<int>(height * scale) };

	LoadSceneOne(App->player->isWin);

	bg = App->tex->Load(bgTexName.data());

	App->menu->mouseText->SetTexArea({ 243, 525, 28, 33 }, { 275, 525, 28, 33 });

	//DeleteScreen();
	return true;
}

// Update: draw background
bool j1FinishGame::Update(float dt)
{

	return true;
}

bool j1FinishGame::CleanUp() 
{
	bool ret = true;

	if (!App->gui->isGuiCleanUp)
		DeleteScene();

	//Delete background
	App->gui->RemoveElem((UIElement**)&background);
	App->gui->RemoveElem((UIElement**)&flag);
	
	active = false;

	return ret;
}

void j1FinishGame::LoadSceneOne(bool isWin) {
	
	UILabel_Info labelInfo;

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	if (isWin) {
		App->audio->PlayMusic(victoryMusicPath.data(), 0.0f); //Music
	  //get an Artifact 
		ArtifactWon(App->player->startGameTimer.ReadSec());

		labelInfo.text = "Congratulations! You have defeated the Horde!";
		labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorBlue;
	}
	else if (!isWin) {
		App->audio->PlayMusic(defeatMusicPath.data(), 0.0f); //Music
		labelInfo.text = "Oh, no! You have been defeated by the Horde!";
		labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorRed;
	}

	labelVector.push_back(App->gui->CreateUILabel({ screen.w / 2, 50 }, labelInfo));

	labelInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	labelInfo.verticalOrientation = VERTICAL_POS_CENTER;

	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = White_;
	labelInfo.fontName = FONT_NAME_WARCRAFT;

	labelInfo.text = "Units produced: "; 
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 175 }, labelInfo));
										    
	labelInfo.text = "Gold gathered: ";	    
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 225 }, labelInfo));
										    
	labelInfo.text = "Enemies killed: ";    
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 325 }, labelInfo));
										  
	labelInfo.text = "Buildings destroyed: ";
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 375 }, labelInfo));

	labelInfo.text = "Total time: ";	    
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 475 }, labelInfo));

	UIImage_Info imageInfo;
	imageInfo.texArea = { 328,384,100,40 };
	imageInfo.verticalOrientation = VERTICAL_POS_CENTER;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;

	imageVector.push_back(App->gui->CreateUIImage({ 275, 175 }, imageInfo));
	imageVector.push_back(App->gui->CreateUIImage({ 275, 225 }, imageInfo));
	imageVector.push_back(App->gui->CreateUIImage({ 275, 325 }, imageInfo));
	imageVector.push_back(App->gui->CreateUIImage({ 275, 375 }, imageInfo));
	imageVector.push_back(App->gui->CreateUIImage({ 275, 475 }, imageInfo));

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;

	labelInfo.text = to_string(App->player->unitProduce);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 175 }, labelInfo));

	labelInfo.text = to_string(App->player->totalGold);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 225 }, labelInfo));
	
	labelInfo.text = to_string(App->player->enemiesKill);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 325 }, labelInfo));
	
	labelInfo.text = to_string(App->player->buildDestroy);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 375 }, labelInfo));
	
	string min = to_string((uint)App->player->startGameTimer.ReadSec() / 60);
	if ((uint)App->player->startGameTimer.ReadSec() / 60 < 10)
		min = "0" + min;
	string sec = to_string((uint)App->player->startGameTimer.ReadSec() % 60);
	if ((uint)App->player->startGameTimer.ReadSec() % 60 < 10)
		sec = "0" + sec;
	labelInfo.text = min + ":" + sec;
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 475 }, labelInfo));

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 1400, 45, 250, 33 };
	returnButt = App->gui->CreateUIButton({ 550, 525 }, buttonInfo, this, nullptr);

	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	if (isWin) 
		labelInfo.hoverColor = ColorBlue;
	else 
		labelInfo.hoverColor = ColorRed;
	labelInfo.text = "Return to Main Menu";
	labelVector.push_back(App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, returnButt));

	//Backgrounds and flag
	if (isWin) {
		UIImage_Info imageInfo;
		imageInfo.texArea = winBG;
		background = App->gui->CreateUIImage({ 0,0 }, imageInfo, this, nullptr);
		background->SetPriorityDraw(PriorityDraw_FRAMEWORK);

		imageInfo.texArea = { 0, 2200, 800, 600 };
		flag = App->gui->CreateUIImage({ 300,202 }, imageInfo, this, nullptr);
		flag->StartAnimation(winFlagAnimation);
		flag->SetPriorityDraw(PriorityDraw_UIINGAME);
	}

	else if (!isWin)
	{ 
		UIImage_Info imageInfo;
		imageInfo.texArea = loseBG;
		background = App->gui->CreateUIImage({ 0,0 }, imageInfo, this, nullptr);
		background->SetPriorityDraw(PriorityDraw_FRAMEWORK);

		imageInfo.texArea = { 0, 4000, 800, 600 };
		flag = App->gui->CreateUIImage({ 300,202 }, imageInfo, this, nullptr);
		flag->StartAnimation(loseFlagAnimation);
		flag->SetPriorityDraw(PriorityDraw_UIINGAME);
	}
}

void j1FinishGame::ArtifactWon(uint time)
{
	UILabel_Info labelInfo;
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.text = "Artifact Obtained: ";

	if (time >= 1500) {
		imageVector.push_back(App->menu->AddArtifact({ 550,175 }, App->gui->bookText, App->gui->bookAnim, 5));
		labelInfo.text += "Book of Medivh";
	}
	else if (time >= 1200) {
		imageVector.push_back(App->menu->AddArtifact({ 550,175 }, App->gui->skullText, App->gui->skullAnim, 5));
		labelInfo.text += "Skull of Gul'dan";
	}
	else if (time >= 1080) {
		imageVector.push_back(App->menu->AddArtifact({ 550,175 }, App->gui->eyeText, App->gui->eyeAnim, 5));
		labelInfo.text += "Eye of Dalaran";
	}
	else if (time >= 0) {
		imageVector.push_back(App->menu->AddArtifact({ 550,175 }, App->gui->scepterText, App->gui->scepterAnim, 5));
		labelInfo.text += "Scepter of Sagreras";
	}
	labelVector.push_back(App->gui->CreateUILabel({ 575, 145 }, labelInfo));

	UIImage_Info imageInfo;
	imageInfo.verticalOrientation = VERTICAL_POS_TOP;
	imageInfo.horizontalOrientation = HORIZONTAL_POS_LEFT;
	imageInfo.texArea = { 268,384,59,59 };

	UIImage* recover = App->gui->CreateUIImage({ 548, 173 }, imageInfo);
	recover->SetPriorityDraw(PriorityDraw_WINDOW);
	imageVector.push_back(recover);
}

void j1FinishGame::DeleteScene() {
	
	for (; !labelVector.empty(); labelVector.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&labelVector.back());
	}

	for (; !imageVector.empty(); imageVector.pop_back())
	{
		App->gui->RemoveElem((UIElement**)&imageVector.back());
	}

	App->gui->RemoveElem((UIElement**)&returnButt);

}

void j1FinishGame::OnUIEvent(UIElement* UIelem, UI_EVENT UIevent) {

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

		if (UIelem == returnButt) {
			App->audio->PlayFx(App->audio->GetFX().button, 0); //Button sound
			DeleteScene();
			App->fade->FadeToBlack(this, App->menu);
		}
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
