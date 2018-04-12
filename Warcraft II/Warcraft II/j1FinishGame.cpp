#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1FinishGame.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "UILabel.h"




j1FinishGame::j1FinishGame()
{
	name.assign("finish");
}

j1FinishGame::~j1FinishGame()
{}

// Load assets
bool j1FinishGame::Start()
{
	/*LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	// Get screen size
	uint width = 0, height = 0, scale = 0;

	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	screen = { 0, 0, static_cast<int>(width * scale), static_cast<int>(height * scale) };
	//
	sliderRect = screen;
	sliderRect.w = 0;*/
	return true;
}

// Update: draw background
bool j1FinishGame::Update(float dt)
{

	return true;
}
