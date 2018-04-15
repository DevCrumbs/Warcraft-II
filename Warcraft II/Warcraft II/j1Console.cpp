#include "j1App.h"
#include "j1Console.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Render.h"
#include "j1Window.h"


j1Console::j1Console() : j1Module()
{
	name.assign("Console");
}

// Destructor
j1Console::~j1Console()
{}


bool j1Console::Start()
{
	inputText = App->gui->CreateUIInputText({ 100,100 }, this);
	return true;
}


bool j1Console::Update(float dt) 
{
	/*
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN) {
		if (!isConsole) {
			isConsole = true;
			inputText->ChangeInputState();
		}
		else {
			isConsole = false;
			inputText->ChangeInputState();
		}
	}

	if (isConsole) {
		App->render->DrawQuad({ 0,0,1100,100 }, 0, 0, 0, 255);
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
			memory.push_front(inputText->text);
			if (inputText->text == "quit")
				exit = true;
			else if (inputText->text == "fullscreen")
				App->win->SetFullscreen();
			inputText->CleanText();
		}
	}
	*/

	return true;
}


bool j1Console::PostUpdate() {
	bool ret = true;

	if (exit)
		ret = false;

	return ret;
}
// Called before quitting
bool j1Console::CleanUp()
{

	return true;
}
