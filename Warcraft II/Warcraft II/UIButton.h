#ifndef __UIButton_H__
#define __UIButton_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UIButton_Info {
	UIElement_Rect normal_tex_area = UIElement_Rect::NO_ELEMENT_RECT_;
	UIElement_Rect hover_tex_area = UIElement_Rect::NO_ELEMENT_RECT_;
	UIElement_Rect pressed_tex_area = UIElement_Rect::NO_ELEMENT_RECT_;

	bool checkbox = false;
	bool checkbox_checked = false;
	UIElement_HORIZONTAL_POS horizontal_orientation = UIElement_HORIZONTAL_POS::LEFT_;
	UIElement_VERTICAL_POS vertical_orientation = UIElement_VERTICAL_POS::TOP_;

	bool draggable = false;
	bool interactive = true;
};

// ---------------------------------------------------

class UIButton : public UIElement
{
public:
	UIButton(iPoint local_pos, UIElement* parent, UIButton_Info& info, j1Module* listener = nullptr);
	void Update(float dt);
	void HandleInput();
	void DebugDraw(iPoint blit_pos) const;

	void ChangeSprite(SDL_Rect tex_area);
	SDL_Rect GetHoverSprite() const;
	SDL_Rect GetPressedSprite() const;
	SDL_Rect GetNormalSprite() const;
	UIEvents GetActualEvent() const;

	bool SlideTransition(float dt, int end_pos_y, float speed = 10.0f, bool bounce = true, float bounce_interval = 1.0f, float bounce_speed = 2.0f, bool down = true);
	bool Bounce(float dt, float bounce_interval = 1.0f, float bounce_speed = 2.0f, bool down = true);
	void InitializeBounce(float bounce_interval = 1.0f, bool down = true);

private:
	UIButton_Info button;
	UIEvents UIevent = NO_EVENT_;

	SDL_Rect normal_tex_area = { 0,0,0,0 };
	SDL_Rect hover_tex_area = { 0,0,0,0 };
	SDL_Rect pressed_tex_area = { 0,0,0,0 };

public:
	bool tab = false;
	bool next_event = false;

	// Bounce parameters
	float bounce_value = 0.0f;
	iPoint start_pos = { 0,0 };
	bool first_bounce = true;
	bool reset = true;
	bool start_bouncing = false;
};

#endif //__UIButton_H__