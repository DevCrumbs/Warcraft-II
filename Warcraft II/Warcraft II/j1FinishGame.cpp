#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1FinishGame.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Player.h"


#include "UILabel.h"
#include "UIButton.h"
#include "UIImage.h"




j1FinishGame::j1FinishGame()
{
	name.assign("finish");
}

bool j1FinishGame::Awake(pugi::xml_node& config) {

	bool ret = true;


	//pugi::xml_node spritesheets = config.child("spritesheets");
	//bgTexName = spritesheets.child("atlas").attribute("name").as_string();

	return true;
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
	LoadScene(App->player->isWin);


	bg = App->tex->Load(bgTexName.data());

	return true;
}

// Update: draw background
bool j1FinishGame::Update(float dt)
{

	App->render->DrawQuad({ 250,155,100,40}, 0, 255, 0, 255);
	App->render->DrawQuad({ 250,210,100,40 }, 0, 255, 0, 255);
	App->render->DrawQuad({ 250,305,100,40 }, 0, 255, 0, 255);
	App->render->DrawQuad({ 250,360,100,40 }, 0, 255, 0, 255);
	App->render->DrawQuad({ 250,455,100,40 }, 0, 255, 0, 255);
	App->render->DrawQuad({ 250,510,100,40 }, 0, 255, 0, 255);

	//App->render->Blit(bg, 0, 0, &screen);
	/*if(App->player->isWin)
		App->render->DrawQuad(screen, 0, 255, 0, 255);
	else
		App->render->DrawQuad(screen, 255, 0, 0, 255);
*/
	return true;
}


void j1FinishGame::LoadScene(bool isWin) {
	
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
										   
	labelInfo.text = "Rooms explored: ";   
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 475 }, labelInfo));
										    
	labelInfo.text = "Total time: ";	    
	labelVector.push_back(App->gui->CreateUILabel({ 50 , 525 }, labelInfo));


}