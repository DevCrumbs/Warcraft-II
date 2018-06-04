#include "j1FogOfWar.h"

#include "j1App.h"
#include "j1Printer.h"
#include "j1Scene.h"
#include "j1EntityFactory.h"
#include "j1Map.h"
#include "j1Render.h"
#include "DynamicEntity.h"
#include "j1Collision.h"

#include "Brofiler\Brofiler.h"

j1FogOfWar::j1FogOfWar() : j1Module()
{
	name = "fow";
}

j1FogOfWar::~j1FogOfWar()
{
	for (std::vector<FogOfWarTile*>::iterator iterator = fowTilesVector.begin(); iterator != fowTilesVector.end(); ++iterator)
	{
		delete *iterator;
		*iterator = nullptr;
	}
	fowTilesVector.clear();
}

// Called before render is available
bool j1FogOfWar::Awake(pugi::xml_node& config)
{
	debugTilesTexName = config.child("debugTilesTex").attribute("name").as_string();
	blackTilesTexName = config.child("blackTilesTex").attribute("name").as_string();
	greyTilesTexName = config.child("greyTilesTex").attribute("name").as_string();

	LoadKeys(config.child("buttons"));

	this->config = App->config.child(this->name.data());

	return true;
}

bool j1FogOfWar::Start()
{
	isFoWDebug = false;

	UnLoadFowMap();

	// Load textures
	debugTilesTex = App->tex->Load(debugTilesTexName.data());
	blackTilesTex = App->tex->Load(blackTilesTexName.data());
	greyTilesTex = App->tex->Load(greyTilesTexName.data());

	return true;
}

bool j1FogOfWar::LoadFoW()
{
	bool ret = true;

	isActive = true;

	width = App->map->data.width ;
	height = App->map->data.height ;

	LoadFoWMap(width, height);

	return ret;
}
bool j1FogOfWar::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);

	ResetTiles();
	TilesNearPlayer();
	
	// SDL_SCANCODE_KP_ENTER
	if (App->input->GetKey(buttonDrawFow) == KEY_DOWN && App->isDebug)
	{
		isActive = !isActive;
	}
	return true;
}

bool j1FogOfWar::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);
	if (isActive)
		Print();
	return true;
}

bool j1FogOfWar::CleanUp()
{
	UnLoadFowMap();

	// Unload textures
	App->tex->UnLoad(debugTilesTex);
	App->tex->UnLoad(blackTilesTex);
	App->tex->UnLoad(greyTilesTex);

	return true;
}

void j1FogOfWar::Print()
{	
	// SDL_SCANCODE_F6
	if (App->isDebug && App->input->GetKey(buttonDebugDrawFow) == KEY_DOWN)
		isFoWDebug = !isFoWDebug;

	iPoint startTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale(),
		-App->render->camera.y / App->win->GetScale());
	iPoint endTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale() + App->render->camera.w,
		-App->render->camera.y / App->win->GetScale() + App->render->camera.h);
	int i = startTile.x;
	if (i < 0)
		i = 0;

	for (; i < width && i < endTile.x + 1; ++i)
	{
		int j = startTile.y;
		if (j < 0)
			j = 0;

		for (; j < height && j < endTile.y + 1; ++j)
		{

			int pos = (width * j) + i;

			iPoint world = App->map->MapToWorld(i, j);
			// LOG("Pos is %i", pos);

			// Determine which sprites are going to be blit
			DetermineSpriteTile(pos);

			FogOfWarTile* currTile = fowTilesVector[pos];

			// Blit the sprites for the current tile
			for (uint x = 0; x < 4; x++) {

				iPoint addedPosition = { 0,0 };
				if (x == 1 || x == 3)
					addedPosition.x = 16;
				if (x == 2 || x == 3)
					addedPosition.y = 16;

				SDL_Rect squareToBlit = { 0,0,0,0 };
				squareToBlit.w = 16;
				squareToBlit.h = 16;

				switch (currTile->tileSprite[x]) {

				case FoWTileSprite_OpenCircleTopLeft: // x:64,y:0
					squareToBlit.x = 64;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_OpenCircleTopRight: // x:80,y:0
					squareToBlit.x = 80;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_OpenCircleBottomLeft: // x:64,y:16
					squareToBlit.x = 64;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_OpenCircleBottomRight: // x:80,y:16
					squareToBlit.x = 80;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_FullCircleTopLeft: // x:32,y:0
					squareToBlit.x = 32;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_FullCircleTopRight: // x:48,y:0
					squareToBlit.x = 48;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_FullCircleBottomLeft: // x:32,y:16
					squareToBlit.x = 32;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_FullCircleBottomRight: // x:48,y:16
					squareToBlit.x = 48;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_QuarterBlack: // x:0,y:0
					break;

				case FoWTileSprite_HorizontalWaveTopLeft: // x:96,y:0
					squareToBlit.x = 96;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_HorizontalWaveTopRight: // x:112,y:0
					squareToBlit.x = 112;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_HorizontalWaveBottomLeft: // x:96,y:16
					squareToBlit.x = 96;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_HorizontalWaveBottomRight: // x:112,y:16
					squareToBlit.x = 112;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_VerticalWaveTopLeft: // x:128,y:0
					squareToBlit.x = 128;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_VerticalWaveTopRight: // x:144,y:0
					squareToBlit.x = 144;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_VerticalWaveBottomLeft: // x:128,y:16
					squareToBlit.x = 128;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_VerticalWaveBottomRight: // x:144,y:16
					squareToBlit.x = 144;
					squareToBlit.y = 16;
					break;
				}

				SDL_Texture* tex = debugTilesTex;

				// 1. Blit tileSprite
				if (currTile->alpha == NORMAL_ALPHA) {

					// Also blit grey squares under the irregular black tiles
					if ((squareToBlit.x != 0 || squareToBlit.y != 0)
						&& IsGreyTileSurroundingTile(pos, x)) {

						if (!isFoWDebug)
							tex = greyTilesTex;
						App->printer->PrintSprite({ world.x + addedPosition.x, world.y + addedPosition.y }, tex, { 0,0,16,16 }, Layers_GreyFoW, 0.0f, { 0,0,0,(Uint8)TRANSLUCID_ALPHA });
					}

					if (!isFoWDebug)
						tex = blackTilesTex;
					App->printer->PrintSprite({ world.x + addedPosition.x, world.y + addedPosition.y }, tex, squareToBlit, Layers_BlackFoW, 0.0f, { 0,0,0,(Uint8)currTile->alpha });
				}
				else if (currTile->alpha == TRANSLUCID_ALPHA) {

					if (!isFoWDebug)
						tex = greyTilesTex;
					App->printer->PrintSprite({ world.x + addedPosition.x, world.y + addedPosition.y }, tex, squareToBlit, Layers_GreyFoW, 0.0f, { 0,0,0,(Uint8)currTile->alpha });
				}

				if (currTile->auxTileSprite[x] == FoWTileSprite_QuarterTransparent)
					continue;

				switch (currTile->auxTileSprite[x]) {

				case FoWTileSprite_OpenCircleTopLeft: // x:64,y:0
					squareToBlit.x = 64;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_OpenCircleTopRight: // x:80,y:0
					squareToBlit.x = 80;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_OpenCircleBottomLeft: // x:64,y:16
					squareToBlit.x = 64;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_OpenCircleBottomRight: // x:80,y:16
					squareToBlit.x = 80;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_FullCircleTopLeft: // x:32,y:0
					squareToBlit.x = 32;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_FullCircleTopRight: // x:48,y:0
					squareToBlit.x = 48;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_FullCircleBottomLeft: // x:32,y:16
					squareToBlit.x = 32;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_FullCircleBottomRight: // x:48,y:16
					squareToBlit.x = 48;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_QuarterBlack: // x:0,y:0
					break;

				case FoWTileSprite_HorizontalWaveTopLeft: // x:96,y:0
					squareToBlit.x = 96;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_HorizontalWaveTopRight: // x:112,y:0
					squareToBlit.x = 112;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_HorizontalWaveBottomLeft: // x:96,y:16
					squareToBlit.x = 96;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_HorizontalWaveBottomRight: // x:112,y:16
					squareToBlit.x = 112;
					squareToBlit.y = 16;
					break;

				case FoWTileSprite_VerticalWaveTopLeft: // x:128,y:0
					squareToBlit.x = 128;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_VerticalWaveTopRight: // x:144,y:0
					squareToBlit.x = 144;
					squareToBlit.y = 0;
					break;
				case FoWTileSprite_VerticalWaveBottomLeft: // x:128,y:16
					squareToBlit.x = 128;
					squareToBlit.y = 16;
					break;
				case FoWTileSprite_VerticalWaveBottomRight: // x:144,y:16
					squareToBlit.x = 144;
					squareToBlit.y = 16;
					break;
				}

				// 2. Blit auxTileSprite
				if (currTile->auxAlpha == NORMAL_ALPHA) {

					if (!isFoWDebug)
						tex = blackTilesTex;
					App->printer->PrintSprite({ world.x + addedPosition.x, world.y + addedPosition.y }, tex, squareToBlit, Layers_BlackFoW, 0.0f, { 0,0,0,(Uint8)currTile->auxAlpha });
				}
				else if (currTile->auxAlpha == TRANSLUCID_ALPHA) {

					if (!isFoWDebug)
						tex = greyTilesTex;
					App->printer->PrintSprite({ world.x + addedPosition.x, world.y + addedPosition.y }, tex, squareToBlit, Layers_GreyFoW, 0.0f, { 0,0,0,(Uint8)currTile->auxAlpha });
				}

				//App->render->DrawQuad({ world.x, world.y, FOW_TILE, FOW_TILE }, 0, 0, 0, currTile->alpha);
			}		
		}//for
	}//for
}

void j1FogOfWar::LoadFoWMap(int mapWidth, int mapHeight)
{
	for (int y = 0; y <mapHeight  / FOW_TILE_MULTIPLIER; ++y)
		for (int x = 0; x < mapWidth / FOW_TILE_MULTIPLIER; ++x)
		{
			FogOfWarTile* aux = new FogOfWarTile;
			aux->pos = iPoint(x, y);
			fowTilesVector.push_back(aux);
		}

}

bool j1FogOfWar::IsOnSight(iPoint pos)
{
	bool ret = true;

	if (isActive) 
	{
		iPoint tile = App->map->WorldToMap(pos.x / App->win->GetScale(), pos.y / App->win->GetScale());
		int fowTile = (tile.y * width) + tile.x;

		if (fowTile < fowTilesVector.size())
			ret = (fowTilesVector[fowTile]->alpha == 0);
	}

	return ret;
}

bool j1FogOfWar::IsOnSight(fPoint pos)
{
	return IsOnSight(iPoint{ int(pos.x),int(pos.y) });
}

void j1FogOfWar::UnLoadFowMap()
{
	for (int i = 0; i < fowTilesVector.size(); i++)
		delete fowTilesVector[i];
	fowTilesVector.clear();

}

int j1FogOfWar::TotalDistanceToPlayer(Entity* entity, int tile)
{
	int totalX = (fowTilesVector[tile]->pos.x * FOW_TILE + (FOW_TILE / 2)) / FOW_TILE - ((int)entity->GetPos().x + 40) / FOW_TILE;
	if (totalX < 0)
		totalX *= -1;
	int totalY = (fowTilesVector[tile]->pos.y * FOW_TILE + (FOW_TILE / 2)) / FOW_TILE - (int)entity->GetPos().y / FOW_TILE;
	if (totalY < 0)
		totalY *= -1;
	// TODO 2 : RETURN DISTANCE (use functions as sqrt (square root))
	return sqrt(totalX * totalX + totalY * totalY);;
}

void j1FogOfWar::DetermineSpriteTile(int tile) 
{
	FogOfWarTile* currTile = fowTilesVector[tile];

	FogOfWarTile* right = nullptr;
	FogOfWarTile* left = nullptr;
	FogOfWarTile* bottom = nullptr;
	FogOfWarTile* top = nullptr;

	int sum = 0;
	
	sum = tile + 1;
	if (sum >= 0 && sum < fowTilesVector.size())
		right = fowTilesVector[sum];

	sum = tile - 1;
	if (sum >= 0 && sum < fowTilesVector.size())
		left = fowTilesVector[sum];

	sum = tile + width;
	if (sum >= 0 && sum < fowTilesVector.size())
		bottom = fowTilesVector[sum];

	sum = tile - width;
	if (sum >= 0 && sum < fowTilesVector.size())
		top = fowTilesVector[sum];

	if (currTile != nullptr
		&& right != nullptr && left != nullptr && bottom != nullptr && top != nullptr) {

		currTile->auxAlpha = 0;

		// tileSprite (black and translucid tiles)
		// BLACK TILE
		if (currTile->alpha == NORMAL_ALPHA) {

			// 3 black tiles, 1 translucid or transparent tile
			/// VerticalWave/HorizontalWave
			if (right->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA
				&& (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_HorizontalWaveTopLeft;
				currTile->tileSprite[3] = FoWTileSprite_HorizontalWaveTopRight;

				// Tiles at both sides of the checked tile
				FogOfWarTile* bottomRightSide = nullptr;
				FogOfWarTile* bottomLeftSide = nullptr;

				int sum = 0;

				sum = tile + width + 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					bottomRightSide = fowTilesVector[sum];

				sum = tile + width - 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					bottomLeftSide = fowTilesVector[sum];

				if (bottomRightSide != nullptr && bottomLeftSide != nullptr) {
				
					if (bottomRightSide->alpha == NORMAL_ALPHA || bottomLeftSide->alpha == NORMAL_ALPHA) {
					
						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if (right->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA && bottom->alpha == NORMAL_ALPHA
				&& (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)) {

				currTile->tileSprite[0] = FoWTileSprite_HorizontalWaveBottomLeft;
				currTile->tileSprite[1] = FoWTileSprite_HorizontalWaveBottomRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;

				// Tiles at both sides of the checked tile
				FogOfWarTile* topRightSide = nullptr;
				FogOfWarTile* topLeftSide = nullptr;

				int sum = 0;

				sum = tile - width + 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					topRightSide = fowTilesVector[sum];

				sum = tile - width - 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					topLeftSide = fowTilesVector[sum];

				if (topRightSide != nullptr && topLeftSide != nullptr) {

					if (topRightSide->alpha == NORMAL_ALPHA || topLeftSide->alpha == NORMAL_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if (right->alpha == NORMAL_ALPHA && bottom->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA
				&& (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0)) {

				currTile->tileSprite[0] = FoWTileSprite_VerticalWaveTopRight;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_VerticalWaveBottomRight;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;

				// Tiles at both sides of the checked tile
				FogOfWarTile* leftTopSide = nullptr;
				FogOfWarTile* leftBottomSide = nullptr;

				int sum = 0;

				sum = tile - 1 - width;
				if (sum >= 0 && sum < fowTilesVector.size())
					leftTopSide = fowTilesVector[sum];

				sum = tile - 1 + width;
				if (sum >= 0 && sum < fowTilesVector.size())
					leftBottomSide = fowTilesVector[sum];

				if (leftTopSide != nullptr && leftBottomSide != nullptr) {

					if (leftTopSide->alpha == NORMAL_ALPHA || leftBottomSide->alpha == NORMAL_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if (bottom->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA
				&& (right->alpha == TRANSLUCID_ALPHA || right->alpha == 0)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_VerticalWaveTopLeft;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_VerticalWaveBottomLeft;

				// Tiles at both sides of the checked tile
				FogOfWarTile* rightTopSide = nullptr;
				FogOfWarTile* rightBottomSide = nullptr;

				int sum = 0;

				sum = tile + 1 - width;
				if (sum >= 0 && sum < fowTilesVector.size())
					rightTopSide = fowTilesVector[sum];

				sum = tile + 1 + width;
				if (sum >= 0 && sum < fowTilesVector.size())
					rightBottomSide = fowTilesVector[sum];

				if (rightTopSide != nullptr && rightBottomSide != nullptr) {

					if (rightTopSide->alpha == NORMAL_ALPHA || rightBottomSide->alpha == NORMAL_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}

			// 3 translucid or transparent tiles, 1 black tile
			/// FullCircle
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0) && (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)
				&& bottom->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)
				&& top->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0) && (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)
				&& left->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}
			else if ((bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0) && (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0) && (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)
				&& right->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}

			// 2 translucid or transparent tiles, 2 black tiles
			/// VerticalWave/HorizontalWave + FullCircle
			/// right (3)
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0)
				&& bottom->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA) {

				/// Special case! Top&Bottom
				currTile->tileSprite[0] = FoWTileSprite_VerticalWaveTopRight;
				currTile->tileSprite[1] = FoWTileSprite_VerticalWaveTopLeft;
				currTile->tileSprite[2] = FoWTileSprite_VerticalWaveBottomRight;
				currTile->tileSprite[3] = FoWTileSprite_VerticalWaveBottomLeft;
			}
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)
				&& bottom->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if ((right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)
				&& top->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}

			/// left (2)
			else if ((left->alpha == TRANSLUCID_ALPHA || left->alpha == 0) && (top->alpha == TRANSLUCID_ALPHA || top->alpha == 0)
				&& right->alpha == NORMAL_ALPHA && bottom->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if ((left->alpha == TRANSLUCID_ALPHA || left->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)
				&& right->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}

			/// top (1)
			else if ((top->alpha == TRANSLUCID_ALPHA || top->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)
				&& right->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA) {

				/// Special case! Right&Left
				currTile->tileSprite[0] = FoWTileSprite_HorizontalWaveBottomLeft;
				currTile->tileSprite[1] = FoWTileSprite_HorizontalWaveBottomRight;
				currTile->tileSprite[2] = FoWTileSprite_HorizontalWaveTopLeft;
				currTile->tileSprite[3] = FoWTileSprite_HorizontalWaveTopRight;
			}

			// All translucid or transparent tiles
			else if ((top->alpha == TRANSLUCID_ALPHA || top->alpha == 0) && (bottom->alpha == TRANSLUCID_ALPHA || bottom->alpha == 0)
				&& (right->alpha == TRANSLUCID_ALPHA || right->alpha == 0) && (left->alpha == TRANSLUCID_ALPHA || left->alpha == 0)) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}
			// All black tiles
			else {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
		}

		// GREY TILE
		else if (currTile->alpha == TRANSLUCID_ALPHA) {

			// 3 black or translucid tiles, 1 transparent tile
			/// VerticalWave/HorizontalWave
			if ((right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)
				&& bottom->alpha == 0) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_HorizontalWaveTopLeft;
				currTile->tileSprite[3] = FoWTileSprite_HorizontalWaveTopRight;

				// Tiles at both sides of the checked tile
				FogOfWarTile* bottomRightSide = nullptr;
				FogOfWarTile* bottomLeftSide = nullptr;

				int sum = 0;

				sum = tile + width + 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					bottomRightSide = fowTilesVector[sum];

				sum = tile + width - 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					bottomLeftSide = fowTilesVector[sum];

				if (bottomRightSide != nullptr && bottomLeftSide != nullptr) {

					if (bottomRightSide->alpha == NORMAL_ALPHA || bottomRightSide->alpha == TRANSLUCID_ALPHA || bottomLeftSide->alpha == NORMAL_ALPHA || bottomLeftSide->alpha == TRANSLUCID_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if ((right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA) && (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA)
				&& top->alpha == 0) {

				currTile->tileSprite[0] = FoWTileSprite_HorizontalWaveBottomLeft;
				currTile->tileSprite[1] = FoWTileSprite_HorizontalWaveBottomRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;

				// Tiles at both sides of the checked tile
				FogOfWarTile* topRightSide = nullptr;
				FogOfWarTile* topLeftSide = nullptr;

				int sum = 0;

				sum = tile - width + 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					topRightSide = fowTilesVector[sum];

				sum = tile - width - 1;
				if (sum >= 0 && sum < fowTilesVector.size())
					topLeftSide = fowTilesVector[sum];

				if (topRightSide != nullptr && topLeftSide != nullptr) {

					if (topRightSide->alpha == NORMAL_ALPHA || topRightSide->alpha == TRANSLUCID_ALPHA || topLeftSide->alpha == NORMAL_ALPHA || topLeftSide->alpha == TRANSLUCID_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if ((right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)
				&& left->alpha == 0) {

				currTile->tileSprite[0] = FoWTileSprite_VerticalWaveTopRight;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_VerticalWaveBottomRight;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;

				// Tiles at both sides of the checked tile
				FogOfWarTile* leftTopSide = nullptr;
				FogOfWarTile* leftBottomSide = nullptr;

				int sum = 0;

				sum = tile - 1 - width;
				if (sum >= 0 && sum < fowTilesVector.size())
					leftTopSide = fowTilesVector[sum];

				sum = tile - 1 + width;
				if (sum >= 0 && sum < fowTilesVector.size())
					leftBottomSide = fowTilesVector[sum];

				if (leftTopSide != nullptr && leftBottomSide != nullptr) {

					if (leftTopSide->alpha == NORMAL_ALPHA || leftTopSide->alpha == TRANSLUCID_ALPHA || leftBottomSide->alpha == NORMAL_ALPHA || leftBottomSide->alpha == TRANSLUCID_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}
			else if ((bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)
				&& right->alpha == 0) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_VerticalWaveTopLeft;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_VerticalWaveBottomLeft;

				// Tiles at both sides of the checked tile
				FogOfWarTile* rightTopSide = nullptr;
				FogOfWarTile* rightBottomSide = nullptr;

				int sum = 0;

				sum = tile + 1 - width;
				if (sum >= 0 && sum < fowTilesVector.size())
					rightTopSide = fowTilesVector[sum];

				sum = tile + 1 + width;
				if (sum >= 0 && sum < fowTilesVector.size())
					rightBottomSide = fowTilesVector[sum];

				if (rightTopSide != nullptr && rightBottomSide != nullptr) {

					if (rightTopSide->alpha == NORMAL_ALPHA || rightTopSide->alpha == TRANSLUCID_ALPHA || rightBottomSide->alpha == NORMAL_ALPHA || rightBottomSide->alpha == TRANSLUCID_ALPHA) {

						currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
						currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
					}
				}
			}

			// 3 transparent tiles, 1 black or translucid tile
			/// FullCircle
			else if (right->alpha == 0 && left->alpha == 0 && top->alpha == 0
				&& (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if (right->alpha == 0 && left->alpha == 0 && bottom->alpha == 0
				&& (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}
			else if (right->alpha == 0 && bottom->alpha == 0 && top->alpha == 0
				&& (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}
			else if (bottom->alpha == 0 && left->alpha == 0 && top->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}

			// 2 transparent tiles, 2 black or translucid tiles
			/// VerticalWave/HorizontalWave + FullCircle
			/// right (3)
			else if (right->alpha == 0 && left->alpha == 0
				&& (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)) {

				/// Special case! Top&Bottom
				currTile->tileSprite[0] = FoWTileSprite_VerticalWaveTopRight;
				currTile->tileSprite[1] = FoWTileSprite_VerticalWaveTopLeft;
				currTile->tileSprite[2] = FoWTileSprite_VerticalWaveBottomRight;
				currTile->tileSprite[3] = FoWTileSprite_VerticalWaveBottomLeft;
			}
			else if (right->alpha == 0 && top->alpha == 0
				&& (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if (right->alpha == 0 && bottom->alpha == 0
				&& (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}

			/// left (2)
			else if (left->alpha == 0 && top->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
			else if (left->alpha == 0 && bottom->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)) {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}

			/// top (1)
			else if (top->alpha == 0 && bottom->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				/// Special case! Right&Left
				currTile->tileSprite[0] = FoWTileSprite_HorizontalWaveBottomLeft;
				currTile->tileSprite[1] = FoWTileSprite_HorizontalWaveBottomRight;
				currTile->tileSprite[2] = FoWTileSprite_HorizontalWaveTopLeft;
				currTile->tileSprite[3] = FoWTileSprite_HorizontalWaveTopRight;
			}

			// All transparent tiles
			else if (top->alpha == 0 && bottom->alpha == 0
				&& right->alpha == 0 && left->alpha == 0) {

				currTile->tileSprite[0] = FoWTileSprite_FullCircleTopLeft;
				currTile->tileSprite[1] = FoWTileSprite_FullCircleTopRight;
				currTile->tileSprite[2] = FoWTileSprite_FullCircleBottomLeft;
				currTile->tileSprite[3] = FoWTileSprite_FullCircleBottomRight;
			}

			// All grey tiles
			else {

				currTile->tileSprite[0] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[1] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[2] = FoWTileSprite_QuarterBlack;
				currTile->tileSprite[3] = FoWTileSprite_QuarterBlack;
			}
		}

		// auxTileSprite (translucid and transparent tiles)
		bool isAuxTileSprite = false;

		// TRANSLUCID TILE
		if (currTile->alpha == TRANSLUCID_ALPHA) {

			// 2 translucid tiles, 2 black tiles (ONLY CORNERS!)
			/// OpenCircle
			/// right (3)
			if (bottom->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA) {

				// Not a corner...
				isAuxTileSprite = true;
			}
			if (bottom->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA) {

				if (bottom->tileSprite[0] == FoWTileSprite_QuarterBlack && left->tileSprite[3] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_OpenCircleBottomLeft;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					currTile->auxAlpha = NORMAL_ALPHA;
					isAuxTileSprite = true;
				}
			}
			if (top->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA ) {

				if (top->tileSprite[2] == FoWTileSprite_QuarterBlack && left->tileSprite[1] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_OpenCircleTopLeft;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					currTile->auxAlpha = NORMAL_ALPHA;
					isAuxTileSprite = true;
				}
			}

			/// left (2)
			if (right->alpha == NORMAL_ALPHA && bottom->alpha == NORMAL_ALPHA) {

				if (right->tileSprite[2] == FoWTileSprite_QuarterBlack && bottom->tileSprite[1] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_OpenCircleBottomRight;

					currTile->auxAlpha = NORMAL_ALPHA;
					isAuxTileSprite = true;
				}
			}
			if (right->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA ) {

				if (right->tileSprite[0] == FoWTileSprite_QuarterBlack && top->tileSprite[3] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_OpenCircleTopRight;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					currTile->auxAlpha = NORMAL_ALPHA;
					isAuxTileSprite = true;
				}
			}

			/// top (1)
			if (right->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA) {

				// Not a corner...
				isAuxTileSprite = true;
			}

			// All transparent tiles
			if (!isAuxTileSprite) {

				currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;
			}
		}

		// TRANSPARENT TILE
		else if (currTile->alpha == 0) {

			// 2 transparent tiles, 2 black or translucid tiles (ONLY CORNERS!)
			/// OpenCircle
			/// right (3)
			if (right->alpha == 0 && left->alpha == 0
				&& (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)) {

				// Not a corner...
				isAuxTileSprite = true;
			}
			if (right->alpha == 0 && top->alpha == 0
				&& (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				if (bottom->tileSprite[0] == FoWTileSprite_QuarterBlack && left->tileSprite[3] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_OpenCircleBottomLeft;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					if (bottom->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA)
						currTile->auxAlpha = NORMAL_ALPHA;
					else
						currTile->auxAlpha = TRANSLUCID_ALPHA;

					isAuxTileSprite = true;
				}
			}
			if (right->alpha == 0 && bottom->alpha == 0
				&& (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				if (top->tileSprite[2] == FoWTileSprite_QuarterBlack && left->tileSprite[1] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_OpenCircleTopLeft;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					if (top->alpha == NORMAL_ALPHA && left->alpha == NORMAL_ALPHA)
						currTile->auxAlpha = NORMAL_ALPHA;
					else
						currTile->auxAlpha = TRANSLUCID_ALPHA;

					isAuxTileSprite = true;
				}
			}

			/// left (2)
			if (left->alpha == 0 && top->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (bottom->alpha == NORMAL_ALPHA || bottom->alpha == TRANSLUCID_ALPHA)) {

				if (right->tileSprite[2] == FoWTileSprite_QuarterBlack && bottom->tileSprite[1] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_OpenCircleBottomRight;

					if (right->alpha == NORMAL_ALPHA && bottom->alpha == NORMAL_ALPHA)
						currTile->auxAlpha = NORMAL_ALPHA;
					else
						currTile->auxAlpha = TRANSLUCID_ALPHA;

					isAuxTileSprite = true;
				}
			}
			if (left->alpha == 0 && bottom->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (top->alpha == NORMAL_ALPHA || top->alpha == TRANSLUCID_ALPHA)) {

				if (right->tileSprite[0] == FoWTileSprite_QuarterBlack && top->tileSprite[3] == FoWTileSprite_QuarterBlack) {

					currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[1] = FoWTileSprite_OpenCircleTopRight;
					currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
					currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;

					if (right->alpha == NORMAL_ALPHA && top->alpha == NORMAL_ALPHA)
						currTile->auxAlpha = NORMAL_ALPHA;
					else
						currTile->auxAlpha = TRANSLUCID_ALPHA;

					isAuxTileSprite = true;
				}
			}

			/// top (1)
			if (top->alpha == 0 && bottom->alpha == 0
				&& (right->alpha == NORMAL_ALPHA || right->alpha == TRANSLUCID_ALPHA) && (left->alpha == NORMAL_ALPHA || left->alpha == TRANSLUCID_ALPHA)) {

				// Not a corner...
				isAuxTileSprite = true;
			}

			// All transparent tiles
			if (!isAuxTileSprite) {

				currTile->auxTileSprite[0] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[1] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[2] = FoWTileSprite_QuarterTransparent;
				currTile->auxTileSprite[3] = FoWTileSprite_QuarterTransparent;
			}
		}
	}
}

bool j1FogOfWar::IsGreyTileSurroundingTile(int tile, uint position) 
{
	FogOfWarTile* right = nullptr;
	FogOfWarTile* left = nullptr;
	FogOfWarTile* bottom = nullptr;
	FogOfWarTile* top = nullptr;

	int sum = 0;

	sum = tile + 1;
	if (sum >= 0 && sum < fowTilesVector.size())
		right = fowTilesVector[sum];

	sum = tile - 1;
	if (sum >= 0 && sum < fowTilesVector.size())
		left = fowTilesVector[sum];

	sum = tile + width;
	if (sum >= 0 && sum < fowTilesVector.size())
		bottom = fowTilesVector[sum];

	sum = tile - width;
	if (sum >= 0 && sum < fowTilesVector.size())
		top = fowTilesVector[sum];

	// 0: top-left, 1: top-right, 2: bottom-left, 3: bottom-right

	switch (position) {
	
	case 0:
		if (top != nullptr && left != nullptr)
			return top->alpha == TRANSLUCID_ALPHA || left->alpha == TRANSLUCID_ALPHA;
		break;
	case 1:
		if (top != nullptr && right != nullptr)
			return top->alpha == TRANSLUCID_ALPHA || right->alpha == TRANSLUCID_ALPHA;
		break;
	case 2:
		if (bottom != nullptr && left != nullptr)
			return bottom->alpha == TRANSLUCID_ALPHA || left->alpha == TRANSLUCID_ALPHA;
		break;
	case 3:
		if (bottom != nullptr && right != nullptr)
			return bottom->alpha == TRANSLUCID_ALPHA || right->alpha == TRANSLUCID_ALPHA;
		break;
	default:
		break;
	}

	return false;
}

void j1FogOfWar::ResetTiles()
{
	for (int i = 0; i < fowTilesVector.size(); i++)
	{
		if (fowTilesVector[i]->alpha == 0)
			fowTilesVector[i]->alpha = TRANSLUCID_ALPHA;
	}
}

void j1FogOfWar::CleanSafeZone(SDL_Rect zone)
{
	iPoint startTile = App->map->WorldToMap(zone.x / App->win->GetScale(),
		zone.y / App->win->GetScale());
	iPoint endTile = App->map->WorldToMap(zone.x / App->win->GetScale() + zone.w,
		zone.y / App->win->GetScale() + zone.h);
	int i = startTile.x;
	if (i < 0)
		i = 0;

	for (; i < width && i < endTile.x + 1; ++i)
	{
		int j = startTile.y;
		if (j < 0)
			j = 0;

		for (; j < height && j < endTile.y + 1; ++j)
		{
			int pos = (width * j) + i;

			fowTilesVector[pos]->alpha = 0;
		}//for
	}//for
}


void j1FogOfWar::TilesNearPlayer()
{
	for (list<DynamicEntity*>::const_iterator iterator = App->entities->activeDynamicEntities.begin(); iterator != App->entities->activeDynamicEntities.end(); ++iterator)
	{

		if ((*iterator)->entitySide == EntitySide_Player)
		{
			// Change camera for entity collider
			ColliderGroup* entityCollider = (*iterator)->GetSightRadiusCollider();
	
			for (vector<Collider*>::iterator colliderIterator = entityCollider->colliders.begin(); colliderIterator != entityCollider->colliders.end(); ++colliderIterator)
			{
				int cont = 0;
				int tile = 64;

				iPoint startTile = App->map->WorldToMap((*colliderIterator)->colliderRect.x - tile / App->win->GetScale(),
					(*colliderIterator)->colliderRect.y - tile / App->win->GetScale());
				iPoint endTile = App->map->WorldToMap((*colliderIterator)->colliderRect.x  + tile / App->win->GetScale() + (*colliderIterator)->colliderRect.w,
					(*colliderIterator)->colliderRect.y + tile / App->win->GetScale() + (*colliderIterator)->colliderRect.h);

				//App->printer->PrintQuad((*colliderIterator)->colliderRect, { 255, 255, 255, 255 });

				int i = startTile.x;
				if (i < 0)
					i = 0;

				for (; i < width && i < endTile.x + 1; ++i)
				{
					int j = startTile.y;
					if (j < 0)
						j = 0;

					for (; j < height && j < endTile.y + 1; ++j)
					{

						int pos = (width * j) + i;

						if (fowTilesVector[pos]->alpha == 0)
							continue;

						SDL_Rect tileRect{ fowTilesVector[pos]->pos.x * FOW_TILE, fowTilesVector[pos]->pos.y * FOW_TILE, fowTilesVector[pos]->size, fowTilesVector[pos]->size };
						if (SDL_HasIntersection(&tileRect, &App->map->playerBase.roomRect))
							continue;

						//SDL_Rect fowTileRect{ fowTilesVector[i]->pos.x, fowTilesVector[i]->pos.y , 32,32 };
						//SDL_Rect collider{ (*iterator)->GetPos().x, (*iterator)->GetPos().y, 128, 128 };

						//if (RectIntersect(&collider, &fowTileRect)) {

						else if (TotalDistanceToPlayer(*iterator, pos) < RADIUS)
						{
							fowTilesVector[pos]->alpha = 0;
							fowTilesVector[pos]->normalAlpha = TRANSLUCID_ALPHA;
						}
						else
							fowTilesVector[pos]->alpha = fowTilesVector[pos]->normalAlpha;
					}
				}
			}
		}
	}
	CleanSafeZone(App->map->playerBase.roomRect);
}

bool j1FogOfWar::LoadKeys(pugi::xml_node& buttons)
{
	bool ret = true;

	ret = LoadKey(&buttonDrawFow, "buttonDrawFow", buttons);
	ret = LoadKey(&buttonDebugDrawFow, "buttonDebugDrawFow", buttons);

	return ret;
}

void j1FogOfWar::SaveKeys()
{
	App->configFile.child("config").child(name.data()).remove_child("buttons");
	pugi::xml_node buttons = App->configFile.child("config").child(name.data()).append_child("buttons");

	buttons.remove_child("buttonDrawFow");
	buttons.remove_child("buttonDebugDrawFow");

	buttons.append_child("buttonDrawFow").append_attribute("buttonDrawFow") = *buttonDrawFow;
	buttons.append_child("buttonDebugDrawFow").append_attribute("buttonDebugDrawFow") = *buttonDrawFow;

}

bool j1FogOfWar::Save(pugi::xml_node& save) const
{
	bool ret = true;

	bool create = false;

	pugi::xml_node general;

	if (save.child("general") == NULL)
	{
		general = save.append_child("general");
		create = true;
	}
	else
	{
		general = save.child("general");
	}

	SaveTiles(fowTilesVector, "fowTile", save.append_child("fowTilesVector"));

	SaveAttribute(width, "width", general, false);
	SaveAttribute(height, "height", general, false);

	SaveAttribute(isActive, "isActive", general, false);

	return ret;
}

bool j1FogOfWar::Load(pugi::xml_node& save)
{


	//<width width = "105" / >
	//	<height height = "130" / >
	//	<isActive isActive = "1" / >

	//pugi::xml_node fowTilesVector
	for (pugi::xml_node iterator = save.child("staticEntities").child("entity"); iterator; iterator = iterator.next_sibling("entity"))
	{

	}
	return true;
}

void j1FogOfWar::SaveTiles(vector<FogOfWarTile*> tiles, char* name, pugi::xml_node node) const
{
	for (int i = 0; i < tiles.size(); i++)
	{
		pugi::xml_node currNode = node.append_child(name);
		
		currNode.append_attribute("alpha") = tiles[i]->alpha;
		currNode.append_attribute("normalAlpha") = tiles[i]->normalAlpha;
		currNode.append_attribute("xPos") = tiles[i]->pos.x;
		currNode.append_attribute("yPos") = tiles[i]->pos.y;
		currNode.append_attribute("size") =	tiles[i]->size;
	}

}