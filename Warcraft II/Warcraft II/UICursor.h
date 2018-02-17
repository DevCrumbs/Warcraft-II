#ifndef __UICursor_H__
#define __UICursor_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UICursor_Info {
	UIE_RECT default = NO_ELEMENT_RECT;
	UIE_RECT onClick = NO_ELEMENT_RECT;
};

// ---------------------------------------------------

class UICursor : public UIElement
{
public:
	UICursor(iPoint localPos, UIElement* parent, UICursor_Info& info, j1Module* listener = nullptr);
	void Update(float dt);
	void SetMousePosition();
	void HandleInput();
	void Draw() const;
	void DrawAbove() const;

private:
	UI_EVENT UIevent = UI_EVENT_NONE;
	UICursor_Info cursor;

	SDL_Rect default = { 0,0,0,0 };
	SDL_Rect onClick = { 0,0,0,0 };
};

#endif //__UICursor_H__