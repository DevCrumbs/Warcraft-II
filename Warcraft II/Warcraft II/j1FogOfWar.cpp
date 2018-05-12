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

bool j1FogOfWar::Start()
{

	UnLoadFowMap();

	return true;
}

bool j1FogOfWar::LoadFoW()
{
	bool ret = false;


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

	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{
		isActive = !isActive;
	}
	return true;
}

bool j1FogOfWar::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);
	if (isActive)
		print();
	return true;
}

bool j1FogOfWar::CleanUp()
{
	UnLoadFowMap();
	return true;
}

void j1FogOfWar::print()
{	
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
			//	LOG("Pos is %i", pos);
			App->render->DrawQuad({ world.x, world.y, FOW_TILE, FOW_TILE }, 0, 0, 0, fowTilesVector[pos]->alpha);


		}//for
	}//for
}


// =================================== PART 1 ===================================

void j1FogOfWar::LoadFoWMap(int mapWidth, int mapHeight)
{
	// TODO 1: WE HAVE TO COVER THE MAP WITH FOW TILES (Use: 2 'for', mapWith & mapHeight to know the limit & FOW_TILE_MULTIPLIER)
	// THIS TO ADD A NEW FOW_TILE : 
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
						if (TotalDistanceToPlayer(*iterator, pos) == RADIUS)
						{
							for (int x = 0; x < TILE_PARTITIONS; x++)
								for (int j = 0; j < TILE_PARTITIONS; j++)
								{
									if (cont < fowSmallerTilesVector.size())
									{
										// TODO 5 (set the smaller tiles position & normalAlpha, example: "fowSmallerTilesVector[contador]->pos.x = ...;" )
										fowSmallerTilesVector[cont]->pos.x = fowTilesVector[pos]->pos.x * FOW_TILE + (x * FOW_TILE / TILE_PARTITIONS);
										fowSmallerTilesVector[cont]->pos.y = fowTilesVector[pos]->pos.y * FOW_TILE + (j * FOW_TILE / TILE_PARTITIONS);
										fowSmallerTilesVector[cont]->normalAlpha = fowTilesVector[pos]->normalAlpha;
									}
									else
									{
										FogOfWarTile* aux = new FogOfWarTile();
										aux->pos.x = fowTilesVector[pos]->pos.x * FOW_TILE + (x * FOW_TILE / TILE_PARTITIONS);
										aux->pos.y = fowTilesVector[pos]->pos.y * FOW_TILE + (j * FOW_TILE / TILE_PARTITIONS);
										aux->normalAlpha = fowTilesVector[pos]->normalAlpha;
										fowSmallerTilesVector.push_back(aux);
									}
									cont++;

								}
							fowTilesVector[pos]->alpha = 0;
						}

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


// =================================== PART 2 ===================================

// TODO 6 (beauty): 
// UNCOMMENT THE CODE BELOW
