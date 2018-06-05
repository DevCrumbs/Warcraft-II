#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Intro.h"
#include "j1Video.h"
#include "j1Menu.h"
#include "j1FadeToBlack.h"
#include "SDL_mixer\include\SDL_mixer.h"

j1Intro::j1Intro() : j1Module()
{
	name.assign("Intro");
}

// Destructor
j1Intro::~j1Intro()
{}

// Called before render is available
bool j1Intro::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Intro::Start()
{
	App->win->SetTitle("Video Player");
	// TODO 1: Call the initialize function from the video module in the start of the scene.
	App->video->Initialize("data/video/intro.avi");
	// TODO 8: Play the music of the video using the audio module. 
	//App->audio->PlayMusic("data/video/sample.ogg", 0.0f);

	return true;
}

// Called each loop iteration
bool j1Intro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Intro::Update(float dt)
{
	if (!App->video->isVideoFinished)
	{
		App->video->GrabAVIFrame();

	}

	else
	{
		App->fade->FadeToBlack(this, App->menu);
		Mix_PauseMusic();
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE)== KEY_DOWN)
	{
		App->fade->FadeToBlack(this, App->menu);
		Mix_PauseMusic();
	}
	

	return true;
}

// Called each loop iteration
bool j1Intro::PostUpdate()
{
	bool ret = true;
	return ret;
}

// Called before quitting
bool j1Intro::CleanUp()
{
	if (active) {
		App->video->CloseAVI();
		active = false;
	}

	return true;
}