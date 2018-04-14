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
;

	bool SetMinimap(SDL_Rect pos, int entityW, int entityH);
	bool LoadMap();

	bool SaveInRenderer(const SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float scale = 1, SDL_Renderer* renderer = nullptr);

	bool AddEntity(Entity* entity);


public:

	bool tab = false;
	bool nextEvent = false;


private:



	UI_EVENT UIevent = UI_EVENT_NONE;
	SDL_Rect normalTexArea = { 0,0,0,0 };

	int entityWidth = 0;
	int entityHeight = 0;

	float scaleFactor = 0.0f;

	SDL_Rect minimapInfo = { 0,0,0,0 };

	SDL_Texture* mapTexture = nullptr;

	list<Entity*> entities;

};

#endif