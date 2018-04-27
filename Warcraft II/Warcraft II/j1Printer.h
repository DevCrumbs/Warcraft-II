#ifndef __j1PRINTER_H__
#define __j1PRINTER_H__

#include "j1Module.h"
#include "j1App.h"
#include "p2Point.h"

#include "SDL\include\SDL.h"

#include <queue>

class SDL_Texture;

enum Layers
{
	Layers_Map = -1,
	Layers_Paws = 0,
	Layers_FloorColliders = 1,
	Layers_Entities = 2,
	Layers_PreviewBuildings = 3,
	Layers_BasicParticles = 4,
	Layers_DragonGryphon = 5
};

class DrawingElem
{
public:

	enum class DElemType
	{
		NONE = -1,
		SPRITE,
		QUAD,
		CIRCLE

	} type = DElemType::NONE;

public:

	DrawingElem(DElemType type) : type(type) {}
};

class Sprite : public DrawingElem
{
public:

	iPoint pos = { 0,0 };
	SDL_Texture* texture = nullptr;
	SDL_Rect squareToBlit = { 0,0,0,0 };
	double angle = 0.0f;
	int distToFeet = 0;
	SDL_Color color = { 255,255,255,255 };

	int layer = 0;

public:

	Sprite(iPoint& pos, SDL_Texture* texture, SDL_Rect& squareToBlit, int layer, double degAngle, SDL_Color color) : DrawingElem(DrawingElem::DElemType::SPRITE), pos(pos), texture(texture), squareToBlit(squareToBlit), distToFeet(distToFeet), layer(layer), angle(degAngle), color(color) {}
};

class Quad : public DrawingElem
{
public:

	SDL_Rect rect = { 0,0,0,0 };
	SDL_Color color = { 255,255,255,255 };
	bool filled = false;
	bool useCamera = false;

	int layer = 0;

public:

	Quad(SDL_Rect rect, SDL_Color color, bool filled, bool useCamera, int layer) : DrawingElem(DrawingElem::DElemType::QUAD), rect(rect), color(color), filled(filled), useCamera(useCamera), layer(layer) {}
};

class Circle : public DrawingElem
{
public:

	iPoint pos = { 0,0 };
	int radius = 0;
	SDL_Color color = { 255,255,255,255 };
	bool useCamera = false;

	int layer = 0;

public:

	Circle(iPoint pos, int radius, SDL_Color color, bool useCamera, int layer) : DrawingElem(DrawingElem::DElemType::CIRCLE), pos(pos), radius(radius), color(color), useCamera(useCamera), layer(layer) {}
};

class Compare
{
public:

	bool operator () (DrawingElem* first, DrawingElem* second)
	{
		if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::SPRITE)
		{
			Sprite* firstSprite = (Sprite*)first;
			Sprite* secondSprite = (Sprite*)second;

			if (firstSprite->layer != secondSprite->layer)
				return firstSprite->layer > secondSprite->layer;
			else
				return firstSprite->pos.y + firstSprite->squareToBlit.h > secondSprite->pos.y + secondSprite->squareToBlit.h;
		}
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::QUAD)
		{
			Quad* firstQuad = (Quad*)first;
			Quad* secondQuad = (Quad*)second;

			if (firstQuad->layer != secondQuad->layer)
				return firstQuad->layer > secondQuad->layer;
			else
				return firstQuad->rect.y + firstQuad->rect.h > secondQuad->rect.y + secondQuad->rect.h;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::CIRCLE)
		{
			Circle* firstCircle = (Circle*)first;
			Circle* secondCircle = (Circle*)second;

			if (firstCircle->layer != secondCircle->layer)
				return firstCircle->layer > secondCircle->layer;
			else
				return firstCircle->pos.y + firstCircle->radius > secondCircle->pos.y + secondCircle->radius;
		}
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::SPRITE)
		{
			return true;
		}
		else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::QUAD)
		{
			return false;
		}
		else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::CIRCLE)
		{
			return false;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::SPRITE)
		{
			return true;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::QUAD)
		{
			return true;
		}
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::CIRCLE)
		{
			return true;
		}
		else
			return true;
	}
};

class j1Printer : public j1Module
{
public:

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:

	// Note: angle required is in degrees, in clockwise direction
	bool PrintSprite(iPoint pos, SDL_Texture* texture, SDL_Rect SquaretoBlit, int layer = 0, float degangle = 0, SDL_Color color = { 255,255,255,255 });
	bool PrintQuad(SDL_Rect rect, SDL_Color color, bool filled = false, bool useCamera = true, int layer = 0);
	bool PrintCircle(iPoint pos, int radius, SDL_Color color, bool useCamera = true, int layer = 0);

private:

	std::priority_queue<DrawingElem*, std::vector<DrawingElem*>, Compare> drawingQueue;
};

#endif //__j1PRINTER_H__