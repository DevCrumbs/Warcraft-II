#include <math.h>

#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1FadeToBlack.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "UILabel.h"




j1FadeToBlack::j1FadeToBlack()
{
	name.assign("fade");
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
	sliderRect = screen;
	sliderRect.w = 0;
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	this->dt = dt;

	if (currentStep == FADE_STEP_NONE)
		return true;

	switch (thisFade)
	{
	case FADE_TYPE_NORMAL:
		NormalFade();
		break;
	case FADE_TYPE_SLIDE:
		SliderFade();
		break;
	case FADE_TYPE_TOTAL_BLACK:
		BlackFade();
		break;
	default:
		break;
	}

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1FadeToBlack::FadeToBlack(j1Module* moduleOff, j1Module* moduleOn, float time, FADE_TYPE fadeType, bool cleanupOff, bool startOn)
{
	bool ret = false;

	this->cleanupOff = cleanupOff;
	this->startOn = startOn;

	if (currentStep != FADE_STEP_TO_BLACK)
	{
		thisFade = fadeType;

		currentStep = FADE_STEP_TO_BLACK;
		startTime = SDL_GetTicks();
		totalTime = (Uint32)(time * 0.5f * 1000.0f);

		off = moduleOff;
		on = moduleOn;

		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::IsFading() const
{
	return currentStep != FADE_STEP_NONE;
}

FADE_STEP j1FadeToBlack::GetStep() const
{
	return currentStep;
}

Uint32 j1FadeToBlack::GetNow() const
{
	return now;
}

Uint32 j1FadeToBlack::GetTotalTime() const
{
	return totalTime;
}

void j1FadeToBlack::NormalFade()
{
	now = SDL_GetTicks() - startTime;
	float normalized = MIN(1.0f, (float)now / (float)totalTime);

	switch (currentStep)
	{
	case FADE_STEP_TO_BLACK:
	{
		if (now >= totalTime)
		{
			if (cleanupOff)
				off->CleanUp();
			if (startOn)
				on->Start();

			totalTime += totalTime;
			startTime = SDL_GetTicks();
			currentStep = FADE_STEP_FROM_BLACK;
		}
	} break;

	case FADE_STEP_FROM_BLACK:
	{
		normalized = 1.0f - normalized;

		if (now >= totalTime) {
			currentStep = FADE_STEP_NONE;
		}
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);
}

void j1FadeToBlack::SliderFade()
{
	now = SDL_GetTicks() - startTime;
	float normalized = MIN(1.0f, (float)now / (float)totalTime);

	switch (currentStep)
	{
	case FADE_STEP_TO_BLACK:

		if (now >= totalTime) {

			if (cleanupOff) {
				off->active = false;
				off->CleanUp();
			}
			if (startOn) {
				on->active = true;
				on->Start();
			}

			totalTime += totalTime;
			startTime = SDL_GetTicks();

			currentStep = FADE_STEP_FROM_BLACK;
		}
		break;

	case FADE_STEP_FROM_BLACK:

		normalized = 1.0f - normalized;

		if (now >= totalTime) {
			currentStep = FADE_STEP_NONE;
		}
		break;
	}

	sliderRect.w = normalized * screen.w;
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, 255.0f);
	SDL_RenderFillRect(App->render->renderer, &sliderRect);
}

void j1FadeToBlack::BlackFade()
{
	now = SDL_GetTicks() - startTime;

	switch (currentStep)
	{
	case FADE_STEP_TO_BLACK:

		if (now >= totalTime) {

			if (cleanupOff)
				off->CleanUp();
			if (startOn)
				on->Start();

			totalTime += totalTime;
			startTime = SDL_GetTicks();

			currentStep = FADE_STEP_FROM_BLACK;

		}
		break;

	case FADE_STEP_FROM_BLACK:

		if (now >= totalTime) {
			currentStep = FADE_STEP_NONE;
		}
		break;
	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, 255.0f);
	SDL_RenderFillRect(App->render->renderer, &screen);
}
