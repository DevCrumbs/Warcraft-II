#include "UILifeBar.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Window.h"

UILifeBar::UILifeBar(iPoint local_pos, UIElement* parent, UILifeBar_Info& info, j1Module* listener, bool isInWorld) : UIElement(local_pos, parent, listener, isInWorld), life_bar(info)
{
	type = UIE_TYPE::UIE_TYPE_LIFE_BAR;

	bar = App->gui->GetRectFromAtlas(life_bar.bar);
	background = App->gui->GetRectFromAtlas(life_bar.background);

	local_pos = life_bar.life_bar_position;
}

void UILifeBar::Update(float dt)
{
	int l;
	l = life_bar.life * 100 / life_bar.maxLife; //Pass life to %
	bar.w = l * life_bar.maxWidth / 100;		//Pass % to width

}

void UILifeBar::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetScreenPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetScreenPos().y - App->render->camera.y) / scale;

	App->render->Blit(App->gui->GetAtlas(), blit_pos.x + life_bar.life_bar_position.x, blit_pos.y + life_bar.life_bar_position.y, &background);
	App->render->Blit(App->gui->GetAtlas(), blit_pos.x + life_bar.life_bar_position.x, blit_pos.y + life_bar.life_bar_position.y, &bar);
}

void UILifeBar::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 255, 20, 255, alpha, false);
}


void UILifeBar::SetLife(const int life)
{
	life_bar.life = life;
}

void UILifeBar::IncreaseLife(const int life)
{
	life_bar.life += life;
	if (life_bar.life > background.w)
		life_bar.life = background.w;
}

void UILifeBar::DecreaseLifeProgress(const int life)
{
	life_bar.life -= life;
	if (life_bar.life < 0)
		life_bar.life = 0;
}

int UILifeBar::GetLife() const
{
	return life_bar.life;
}