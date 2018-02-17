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
	FONT_NAME fontName = FONT_NAME_DEFAULT;
	SDL_Color normalColor = White_;
	SDL_Color hoverColor = White_;
	SDL_Color pressedColor = White_;

	UIE_HORIZONTAL_POS horizontalOrientation = HORIZONTAL_POS_LEFT;
	UIE_VERTICAL_POS verticalOrientation = VERTICAL_POS_TOP;

	bool draggable = false;
	bool interactive = true;
	bool interactionFromFather = false;

	// Support for paragraphs
	int textWrapLength = 0;
};

// ---------------------------------------------------

class UILabel : public UIElement
{
public:
	UILabel(iPoint localPos, UIElement* parent, UILabel_Info& info, j1Module* listener = nullptr);
	~UILabel();
	void Update(float dt);
	void HandleInput();
	bool MouseHover() const;
	void Draw() const;
	void DebugDraw(iPoint blitPos) const;

	void SetText(string text);
	void SetColor(SDL_Color color, bool normal = false, bool hover = false, bool pressed = false);
	SDL_Color GetColor(bool normal = true, bool hover = false, bool pressed = false);

	bool IntermitentFade(float seconds = 1.0f, bool loop = true, bool halfLoop = false);
	void ResetFade();

	bool FromAlphaToAlphaFade(float from = 0.0f, float to = 0.0f, float seconds = 1.0f);

	void RandomAlphaPainting(float dt, SDL_Color color = White_, int baseAlpha = 255.0f, int minAlpha = 0.0f, int maxAlpha = 255.0f, float speed = 1.0f);

	bool SlideTransition(float dt, int endPosY, float speed = 10.0f, bool bounce = true, float bounceInterval = 1.0f, float bounceSpeed = 2.0f, bool down = true);
	bool Bounce(float dt, float bounceInterval = 1.0f, float bounceSpeed = 2.0f, bool down = true);
	void InitializeBounce(float bounceInterval = 1.0f, bool down = true);

private:
	UILabel_Info label;
	_TTF_Font* font = nullptr;
	const SDL_Texture* tex = nullptr;
	SDL_Color color = { 255,255,255,255 };
	UI_EVENT UIevent = UI_EVENT_NONE;
	bool nextEvent = false;

	// Fade parameters
	float totalTime = 0.0f;
	float startTime = 0.0f;
	bool isInvisible = true;
	bool reset = true;

	// Alpha painting parameters
	float timer = 0.0f;

	// Bounce parameters
	float bounceValue = 0.0f;
	iPoint startPos = { 0,0 };
	bool firstBounce = true;
	bool startBouncing = false;
};

#endif //__UILabel_H__