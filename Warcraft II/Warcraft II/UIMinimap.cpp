#include "p2Log.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "Entity.h" 

#include "UIMinimap.h"

UIMinimap::UIMinimap(iPoint localPos, UIElement* parent, UIMinimap_Info& info, j1Module* listener) : UIElement({ info.minimapInfo.x,info.minimapInfo.y }, nullptr, nullptr, false)
{
	type = UIE_TYPE_MINIMAP;
	
	width = info.minimapInfo.w;
	height = info.minimapInfo.h;

	minimapInfo = info.minimapInfo;

	entityWidth = info.entityWidth;
	entityHeight = info.entityWidth;

	priority = PriorityDraw_FRAMEWORK;

	LoadMap();
}


UIMinimap::UIMinimap() : UIElement({ 0,0 }, nullptr, nullptr, false)
{
	type = UIE_TYPE_MINIMAP;

}

UIMinimap::~UIMinimap()
{

	SDL_DestroyTexture(mapTexture);

	entities.clear();

}

void UIMinimap::Draw() const
{
	SDL_fRect camera = App->render->camera;

	App->render->SetViewPort(minimapInfo);

	///-----------------	 Draw the map
	int offsetX = camera.x * scaleFactor + 40;
	int offsetY = camera.y * scaleFactor + 40;


	if (offsetX > 0)
		offsetX = 0;
	else if (offsetX < -maxOffsetX)
		offsetX = -maxOffsetX;

	if (offsetY > 0)
		offsetY = 0;
	else if (offsetY < -maxOffsetY)
		offsetY = -maxOffsetY;


	App->render->Blit(mapTexture, offsetX,offsetY, NULL, 0);


	///-----------------	 Draw all entities in the minimap
	for (list<Entity*>::iterator iterator = entities.begin(); iterator != entities.end(); ++iterator)
	{
		
		SDL_Rect rect{	(*iterator)->pos.x * scaleFactor + minimapInfo.x,
						(*iterator)->pos.y * scaleFactor + minimapInfo.y,
						entityWidth * scaleFactor, entityHeight * scaleFactor };

		
		App->render->DrawQuad(rect, (*iterator)->minimapDrawColor.r, (*iterator)->minimapDrawColor.g,
							 (*iterator)->minimapDrawColor.b, (*iterator)->minimapDrawColor.a, true, false);
	}


	///-----------------	 Draw the camera rect
	SDL_Rect rect{ -camera.x * scaleFactor + offsetX, -camera.y * scaleFactor + offsetY,
					camera.w * scaleFactor, camera.h * scaleFactor };

	App->render->DrawQuad(rect, 255, 255, 0, 255, false, false);

	App->render->ResetViewPort();

}

bool UIMinimap::SetMinimap(SDL_Rect pos, int entityW, int entityH)
{
	bool ret = false;

	minimapInfo = pos;

	entityWidth = entityW;
	entityHeight = entityH;


	ret = LoadMap();

	ret = true;

	return true;
}


bool UIMinimap::LoadMap()
{
	bool ret = true;


	SDL_Renderer* renderer = nullptr;
	SDL_Renderer* rendererAux = nullptr;
	SDL_Surface* mapSurface = nullptr;
	SDL_Surface* minimapSurface = nullptr;

	///-----------------	Compute the scale factor 
//	float mapSize = App->map->data.width * App->map->data.tileWidth;
//	scaleFactor = minimapInfo.w / mapSize;
	scaleFactor = minimapInfo.w / (float)(50 * 32);

	///-----------------	 Create a RGB surface
	mapSurface = SDL_CreateRGBSurface(0, App->map->data.width * App->map->data.tileWidth, App->map->data.height * App->map->data.tileHeight, 32, 0, 0, 0, 0);
	if (mapSurface == NULL)
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());


	///-----------------	Create a renderer 
	renderer = SDL_CreateSoftwareRenderer(mapSurface);

	if (renderer == NULL)
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());



	char* path = "wastelandTiles.png";

	SDL_Texture* tex = App->tex->Load(path, renderer);

	for (list<MapLayer*>::const_iterator layer = App->map->data.layers.begin();
		layer != App->map->data.layers.end(); ++layer)
	{
		if (!(*layer)->properties.GetProperty("Draw", false))
			continue;

		if (!ret)
			break;

		for (int i = 0; i < (*layer)->width; ++i)
		{
			for (int j = 0; j < (*layer)->height; ++j)
			{
				int tile_id = (*layer)->Get(i, j);
				if (tile_id > 0) {

					TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

					SDL_Rect rect = tileset->GetTileRect(tile_id);

					SDL_Rect* section = &rect;
					iPoint world = App->map->MapToWorld(i, j);
				//	world.x *= scaleFactor;
				//	world.y *= scaleFactor;
					//	ret = SaveInRenderer(tex, world.x, world.y, section, scaleFactor, renderer);
					ret = SaveInRenderer(tex, world.x, world.y, section, 1, renderer);
				}
			}
		}
	}
	App->tex->UnLoad(tex);
	
	///-----------------	Load aux renderer
	minimapSurface = SDL_CreateRGBSurface(0, App->map->data.width * App->map->data.tileWidth * scaleFactor, App->map->data.height * App->map->data.tileHeight * scaleFactor, 32, 0, 0, 0, 0);
	if (mapSurface == NULL)
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());

	rendererAux = SDL_CreateSoftwareRenderer(minimapSurface);
	if (rendererAux == NULL)
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());

	///-----------------	 Save texture in aux renderer
	SDL_RenderReadPixels(renderer, NULL, 0, mapSurface->pixels, mapSurface->pitch);
	mapTexture = SDL_CreateTextureFromSurface(rendererAux, mapSurface);

	///-----------------	Clear Renderer
	SDL_RenderClear(renderer);

	if (SDL_RenderClear(renderer) == 0)
		renderer = nullptr;
	else
		LOG("Could not clear the renderer! SDL_Error: %s\n", SDL_GetError());

	///-----------------	Clear Surface
	SDL_FreeSurface(mapSurface);


	///-----------------	Save in renderer
	ret = SaveInRenderer(mapTexture, 0,0, NULL, scaleFactor, rendererAux);

	///-----------------	Destroy map texture
	SDL_DestroyTexture(mapTexture);

	///-----------------	Save map texture
	SDL_RenderReadPixels(rendererAux, NULL, 0, minimapSurface->pixels, minimapSurface->pitch);

	mapTexture = SDL_CreateTextureFromSurface(App->render->renderer, minimapSurface);
	SDL_SaveBMP(minimapSurface, "aqui.png");

	maxOffsetX = ((App->map->data.width * App->map->data.tileWidth) * scaleFactor) - minimapInfo.w;
	maxOffsetY = ((App->map->data.height * App->map->data.tileHeight) * scaleFactor) - minimapInfo.h;

	return ret;
}

bool UIMinimap::SaveInRenderer(const SDL_Texture* texture, int x, int y, const SDL_Rect* section, float scale, SDL_Renderer* renderer)
{
	bool ret = true;
	float bScale = scale;

	SDL_Rect rect;
	rect.x = x;
		rect.y = y;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= bScale;
	rect.h *= bScale;

	SDL_Point* p = NULL;
	SDL_Point pivot;


	if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, section, &rect, 0, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool UIMinimap::AddEntity(Entity* entity)
{
	bool ret = false;

	if (entity != nullptr)
	{
		this->entities.push_back(entity);
		ret = true;
	}
	return ret;
}

