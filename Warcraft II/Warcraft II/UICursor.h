#ifndef __UICursor_H__
#define __UICursor_H__

#include "SDL\include\SDL.h"
#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

struct UICursor_Info {
	SDL_Rect default = { 0,0,0,0 };
	SDL_Rect onClick = { 0,0,0,0 };
	SDL_Rect onMine = { 0,0,0,0 };
	SDL_Rect onMineClick = { 0,0,0,0 };
	SDL_Rect onEnemies = { 0,0,0,0 };
	SDL_Rect onEnemiesClick = { 0,0,0,0 };
};

// ---------------------------------------------------

class UICursor : public UIElement
{
public:
	UICursor(iPoint localPos, UIElement* parent, UICursor_Info& info, j1Module* listener = nullptr , bool isInWorld = false);
	void Update(float dt);
	void SetMousePosition();
	void SetTexArea(SDL_Rect texAreaDefault, SDL_Rect texAreaOnClick);
	void HandleInput();
	void DrawAbove() const;

	SDL_Rect GetDefaultTexArea() const;

public:
	SDL_Rect default = { 0,0,0,0 };
	SDL_Rect onClick = { 0,0,0,0 };
	SDL_Rect onMine = { 0,0,0,0 };
	SDL_Rect onMineClick = { 0,0,0,0 };
	SDL_Rect onEnemies = { 0,0,0,0 };
	SDL_Rect onEnemiesClick = { 0,0,0,0 };

	UICursor_Info cursor;

private:
	UI_EVENT UIevent = UI_EVENT_NONE;
};

#endif //__UICursor_H__