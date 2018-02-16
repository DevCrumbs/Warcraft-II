#ifndef __UIImage_H__
#define __UIImage_H__

#include "SDL\include\SDL.h"
#include "Defs.h"
#include "Animation.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UIImage_Info {
	UIElement_Rect tex_area = UIElement_Rect::NO_ELEMENT_RECT_;
	UIElement_HORIZONTAL_POS horizontal_orientation = UIElement_HORIZONTAL_POS::LEFT_;
	UIElement_VERTICAL_POS vertical_orientation = UIElement_VERTICAL_POS::TOP_;

	bool quad = false;
	SDL_Rect quad_area = { 0,0,0,0 };

	SDL_Color color = { 0,0,0,255 };
	bool draggable = false;
};

// ---------------------------------------------------

class UIImage : public UIElement
{
public:
	UIImage(iPoint local_pos, UIElement* parent, UIImage_Info& info, j1Module* listener = nullptr);
	void Draw() const;
	void DebugDraw(iPoint blit_pos) const;
	void SetColor(const SDL_Color color);
	void StartAnimation(Animation anim);
	void Update(float dt);
	SDL_Color GetColor();
	void SetNewRect(SDL_Rect& new_rect);
	SDL_Rect GetRect();

	void ResetFade();
	bool FromAlphaToAlphaFade(float from = 0.0f, float to = 0.0f, float seconds = 1.0f);

private:
	UIImage_Info image;

	// Fade parameters
	float total_time = 0.0f;
	float start_time = 0.0f;
	bool reset = true;

	Animation anim_to_play;
	Animation* anim;
	float speed = 0.0f;
	bool start_aimation = false;
};

#endif //__UIImage_H__