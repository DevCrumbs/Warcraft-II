#ifndef __UIMinimap_H__
#define __UIMinimap_H__

#include "SDL\include\SDL.h"

#include "Defs.h"

#include "j1App.h"
#include "UIElement.h"
#include "j1EntityFactory.h"
#include "j1Gui.h"
#include "j1Timer.h"

struct Room;

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

	void Draw() const;

	void HandleInput(float dt);

	iPoint GetEntitiesGoal() const;

	iPoint GetMousePos();

	iPoint MinimapToMap(iPoint pos);
	SDL_Rect MapToMinimap(SDL_Rect pos) const;

	iPoint MinimapToMap();


	bool SetMinimap(SDL_Rect pos, int entityW, int entityH);
	bool DrawRoomCleared(Room room);
	void DrawFoW() const;
	bool LoadMap();

	SDL_Texture* CreateMinimapTexture(SDL_Rect mapSize, SDL_Renderer* renderer, SDL_Surface* mapSurface, float scaleFactor);


	bool SaveInRenderer(const SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float scale = 1, SDL_Renderer* renderer = nullptr);



public:

	bool tab = false;
	bool nextEvent = false;

	bool movingMap = false;

private:

	UI_EVENT UIevent = UI_EVENT_NONE;
	SDL_Rect normalTexArea = { 0,0,0,0 };

	int entityWidth = 0;
	int entityHeight = 0;

	float currentScaleFactor = 0.0f;
	float hiLevelScaleFactor = 0.0f;
	float lowLevelScaleFactor = 0.0f;

	SDL_Rect minimapInfo = { 0,0,0,0 };

	SDL_Texture* hiLevelMapTexture = nullptr;
	SDL_Texture* lowLevelMapTexture = nullptr;
	SDL_Texture* currentMapTexture = nullptr;

	KEY_STATE moveMinimap = KEY_IDLE;

	SDL_Rect hiLevelTextureSize{ 0,0,0,0 };
	SDL_Rect lowLevelTextureSize{ 0,0,0,0 };

	mutable list<Entity*> entities;

	iPoint cameraOffset{ 0,0 };

	mutable int maxOffsetX = 0;
	mutable int maxOffsetY = 0;
	mutable int offsetX = 0;
	mutable int offsetY = 0;
	mutable int prevOffsetX = 0;
	mutable int prevOffsetY = 0;

	bool moveCamera = false;

	int zoomFactor = 1;

	bool lowLevel = false;

	iPoint entitiesGoal{ 0,0 };

	list<DynamicEntity*>* activeDynamicEntities;
	list<StaticEntity*>* activeStaticEntities;

	mutable bool isRoomCleared = false;
	mutable SDL_Rect roomClearedRect{ 0,0,0,0 };
	mutable bool startRoomClearedTimer = false;
	mutable j1Timer roomClearedTimer;


};

#endif