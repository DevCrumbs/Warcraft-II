#ifndef __UIMinimap_H__
#define __UIMinimap_H__

#include "SDL\include\SDL.h"

#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1EntityFactory.h"
#include "j1Gui.h"

struct UIMinimap_Info 
{
	bool draggable = false;
	bool interactive = false;
	bool interactionFromFather = false;

	int entityWidth = 0;
	int entityHeight = 0;

	SDL_Rect minimapInfo = { 0,0,0,0 };
};

enum MINIMAP_TYPE
{
	MinimapType_NONE,
	MinimapType_FULL_MAP,
	MinimapType_SECCTION
};

class UIMinimap : public UIElement
{
public:
	UIMinimap(iPoint localPos, UIElement* parent, UIMinimap_Info& info, j1Module* listener = nullptr);
	UIMinimap();

	~UIMinimap();

	void Update(float dt);

	void HandleInput(float dt);

	iPoint GetMousePos();

	iPoint MinimapToMap(iPoint pos);
	iPoint MinimapToMap();


	bool SetMinimap(SDL_Rect pos, int entityW, int entityH);
	bool LoadMap();

	bool SaveInRenderer(const SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float scale = 1, SDL_Renderer* renderer = nullptr);



public:

	bool tab = false;
	bool nextEvent = false;

	bool movingMap = false;


private:

	UI_EVENT UIevent = UI_EVENT_NONE;
	SDL_Rect normalTexArea = { 0,0,0,0 };

	SDL_Rect textArea{ 0,0,0,0 };

	int entityWidth = 0;
	int entityHeight = 0;

	float scaleFactor = 0.0f;

	SDL_Rect minimapInfo = { 0,0,0,0 };

	SDL_Texture* mapTexture = nullptr;


	mutable list<Entity*> entities;


	int maxOffsetX = 0;
	int maxOffsetY = 0;
	int offsetX = 0;
	int offsetY = 0;
	int prevOffsetX = 0;
	int prevOffsetY = 0;

	list<DynamicEntity*>* activeDynamicEntities;
	list<StaticEntity*>* activeStaticEntities;

};

#endif