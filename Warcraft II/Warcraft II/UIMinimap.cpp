#include "p2Log.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "Entity.h" 

#include "UIMinimap.h"

UIMinimap::UIMinimap(iPoint localPos, UIElement* parent, UIMinimap_Info& info, j1Module* listener) : UIElement(localPos, parent, listener) 
{
	type = UIE_TYPE_MINIMAP;
}


UIMinimap::UIMinimap() : UIElement()
{
	type = UIE_TYPE_MINIMAP;

}

UIMinimap::~UIMinimap()
{

	SDL_DestroyTexture(mapTexture);

	entities.clear();

}

void UIMinimap::Update(float dt)
{
	SDL_Rect camera = App->render->camera;

	//TODO 3 Draw the map (App->render->Blit()), (SDL_Texture* mapTexture, SDL_Rect minimapInfo).
	///Remember to substract the camera coordinates to the minimap coordinates so it stays in the same screen position
	App->render->Blit(mapTexture, minimapInfo.x - camera.x, minimapInfo.y - camera.y);


	//TODO 4 Draw all entities in the minimap
	///TODO 4.1 Iterate all entities (list<Entity*> entities)
	for (list<Entity*>::iterator iterator = entities.begin(); iterator != entities.end(); ++iterator)
	{
		///Bonus code: Use this in TODO 4.1 to compute the entity rect
		SDL_Rect rect{	(*iterator)->position.x * scaleFactor + minimapInfo.x,
						(*iterator)->position.y * scaleFactor + minimapInfo.y,
						entityWidth * scaleFactor, entityHeight * scaleFactor };

		/// To draw entities we will be using quads (App->render->DrawQuad()) use (*iterator)->minimapDrawColor to acces to the entity color (r, g, b, a)
		/// We want the quad to be filled and it shouldn't use the camera (keep in mind the bools in App->render->DrawQuad())
		App->render->DrawQuad(rect, (*iterator)->minimapDrawColor.r, (*iterator)->minimapDrawColor.g,
							 (*iterator)->minimapDrawColor.b, (*iterator)->minimapDrawColor.a, true, false);
	}


	//TODO 5 Draw the camera rect (So player know where he is) using a quad (App->render->DrawQuad())
	///Bonus code: Use this in TODO 5 to compute the camera rect 
	SDL_Rect rect{ -camera.x * scaleFactor + minimapInfo.x, -camera.y * scaleFactor + minimapInfo.y,
					camera.w * scaleFactor, camera.h * scaleFactor };

	/// We don't want the quad to be filled and it shouldn't use the camera (keep in mind the bools in App->render->DrawQuad())
	App->render->DrawQuad(rect, 255, 255, 0, 255, false, false);

}

bool UIMinimap::SetMinimap(SDL_Rect pos, int entityW, int entityH)
{
	bool ret = false;

	// TODO 1: Set minimap info (SDL_Rect minimapInfo, int entityWidth,	int entityHeight)

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
	SDL_Surface* mapSurface = nullptr;

	if (App->map->isMapLoaded)
	{

		// TODO 2 Save map into a texture (SDL_Texture* mapTexture)

		///TODO 2.1 Compute the scale factor (float scaleFactor) for the Minimap. Hint: mapSize * scaleFactor = minimapSize
		float mapSize = App->map->data.width * App->map->data.tileWidth;
		scaleFactor = minimapInfo.w / mapSize;


		///TODO 2.2 Create a RGB surface (SDL_Surface* mapSurface, SDL_CreateRGBSurface()). Hint: The size of the surface is the size of the minimap	
		mapSurface = SDL_CreateRGBSurface(0, minimapInfo.w, minimapInfo.h, 32, 0, 0, 0, 0);
		 if (mapSurface == NULL)
			SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());


		 ///TODO 2.3 Create a renderer (SDL_Renderer* renderer) using the surface (SDL_Surface* mapSurface) (SDL_CreateSoftwareRenderer())
		renderer = SDL_CreateSoftwareRenderer(mapSurface);

		if (renderer == NULL)
			LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());	
		


		char* path = TexturePath;
		SDL_Texture* tex = App->tex->Load(path, renderer);

		for (list<MapLayer*>::const_iterator layer = App->map->data.layers.begin();
			layer != App->map->data.layers.end(); ++layer)
		{
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
						world.x *= scaleFactor;
						world.y *= scaleFactor;
						ret = SaveInRenderer(tex, world.x, world.y, section, scaleFactor, renderer);
					}
				}
			}
		}
		App->tex->UnLoad(tex);

		///TODO 2.4 Read the renderer pixels (SDL_RenderReadPixels()) using the renderer (SDL_Renderer*) and the pixels (SDL_Surface*->pixels) and pitch (SDL_Surface*->pitch) of the surface 
		SDL_RenderReadPixels(renderer, NULL, 0, mapSurface->pixels, mapSurface->pitch);


		///TODO 2.5 Create a texture from the surface (SDL_Surface* mapSurface) (SDL_CreateTextureFromSurface()) and store it in the map texture (SDL_Texture* mapTexture)
		mapTexture = SDL_CreateTextureFromSurface(App->render->renderer, mapSurface);


		///FYI You can use this same method to take a screenshot of your game using SDL_SaveBMP(SDL_Surface* surface, const char*  file) at the post update :)

		if (SDL_RenderClear(renderer) == 0)
			renderer = nullptr;
		else
			LOG("Could not clear the renderer! SDL_Error: %s\n", SDL_GetError());

		SDL_FreeSurface(mapSurface);
	}
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

