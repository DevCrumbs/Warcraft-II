#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
#include "p2List.h"

struct SDL_Texture;
//class GuiImage;
//class GuiText;
class UIImage;
class UILabel;
class UIButton;
//class Window;
//class InputText;
class UISlider;

enum MenuActions
{
	MenuActions_NONE,
	MenuActions_EXIT,
	MenuActions_PLAY,
	MenuActions_SETTINGS,
	MenuActions_RETURN
};

class j1Menu : public j1Module
{
public:

	j1Menu();

	// Destructor
	virtual ~j1Menu();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CreateMenu();
	void CreateSettings();
	//void CreateCredits();
	void DeleteSettings();
	void CreateLoading();
	void DeteleMenu();

	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);


private:

	//Main Menu
	UIButton* PlayButt = nullptr;
	UILabel*  PlayLabel = nullptr;
	UIButton* ExitButt = nullptr;
	UILabel*  ExitLabel = nullptr;
	UIButton* SettingsButt = nullptr;
	UILabel*  SettingsLabel = nullptr;

	//Settings
	UISlider* FPSString = nullptr;
	UILabel*  FPSLabel = nullptr;
	UIButton* AudioFXButt = nullptr;
	UILabel*  AudioFXLabel = nullptr;
	UIButton* AudioMusicButt = nullptr;
	UILabel*  AudioMusicLabel = nullptr;
	UIButton* ReturnButt = nullptr;
	UILabel*  ReturnLabel = nullptr;


	MenuActions menuActions;

public:
	uint tab_button = 0;
	

	bool debug = false;
};

#endif