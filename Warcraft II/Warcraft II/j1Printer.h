#ifndef __j1PRINTER_H__
#define __j1PRINTER_H__

#include "j1Module.h"
#include "j1App.h"
#include "p2Point.h"
#include "p2Log.h"

#include "SDL\include\SDL.h"

#include <queue>

class SDL_Texture;

enum Layers
{
	Layers_Map = -2,
	Layers_Paws = 0,
	Layers_FloorColliders = 1,
	Layers_Entities = 2,
	Layers_PreviewBuildings = 3,
	Layers_PreviewBuildingsQuad = 4,
	Layers_BasicParticles = 5,
	Layers_DragonGryphon = 6,
	Layers_QuadsPrinters = 7,
	Layers_GreyFoW = 8,
	Layers_BlackFoW = 9
};

class DrawingElem
{
public:

	enum class DElemType
	{
		NONE = -1,
		SPRITE,
		QUAD,
		CIRCLE,
		LINE

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
	double degAngle = 0.0f;
	int distToFeet = 0;
	SDL_Color color = { 255,255,255,255 };

	int layer = 0;

public:

	Sprite(iPoint& pos, SDL_Texture* texture, SDL_Rect& squareToBlit, int layer, double degAngle, SDL_Color color) : DrawingElem(DrawingElem::DElemType::SPRITE), pos(pos), texture(texture), squareToBlit(squareToBlit), distToFeet(distToFeet), layer(layer), degAngle(degAngle), color(color) {}
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

class Line : public DrawingElem
{
public:

	iPoint pointA = { 0,0 };
	iPoint pointB = { 0,0 };
	SDL_Color color = { 255,255,255,255 };
	bool useCamera = false;

	int layer = 0;

public:

	Line(iPoint pointA, iPoint pointB, SDL_Color color, bool useCamera, int layer) : DrawingElem(DrawingElem::DElemType::LINE), pointA(pointA), pointB(pointB), color(color), useCamera(useCamera), layer(layer) {}
};

class Compare
{
public:

	bool operator() (DrawingElem* first, DrawingElem* second)
	{
		// Same elements
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
		else if (first->type == DrawingElem::DElemType::LINE && second->type == DrawingElem::DElemType::LINE)
		{
			Line* firstLine = (Line*)first;
			Line* secondLine = (Line*)second;

			if (firstLine->layer != secondLine->layer)
				return firstLine->layer > secondLine->layer;
			else
				return firstLine->pointA.y > secondLine->pointA.y;
		}

		// Different elements
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::SPRITE)
		{
			Quad* firstQuad = (Quad*)first;
			Sprite* secondSprite = (Sprite*)second;

			if (firstQuad->layer != secondSprite->layer)
				return firstQuad->layer > secondSprite->layer;
			else
				return firstQuad->rect.y + firstQuad->rect.h > secondSprite->pos.y + secondSprite->squareToBlit.h;
		}
		else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::QUAD)
		{
			Sprite* firstSprite = (Sprite*)first;
			Quad* secondQuad = (Quad*)second;

			if (firstSprite->layer != secondQuad->layer)
				return firstSprite->layer > secondQuad->layer;
			else
				return firstSprite->pos.y + firstSprite->squareToBlit.h > secondQuad->rect.y + secondQuad->rect.h;
		}
		else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::CIRCLE)
		{
			Sprite* firstSprite = (Sprite*)first;
			Circle* secondCircle = (Circle*)second;

			if (firstSprite->layer != secondCircle->layer)
				return firstSprite->layer > secondCircle->layer;
			else
				return firstSprite->pos.y + firstSprite->squareToBlit.h > secondCircle->pos.y + secondCircle->radius;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::SPRITE)
		{
			Circle* firstCircle = (Circle*)first;
			Sprite* secondSprite = (Sprite*)second;

			if (firstCircle->layer != secondSprite->layer)
				return firstCircle->layer > secondSprite->layer;
			else
				return firstCircle->pos.y + firstCircle->radius > secondSprite->pos.y + secondSprite->squareToBlit.h;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::QUAD)
		{
			Circle* firstCircle = (Circle*)first;
			Quad* secondQuad = (Quad*)second;

			if (firstCircle->layer != secondQuad->layer)
				return firstCircle->layer > secondQuad->layer;
			else
				return firstCircle->pos.y + firstCircle->radius > secondQuad->rect.y + secondQuad->rect.h;
		}
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::CIRCLE)
		{
			Quad* firstQuad = (Quad*)first;
			Circle* secondCircle = (Circle*)second;

			if (firstQuad->layer != secondCircle->layer)
				return firstQuad->layer > secondCircle->layer;
			else
				return firstQuad->rect.y + firstQuad->rect.h > secondCircle->pos.y + secondCircle->radius;
		}
		//
		else if (first->type == DrawingElem::DElemType::LINE && second->type == DrawingElem::DElemType::SPRITE)
		{
			Line* firstLine = (Line*)first;
			Sprite* secondSprite = (Sprite*)second;

			if (firstLine->layer != secondSprite->layer)
				return firstLine->layer > secondSprite->layer;
			else
				return firstLine->pointA.y > secondSprite->pos.y + secondSprite->squareToBlit.h;
		}
		else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::LINE)
		{
			Sprite* firstSprite = (Sprite*)first;
			Line* secondLine = (Line*)second;

			if (firstSprite->layer != secondLine->layer)
				return firstSprite->layer > secondLine->layer;
			else
				return firstSprite->pos.y + firstSprite->squareToBlit.h > secondLine->pointA.y;
		}
		else if (first->type == DrawingElem::DElemType::LINE && second->type == DrawingElem::DElemType::QUAD)
		{
			Line* firstLine = (Line*)first;
			Quad* secondQuad = (Quad*)second;

			if (firstLine->layer != secondQuad->layer)
				return firstLine->layer > secondQuad->layer;
			else
				return firstLine->pointA.y > secondQuad->rect.y + secondQuad->rect.h;
		}
		else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::LINE)
		{
			Quad* firstQuad = (Quad*)first;
			Line* secondLine = (Line*)second;

			if (firstQuad->layer != secondLine->layer)
				return firstQuad->layer > secondLine->layer;
			else
				return firstQuad->rect.y + firstQuad->rect.h > secondLine->pointA.y;
		}
		else if (first->type == DrawingElem::DElemType::LINE && second->type == DrawingElem::DElemType::CIRCLE)
		{
			Line* firstLine = (Line*)first;
			Circle* secondCircle = (Circle*)second;

			if (firstLine->layer != secondCircle->layer)
				return firstLine->layer > secondCircle->layer;
			else
				return firstLine->pointA.y > secondCircle->pos.y + secondCircle->radius;
		}
		else if (first->type == DrawingElem::DElemType::CIRCLE && second->type == DrawingElem::DElemType::LINE)
		{
			Circle* firstCircle = (Circle*)first;
			Line* secondLine = (Line*)second;

			if (firstCircle->layer != secondLine->layer)
				return firstCircle->layer > secondLine->layer;
			else
				return firstCircle->pos.y + firstCircle->radius > secondLine->pointA.y;
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
	bool PrintLine(iPoint pointA, iPoint pointB, SDL_Color color, bool useCamera = true, int layer = 0);

private:

	std::priority_queue<DrawingElem*, std::vector<DrawingElem*>, Compare> drawingQueue;
};

#endif //__j1PRINTER_H__