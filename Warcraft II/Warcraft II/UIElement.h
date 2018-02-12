#ifndef __UIElement_H__
#define __UIElement_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "p2Point.h"

class j1Module;

enum UIElement_TYPE
{
	NO_TYPE_,
	IMAGE_,
	LABEL_,
	BUTTON_,
	WINDOW_,
	CURSOR_,
	MAX_TYPES_
};

enum UIElement_HORIZONTAL_POS {
	LEFT_,
	RIGHT_,
	CENTER_
};

enum UIElement_VERTICAL_POS {
	TOP_,
	BOTTOM_,
	MIDDLE_
};

enum UIEvents {
	NO_EVENT_,
	MOUSE_ENTER_,
	MOUSE_LEAVE_,
	MOUSE_RIGHT_CLICK_,
	MOUSE_LEFT_CLICK_,
	MOUSE_RIGHT_UP_,
	MOUSE_LEFT_UP_,
	MAX_EVENTS_
};

// ---------------------------------------------------

class UIElement
{
protected:
	UIElement_TYPE type = UIElement_TYPE::NO_TYPE_;
	UIElement_HORIZONTAL_POS horizontal = UIElement_HORIZONTAL_POS::LEFT_;
	UIElement_VERTICAL_POS vertical = UIElement_VERTICAL_POS::TOP_;
	j1Module* listener = nullptr;

	bool draggable = false;
	bool interactive = true;
	iPoint mouse_click_pos = { 0,0 };

	// Texture parameters
	SDL_Rect tex_area = { 0,0,0,0 };
	int width = 0, height = 0;

private:
	iPoint local_pos = { 0,0 };
	UIElement* parent = nullptr;

public:
	UIElement(iPoint local_pos, UIElement* parent, j1Module* listener);

	virtual ~UIElement();

	virtual void Update(float dt);

	virtual void Draw() const;

	virtual void HandleInput();

	virtual bool HasToBeRemoved() const;

	virtual UIElement_TYPE GetType() const;

	virtual void DebugDraw(iPoint blit_pos) const;

	bool MouseHover() const;

	virtual void UpdateDragging(float dt);

	void SetOrientation();

	void SetInteraction(bool interactive);

	SDL_Rect GetScreenRect() const;
	SDL_Rect GetLocalRect() const;
	iPoint GetScreenPos() const;
	iPoint GetLocalPos() const;
	void SetLocalPos(iPoint local_pos);
	void IncreasePos(iPoint add_local_pos);
	void DecreasePos(iPoint add_local_pos);

	UIElement* GetParent() const;

public:
	bool drag = false;
	bool to_remove = false;
};

#endif //__UIElement_H__
