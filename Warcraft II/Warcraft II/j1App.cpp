#include <iostream> 

#include "Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Particles.h"
#include "j1Collision.h"
#include "j1FadeToBlack.h"
#include "j1EntityFactory.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"

#include "j1App.h"
#include "Brofiler\Brofiler.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	wantToSave = wantToLoad = false;

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	particles = new j1Particles();
	collision = new j1Collision();
	fade = new j1FadeToBlack();
	entities = new j1EntityFactory();
	pathfinding = new j1PathFinding();
	font = new j1Fonts();
	gui = new j1Gui();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(entities);
	AddModule(particles);
	AddModule(collision);
	AddModule(font);
	AddModule(gui);

	AddModule(scene);
	AddModule(fade);

	// render last to swap buffer
	AddModule(render);
}

// Destructor
j1App::~j1App()
{
	// release modules
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend())
	{
		delete *item;
		item++;
	}
	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	pugi::xml_document	configFile;
	pugi::xml_node		config;
	pugi::xml_node		appConfig;

	bool ret = false;

	loadGame = "save_game.xml";
	saveGame = "save_game.xml";

	config = LoadConfig(configFile); //root node

	if (!config.empty())
	{
		// self-config
		ret = true;
		appConfig = config.child("app");
		title.assign(appConfig.child("title").childValue());
		organization.assign(appConfig.child("organization").childValue());
		capFrames = config.child("renderer").child("CapFrames").attribute("value").as_uint();
	}

	if (ret == true)
	{
		list<j1Module*>::const_iterator item;
		item = modules.begin();

		while (item != modules.end() && ret)
		{
			ret = (*item)->Awake(config.child((*item)->name.data()));
			item++;
		}
	}

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();

	while (item != modules.end() && ret)
	{
		if ((*item)->active)
			ret = (*item)->Start();

		item++;
	}

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) || quitGame)
		ret = false;

	if (ret)
		ret = PreUpdate();

	if (ret)
		ret = DoUpdate();

	if (ret)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& configFile) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = configFile.loadFile("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = configFile.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	perfClock.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);

	if (wantToSave)
		SavegameNow();

	if (wantToLoad)
		LoadGameNow();

	float msSinceStartup = clock.Read();

	uint32 actualFrameMs = perfClock.ReadMs();

	lastFrameMs = actualFrameMs;

	uint32 framesOnLastUpdate = 0;
	frameCount++;

	/*
	if (App->entities->playerData != nullptr) {
		if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN
			&& (App->entities->playerData->animationPlayer == &App->entities->playerData->player.idle || App->entities->playerData->animationPlayer == &App->entities->playerData->player.idle2)
			&& !App->menu->active)
			toCap = !toCap;
	}
	else
		if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN && !App->menu->active)
			toCap = !toCap;
	*/

	// Cap frames
	if (!App->render->vsync && toCap) {
		float toVsync = 1000 / capFrames;

		if (actualFrameMs < toVsync)
			SDL_Delay(toVsync - actualFrameMs);
	}

	double fps = 1000.0f / perfClock.ReadMs();

	double avgFPS = (float)frameCount / clock.ReadSec();

	dt = 1.0f / fps;

	string capOnOff;
	if (toCap)
		capOnOff = "on";
	else
		capOnOff = "off";


	string vSyncOnOff;
	if (App->render->vsync)
		vSyncOnOff = "on";
	else
		vSyncOnOff = "off";

	string godMode;
	if (App->scene->god)
		godMode = "on";
	else
		godMode = "off";

	static char title[256];

	sprintf_s(title, 256, "FPS: %.2f | AvgFPS: %.2f | Last Frame Ms: %02u | capFrames: %s | Vsync: %s",
		fps, avgFPS, actualFrameMs, capOnOff.data(), vSyncOnOff.data());

	if (App->scene->pause) {
		auxiliarDt = dt;
		dt = 0.0f;
	}

	App->win->SetTitle(title);
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();
	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Azure);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	item = modules.begin();

	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::LightSeaGreen);

	bool ret = true;
	list<j1Module*>::const_iterator item;
	j1Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret; ++item)
	{
		pModule = *item;

		if (!pModule->active) {
			continue;
		}

		ret = (*item)->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend() && ret)
	{
		ret = (*item)->CleanUp();
		item++;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.data();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.data();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list

	wantToLoad = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	wantToSave = true;
}

// ---------------------------------------
void j1App::GetSaveGames(list<string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.loadFile(loadGame.data());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", loadGame.data());

		root = data.child("game_state");

		list<j1Module*>::const_iterator item = modules.begin();
		ret = true;

		while (item != modules.end() && ret)
		{
			ret = (*item)->Load(root.child((*item)->name.data()));
			item++;
		}

		data.reset();
		if (ret)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (*item) ? (*item)->name.data() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", loadGame.data(), result.description());

	wantToLoad = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", saveGame.data());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret)
	{
		ret = (*item)->Save(root.append_child((*item)->name.data()));
		item++;
	}

	if (ret)
	{
		data.save_file(saveGame.data());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (*item) ? (*item)->name.data() : "unknown");

	data.reset();
	wantToSave = false;
	return ret;
}