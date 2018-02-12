#ifndef __UICursor_H__
#define __UICursor_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UICursor_Info {
	UIElement_Rect default = UIElement_Rect::NO_ELEMENT_RECT_;
	UIElement_Rect on_click = UIElement_Rect::NO_ELEMENT_RECT_;
};

// ---------------------------------------------------

class UICursor : public UIElement
{
public:
	UICursor(iPoint local_pos, UIElement* parent, UICursor_Info& info, j1Module* listener = nullptr);
	void Update(float dt);
	void SetMousePosition();
	void HandleInput();
	void Draw() const;
	void DrawAbove() const;

private:
	UIEvents UIevent = NO_EVENT_;
	UICursor_Info cursor;

	SDL_Rect default = { 0,0,0,0 };
	SDL_Rect on_click = { 0,0,0,0 };
};

#endif //__UICursor_H__