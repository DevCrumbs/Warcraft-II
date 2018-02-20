#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include <vector>
#include <string>

#include "j1Module.h"
#include "Animation.h"

#define CAMERA_MOVEMENT_MARGIN 0.05f
#define CAMERA_SPEED 3500

using namespace std;

struct SDL_Texture;
struct UILifeBar;
struct UIWindow;
struct UILabel;
struct UIButton;
struct UIImage;
struct UISlider;

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

	void MoveCamera(float dt);

public:
	// Camera
	float up = false, down = false, left = false, right = false;
	uint width = 0;
	uint height = 0;
	uint scale = 0;

	// Player
	bool god = false;

	bool pause = false;

private:

	string orthogonalMap, isometricMap, warcraftMap;
	string orthogonalTexName, isometricTexName, warcraftTexName;
	bool orthogonalActive, isometricActive, warcraftActive;

	SDL_Texture* debugTex = nullptr;

	iPoint mouse = { 0,0 };
};

#endif //__j1SCENE1_H__
