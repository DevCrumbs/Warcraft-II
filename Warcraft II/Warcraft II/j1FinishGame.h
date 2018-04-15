#ifndef __j1MODULEFINISHGAME_H__
#define __j1MODULEFINISHGAME_H__

#include "j1Module.h"

#include "Animation.h"
//#include <list>
#include <vector>

#include "SDL\include\SDL_rect.h"

struct UILabel;
struct UIImage;
struct UIButton;

class j1FinishGame : public j1Module
{
public:
	j1FinishGame();
	~j1FinishGame();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool CleanUp();


private:

	void LoadSceneOne(bool isWin);
	void LoadSceneTwo();
	void DeleteScene();
	void DeleteSceneTwo();

	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

private:

	vector<UILabel*> labelVector;
	vector<UIImage*> imageVector;
	UIButton* continueButt = nullptr, *returnButt = nullptr;

	SDL_Rect screen;

	string bgTexName;
	SDL_Texture* bg;

	//Values

	uint roomsExploredCont = 0u;

	//Music paths
	string victoryMusicPath;
	string defeatMusicPath;


};

#endif //__j1MODULEFINISHGAME_H__