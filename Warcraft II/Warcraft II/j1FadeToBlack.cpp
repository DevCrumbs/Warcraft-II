#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1FadeToBlack.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "UILabel.h"

#include <math.h>

#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

j1FadeToBlack::j1FadeToBlack()
{
	name.create("fade");
}

j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	// Get screen size
	uint width = 0, height = 0, scale = 0;

	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	screen = { 0, 0, static_cast<int>(width * scale), static_cast<int>(height * scale) };
	//
	Slider_rect = screen;
	Slider_rect.w = 0;
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	this->dt = dt;

	if (current_step == fade_step::none)
		return true;

	switch (thisFade)
	{
	case normal_fade:
		NormalFade();
		break;
	case slider_fade:
		SliderFade();
		break;
	case total_black_fade:
		BlackFade();
		break;
	}

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1FadeToBlack::FadeToBlack(j1Module* module_off, j1Module* module_on, float time, fades kind_of_fade, bool cleanup_off, bool start_on)
{
	bool ret = false;

	this->cleanup_off = cleanup_off;
	this->start_on = start_on;

	if (current_step != fade_step::fade_to_black)
	{
		thisFade = kind_of_fade;

		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);

		off = module_off;
		on = module_on;

		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::IsFading() const
{
	return current_step != fade_step::none;
}

fade_step j1FadeToBlack::GetStep() const
{
	return current_step;
}

Uint32 j1FadeToBlack::GetNow() const
{
	return now;
}

Uint32 j1FadeToBlack::GetTotalTime() const
{
	return total_time;
}

void j1FadeToBlack::NormalFade()
{
	now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			if (cleanup_off)
				off->CleanUp();
			if (start_on)
				on->Start();

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time) {
			current_step = fade_step::none;
		}
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);
}

void j1FadeToBlack::SliderFade()
{
	now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:

		if (now >= total_time) {

			if (cleanup_off) {
				off->active = false;
				off->CleanUp();
			}
			if (start_on) {
				on->active = true;
				on->Start();
			}

			total_time += total_time;
			start_time = SDL_GetTicks();

			current_step = fade_from_black;
		}
		break;

	case fade_step::fade_from_black:

		normalized = 1.0f - normalized;

		if (now >= total_time) {
			current_step = fade_step::none;
		}
		break;
	}

	Slider_rect.w = normalized*screen.w;
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, 255.0f);
	SDL_RenderFillRect(App->render->renderer, &Slider_rect);
}

void j1FadeToBlack::BlackFade()
{
	now = SDL_GetTicks() - start_time;

	switch (current_step)
	{
	case fade_step::fade_to_black:

		if (now >= total_time) {

			if (cleanup_off)
				off->CleanUp();
			if (start_on)
				on->Start();

			total_time += total_time;
			start_time = SDL_GetTicks();

			current_step = fade_from_black;

		}
		break;

	case fade_step::fade_from_black:

		if (now >= total_time) {
			current_step = fade_step::none;
		}
		break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, 255.0f);
	SDL_RenderFillRect(App->render->renderer, &screen);
}
