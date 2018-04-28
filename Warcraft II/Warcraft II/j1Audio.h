#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#include <list>
#include <string>
using namespace std;
typedef unsigned int FX;

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define MAX_AUDIO_VOLUM 128

struct _Mix_Music;
struct Mix_Chunk;

struct GameSounds {

	FX button = 0;
	FX buildingConstruction = 0;
	FX errorButt = 0;
	FX errorButtBuilding = 0;
	FX chickenFarm = 0;
	FX goldMine = 0;
	FX gryphonAviary = 0;
	FX mageTower = 0;
	FX stables = 0;
	FX repairBuild = 0;
	FX destroyBuild = 0;
	FX humanDeath = 0;
	FX orcDeath = 0;
	FX prisionerRescue = 0;
	FX boarDeath = 0;
	FX sheepDeath = 0;
	FX archerCommand = 0;
	FX archerReady = 0;
	FX archerSelected = 0;
	FX footmanCommand = 0;
	FX footmanReady = 0;
	FX footmanSelected = 0;
	FX axeThrow = 0;
	FX arrowThrow = 0;
	FX swordClash = 0;
	FX runeStone = 0;

};

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Controle music volume
	void ChangeMusicVolume(bool positive);

	// Set music volume
	void SetMusicVolume(int volume);

	// Set fx volume
	void SetFxVolume(int volume);

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	// Pause music
	void PauseMusic() const;

	//Charge Sound Effects
	void ChargeFX();

	GameSounds GetFX();

public:
	int musicVolume = 0;
	int fxVolume = 40;

private:

	string				musicFolder;
	string				fxFolder;
	_Mix_Music*			music = nullptr;
	list<Mix_Chunk*>	fx;

	GameSounds gameSounds;

	//FX paths
	//---------------------
	string mainButtonSound;
	string buildingConstructionSound;
	string errorButtonSound;
	string buildingErrorButtonSound;
	string chickenFarmSound;
	string goldMineSound;
	string gryphonAviarySound;
	string mageTowerSound;
	string stablesSound;
	string repairBuildingSound;
	string destroyBuildingSound;
	string runeStoneSound;

	string humanDeadSound;
	string orcDeadSound;
	string prisonerRescueSound;
	string crittersBoarDead;
	string crittersSheepDead;

	string archerGoToPlaceSound;
	string archerReadySound;
	string archerSelectedSound;
	string footmanGoToPlaceSound;
	string footmanReadySound;
	string footmanSelectedSound;

	string axeThrowSound;
	string bowFireSound;
	string swordSound;
	//---------------------

	// FX
	//---------------------

	

	//---------------------
};

#endif //__j1AUDIO_H__
