#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"

#include <list>
#include <string>
using namespace std;

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

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

private:

	string				musicFolder;
	string				fxFolder;
	_Mix_Music*			music = nullptr;
	list<Mix_Chunk*>	fx;

public:
	int musicVolume = 0;
	int fxVolume = 40;
};

#endif //__j1AUDIO_H__
