#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"
#include "UIInputText.h"

#include <list>
#include <string>
using namespace std;

class UIInputText;

class j1Console : public j1Module
{
public:

	j1Console();

	// Destructor
	virtual ~j1Console();

	bool Start();

	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:



public:

	//list<string> memory;

	//UIInputText* inputText;
	//bool isConsole = false;
	//bool exit = false;

	//string		name;
};

#endif //__j1CONSOLE_H__
