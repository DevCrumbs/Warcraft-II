#ifndef __j1ENEMYSURGE_H__
#define __j1ENEMYSURGE_H__

#include "j1Module.h"

#include <list>
#include <string>
using namespace std;

class j1EnemySurge : public j1Module
{
public:

	j1EnemySurge();

	// Destructor
	virtual ~j1EnemySurge();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();

	bool Update(float ft);

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

public:

private:

};

#endif //__j1EnemySurge_H__
