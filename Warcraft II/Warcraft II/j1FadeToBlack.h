#ifndef __j1MODULEFADETOBLACK_H__
#define __j1MODULEFADETOBLACK_H__

#include "j1Module.h"

#include "Animation.h"

#include "SDL\include\SDL_rect.h"

struct UILabel;

enum FADE_TYPE
{
	FADE_TYPE_NONE,
	FADE_TYPE_NORMAL,
	FADE_TYPE_SLIDE,
	FADE_TYPE_TOTAL_BLACK
};

enum FADE_STEP
{
	FADE_STEP_NONE,
	FADE_STEP_TO_BLACK,
	FADE_STEP_FROM_BLACK
};

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	~j1FadeToBlack();

	bool Start();
	bool Update(float dt);
	bool FadeToBlack(j1Module* moduleOff, j1Module* module_on, float time = 2.0f, FADE_TYPE fadeType = FADE_TYPE_NORMAL, bool cleanupOff = true, bool startOn = true);

	bool IsFading() const;
	FADE_STEP GetStep() const;
	Uint32 GetNow() const;
	Uint32 GetTotalTime() const;

private:
	void NormalFade();
	void SliderFade();
	void BlackFade();

private:
	j1Module* off = nullptr;
	j1Module* on = nullptr;

	FADE_STEP currentStep = FADE_STEP_NONE;

	Uint32 now = 0;
	Uint32 startTime = 0;
	Uint32 totalTime = 0;

	SDL_Rect screen;
	SDL_Rect sliderRect;
	float dt = 0.0f;
	FADE_TYPE thisFade = FADE_TYPE_NONE;

	bool cleanupOff = true;
	bool startOn = true;
};

#endif //__j1MODULEFADETOBLACK_H__