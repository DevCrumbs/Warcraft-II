#include "p2Log.h"

#include "j1Printer.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "Entity.h" 
#include "j1Printer.h"

#include "UIMinimap.h"

UIMinimap::UIMinimap(iPoint localPos, UIElement* parent, UIMinimap_Info& info, j1Module* listener) : UIElement({ info.minimapInfo.x,info.minimapInfo.y }, nullptr, nullptr, false)
{
	type = UIE_TYPE_MINIMAP;
	
	width = info.minimapInfo.w;
	height = info.minimapInfo.h;

	minimapInfo = info.minimapInfo;

	entityWidth = info.entityWidth;
	entityHeight = info.entityWidth;

	entityWidth = 32 * 3;
	entityHeight = 32 * 3;

	width = info.minimapInfo.w;
	height = info.minimapInfo.h;

	priority = PriorityDraw_FRAMEWORK;

	LoadMap();

	activeDynamicEntities = &App->entities->activeDynamicEntities;
	activeStaticEntities = &App->entities->activeStaticEntities;
}


UIMinimap::UIMinimap() : UIElement({ 0,0 }, nullptr, nullptr, false)
{
	type = UIE_TYPE_MINIMAP;

}

UIMinimap::~UIMinimap()
{
	SDL_DestroyTexture(mapTexture);
}

void UIMinimap::Update(float dt)
{
	HandleInput(dt);
}

void UIMinimap::Draw() const
{
	SDL_fRect camera = App->render->camera;

	App->render->SetViewPort(minimapInfo);

	//prevOffsetX = offsetX;
	//prevOffsetY = offsetY;

	//offsetX = camera.x * scaleFactor + 40;
	//offsetY = camera.y * scaleFactor + 40;



	//if (offsetX > 0)
	//	offsetX = 0;
	//else if (offsetX < -maxOffsetX)
	//	offsetX = -maxOffsetX;

	//if (offsetY > 0)
	//	offsetY = 0;
	//else if (offsetY < -maxOffsetY)
	//	offsetY = -maxOffsetY;



	///-----------------	 Draw the map
	App->render->Blit(mapTexture, offsetX, offsetY, NULL, 0);

	///-----------------	 Draw all entities in the minimap
	for (list<DynamicEntity*>::iterator iterator = (*activeDynamicEntities).begin(); iterator != (*activeDynamicEntities).end(); ++iterator)
	{
		SDL_Rect rect{ (*iterator)->pos.x * scaleFactor + offsetX + cameraOffset.x,
						(*iterator)->pos.y * scaleFactor + offsetY + cameraOffset.y,
						entityWidth * scaleFactor, entityHeight * scaleFactor };

		SDL_Color color{ 0,0,0,0 };
		switch ((*iterator)->entitySide)
		{
		case EntitySide_NoSide:
			break;
		case EntitySide_Player:
			color = { 0,125,255,255 };
			break;
		case EntitySide_Enemy:
			color = { 255,0,0,255 };
			break;
		case EntitySide_EnemyBuildings:
			color = { 255,0,0,255 };//
			break;
		case EntitySide_Neutral:
			break;
		default:
			break;
		}
		App->render->DrawQuad(rect, color.r, color.g, color.b, color.a, true, false);

	}

	for (list<StaticEntity*>::iterator iterator = (*activeStaticEntities).begin(); iterator != (*activeStaticEntities).end(); ++iterator)
	{
		iPoint size = (*iterator)->GetSize();

		SDL_Rect rect{ (*iterator)->pos.x * scaleFactor + offsetX + cameraOffset.x,
						(*iterator)->pos.y * scaleFactor + offsetY + cameraOffset.y,
						size.x * scaleFactor, size.y * scaleFactor };
			
		SDL_Color color{ 0,0,0,0 };
		switch ((*iterator)->staticEntityCategory)
		{
		case StaticEntityCategory_NoCategory:
			break;
		case StaticEntityCategory_HumanBuilding:
			color = { 0,0,255,255 };
			break;
		case StaticEntityCategory_OrcishBuilding:
			color = { 140,11,20,255 };
			break;
		case StaticEntityCategory_NeutralBuilding:
			color = { 255,255,0,255 };//
			break;
		default:
			break;
		}
		App->render->DrawQuad(rect, color.r, color.g, color.b, color.a, true, false);
	}

	///-----------------	 Draw the camera rect
	SDL_Rect rect{ -camera.x * scaleFactor + offsetX + cameraOffset.x, -camera.y * scaleFactor + offsetY + cameraOffset.y,
		camera.w * scaleFactor, camera.h * scaleFactor };

	App->render->DrawQuad(rect, 255, 255, 0, 255, false, false);

	App->render->ResetViewPort();

}

void UIMinimap::HandleInput(float dt) 
{
	if (MouseHover())
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			iPoint newCameraPos = MinimapToMap();
			App->render->camera.x = -newCameraPos.x + App->render->camera.w / 2;
			App->render->camera.y = -newCameraPos.y + App->render->camera.h / 2;

			if (App->render->camera.x > 0)
				App->render->camera.x = 0;
			if (App->render->camera.y > 0)
				App->render->camera.y = 0;
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			iPoint pos = MinimapToMap();
			SDL_Rect rect{ pos.x,pos.y, 32,32 };
			App->render->DrawQuad(rect, 255, 255, 255);
		}
	}

}

iPoint UIMinimap::GetMousePos() 
{
	iPoint ret{ 0,0 };

	iPoint screenPos = GetScreenPos();
	int x = 0, y = 0;

	App->input->GetMousePosition(x, y);

	ret.x = x - screenPos.x;
	ret.y = y - screenPos.y;

	return ret;
}

iPoint UIMinimap::MinimapToMap(iPoint pos)
{
	iPoint minimapPos{ 0,0 };
	iPoint mapPos{ 0,0 };

	minimapPos.x = pos.x - offsetX - cameraOffset.x;
	minimapPos.y = pos.y - offsetY - cameraOffset.y;

	mapPos.x = minimapPos.x / scaleFactor;
	mapPos.y = minimapPos.y / scaleFactor;

	return mapPos;
}

iPoint UIMinimap::MinimapToMap()
{
	return MinimapToMap(GetMousePos());
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
	scaleFactor = minimapInfo.h / (float)(App->map->data.height * App->map->data.tileHeight);

	///-----------------	 Create a RGB surface
	mapSurface = SDL_CreateRGBSurface(0, App->map->data.width * App->map->data.tileWidth, App->map->data.height * App->map->data.tileHeight, 32, 0, 0, 0, 0);
	if (mapSurface == NULL)
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());


	///-----------------	Create a renderer 
	renderer = SDL_CreateSoftwareRenderer(mapSurface);

	if (renderer == NULL)
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());


	SDL_Texture* tex = App->tex->Load(App->map->tilesetPath.data(), renderer);

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

	textureSize = { 0,0,int(App->map->data.width * App->map->data.tileWidth * scaleFactor), int(App->map->data.height * App->map->data.tileHeight * scaleFactor) };

	int originalW = textureSize.w;
	int originalH = textureSize.h;

	if (textureSize.w > textureSize.h)
		textureSize.h = textureSize.w;
	else
		textureSize.w = textureSize.h;

	cameraOffset.x = (textureSize.w - originalW) / 2;
	cameraOffset.y = (textureSize.h - originalH) / 2;
	
	///-----------------	Load aux renderer
	minimapSurface = SDL_CreateRGBSurface(0, textureSize.w, textureSize.h, 32, 0, 0, 0, 0);
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
	ret = SaveInRenderer(mapTexture, cameraOffset.x, cameraOffset.y, NULL, scaleFactor, rendererAux);

	///-----------------	Destroy map texture
	SDL_DestroyTexture(mapTexture);

	///-----------------	Save map texture
	SDL_RenderReadPixels(rendererAux, NULL, 0, minimapSurface->pixels, minimapSurface->pitch);

	mapTexture = SDL_CreateTextureFromSurface(App->render->renderer, minimapSurface);

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


