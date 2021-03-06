#ifndef __j1WINDOW_H__
#define __j1WINDOW_H__

#include "j1Module.h"

struct SDL_Window;
struct SDL_Surface;

class j1Window : public j1Module
{
public:

	j1Window();

	// Destructor
	virtual ~j1Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* newTitle);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	float GetScale() const;
	void SetFullscreen();

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screenSurface = nullptr;
	SDL_Surface* iconSurface = nullptr;

	bool fullscreen = false;

	bool isScreenUpdate = false;

	// Screen parameters
	uint		width = 0;
	uint		height = 0;
	float		scale = 0;

private:
	string		title;
	string		icon;


};

#endif //__j1WINDOW_H__
