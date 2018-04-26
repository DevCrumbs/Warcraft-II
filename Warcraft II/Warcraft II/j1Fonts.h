#ifndef __j1FONTS_H__
#define __j1FONTS_H__
#include <list>
#include <map>

#include "SDL\include\SDL_pixels.h"

#include "j1Module.h"

using namespace std;

#define DEFAULT_FONT "data/fonts/EurostileLTStd-BoldEx2.otf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

enum FONT_NAME {
	FONT_NAME_DEFAULT,
	FONT_NAME_WARCRAFT,
	FONT_NAME_WARCRAFT9,
	FONT_NAME_WARCRAFT11,
	FONT_NAME_WARCRAFT14,
	FONT_NAME_WARCRAFT20,
	FONT_NAME_WARCRAFT25,
	FONT_NAME_MAX_FONTS
};

class j1Fonts : public j1Module
{
public:

	j1Fonts();

	// Destructor
	virtual ~j1Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();


	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Uint32 wrap_length = 0);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

	_TTF_Font* GetFont(FONT_NAME fontName);

public:

	list<_TTF_Font*>	fonts;
	_TTF_Font*			default = nullptr;

private:
	map<FONT_NAME, _TTF_Font*> mapFonts;

};


#endif //__j1FONTS_H__
