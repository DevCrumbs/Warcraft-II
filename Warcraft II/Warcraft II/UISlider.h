#ifndef __UISlider_H__
#define __UISlider_H__

#include "SDL\include\SDL.h"


#include "UIElement.h"
#include "j1App.h"
#include "j1Gui.h"

struct UISlider_Info {
	SDL_Rect tex_area = { 0,0,0,0 };
	SDL_Rect button_slider_area = { 0,0,0,0 };
	UIE_HORIZONTAL_POS horizontal_orientation = HORIZONTAL_POS_LEFT;
	UIE_VERTICAL_POS vertical_orientation = VERTICAL_POS_TOP;
	SDL_Color color = { 0,0,0,255 };
	iPoint slider_button_pos = { 0,0 };
	int offset = 0;
	int buggy_offset = 0;

};

// ---------------------------------------------------

class UISlider : public UIElement
{
public:
	UISlider(iPoint local_pos, UIElement* parent, UISlider_Info& info, j1Module* listener = nullptr, bool isInWorld = false);
	void Draw() const;
	void Update(float dt);
	void DebugDraw(iPoint blit_pos) const;
	void HandleInput();
	void SetNewRect(SDL_Rect& new_rect);
	SDL_Rect GetRect();
	uint GetPercent();

private:
	UISlider_Info slider;

	UI_EVENT UIevent = UI_EVENT_NONE;
	bool next_event = false;

	SDL_Rect button_slider_area = { 0,0,0,0 };
	SDL_Rect tex_area = { 0,0,0,0 };


public:
	bool lets_move = false;

	// Bounce parameters
	float bounce_value = 0.0f;
	iPoint start_pos = { 0,0 };
	bool first_bounce = true;
	bool reset = true;
	bool start_bouncing = false;
};

#endif // __UISlider_H__