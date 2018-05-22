#ifndef __UIButton_H__
#define __UIButton_H__

#include "SDL\include\SDL.h"

#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UIButton_Info {
	SDL_Rect normalTexArea = { 0, 0, 0, 0 };
	SDL_Rect hoverTexArea = { 0, 0, 0, 0 };
	SDL_Rect pressedTexArea = { 0, 0, 0, 0 };

	bool checkbox = false;
	bool isChecked = false;
	UIE_HORIZONTAL_POS horizontalOrientation = HORIZONTAL_POS_LEFT;
	UIE_VERTICAL_POS verticalOrientation = VERTICAL_POS_TOP;

	bool draggable = false;
	bool interactive = true;
};

// ---------------------------------------------------

class UIButton : public UIElement
{
public:
	UIButton(iPoint localPos, UIElement* parent, UIButton_Info& info, j1Module* listener = nullptr, bool isInWolrd = false);
	~UIButton();
	void Update(float dt);
	void HandleInput();
	void DebugDraw(iPoint blitPos) const;

	void ChangeSprite(SDL_Rect texArea);
	void ChangesTextsAreas(bool isDiferent, SDL_Rect normalText = {0,0,0,0}, SDL_Rect hoverText = { 0,0,0,0 });
	SDL_Rect GetHoverSprite() const;
	SDL_Rect GetPressedSprite() const;
	SDL_Rect GetNormalSprite() const;
	UI_EVENT GetActualEvent() const;

	//void Draw() const;

	bool SlideTransition(float dt, int endPosY, float speed = 10.0f, bool bounce = true, float bounceInterval = 1.0f, float bounceSpeed = 2.0f, bool down = true);
	bool Bounce(float dt, float bounceInterval = 1.0f, float bounceSpeed = 2.0f, bool down = true);
	void InitializeBounce(float bounceInterval = 1.0f, bool down = true);

public:
	bool tab = false;
	bool nextEvent = false;

	// Bounce parameters
	float bounceValue = 0.0f;
	iPoint startPos = { 0,0 };
	bool firstBounce = true;
	bool reset = true;
	bool startBouncing = false;

private:
	bool isInWorld = false;
	UIButton_Info button;
	UI_EVENT UIevent = UI_EVENT_NONE;

	SDL_Rect normalTexArea = { 0,0,0,0 };
	SDL_Rect hoverTexArea = { 0,0,0,0 };
	SDL_Rect pressedTexArea = { 0,0,0,0 };
};

#endif //__UIButton_H__