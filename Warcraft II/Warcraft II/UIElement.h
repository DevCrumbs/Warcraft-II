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
	UIE_TYPE_INPUT_TEXT
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

	UIElement* GetParent() const;

public:
	bool drag = false;
	bool toRemove = false;

protected:
	UIE_TYPE type = UIE_TYPE_NO_TYPE;
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

private:
	iPoint localPos = { 0,0 };
	UIElement* parent = nullptr;

};

#endif //__UIElement_H__
