#include "p2Log.h"

#include "j1Printer.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "Entity.h" 
#include "j1Printer.h"
#include "j1Scene.h"
#include "j1FogOfWar.h"

#include "UIMinimap.h"


#include "Brofiler\Brofiler.h"

UIMinimap::UIMinimap(iPoint localPos, UIElement* parent, UIMinimap_Info& info, j1Module* listener) : UIElement({ info.minimapInfo.x,info.minimapInfo.y }, parent, listener, false)
{
	type = UIE_TYPE_MINIMAP;
	
	width = info.minimapInfo.w;
	height = info.minimapInfo.h;

	minimapInfo = info.minimapInfo;

	entityWidth = info.entityWidth;
	entityHeight = info.entityWidth;

	zoomFactor = 3;

	entityWidth = 32;
	entityHeight = 32;

	width = info.minimapInfo.w;
	height = info.minimapInfo.h;

	moveMinimap = KEY_REPEAT;

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
	SDL_DestroyTexture(hiLevelMapTexture);
	SDL_DestroyTexture(lowLevelMapTexture);

	hiLevelMapTexture = lowLevelMapTexture = currentMapTexture = nullptr;
}

void UIMinimap::Update(float dt)
{
	HandleInput(dt);
}

void UIMinimap::Draw() const
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Magenta);

	SDL_fRect camera = App->render->camera;

	App->render->SetViewPort(minimapInfo);


	if (lowLevel)
	{
		prevOffsetX = offsetX;
		prevOffsetY = offsetY;

		offsetX = camera.x * lowLevelScaleFactor + 16;
		offsetY = camera.y * lowLevelScaleFactor + 16;


		if (offsetX > 0)
			offsetX = 0;
		else if (offsetX < -maxOffsetX)
			offsetX = -maxOffsetX;

		if (offsetY > 0)
			offsetY = 0;
		else if (offsetY < -maxOffsetY)
			offsetY = -maxOffsetY;
	}


	///-----------------	 Draw the map
	App->render->Blit(currentMapTexture, offsetX, offsetY, NULL, 0);

	///-----------------	 Draw all entities in the minimap
	for (list<DynamicEntity*>::iterator iterator = (*activeDynamicEntities).begin(); iterator != (*activeDynamicEntities).end(); ++iterator)
	{
		SDL_Rect rect{ 0,0,0,0 };
		if (App->fow->isActive && (*iterator)->entitySide != EntitySide_Player)
		{
				iPoint pos = (*iterator)->GetLastSeenTile();
				if (pos.x < 0 || pos.y < 0)
					continue;
				pos.x *= 32;
				pos.y *= 32;
				rect = LoadEntityRect(pos);
		}
		else
		{
			fPoint pos = (*iterator)->GetPos();
			if (pos.x < 0 || pos.y < 0)
				continue;
			rect = LoadEntityRect(pos);
		}

		SDL_Color color{ 0,0,0,0 };
		switch ((*iterator)->entitySide)
		{
		case EntitySide_NoSide:
			break;
		case EntitySide_Player:
		{
			color = { 0,125,255,255 };
		}
			break;
		case EntitySide_Enemy:
			color = { 255,0,0,255 };
			break;
		case EntitySide_EnemyBuildings:
			color = { 255,0,0,255 };//
			break;
		case EntitySide_EnemyShip:
			color = { 238,130,238,255 };//
			rect.w *= 2;
			rect.h *= 2;
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

		SDL_Rect rect{ (*iterator)->pos.x * currentScaleFactor + offsetX + cameraOffset.x,
						(*iterator)->pos.y * currentScaleFactor + offsetY + cameraOffset.y,
						size.x * currentScaleFactor, size.y * currentScaleFactor };

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


	//Draw FoW
	if (App->fow->isActive)
		DrawFoW();

	///-----------------	 Draw the camera rect
	SDL_Rect rect{ -camera.x * currentScaleFactor + offsetX + cameraOffset.x, -camera.y * currentScaleFactor + offsetY + cameraOffset.y,
		camera.w * currentScaleFactor, camera.h * currentScaleFactor };

	App->render->DrawQuad(rect, 255, 255, 0, 255, false, false);

	if (isRoomCleared)
	{
		if (startRoomClearedTimer)
		{
			roomClearedTimer.Start();
			startRoomClearedTimer = false;
		}

		if (roomClearedTimer.Read() < 50)
		{
			App->render->DrawQuad(roomClearedRect, 255, 255, 255, 255, true, false);
		}
		else
		{
			isRoomCleared = false;
			roomClearedRect = { 0,0,0,0 };
		}
	}

	App->render->ResetViewPort();
}

SDL_Rect UIMinimap::LoadEntityRect(iPoint pos) const
{
	SDL_Rect rect{ 0,0,0,0 };

	rect.x = pos.x * currentScaleFactor + offsetX + cameraOffset.x;
	rect.y = pos.y * currentScaleFactor + offsetY + cameraOffset.y;
	rect.w = entityWidth * currentScaleFactor * zoomFactor;
	rect.h = entityHeight * currentScaleFactor * zoomFactor;

	return rect;
}

SDL_Rect UIMinimap::LoadEntityRect(fPoint pos) const
{
	SDL_Rect rect{ 0,0,0,0 };

	rect.x = pos.x * currentScaleFactor + offsetX + cameraOffset.x;
	rect.y = pos.y * currentScaleFactor + offsetY + cameraOffset.y;
	rect.w = entityWidth * currentScaleFactor * zoomFactor;
	rect.h = entityHeight * currentScaleFactor * zoomFactor;

	return rect;
}


void UIMinimap::HandleInput(float dt)
{
	if (MouseHover())
	{
		//Move minimap

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			moveCamera = true;
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == moveMinimap && moveCamera)
		{
			iPoint newCameraPos = MinimapToMap();
			App->render->camera.x = -newCameraPos.x + App->render->camera.w / 2;
			App->render->camera.y = -newCameraPos.y + App->render->camera.h / 2;

			if (App->render->camera.x > 0)
				App->render->camera.x = 0;
			if (App->render->camera.y > 0)
				App->render->camera.y = 0;
		}
		//Comand troops to goal
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			entitiesGoal = MinimapToMap();
			if (listener != nullptr)
			{
				listener->OnUIEvent(this, UI_EVENT_MOUSE_RIGHT_CLICK);
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		moveCamera = false;
	}

	if (App->scene->isMinimapChanged || App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		if (lowLevel)
		{
			lowLevel = false;
			zoomFactor = 3;
			currentMapTexture = hiLevelMapTexture;
			currentScaleFactor = hiLevelScaleFactor;
			offsetX = offsetY = 0;
			moveMinimap = KEY_REPEAT;
		}
		else
		{
			lowLevel = true;
			zoomFactor = 1;
			currentMapTexture = lowLevelMapTexture;
			currentScaleFactor = lowLevelScaleFactor;
			moveMinimap = KEY_DOWN;
		}
		App->scene->isMinimapChanged = false;
	}
}

iPoint UIMinimap::GetEntitiesGoal() const
{
	return entitiesGoal;
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

	mapPos.x = minimapPos.x / currentScaleFactor;
	mapPos.y = minimapPos.y / currentScaleFactor;

	return mapPos;
}

SDL_Rect UIMinimap::MapToMinimap(SDL_Rect pos) const
{
	SDL_Rect minimapPos{ 0,0,0,0 };
	SDL_Rect mapPos{ 0,0,0,0 };

	minimapPos.x = pos.x;
	minimapPos.y = pos.y;
	minimapPos.w = pos.w;
	minimapPos.h = pos.h;

	mapPos.x = (minimapPos.x * currentScaleFactor) + offsetX + cameraOffset.x;
	mapPos.y = (minimapPos.y * currentScaleFactor) + offsetY + cameraOffset.y;
	mapPos.w = minimapPos.w * currentScaleFactor;
	mapPos.h = minimapPos.h * currentScaleFactor;

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

	return true;
}

bool UIMinimap::DrawRoomCleared(Room room)
{
	bool ret = false;

	//if (room.isCleared)
	{
		roomClearedRect = MapToMinimap(room.roomRect);
		isRoomCleared = true;
		startRoomClearedTimer = true;
		ret = true;
	}

	return ret;
}

void UIMinimap::DrawFoW() const
{
	int cont = 0;
	int nextTile = 0;
	int fowSize = App->fow->fowTilesVector.size();
	//for (vector<FogOfWarTile*>::iterator tiles = App->fow->fowTilesVector.begin(); tiles != App->fow->fowTilesVector.end();)
	//{
	int tiles = 0;

	float pos = 0;

	while (tiles < fowSize)
	{
		pos = App->fow->fowTilesVector[tiles]->pos.x;

		SDL_Rect tileRect = MapToMinimap({ App->fow->fowTilesVector[tiles]->pos.x * 32, App->fow->fowTilesVector[tiles]->pos.y * 32,
							(App->fow->fowTilesVector[tiles]->size * zoomFactor), (App->fow->fowTilesVector[tiles]->size * zoomFactor) });

		tileRect.w += 2;
		tileRect.h += 2;

		App->render->DrawQuad(tileRect, 0, 0, 0, App->fow->fowTilesVector[tiles]->alpha, true, false);

		tiles += zoomFactor;

		if (tiles % App->fow->width == 0)
		{
			tiles += App->fow->width * (zoomFactor - 1);
		}
	}


	//iPoint startTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale(),
	//	-App->render->camera.y / App->win->GetScale());
	//iPoint endTile = App->map->WorldToMap(-App->render->camera.x / App->win->GetScale() + App->render->camera.w,
	//	-App->render->camera.y / App->win->GetScale() + App->render->camera.h);
}

bool UIMinimap::LoadMap()
{
	bool ret = true;


	SDL_Renderer* renderer = nullptr;
	SDL_Renderer* rendererAux = nullptr;
	SDL_Surface* mapSurface = nullptr;
	SDL_Surface* minimapSurface = nullptr;

	///-----------------	Compute the scale factor 
	currentScaleFactor = hiLevelScaleFactor = minimapInfo.h / (float)(App->map->data.height * App->map->data.tileHeight);
	lowLevelScaleFactor = minimapInfo.h / (float)(40 * App->map->data.tileHeight);

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

					ret = SaveInRenderer(tex, world.x, world.y, section, 1, renderer);
				}
			}
		}
	}
	App->tex->UnLoad(tex);

	hiLevelTextureSize = { 0,0,int(App->map->data.width * App->map->data.tileWidth * hiLevelScaleFactor), 
							int(App->map->data.height * App->map->data.tileHeight * hiLevelScaleFactor) };

	lowLevelTextureSize = { 0,0, int(App->map->data.width * App->map->data.tileWidth * lowLevelScaleFactor), 
							int(App->map->data.height * App->map->data.tileHeight * lowLevelScaleFactor) };

	int originalW = hiLevelTextureSize.w;
	int originalH = hiLevelTextureSize.h;

	if (hiLevelTextureSize.w > hiLevelTextureSize.h)
		hiLevelTextureSize.h = hiLevelTextureSize.w;
	else
		hiLevelTextureSize.w = hiLevelTextureSize.h;

	cameraOffset.x = (hiLevelTextureSize.w - originalW) / 2;
	cameraOffset.y = (hiLevelTextureSize.h - originalH) / 2;
	
	///-----------------	Save textures
	currentMapTexture = hiLevelMapTexture = CreateMinimapTexture(hiLevelTextureSize,renderer, mapSurface, hiLevelScaleFactor);
	lowLevelMapTexture = CreateMinimapTexture(lowLevelTextureSize, renderer, mapSurface, lowLevelScaleFactor);

	///-----------------	Clear Renderer
	SDL_RenderClear(renderer);

	if (SDL_RenderClear(renderer) == 0)
		renderer = nullptr;
	else
		LOG("Could not clear the renderer! SDL_Error: %s\n", SDL_GetError());

	///-----------------	Clear Surface
	SDL_FreeSurface(mapSurface);
	///-----------------	Destroy map texture


	maxOffsetX = ((App->map->data.width * App->map->data.tileWidth) * lowLevelScaleFactor) - minimapInfo.w;
	maxOffsetY = ((App->map->data.height * App->map->data.tileHeight) * lowLevelScaleFactor) - minimapInfo.h;

	return ret;
}

SDL_Texture* UIMinimap::CreateMinimapTexture(SDL_Rect mapSize, SDL_Renderer* renderer, SDL_Surface* mapSurface, float scaleFactor)
{
	SDL_Texture*  minimapTexture = nullptr;

	SDL_Surface* minimapSurface = nullptr;
	SDL_Renderer* rendererAux = nullptr;

	///-----------------	Load aux renderer
	minimapSurface = SDL_CreateRGBSurface(0, mapSize.w, mapSize.h, 32, 0, 0, 0, 0);
	if (minimapSurface == NULL)
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());

	rendererAux = SDL_CreateSoftwareRenderer(minimapSurface);
	if (rendererAux == NULL)
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());

	///-----------------	Save original texture
	SDL_RenderReadPixels(renderer, NULL, 0, mapSurface->pixels, mapSurface->pitch);
	SDL_Texture* originalTexture = SDL_CreateTextureFromSurface(rendererAux, mapSurface);
	if (originalTexture == nullptr)
		LOG("Could not load original texture! SDL_Error: %s\n", SDL_GetError());

	///-----------------	Save in renderer
	SaveInRenderer(originalTexture, cameraOffset.x, cameraOffset.y, NULL, scaleFactor, rendererAux);

	///-----------------	Save map texture
	SDL_RenderReadPixels(rendererAux, NULL, 0, minimapSurface->pixels, minimapSurface->pitch);
	minimapTexture = SDL_CreateTextureFromSurface(App->render->renderer, minimapSurface);

	SDL_DestroyTexture(originalTexture);

	return minimapTexture;
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

	if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, section, &rect, 0, NULL, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


