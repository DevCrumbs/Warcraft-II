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
	int size = 0;
	int alpha = 255;
	int normalAlpha = 255;
};

class j1FogOfWar : public j1Module
{
public:
	j1FogOfWar();
	~j1FogOfWar();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void print();

	// Part 1
	void LoadFoWMap(int mapWidth, int mapHeight);
	void UnLoadFowMap();
	void TilesNearPlayer();
	int TotalDistanceToPlayer(Entity* entity, int tile);

	void ResetTiles();

	void CleanSafeZone(SDL_Rect zone);

public:
	std::vector<FogOfWarTile*> fowTilesVector;
	std::vector<FogOfWarTile*> fowSmallerTilesVector;
};

#endif