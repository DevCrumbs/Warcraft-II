#include "UIImage.h"
#include "j1Render.h"
#include "j1Window.h"

UIImage::UIImage(iPoint local_pos, UIElement* parent, UIImage_Info& info, j1Module* listener) : UIElement(local_pos, parent, listener), image(info)
{
	type = UIElement_TYPE::IMAGE_;

	tex_area = App->gui->GetRectFromAtlas(image.tex_area);

	draggable = image.draggable;
	horizontal = image.horizontal_orientation;
	vertical = image.vertical_orientation;
	width = tex_area.w;
	height = tex_area.h;

	SetOrientation();
}

void UIImage::Update(float dt)
{
	if (start_aimation && anim_to_play.Finished()) {
		anim_to_play.Reset();
		start_aimation = false;
	}
	else if (start_aimation) {
		anim_to_play.speed = speed * dt;
		anim = &anim_to_play;
	}
}

void UIImage::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetScreenPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetScreenPos().y - App->render->camera.y) / scale;

	if (image.quad) {
		SDL_SetRenderDrawColor(App->render->renderer, image.color.r, image.color.g, image.color.b, image.color.a);
		SDL_RenderFillRect(App->render->renderer, &image.quad_area);
	}
	else {
		if (tex_area.w != 0 && start_aimation)
			App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y, &anim->GetCurrentFrame());
		else if (tex_area.w != 0 && !start_aimation) {
			App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y, &tex_area);
		}
		else
			App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y);
	}

	if (App->gui->debug_draw)
		DebugDraw(blit_pos);
}

void UIImage::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
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

void UIImage::SetNewRect(SDL_Rect& new_rect)
{
	tex_area = new_rect;
}

SDL_Rect UIImage::GetRect()
{
	return tex_area;
}
void UIImage::StartAnimation(Animation anim)
{
	start_aimation = true;
	anim_to_play = anim;
	speed = anim_to_play.speed;
	this->anim = &anim_to_play;
}

