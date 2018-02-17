#include "UIImage.h"
#include "j1Render.h"
#include "j1Window.h"

UIImage::UIImage(iPoint localPos, UIElement* parent, UIImage_Info& info, j1Module* listener) : UIElement(localPos, parent, listener), image(info)
{
	type = UIE_TYPE_IMAGE;

	texArea = App->gui->GetRectFromAtlas(image.texArea);

	draggable = image.draggable;
	horizontal = image.horizontalOrientation;
	vertical = image.verticalOrientation;
	width = texArea.w;
	height = texArea.h;

	SetOrientation();
}

void UIImage::Update(float dt)
{
	if (startAimation && animToPlay.Finished()) {
		animToPlay.Reset();
		startAimation = false;
	}
	else if (startAimation) {
		animToPlay.speed = speed * dt;
		anim = &animToPlay;
	}
}

void UIImage::Draw() const
{
	iPoint blitPos;
	int scale = App->win->GetScale();
	blitPos.x = (GetScreenPos().x - App->render->camera.x) / scale;
	blitPos.y = (GetScreenPos().y - App->render->camera.y) / scale;

	if (image.quad) {
		SDL_SetRenderDrawColor(App->render->renderer, image.color.r, image.color.g, image.color.b, image.color.a);
		SDL_RenderFillRect(App->render->renderer, &image.quadArea);
	}
	else {
		if (texArea.w != 0 && startAimation)
			App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y, &anim->GetCurrentFrame());
		else if (texArea.w != 0 && !startAimation) {
			App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y, &texArea);
		}
		else
			App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y);
	}

	if (App->gui->isDebug)
		DebugDraw(blitPos);
}

void UIImage::DebugDraw(iPoint blitPos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blitPos.x, blitPos.y, width, height };
	App->render->DrawQuad(quad, 255, 0, 40, alpha, false);
}

//---------------------------------------------------------------

void UIImage::SetColor(const SDL_Color color)
{
	image.color = color;
}

SDL_Color UIImage::GetColor()
{
	return image.color;
}

bool UIImage::FromAlphaToAlphaFade(float from, float to, float seconds)
{
	bool ret = false;

	if (reset) {
		startTime = SDL_GetTicks();
		reset = false;
	}

	// Math operations
	totalTime = (Uint32)(seconds * 0.5f * 1000.0f);

	Uint32 now = (SDL_GetTicks() - startTime);
	float normalized = MIN(1.0f, (float)now / (float)totalTime);
	float normalized2 = MIN(1.0f, (float)now / (float)totalTime);
	normalized2 = 1 - normalized2;

	float alpha = (to - from) * normalized;
	float alpha2 = (from - to) * normalized2;

	// Color change
	if (from > to) {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha2 });
		if (alpha == from) {
			ResetFade();
			ret = true;
		}
	}
	else {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha });
		if (alpha2 == from) {
			ResetFade();
			ret = true;
		}
	}

	return ret;
}

void UIImage::ResetFade()
{
	reset = true;
}

void UIImage::SetNewRect(SDL_Rect& newRect)
{
	texArea = newRect;
}

SDL_Rect UIImage::GetRect()
{
	return texArea;
}
void UIImage::StartAnimation(Animation anim)
{
	startAimation = true;
	animToPlay = anim;
	speed = animToPlay.speed;
	this->anim = &animToPlay;
}

