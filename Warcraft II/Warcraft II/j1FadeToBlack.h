#ifndef __j1MODULEFADETOBLACK_H__
#define __j1MODULEFADETOBLACK_H__

#include "j1Module.h"

#include "Animation.h"

#include "SDL\include\SDL_rect.h"

struct UILabel;

enum fades
{
	no_fade,
	normal_fade,
	slider_fade,
	total_black_fade
};

enum fade_step
{
	none,
	fade_to_black,
	fade_from_black
};

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	~j1FadeToBlack();

	bool Start();
	bool Update(float dt);
	bool FadeToBlack(j1Module* module_off, j1Module* module_on, float time = 2.0f, fades kind_of_fade = normal_fade, bool cleanup_off = true, bool start_on = true);

	bool IsFading() const;
	fade_step GetStep() const;
	Uint32 GetNow() const;
	Uint32 GetTotalTime() const;

private:
	void NormalFade();
	void SliderFade();
	void BlackFade();

private:
	j1Module* off;
	j1Module* on;

	fade_step current_step = fade_step::none;

	Uint32 now = 0;
	Uint32 start_time = 0;
	Uint32 total_time = 0;

	SDL_Rect screen;
	SDL_Rect Slider_rect;
	float dt = 0.0f;
	fades thisFade;

	bool cleanup_off = true;
	bool start_on = true;
};

#endif //__j1MODULEFADETOBLACK_H__