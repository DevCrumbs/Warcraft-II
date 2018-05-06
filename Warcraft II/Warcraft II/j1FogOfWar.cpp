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

}

bool j1FogOfWar::Start()
{
	// UNCOMMENT THIS IF THE FOW OF WAR WORKS !! THIS WILL ACTIVE FOW AT GAME START
	uint w, h;

	LoadFoWMap(119, 149);

	return true;
}

bool j1FogOfWar::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);

	ResetTiles();
	TilesNearPlayer();
	return true;
}

bool j1FogOfWar::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);
	print();
	SmallerTilesNearPlayer();
	return true;
}

bool j1FogOfWar::CleanUp()
{
	UnLoadFowMap();
	return true;
}

void j1FogOfWar::print()
{
	//for (int i = 0; i < fowTilesVector.size(); i++)
	//{
	//	
	//}


	iPoint startTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale(),
		-App->render->camera.y / App->win->GetScale());
	iPoint endTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale() + App->render->camera.w,
		-App->render->camera.y / App->win->GetScale() + App->render->camera.h);
	int i = startTile.x;
	if (i < 0 && endTile.x >= 0)
		i = 0;

	for (; i < 119 && i < endTile.x + 1; ++i) 
	{
		int j = startTile.y;
		if (j < 0 && endTile.y >= 0)
			j = 0;

		for (; j < 149 && j < endTile.y + 1; ++j) 
		{

			int pos = 119 * i + j;
		//	LOG("Pos is %i", pos);
			App->render->DrawQuad({ i * FOW_TILE, j * FOW_TILE, FOW_TILE, FOW_TILE }, 0, 0, 0, fowTilesVector[pos]->alpha);

		}
	}//for
}//for


// =================================== PART 1 ===================================

void j1FogOfWar::LoadFoWMap(int mapWidth, int mapHeight)
{
	// TODO 1: WE HAVE TO COVER THE MAP WITH FOW TILES (Use: 2 'for', mapWith & mapHeight to know the limit & FOW_TILE_MULTIPLIER)
	// THIS TO ADD A NEW FOW_TILE : 
	for (int x = 0; x < mapWidth / FOW_TILE_MULTIPLIER; x++)
		for (int y = 0; y < mapHeight / FOW_TILE_MULTIPLIER; y++)
		{
			FogOfWarTile* aux = new FogOfWarTile;
			aux->pos = iPoint(x, y);
			fowTilesVector.push_back(aux);
		}

}

void j1FogOfWar::UnLoadFowMap()
{
	for (int i = 0; i < fowTilesVector.size(); i++)
		delete fowTilesVector[i];
	fowTilesVector.clear();

	for (int i = 0; i < fowSmallerTilesVector.size(); i++)
		delete fowSmallerTilesVector[i];
	fowSmallerTilesVector.clear();
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

void j1FogOfWar::ResetTiles()
{
	for (int i = 0; i < fowTilesVector.size(); i++)
	{
		if (fowTilesVector[i]->alpha == 0)
			fowTilesVector[i]->alpha = TRANSLUCID_ALPHA;
	}
}


void j1FogOfWar::TilesNearPlayer()
{
	int contador = 0;

	for (list<DynamicEntity*>::const_iterator iterator = App->entities->activeDynamicEntities.begin(); iterator != App->entities->activeDynamicEntities.end(); ++iterator)
	{
	
		if ((*iterator)->entitySide == EntitySide_Player)
		{
			for (int i = 0; i < fowTilesVector.size(); i++)
			{
				if (fowTilesVector[i]->alpha == 0)
					continue;

				//SDL_Rect fowTileRect{ fowTilesVector[i]->pos.x, fowTilesVector[i]->pos.y , 32,32 };
				//SDL_Rect collider{ (*iterator)->GetPos().x, (*iterator)->GetPos().y, 128, 128 };

				//if (RectIntersect(&collider, &fowTileRect)) {
					if (TotalDistanceToPlayer(*iterator, i) == RADIUS)
					{
						for (int x = 0; x < TILE_PARTITIONS; x++)
							for (int j = 0; j < TILE_PARTITIONS; j++)
							{
								if (contador < fowSmallerTilesVector.size())
								{
									// TODO 5 (set the smaller tiles position & normalAlpha, example: "fowSmallerTilesVector[contador]->pos.x = ...;" )
									fowSmallerTilesVector[contador]->pos.x = fowTilesVector[i]->pos.x * FOW_TILE + (x * FOW_TILE / TILE_PARTITIONS);
									fowSmallerTilesVector[contador]->pos.y = fowTilesVector[i]->pos.y * FOW_TILE + (j * FOW_TILE / TILE_PARTITIONS);
									fowSmallerTilesVector[contador]->normalAlpha = fowTilesVector[i]->normalAlpha;
								}
								else
								{
									FogOfWarTile* aux = new FogOfWarTile();
									aux->pos.x = fowTilesVector[i]->pos.x * FOW_TILE + (x * FOW_TILE / TILE_PARTITIONS);
									aux->pos.y = fowTilesVector[i]->pos.y * FOW_TILE + (j * FOW_TILE / TILE_PARTITIONS);
									aux->normalAlpha = fowTilesVector[i]->normalAlpha;
									fowSmallerTilesVector.push_back(aux);
								}
								contador++;

							}
						fowTilesVector[i]->alpha = 0;
					}

					else if (TotalDistanceToPlayer(*iterator, i) < RADIUS)
					{
						fowTilesVector[i]->alpha = 0;
						fowTilesVector[i]->normalAlpha = TRANSLUCID_ALPHA;
					}
					else
						fowTilesVector[i]->alpha = fowTilesVector[i]->normalAlpha;
				}
			}
//		}
	}
}

// =================================== PART 2 ===================================

// TODO 6 (beauty): 
// UNCOMMENT THE CODE BELOW

int j1FogOfWar::TotalDistanceToPlayerSmallers(iPoint pos)
{
	return 0;
	//iPoint playerPos((int)App->scene->player->pos.x + 50, (int)App->scene->player->pos.y);
	//// TODO 6 : PRINT LITTLE TILES KNOWING DISTANCE
	////WHERE WE WILL CALCULATE THE DISTANCE IN PIXELS AND NOT IN TILES !!
	//int totalX = pos.x + (FOW_TILE / TILE_PARTITIONS / 2) - playerPos.x;
	//if (totalX < 0)
	//	totalX *= -1;
	//int totalY = pos.y + (FOW_TILE / TILE_PARTITIONS / 2) - playerPos.y;
	//if (totalY < 0)
	//	totalY *= -1;
	//return sqrt(totalX * totalX + totalY * totalY);
}

void j1FogOfWar::SmallerTilesNearPlayer()
{
	//for (int i = 0; i < fowSmallerTilesVector.size(); i++)
	//{
	//	if (TotalDistanceToPlayerSmallers(fowSmallerTilesVector[i]->pos) < RADIUS * FOW_TILE)
	//		fowSmallerTilesVector[i]->alpha = 0;
	//	else
	//		fowSmallerTilesVector[i]->alpha = fowSmallerTilesVector[i]->normalAlpha;
	//}

	//for (int i = 0; i < fowSmallerTilesVector.size(); i++)
	//{
	//	App->render->DrawQuad({ fowSmallerTilesVector[i]->pos.x, fowSmallerTilesVector[i]->pos.y, FOW_TILE / TILE_PARTITIONS, FOW_TILE / TILE_PARTITIONS }, 0, 0, 0, fowSmallerTilesVector[i]->alpha);
	//}
}