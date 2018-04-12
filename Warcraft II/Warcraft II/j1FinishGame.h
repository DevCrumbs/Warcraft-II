#ifndef __j1MODULEFINISHGAME_H__
#define __j1MODULEFINISHGAME_H__

#include "j1Module.h"

#include "Animation.h"

#include "SDL\include\SDL_rect.h"

struct UILabel;

class j1FinishGame : public j1Module
{
public:
	j1FinishGame();
	~j1FinishGame();

	bool Start();
	bool Update(float dt);


private:


};

#endif //__j1MODULEFINISHGAME_H__