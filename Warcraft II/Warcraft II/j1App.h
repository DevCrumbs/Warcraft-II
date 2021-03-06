#ifndef __j1APP_H__
#define __j1APP_H__

#include "j1Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"

#include <list>
#include <string>
using namespace std;

#define MS_PATHFINDING 8.0

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1Scene;
class j1Intro;
class j1Map;
class j1Particles;
class j1Collision;
class j1FadeToBlack;
class j1PathFinding;
class j1EntityFactory;
class j1Fonts;
class j1Gui;
class j1Player;
class j1Console;
class j1Menu;
class j1Movement;
class j1PathManager;
class j1FinishGame;
class j1Printer;
class j1EnemyWave;
class j1FogOfWar;
class j1Video;

struct SDL_Rect;

pugi::xml_node GetNode(pugi::xml_node & node, char * name, bool create);

void SaveAttribute(int value, char * name, pugi::xml_node & general, bool createGeneral = false);
void SaveAttribute(SDL_Rect value, char * name, pugi::xml_node & node, bool createGeneral);
void SaveAttribute(uchar * value, char * name, pugi::xml_node & node, bool createGeneral, int size = 0);

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(j1Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(list<string>& list_to_fill) const;

	uint32 GetSecondsSinceAppStartUp();

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Load save file
	pugi::xml_node LoadSave(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

	double CalculateDt() const;

public:

	// Modules
	j1Window*					win = nullptr;
	j1Input*					input = nullptr;
	j1Render*					render = nullptr;
	j1Textures*					tex = nullptr;
	j1Audio*					audio = nullptr;
	j1Scene*					scene = nullptr;
	j1Intro*					intro = nullptr;
	j1Map*						map = nullptr;
	j1Particles*				particles = nullptr;
	j1Collision*				collision = nullptr;
	j1FadeToBlack*				fade = nullptr;
	j1EntityFactory*			entities = nullptr;
	j1PathFinding*				pathfinding = nullptr;
	j1Fonts*					font = nullptr;
	j1Gui*						gui = nullptr;
	j1Player*					player = nullptr;
	j1Console*					console = nullptr;
	j1Menu*						menu = nullptr;
	j1Movement*					movement = nullptr;
	j1PathManager*				pathmanager = nullptr;
	j1FinishGame*				finish = nullptr;
	j1Printer*					printer = nullptr;
	j1EnemyWave*				wave = nullptr;
	j1FogOfWar*					fow = nullptr;
	j1Video*					video = nullptr;

private:
	

	list<j1Module*>		modules;
	uint				frames = 0;
	double				dt = 0;
	int					argc = 0;
	char**				args;

	string				title;
	string				organization;

	mutable bool		wantToSave = false;
	bool				wantToLoad = false;
	mutable string		saveGame;

	uint64 lastFrameMs = 0;
	uint64 frameCount = 0;
	j1Timer clock;
	j1PerfTimer perfClock;

public:

	pugi::xml_document	configFile;
	pugi::xml_node		config;

	string				loadGame;
	uint				capFrames = 0;
	bool				toCap = true;
	bool				quitGame = false;
	double				auxiliarDt = 0;
	bool				isDebug = false;
};

extern j1App* App; // No student is asking me about that ... odd :-S

#endif //__j1APP_H__