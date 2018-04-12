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
	if(App->player->isWin)
		LoadWinScene();
	else
		LoadLoseScene();

	bg = App->tex->Load(bgTexName.data());

	return true;
}

// Update: draw background
bool j1FinishGame::Update(float dt)
{

	//App->render->Blit(bg, 0, 0, &screen);
	if(App->player->isWin)
		App->render->DrawQuad(screen, 0, 255, 0, 255);
	else
		App->render->DrawQuad(screen, 255, 0, 0, 255);

	return true;
}


void j1FinishGame::LoadWinScene() {
	UILabel_Info labelInfo;

	labelInfo.text = "Congratulations! Yo have defeated the Horde!";
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = ColorBlue;
	//labelInfo.fontName = FONT_NAME_WARCRAFT20;

	titleLabel = App->gui->CreateUILabel({ screen.w / 2, 50 }, labelInfo);

}

void j1FinishGame::LoadLoseScene() {
	UILabel_Info labelInfo;

	labelInfo.text = "Oh, no! Yo have been defeated by the Horde!";
	labelInfo.horizontalOrientation = HORIZONTAL_POS_CENTER;
	labelInfo.normalColor = labelInfo.hoverColor = labelInfo.pressedColor = Black_;
	//labelInfo.fontName = FONT_NAME_WARCRAFT20;

	titleLabel = App->gui->CreateUILabel({ screen.w / 2, 50 }, labelInfo);

}