#ifndef __UIElement_H__
#define __UIElement_H__

#include "SDL\include\SDL.h"

#include "Defs.h"

#include "p2Point.h"

class j1Module;

enum UIE_TYPE
{
	UIE_TYPE_NO_TYPE,
	UIE_TYPE_IMAGE,
	UIE_TYPE_LABEL,
	UIE_TYPE_BUTTON,
	UIE_TYPE_WINDOW,
	UIE_TYPE_CURSOR,
	UIE_TYPE_MAX_TYPES,
	UIE_TYPE_INPUT_TEXT,
	UIE_TYPE_LIFE_BAR,
	UIE_TYPE_SLIDER,
	UIE_TYPE_MINIMAP
};

enum UIE_HORIZONTAL_POS {
	HORIZONTAL_POS_LEFT,
	HORIZONTAL_POS_RIGHT,
	HORIZONTAL_POS_CENTER
};

enum UIE_VERTICAL_POS {
	VERTICAL_POS_TOP,
	VERTICAL_POS_BOTTOM,
	VERTICAL_POS_CENTER
};

enum UI_EVENT {
	UI_EVENT_NONE,
	UI_EVENT_MOUSE_ENTER,
	UI_EVENT_MOUSE_LEAVE,
	UI_EVENT_MOUSE_RIGHT_CLICK,
	UI_EVENT_MOUSE_LEFT_CLICK,
	UI_EVENT_MOUSE_RIGHT_UP,
	UI_EVENT_MOUSE_LEFT_UP,
	UI_EVENT_MAX_EVENTS
};

enum UI_TEXT_AREA_TYPE {
	UI_TEXT_AREA_TYPE_NOTYPE,
	UI_TEXT_AREA_TYPE_NORMAL,
	UI_TEXT_AREA_TYPE_HOVER,
	UI_TEXT_AREA_TYPE_PRESSED
};
enum PriorityDraw {

	PriorityDraw_NONE,
	PriorityDraw_LIFEBAR_INGAME = -1,
	PriorityDraw_BUTTONSINGAME,
	PriorityDraw_UNDER_FRAMEWORK,
	PriorityDraw_FRAMEWORK,
	PriorityDraw_UIINGAME,
	PriorityDraw_PAUSEMENU,
	PriorityDraw_SLIDER,
	PriorityDraw_IMAGE,
	PrioriryDraw_LABEL,

	PriorityDraw_CURSOR

};
// ---------------------------------------------------

class UIElement
{
public:
	UIElement(iPoint localPos, UIElement* parent, j1Module* listener, bool isInWorld);

	virtual ~UIElement();

	virtual void Update(float dt);

	virtual void Draw() const;

	virtual void HandleInput();

	virtual bool HasToBeRemoved() const;

	virtual UIE_TYPE GetType() const;

	virtual void DebugDraw(iPoint blitPos) const;

	bool MouseHover() const;

	virtual void UpdateDragging(float dt);

	void SetOrientation();

	void SetInteraction(bool interactive);

	SDL_Rect GetScreenRect() const;
	SDL_Rect GetLocalRect() const;
	iPoint GetScreenPos() const;
	iPoint GetLocalPos() const;
	void SetLocalPos(iPoint localPos);
	void IncreasePos(iPoint add_localPos);
	void DecreasePos(iPoint add_localPos);
	void SetPriorityDraw(PriorityDraw priority);
	PriorityDraw GetPriorityDraw() const;

	UIElement* GetParent() const;

	// Blit
	void SetBlitState(bool isBlit);
	bool GetBlitState() const;

public:

	bool drag = false;
	bool toRemove = false;
	bool isBlit = true;
	bool isActive = true;

	UIE_TYPE type = UIE_TYPE_NO_TYPE;

protected:

	UIE_HORIZONTAL_POS horizontal = HORIZONTAL_POS_LEFT;
	UIE_VERTICAL_POS vertical = VERTICAL_POS_TOP;

	j1Module* listener = nullptr;

	bool draggable = false;
	bool interactive = true;
	bool isInWorld = false;
	iPoint mouseClickPos = { 0,0 };

	// Texture parameters
	SDL_Rect texArea = { 0,0,0,0 };
	int width = 0, height = 0;
	PriorityDraw priority = PriorityDraw_NONE;

private:

	iPoint localPos = { 0,0 };
	UIElement* parent = nullptr;
};

#endif //__UIElement_H__
