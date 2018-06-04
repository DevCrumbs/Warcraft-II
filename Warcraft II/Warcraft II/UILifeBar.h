#ifndef __UILifeBar_H__
#define __UILifeBar_H__

#include "SDL\include\SDL.h"

#include "UIElement.h"
#include "j1App.h"
#include "j1Gui.h"

struct UILifeBar_Info 
{
	UILifeBar_Info() {}
	UILifeBar_Info(const UILifeBar_Info& l);
	~UILifeBar_Info() {}

	SDL_Rect background = {0, 0, 0, 0};
	SDL_Rect bar =        {0, 0, 0, 0};
						  
	UIE_HORIZONTAL_POS horizontalOrientation = HORIZONTAL_POS_LEFT;
	UIE_VERTICAL_POS verticalOrientation = VERTICAL_POS_TOP;

	int life = 0;
	int maxLife = 0;
	int maxWidth = 0;
	iPoint lifeBarPosition = { 0,0 };
	iPoint bgPosition = { 0,0 };
};

// ---------------------------------------------------

class UILifeBar : public UIElement
{
public:
	UILifeBar(iPoint local_pos, UIElement* parent, UILifeBar_Info& info, j1Module* listener = nullptr, bool isInWorld = false);
	void DebugDraw(iPoint blit_pos) const;

	void Draw() const;
	void Update(float dt);

	void SetLife(const int life);
	void SetMaxLife(const int maxLife);
	void SetLifeBarPosition(iPoint pos);
	void IncreaseLife(const int life);
	void DecreaseLife(const int life);
	int GetLife() const;

public:

	UILifeBar_Info lifeBar;

private:

	bool isInWorld = false;

	SDL_Rect bar = { 0,0,0,0 };
	SDL_Rect background = { 0,0,0,0 };
};
#endif

