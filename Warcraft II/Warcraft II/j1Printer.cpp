#include "j1Printer.h"
#include "j1Render.h"

#include "Brofiler\Brofiler.h"

bool j1Printer::Awake(pugi::xml_node&)
{
	return true;
}
bool j1Printer::Start()
{
	return true;
}
bool j1Printer::PreUpdate()
{
	return true;
}
bool j1Printer::Update(float dt)
{
	return true;
}
bool j1Printer::PostUpdate()
{
	BROFILER_CATEGORY("Printer Blit", Profiler::Color::Azure);

	while (!drawingQueue.empty())
	{
		DrawingElem* delem = drawingQueue.top();

		switch (delem->type)
		{
		case DrawingElem::DElemType::SPRITE:
		{
			Sprite* sprite = (Sprite*)delem;
			//SDL_SetTextureColorMod(sprite->texture, sprite->color.r, sprite->color.g, sprite->color.b);
			App->render->Blit(sprite->texture, sprite->pos.x, sprite->pos.y, &sprite->squareToBlit, 1.0f, sprite->degAngle);
			//SDL_SetTextureColorMod(sprite->texture, 255, 255, 255);
			break;
		}
		case DrawingElem::DElemType::QUAD:
		{
			Quad* quad = (Quad*)delem;
			App->render->DrawQuad(quad->rect, quad->color.r, quad->color.g, quad->color.b, quad->color.a, quad->filled, quad->useCamera);
			break;
		}
		case DrawingElem::DElemType::CIRCLE:
		{
			Circle* circle = (Circle*)delem;
			App->render->DrawCircle(circle->pos.x, circle->pos.y, circle->radius, circle->color.r, circle->color.g, circle->color.b, circle->color.a, circle->useCamera);
			break;
		}
		case DrawingElem::DElemType::LINE:
		{
			Line* line = (Line*)delem;
			App->render->DrawLine(line->pointA.x, line->pointA.y, line->pointB.x, line->pointB.y, line->color.r, line->color.g, line->color.b, line->color.a, line->useCamera);
			break;
		}
		default:
			break;
		}

		drawingQueue.pop();
		delete delem;
	}
	return true;
}

bool j1Printer::CleanUp()
{
	while (!drawingQueue.empty())
	{
		DrawingElem* delem = drawingQueue.top();
		drawingQueue.pop();
		delete delem;
	}
	return drawingQueue.empty();
}

bool j1Printer::PrintSprite(iPoint pos, SDL_Texture* texture, SDL_Rect squareToBlit, int layer, float degAngle, SDL_Color color)
{
	Sprite* sprite = new Sprite(pos, texture, squareToBlit, layer, degAngle, color);
	drawingQueue.push(sprite);

	return true;
}

bool j1Printer::PrintQuad(SDL_Rect rect, SDL_Color color, bool filled, bool useCamera, int layer)
{
	Quad* quad = new Quad(rect, color, filled, useCamera, layer);
	drawingQueue.push(quad);

	return true;
}

bool j1Printer::PrintCircle(iPoint pos, int radius, SDL_Color color, bool useCamera, int layer) 
{
	Circle* circle = new Circle(pos, radius, color, useCamera, layer);
	drawingQueue.push(circle);

	return true;
}

bool j1Printer::PrintLine(iPoint pointA, iPoint pointB, SDL_Color color, bool useCamera, int layer) 
{
	Line* line = new Line(pointA, pointB, color, useCamera, layer);
	drawingQueue.push(line);

	return true;
}