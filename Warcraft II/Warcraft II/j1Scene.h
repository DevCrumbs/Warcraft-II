#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"

#include <vector>
#include <string>
using namespace std;

struct SDL_Texture;
struct UILifeBar;
struct UIWindow;
struct UILabel;
struct UIButton;
struct UIImage;
struct UISlider;
class UIInputText;
enum StaticEntityType;

class j1Scene : public j1Module
{
public:
	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

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

	// On triggered
	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

	// Save
	bool Save(pugi::xml_node&) const;

	// Load
	bool Load(pugi::xml_node&);

	// Debug keys
	void DebugKeys();

	void CheckCameraMovement(float dt);

	void LoadInGameUI();
	void LoadBuildingMenu();
	void UnLoadBuildingMenu();

	bool LoadKeys(pugi::xml_node&);

public:
	// Camera
	float up = false, down = false, left = false, right = false;
	uint width = 0;
	uint height = 0;
	float scale = 0;

	// Player
	bool god = false;

	bool pause = false;

	StaticEntityType GetAlphaBuilding();
	void SetAplphaBuilding(StaticEntityType alphaBuilding);

private:

	//UI
	UIButton * buildingButton, *chickenFarmButton, *elvenLumberButton, *blackSmithButton, *stablesButton, *gryphonAviaryButton, *mageTowerButton, *churchButton, *scoutTowerButton, *guardTowerButton, *cannonTowerButton;
	UILabel * buildingLabel, *chickenFarmLabel, *elvenLumberLabel, *blackSmithLabel, *stablesLabel, *gryphonAviaryLabel, *mageTowerLabel, *churchLabel, *scoutTowerLabel, *guardTowerLabel, *cannonTowerLabel;
	UIImage * buildingMenu;

	bool buildingMenuOn = false;

	string orthogonalMap, isometricMap, warcraftMap;
	string orthogonalTexName, isometricTexName, warcraftTexName;
	bool orthogonalActive, isometricActive, warcraftActive;

	SDL_Texture* debugTex =	nullptr;

	iPoint mouse = { 0,0 };

	//Camera attributes
	float camSpeed = 0.0f;
	int camMovMargin = 0;

	SDL_Scancode buttonSaveGame =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonLoadGame =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonFullScreen = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonGodMode =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveUp =		SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveDown =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveLeft =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonMoveRight =  SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonLeaveGame =	SDL_SCANCODE_UNKNOWN;
	SDL_Scancode buttonReloadMap = SDL_SCANCODE_UNKNOWN;

	StaticEntityType alphaBuilding;

};

#endif //__j1SCENE1_H__
