#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include "j1Module.h"

#include "SDL\include\SDL_gamecontroller.h"
#include "SDL\include\SDL_scancode.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButtonDown(int id) const
	{
		return mouseButtons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

	bool IsAnyKeyPressed();

private:
	bool		windowEvents[WE_COUNT];
	KEY_STATE*	keyboard = nullptr;
	KEY_STATE	mouseButtons[NUM_MOUSE_BUTTONS];
	int			mouseMotionX = 0;
	int			mouseMotionY = 0;
	int			mouseX = 0;
	int			mouseY = 0;

	bool		isPressed = false;
};

#endif // __j1INPUT_H__