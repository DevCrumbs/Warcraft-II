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


#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"




j1FinishGame::j1FinishGame()
{
	name.assign("finish");
}


j1FinishGame::~j1FinishGame()
{}

// Load assets
bool j1FinishGame::Start()
{
	// Get screen size
	App->render->camera.x = App->render->camera.y = 0;
	uint width = 0, height = 0, scale = 0;

	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	screen = { 0, 0, static_cast<int>(width * scale), static_cast<int>(height * scale) };
	LoadSceneOne(App->player->isWin);


	bg = App->tex->Load(bgTexName.data());

	//DeleteScreen();
	return true;
}

// Update: draw background
bool j1FinishGame::Update(float dt)
{

	return true;
}

bool j1FinishGame::CleanUp() {
	DeleteScene();
	App->menu->active = true;
	return true;
}

void j1FinishGame::LoadSceneOne(bool isWin) {
	
	UILabel_Info labelInfo;

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	if (isWin) {
		labelInfo.text = "Congratulations! You have defeated the Horde!";
		labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorBlue;
	}
	else {
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
										   
	/*labelInfo.text = "Rooms explored: ";   
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 475 }, labelInfo));*/  //COMMING SOON 
										    
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
	//imageVector.push_back(App->gui->CreateUIImage({ 275, 525 }, imageInfo));

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;

	labelInfo.text = to_string(App->player->unitProduce);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 175 }, labelInfo));

	labelInfo.text = to_string(App->player->totalGold);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 225 }, labelInfo));
	
	labelInfo.text = to_string(App->player->enemiesKill);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 325 }, labelInfo));
	
	labelInfo.text = to_string(App->player->buildDestroy);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 375 }, labelInfo));
	
	/*labelInfo.text = to_string(roomsExploredCont);
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 475 }, labelInfo));*/
	
	string min = to_string((uint)App->player->startGameTimer.ReadSec() / 60);
	if ((uint)App->player->startGameTimer.ReadSec() / 60 < 10)
		min = "0" + min;
	string sec = to_string((uint)App->player->startGameTimer.ReadSec() % 60);
	if ((uint)App->player->startGameTimer.ReadSec() % 60 < 10)
		sec = "0" + sec;
	labelInfo.text = min + ":" + sec;
	labelVector.push_back(App->gui->CreateUILabel({ 275 , 475 }, labelInfo));

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 129, 33 };
	continueButt = App->gui->CreateUIButton({ 625, 525 }, buttonInfo, this, nullptr);

	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.hoverColor = ColorGreen;

	labelInfo.text = "Continue";
	labelVector.push_back(App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, continueButt));


}

void j1FinishGame::DeleteScene() {
	
	for (; !labelVector.empty(); labelVector.pop_back())
	{
		App->gui->DestroyElement((UIElement**)&labelVector.back());
	}

	for (; !imageVector.empty(); imageVector.pop_back())
	{
		App->gui->DestroyElement((UIElement**)&imageVector.back());
	}
	App->gui->DestroyElement((UIElement**)&continueButt);
	App->gui->DestroyElement((UIElement**)&returnButt);

}

void j1FinishGame::LoadSceneTwo() {
	UILabel_Info labelInfo;

	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorGrey;
	labelInfo.text = "Alpha out on May the 13h";
	labelVector.push_back(App->gui->CreateUILabel({ 50, 542 }, labelInfo));

	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.fontName = FONT_NAME_WARCRAFT25;
	labelInfo.text = "Thanks for playing the demo!";
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorGreen;
	labelVector.push_back(App->gui->CreateUILabel({ screen.w / 2, 50 }, labelInfo));

	UIButton_Info buttonInfo;
	buttonInfo.normalTexArea = { 2000, 0, 250, 33 };
	returnButt = App->gui->CreateUIButton({ 550, 525 }, buttonInfo, this, nullptr);

	labelInfo.fontName = FONT_NAME_WARCRAFT20;
	labelInfo.normalColor = White_;
	labelInfo.text = "Return to Main Menu";
	labelVector.push_back(App->gui->CreateUILabel({ buttonInfo.normalTexArea.w / 2 ,buttonInfo.normalTexArea.h / 2 }, labelInfo, this, returnButt));
	

	labelInfo.hoverColor = labelInfo.pressedColor = White_;
	labelInfo.fontName = FONT_NAME_WARCRAFT;
	labelInfo.textWrapLength = 600;
	string s = "If you want to know which artifact you would have gained, do not forget to play th full game when it is relased! The enemy base awaits for your troops...";
	labelInfo.text = s;
	labelVector.push_back(App->gui->CreateUILabel({ 750, 400 }, labelInfo));

	imageVector.push_back(App->menu->AddArtifact({ 125,200 }, App->gui->bookText, App->gui->bookAnim));
	imageVector.push_back(App->menu->AddArtifact({ 275,300 }, App->gui->skullText, App->gui->skullAnim));
	imageVector.push_back(App->menu->AddArtifact({ 450,300 }, App->gui->eyeText, App->gui->eyeAnim));
	imageVector.push_back(App->menu->AddArtifact({ 600,200 }, App->gui->scepterText, App->gui->scepterAnim));

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

		if (UIelem == continueButt) {
			DeleteScene();
			LoadSceneTwo();
		}

		if (UIelem == returnButt) {
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
