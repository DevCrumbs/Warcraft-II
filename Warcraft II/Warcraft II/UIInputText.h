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

	UIInputText(iPoint localPos, UIElement* parent, j1Module* listener);
	~UIInputText() {}

	void Update(float dt);

	iPoint original_pos;

private:

	bool InputText_Actived = false;

	std::string text;

	SDL_Texture* texture = nullptr;
	SDL_Rect r;

};



