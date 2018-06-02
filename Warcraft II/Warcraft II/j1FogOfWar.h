#ifndef __FoW_H__
#define __FoW_H__

#include "j1Module.h"
#include "p2Point.h"
#include <vector>
#include <list>
#include <string>

#include "SDL\include\SDL_rect.h"

// defines 1
#define ORIGINAL_TILE App->map->defaultTileSize				// TILES FROM MAP
#define FOW_TILE_MULTIPLIER 1								// MULTIPLIER (FOW TILES CAN BE BIGGER)
#define FOW_TILE (ORIGINAL_TILE * FOW_TILE_MULTIPLIER)		// FOW OF WAR TILES
#define TILE_PARTITIONS 1									// PARTITIONS FOR ART OPTIMIZATION

#define RADIUS 6											// FOW TRANSPARENT RADIUS FROM PLAYER
#define TRANSLUCID_ALPHA 125								// WHEN A FOW TILES HAVE BEEN DISCOVERED BUT IS NOT SEEN
#define NORMAL_ALPHA 235

struct SDL_Texture;
struct SDL_Rect;

class Entity;

enum FoWTileSprite {

	// w:16,h:16

	/// ONLY FOR TRANSPARENT TILES
	FoWTileSprite_OpenCircleTopLeft, // x:64,y:0
	FoWTileSprite_OpenCircleTopRight, // x:80,y:0
	FoWTileSprite_OpenCircleBottomLeft, // x:64,y:16
	FoWTileSprite_OpenCircleBottomRight, // x:80,y:16

	FoWTileSprite_FullCircleTopLeft, // x:32,y:0
	FoWTileSprite_FullCircleTopRight, // x:48,y:0
	FoWTileSprite_FullCircleBottomLeft, // x:32,y:16
	FoWTileSprite_FullCircleBottomRight, // x:48,y:16

	FoWTileSprite_QuarterBlack, // x:0,y:0
	FoWTileSprite_QuarterTransparent,

	FoWTileSprite_HorizontalWaveTopLeft, // x:96,y:0
	FoWTileSprite_HorizontalWaveTopRight, // x:112,y:0
	FoWTileSprite_HorizontalWaveBottomLeft, // x:96,y:16
	FoWTileSprite_HorizontalWaveBottomRight, // x:112,y:16

	FoWTileSprite_VerticalWaveTopLeft, // x:128,y:0
	FoWTileSprite_VerticalWaveTopRight, // x:144,y:0
	FoWTileSprite_VerticalWaveBottomLeft, // x:128,y:16
	FoWTileSprite_VerticalWaveBottomRight, // x:144,y:16
};

// 0: top-left, 1: top-right, 2: bottom-left, 3: bottom-right

struct FogOfWarTile
{
	iPoint pos;
	int size = 32;
	int alpha = NORMAL_ALPHA;
	int normalAlpha = NORMAL_ALPHA;
	int auxAlpha = 0;

	FoWTileSprite tileSprite[4] = { FoWTileSprite_QuarterBlack, FoWTileSprite_QuarterBlack, FoWTileSprite_QuarterBlack, FoWTileSprite_QuarterBlack };
	FoWTileSprite auxTileSprite[4] = { FoWTileSprite_QuarterTransparent, FoWTileSprite_QuarterTransparent, FoWTileSprite_QuarterTransparent, FoWTileSprite_QuarterTransparent };
};

class j1FogOfWar : public j1Module
{
public:
	j1FogOfWar();
	~j1FogOfWar();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool LoadFoW();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void Print();

	void LoadFoWMap(int mapWidth, int mapHeight);
	bool IsOnSight(iPoint pos);
	bool IsOnSight(fPoint pos);
	void UnLoadFowMap();
	void TilesNearPlayer();
	int TotalDistanceToPlayer(Entity* entity, int tile);

	// Beauty
	void DetermineSpriteTile(int tile);
	bool IsGreyTileSurroundingTile(int tile, uint position);

	void ResetTiles();

	void CleanSafeZone(SDL_Rect zone);

public:
	std::vector<FogOfWarTile*> fowTilesVector;
	std::vector<FogOfWarTile*> fowSmallerTilesVector;

	int width = 0;
	int height = 0;

	bool isActive = true;
	bool isFoWDebug = false;

	// Beauty
	string debugTilesTexName;
	string blackTilesTexName;
	string greyTilesTexName;
	SDL_Texture* debugTilesTex = nullptr;
	SDL_Texture* blackTilesTex = nullptr;
	SDL_Texture* greyTilesTex = nullptr;
};

#endif