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
	FX gameStart = 0;
	FX changeKey = 0;
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

	//Death
	FX humanDeath = 0;
	FX orcDeath = 0;
	FX dragonDeath = 0;
	FX prisionerRescue = 0;
	FX boarDeath = 0;
	FX sheepDeath = 0;

	//Base under attack && Room Clear
	FX baseUnderAttack1 = 0;
	FX baseUnderAttack2 = 0;
	FX roomClear = 0;

	//Archer
	FX archerCommand1 = 0;
	FX archerCommand2 = 0;
	FX archerCommand3 = 0;
	FX archerCommand4 = 0;
	FX archerReady = 0;
	FX archerSelected1 = 0;
	FX archerSelected2 = 0;
	FX archerSelected3 = 0;
	FX archerSelected4 = 0;

	//Footman
	FX footmanCommand1 = 0;
	FX footmanCommand2 = 0;
	FX footmanCommand3 = 0;
	FX footmanCommand4 = 0;
	FX footmanReady = 0;
	FX footmanSelected1 = 0;
	FX footmanSelected2 = 0;
	FX footmanSelected3 = 0;
	FX footmanSelected4 = 0;
	FX footmanSelected5 = 0;

	//Gryphon
	FX griffonCommand = 0;
	FX griffonReady = 0;
	FX griffonSelected = 0;
	FX griffonDeath = 0;
	FX griffonAttack = 0;

	FX axeThrow = 0;
	FX arrowThrow = 0;
	FX swordClash = 0;
	
	FX healSound = 0;
	FX goldGetSound = 0;

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
	string gameStartSound;
	string changeKeySound;
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

	string humanDeadSound;
	string orcDeadSound;
	string dragonDeadSound;
	string prisonerRescueSound;
	string crittersBoarDead;
	string crittersSheepDead;

	//Base under attack && Room Clear
	string baseUnderAttackSound1;
	string baseUnderAttackSound2;
	string roomClearSound;

	//Archer
	string archerGoToPlaceSound1;
	string archerGoToPlaceSound2;
	string archerGoToPlaceSound3;
	string archerGoToPlaceSound4;
	string archerReadySound;
	string archerSelectedSound1;
	string archerSelectedSound2;
	string archerSelectedSound3;
	string archerSelectedSound4;

	//Footman
	string footmanGoToPlaceSound1;
	string footmanGoToPlaceSound2;
	string footmanGoToPlaceSound3;
	string footmanGoToPlaceSound4;
	string footmanReadySound;
	string footmanSelectedSound1;
	string footmanSelectedSound2;
	string footmanSelectedSound3;
	string footmanSelectedSound4;
	string footmanSelectedSound5;

	//Gryphon
	string griffonGoToPlaceSound; 
	string griffonReadySound; 
	string griffonSelectedSound;  
	string griffonDeathSound; 
	string griffonAttackSound;
	
	string axeThrowSound;
	string bowFireSound;
	string swordSound;

	string healSoundPath;
	string goldGetSoundPath;
	//---------------------

};

#endif //__j1AUDIO_H__
