#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
#include "p2List.h"
#include "UIElement.h"

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
	MenuActions_NONE = -1,
	MenuActions_PLAY_EASYONE,
	MenuActions_PLAY_EASYTWO,
	MenuActions_PLAY_MEDIUMONE,
	MenuActions_PLAY_MEDIUMTWO,
	MenuActions_PLAY_HARD,
	MenuActions_LOADGAME,
	MenuActions_SETTINGS,
	MenuActions_RETURN,
	MenuActions_SLIDERFX,
	MenuActions_SLIDERMUSIC,
	MenuActions_NEWGAME,
	MenuActions_CREDITS,
	MenuActions_EXIT,
};
struct SliderStruct
{
	UISlider* slider = nullptr;
	UILabel*  name = nullptr;
	UILabel*  value = nullptr; 
};

struct ArtifactsCollection
{
	int book, skull, scepter, eye = 0;
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

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CreateMenu();
	void CreateSettings();
	void CreateNewGame();
	void CreateCredits();
	//void CreateLoading();

	void DeleteSettings();
	void DeteleMenu();
	void DeleteNewGame();
	void DeleteCredits();

	void CreateSimpleButt(SDL_Rect normal, SDL_Rect hover, SDL_Rect click, iPoint pos, UIButton*& butt,
		UIE_HORIZONTAL_POS hPos = HORIZONTAL_POS_LEFT, UIE_VERTICAL_POS vPos = VERTICAL_POS_TOP);
	void AddSlider(SliderStruct &sliderStruct, iPoint pos, string NameText, float numberValue, SDL_Rect buttText, SDL_Rect bgText, j1Module* listener);
	UIImage* AddArtifact(iPoint pos, SDL_Rect textArea, Animation anim, int speed);

	void UpdateSlider(SliderStruct &sliderStruct);

	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

public:

	UICursor* mouseText = nullptr;

	uint tab_button = 0;

	bool debug = false;

	ArtifactsCollection artifactsEasyOne, artifactsEasyTwo, artifactsMediumOne, artifactsMediumTwo, artifactsHard;
private:

	//Main Menu
	UIImage* mainMenuImg = nullptr, *settingsBackground = nullptr, *logoImg = nullptr;
	UILabel*  playLabel = nullptr, *exitLabel = nullptr, *settingsLabel = nullptr, *creditsLabel = nullptr, *loadLabel = nullptr;

	Animation menuImgAnim;

	//Settings
	SliderStruct audioFX;
	SliderStruct audioMusic;
	UIButton* fullScreenButt = nullptr;
	UILabel*  returnLabel = nullptr, *fullScreenLabel = nullptr;

	//Start New Game
	UIButton* easyOneButt = nullptr, *easyTwoButt = nullptr, *mediumOneButt = nullptr, *mediumTwoButt = nullptr, *hardButt = nullptr;
	UILabel* bookArtifact = nullptr, *skullArtifact = nullptr, *eyeArtifact = nullptr, *scepterArtifact = nullptr;

	//Credits
	UILabel* sandraLead = nullptr, *oscarCode = nullptr, *davidQA = nullptr, *joanDesigner = nullptr, *manavManagment = nullptr, *davidArt = nullptr, *aleixUI = nullptr;

	//More than one page
	vector<UILabel*> staticLabels; //Not Interactives
	vector<UIImage*> artifacts;


	MenuActions menuActions = MenuActions_NONE;


	//Audio path
	string mainMenuMusicName;

	bool isExit = false;
	bool isFadetoScene = false;
	bool isMouseTextCreated = false;

};

#endif