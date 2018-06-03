#ifndef __FoW_H__
#define __FoW_H__

#include "j1Module.h"
#include "p2Point.h"
#include <vector>
#include <list>

#include "SDL\include\SDL_rect.h"

// defines 1
#define ORIGINAL_TILE App->map->defaultTileSize				// TILES FROM MAP
#define FOW_TILE_MULTIPLIER 1								// MULTIPLIER (FOW TILES CAN BE BIGGER)
#define FOW_TILE (ORIGINAL_TILE * FOW_TILE_MULTIPLIER)		// FOW OF WAR TILES
#define TILE_PARTITIONS 1									// PARTITIONS FOR ART OPTIMIZATION

#define RADIUS 6											// FOW TRANSPARENT RADIUS FROM PLAYER
#define TRANSLUCID_ALPHA 125								// WHEN A FOW TILES HAVE BEEN DISCOVERED BUT IS NOT SEEN

class Entity;

struct FogOfWarTile
{
	iPoint pos;
	int size = 32;
	int alpha = 235;
	int normalAlpha = 235;
};

class j1FogOfWar : public j1Module
{
public:
	j1FogOfWar();
	~j1FogOfWar();

	bool Start();
	bool LoadFoW();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void print();

	void LoadFoWMap(int mapWidth, int mapHeight);
	bool IsOnSight(iPoint pos);
	bool IsOnSight(fPoint pos);
	void UnLoadFowMap();
	void TilesNearPlayer();
	bool LoadKeys(pugi::xml_node & buttons);
	int TotalDistanceToPlayer(Entity* entity, int tile);

	void ResetTiles();

	void CleanSafeZone(SDL_Rect zone);

public:
	std::vector<FogOfWarTile*> fowTilesVector;
	std::vector<FogOfWarTile*> fowSmallerTilesVector;

	int width = 0;
	int height = 0;

	bool isActive = true;

private:
	SDL_Scancode* buttonDrawFow = nullptr;
};

#endif