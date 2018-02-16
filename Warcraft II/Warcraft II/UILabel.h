#ifndef __UILabel_H__
#define __UILabel_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct _TTF_Font;
struct SDL_Color;

struct UILabel_Info {
	string text;
	Font_Names font_name = Font_Names::DEFAULT_;
	SDL_Color normal_color = White_;
	SDL_Color hover_color = White_;
	SDL_Color pressed_color = White_;

	UIElement_HORIZONTAL_POS horizontal_orientation = UIElement_HORIZONTAL_POS::LEFT_;
	UIElement_VERTICAL_POS vertical_orientation = UIElement_VERTICAL_POS::TOP_;

	bool draggable = false;
	bool interactive = true;
	bool interaction_from_father = false;

	// Support for paragraphs
	int text_wrap_length = 0;
};

// ---------------------------------------------------

class UILabel : public UIElement
{
public:
	UILabel(iPoint local_pos, UIElement* parent, UILabel_Info& info, j1Module* listener = nullptr);
	~UILabel();
	void Update(float dt);
	void HandleInput();
	bool MouseHover() const;
	void Draw() const;
	void DebugDraw(iPoint blit_pos) const;

	void SetText(string text);
	void SetColor(SDL_Color color, bool normal = false, bool hover = false, bool pressed = false);
	SDL_Color GetColor(bool normal = true, bool hover = false, bool pressed = false);

	bool IntermitentFade(float seconds = 1.0f, bool loop = true, bool half_loop = false);
	void ResetFade();

	bool FromAlphaToAlphaFade(float from = 0.0f, float to = 0.0f, float seconds = 1.0f);

	void RandomAlphaPainting(float dt, SDL_Color color = White_, int base_alpha = 255.0f, int min_alpha = 0.0f, int max_alpha = 255.0f, float speed = 1.0f);

	bool SlideTransition(float dt, int end_pos_y, float speed = 10.0f, bool bounce = true, float bounce_interval = 1.0f, float bounce_speed = 2.0f, bool down = true);
	bool Bounce(float dt, float bounce_interval = 1.0f, float bounce_speed = 2.0f, bool down = true);
	void InitializeBounce(float bounce_interval = 1.0f, bool down = true);

private:
	UILabel_Info label;
	_TTF_Font* font = nullptr;
	const SDL_Texture* tex = nullptr;
	SDL_Color color = { 255,255,255,255 };
	UIEvents UIevent = NO_EVENT_;
	bool next_event = false;

	// Fade parameters
	float totalTime = 0.0f;
	float startTime = 0.0f;
	bool is_invisible = true;
	bool reset = true;

	// Alpha painting parameters
	float timer = 0.0f;

	// Bounce parameters
	float bounce_value = 0.0f;
	iPoint start_pos = { 0,0 };
	bool first_bounce = true;
	bool start_bouncing = false;
};

#endif //__UILabel_H__