#ifndef __j1FONTS_H__
#define __j1FONTS_H__
#include <list>

#include "SDL\include\SDL_pixels.h"

#include "j1Module.h"

using namespace std;

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class j1Fonts : public j1Module
{
public:

	j1Fonts();

	// Destructor
	virtual ~j1Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Uint32 wrap_length = 0);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	list<_TTF_Font*>	fonts;
	_TTF_Font*			default = nullptr;
};


#endif //__j1FONTS_H__
