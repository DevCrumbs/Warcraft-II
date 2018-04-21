#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
#include "p2List.h"

#include <vector>

struct SDL_Texture;
class UIImage;
class UILabel;
class UIButton;
class UISlider;
struct UICursor;
struct Particle;

enum MenuActions
{
	MenuActions_NONE,
	MenuActions_EXIT,
	MenuActions_PLAY,
	MenuActions_SETTINGS,
	MenuActions_RETURN,
	MenuActions_SLIDERFX,
	MenuActions_SLIDERMUSIC
};
struct SliderStruct
{
	UISlider* slider = nullptr;
	UILabel*  name = nullptr;
	UILabel*  value = nullptr; 
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

	void AddSlider(SliderStruct &sliderStruct, iPoint pos, string NameText, float numberValue, SDL_Rect buttText, SDL_Rect bgText, j1Module* listener);
	void UpdateSlider(SliderStruct &sliderStruct);

	UIImage* AddArtifact(iPoint pos, SDL_Rect textArea, Animation anim);

private:

	void ChargeGameSounds();

public:

	UICursor* mouseText = nullptr;

	uint tab_button = 0;

	bool debug = false;

private:

	//Main Menu
	UIImage* mainMenuImg = nullptr;
	UIImage* logoImg = nullptr;
	UIButton* playButt = nullptr;
	UILabel*  playLabel = nullptr;
	UIButton* exitButt = nullptr;
	UILabel*  exitLabel = nullptr;
	UIButton* settingsButt = nullptr;
	UILabel*  settingsLabel = nullptr;
	vector<UIImage*> artifacts;
	Animation menuImgAnim;

	//Settings
	SliderStruct audioFX;
	SliderStruct audioMusic;
	UIButton* returnButt = nullptr, *fullScreenButt = nullptr;
	UILabel*  returnLabel = nullptr, *fullScreenLabel = nullptr;

	Particle* parchment = nullptr;

	MenuActions menuActions;

	//Audio paths
	string mainMenuMusicName;
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

	bool isExit = false;
	bool isFadetoScene = false;
	bool isMouseTextCreated = false;
	bool isSoundCharged = false;

};

#endif