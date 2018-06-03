#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Window.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1Map.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	vsync = config.child("vsync").attribute("value").as_bool();
	if (vsync == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);


	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screenSurface->w / App->win->GetScale();
		camera.h = App->win->screenSurface->h / App->win->GetScale();
		camera.x = 0;
		camera.y = 0;
	}

	uint width = 0, height = 0;
	App->win->GetWindowSize(width, height);

	SDL_RenderSetLogicalSize(renderer, width, height);

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);

	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);

	return true;
}

bool j1Render::Update(float dt)
{
	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{	
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	float scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

fPoint j1Render::GetMidCameraPos() const 
{
	return { (camera.w / 2) - camera.x, (camera.h / 2) - camera.y };
}

iPoint j1Render::FindCameraPosFromCenterPos(iPoint centerPos)
{
	iPoint finalPos = centerPos;
	finalPos.x -= camera.w / 2;
	finalPos.y -= camera.h / 2;

	finalPos.x = -finalPos.x;
	finalPos.y = -finalPos.y;

	// Check the boundaries of the map and reposition the camera accordingly
	uint width = 0;
	uint height = 0;
	float scale = 0;
	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();

	int downMargin = -(App->map->data.height * App->map->data.tileHeight) + height / scale;
	int rightMargin = -(App->map->data.width * App->map->data.tileWidth) + width / scale;

	// Succeed! The camera fits the position calculated
	if (finalPos.y <= 0 && finalPos.y >= downMargin && finalPos.x <= 0 && finalPos.x >= rightMargin)
		return finalPos;

	// Failed... Find the closest position for the camera to the position calculated
	/// First, find the condition/s that failed
	if (finalPos.y >= 0)
		finalPos.y = 0;
	else if (finalPos.y <= downMargin)
		finalPos.y = downMargin;

	if (finalPos.x >= 0)
		finalPos.x = 0;
	else if (finalPos.x <= rightMargin)
		finalPos.x = rightMargin;

	return finalPos;
}

// Blit to screen
bool j1Render::Blit(const SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip rendererFlip, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	float scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, (SDL_Texture*)texture, section, &rect, angle, p, rendererFlip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool useCamera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (useCamera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (useCamera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for (uint i = 0; i < 360; ++i)
	{
		if (useCamera) {

			points[i].x = (int)(x * scale + camera.x + radius * cos(i * factor));
			points[i].y = (int)(y * scale + camera.y + radius * sin(i * factor));
		}
		else {

			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)(y + radius * sin(i * factor));
		}
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::IsInScreen(const SDL_Rect& item) const
{
	SDL_Rect cameraRect{ -camera.x, -camera.y, camera.w, camera.h };
	return SDL_HasIntersection(&item, &cameraRect);
}

bool j1Render::IsInScreen(const iPoint& item) const
{
	SDL_Rect itemRect{ item.x,item.y,1,1 };

	return IsInScreen(itemRect);
}

bool j1Render::IsInScreen(const fPoint& item) const
{
	SDL_Rect itemRect{ item.x,item.y,1,1 };

	return IsInScreen(itemRect);
}

bool j1Render::IsInRectangle(const SDL_Rect& rectangle, const SDL_Rect& item) const 
{
	return SDL_HasIntersection(&item, &rectangle);
}


