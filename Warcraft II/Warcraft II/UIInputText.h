#pragma once

#include "j1App.h"
#include "UIElement.h"
#include "j1Gui.h"

#include <string.h>
#include "SDL\include\SDL.h"

class UIInputText : public UIElement
{
private:

public:

	UIInputText(iPoint localPos, UIElement* parent, j1Module* listener = nullptr, bool isInWorld = false);
	~UIInputText();

	void Update(float dt);
	void ChangeInputState();
	void CleanText();

	iPoint original_pos;
	std::string text;

private:

	bool isInputText = false;

	SDL_Texture* texture = nullptr;
	SDL_Rect r{ 0,0,0,0 };

};



