#ifndef __j1MODULEFINISHGAME_H__
#define __j1MODULEFINISHGAME_H__

#include "j1Module.h"

#include "Animation.h"
#include <list>

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

	void LoadScene(bool isWin);


private:

	
	vector<UILabel*> labelVector;
	vector<UIImage*> imageVector;

	SDL_Rect screen;

	string bgTexName;
	SDL_Texture* bg;

};

#endif //__j1MODULEFINISHGAME_H__