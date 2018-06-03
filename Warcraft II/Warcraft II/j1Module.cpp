#include "j1Module.h"

#include "j1App.h"
#include "j1Input.h"
#include "p2Log.h"


bool j1Module::LoadKey(SDL_Scancode* button, char* name, pugi::xml_node& buttons)
{
	bool ret = false;

	if (button != nullptr && name != nullptr)
	{
		SDL_Scancode buttonValue = (SDL_Scancode)buttons.attribute(name).as_int();

		if (buttonValue != SDL_SCANCODE_UNKNOWN)
		{
			*button = buttonValue;
			App->input->AddKey(button);
			ret = true;
		}
		else
			LOG("Could not load %c button", name);
	}
	return ret;
}