#include <iostream>

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

#include "Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"

using namespace std;

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.assign("audio");
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;

	musicFolder.assign(config.child("music_folder").childValue());
	fxFolder.assign(config.child("fx_folder").childValue());

	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	musicVolume = config.child("volume").attribute("default").as_int();

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	//Sounds
	pugi::xml_node audioPaths = config.child("audioPaths");
	pugi::xml_node sounds = audioPaths.child("sounds");

	pugi::xml_node uIButtonsSounds = sounds.child("buttonPaths");
	mainButtonSound = uIButtonsSounds.attribute("menuButton").as_string();
	errorButtonSound = uIButtonsSounds.attribute("errorBttn").as_string();

	pugi::xml_node buildingSounds = sounds.child("buildingPaths");
	buildingConstructionSound = buildingSounds.attribute("buildingConstruction").as_string();
	buildingErrorButtonSound = buildingSounds.attribute("errorBttn").as_string();
	chickenFarmSound = buildingSounds.attribute("chickenFarm").as_string();
	goldMineSound = buildingSounds.attribute("goldMine").as_string();
	gryphonAviarySound = buildingSounds.attribute("gryphAviar").as_string();
	mageTowerSound = buildingSounds.attribute("mageTower").as_string();
	stablesSound = buildingSounds.attribute("stables").as_string();
	repairBuildingSound = buildingSounds.attribute("repair").as_string();
	destroyBuildingSound = buildingSounds.attribute("destroyBuilding").as_string();
	runeStoneSound = buildingSounds.attribute("runeStone").as_string();

	pugi::xml_node unitsSounds = sounds.child("unitsPaths");
	humanDeadSound = unitsSounds.attribute("humanDeadSound").as_string();
	orcDeadSound = unitsSounds.attribute("orcDeadSound").as_string();
	prisonerRescueSound = unitsSounds.attribute("prisonerRescue").as_string();

	pugi::xml_node crittersSounds = sounds.child("crittersPaths");
	crittersBoarDead = crittersSounds.attribute("boarDead").as_string();
	crittersSheepDead = crittersSounds.attribute("sheepDead").as_string();

	pugi::xml_node archerSounds = sounds.child("archerPaths");
	archerGoToPlaceSound = archerSounds.attribute("goToPlace").as_string();
	archerReadySound = archerSounds.attribute("ready").as_string();
	archerSelectedSound = archerSounds.attribute("selected").as_string();

	pugi::xml_node footmanSounds = sounds.child("footmanPaths");
	footmanGoToPlaceSound = footmanSounds.attribute("goToPlace").as_string();
	footmanReadySound = footmanSounds.attribute("ready").as_string();
	footmanSelectedSound = footmanSounds.attribute("selected").as_string();

	pugi::xml_node attackSounds = sounds.child("attackPaths");
	axeThrowSound = attackSounds.attribute("axeThrow").as_string();
	bowFireSound = attackSounds.attribute("bowFire").as_string();
	swordSound = attackSounds.attribute("sword").as_string();


	return ret;
}

bool j1Audio::Start()
{
	ChargeFX();

	return true;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	list<Mix_Chunk*>::const_iterator item;
	for (item = fx.begin(); item != fx.end(); ++item)
		Mix_FreeChunk(*item);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	string tmp = musicFolder.data();
	tmp += path;

	music = Mix_LoadMUS(tmp.data());

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	string tmp = fxFolder.data();
	tmp += path;

	Mix_Chunk* chunk = Mix_LoadWAV(tmp.data());

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (id > 0 && id <= fx.size())
	{
		list< Mix_Chunk*>::const_iterator it;
		it = next(fx.begin(), id - 1);
		Mix_PlayChannel(-1, *it, repeat);
	}

	return ret;
}

// Control music volume
void j1Audio::ChangeMusicVolume(bool positive) {
	if (positive && musicVolume <= 120) {
		cout << "Music volume was: " << Mix_VolumeMusic(musicVolume += 8) << endl;
	}
	else if (!positive && musicVolume >= 8) {
		cout << "Music volume was: " << Mix_VolumeMusic(musicVolume -= 8) << endl;
	}

	cout << "Music volume is now: " << Mix_VolumeMusic(-1) << endl;
}

bool j1Audio::Load(pugi::xml_node& save) {
	if (save.child("volume").attribute("default") != NULL) {
		musicVolume = save.child("volume").attribute("default").as_int();
		Mix_VolumeMusic(musicVolume);
	}

	return true;
}

bool j1Audio::Save(pugi::xml_node& save) const {

	if (save.child("volume") == NULL)
		save.append_child("volume").append_attribute("default") = musicVolume;
	else
		save.child("volume").attribute("default").set_value(musicVolume);

	return true;
}

// Pause music
void j1Audio::PauseMusic() const {
	Mix_FadeOutMusic(300);
}

void j1Audio::ChargeFX()
{
	gameSounds.button = App->audio->LoadFx(mainButtonSound.data()); //1 Normal bttn sound
	gameSounds.buildingConstruction = App->audio->LoadFx(buildingConstructionSound.data()); //2 Construction building
	gameSounds.errorButt = App->audio->LoadFx(errorButtonSound.data()); //3 Normal error bttn sound
	gameSounds.errorButtBuilding = App->audio->LoadFx(buildingErrorButtonSound.data()); //4 Building placement error sound
	gameSounds.chickenFarm = App->audio->LoadFx(chickenFarmSound.data()); //5 chicken farm sound
	gameSounds.goldMine = App->audio->LoadFx(goldMineSound.data()); //6 gold mine sound
	gameSounds.gryphonAviary = App->audio->LoadFx(gryphonAviarySound.data()); //7 gryphon aviary sound
	gameSounds.mageTower = App->audio->LoadFx(mageTowerSound.data()); //8 mage tower sound
	gameSounds.stables = App->audio->LoadFx(stablesSound.data()); //9 stables sound
	gameSounds.repairBuild = App->audio->LoadFx(repairBuildingSound.data()); //10 repair building sound
	gameSounds.destroyBuild = App->audio->LoadFx(destroyBuildingSound.data()); //11 destroy building sound
	
	gameSounds.humanDeath = App->audio->LoadFx(humanDeadSound.data()); //12
	gameSounds.orcDeath = App->audio->LoadFx(orcDeadSound.data()); //13
	gameSounds.prisionerRescue = App->audio->LoadFx(prisonerRescueSound.data()); //14
	gameSounds.boarDeath = App->audio->LoadFx(crittersBoarDead.data()); //15
	gameSounds.sheepDeath = App->audio->LoadFx(crittersSheepDead.data()); //16
	gameSounds.archerCommand = App->audio->LoadFx(archerGoToPlaceSound.data()); //17
	gameSounds.archerReady = App->audio->LoadFx(archerReadySound.data()); //18
	gameSounds.archerSelected = App->audio->LoadFx(archerSelectedSound.data()); //19
	gameSounds.footmanCommand = App->audio->LoadFx(footmanGoToPlaceSound.data()); //20
	gameSounds.footmanReady = App->audio->LoadFx(footmanReadySound.data()); //21
	gameSounds.footmanSelected = App->audio->LoadFx(footmanSelectedSound.data()); //22
	gameSounds.axeThrow = App->audio->LoadFx(axeThrowSound.data()); //23
	gameSounds.arrowThrow = App->audio->LoadFx(bowFireSound.data()); //24
	gameSounds.swordClash = App->audio->LoadFx(swordSound.data()); //25
	gameSounds.runeStone = App->audio->LoadFx(runeStoneSound.data()); //26
}

GameSounds j1Audio::GetFX()
{
	return gameSounds;
}

// Set music volume
void j1Audio::SetMusicVolume(int volume) {
	Mix_VolumeMusic(volume);
	musicVolume = volume;
}

// Set FX volume
void j1Audio::SetFxVolume(int volume) {
	Mix_Volume(-1, volume);
	fxVolume = volume;
}