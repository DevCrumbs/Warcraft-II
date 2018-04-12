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

	bool Awake(pugi::xml_node& config) { return true; }
	bool Start();
	bool Update(float dt);


private:

	void LoadSceneOne(bool isWin);
	void LoadSceneTwo();
	void DeleteScene();
	void DeleteSceneTwo();

	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

private:

	vector<UILabel*> labelVector;
	vector<UIImage*> imageVector;
	UIButton* continueButt = nullptr;

	SDL_Rect screen;

	string bgTexName;
	SDL_Texture* bg;

	//Values

	uint roomsExploredCont = 0u;


};

#endif //__j1MODULEFINISHGAME_H__