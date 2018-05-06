#include "UILifeBar.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Window.h"

UILifeBar::UILifeBar(iPoint local_pos, UIElement* parent, UILifeBar_Info& info, j1Module* listener, bool isInWorld) : UIElement(local_pos, parent, listener, isInWorld), lifeBar(info)
{
	type = UIE_TYPE_LIFE_BAR;

	bar = App->gui->GetRectFromAtlas(lifeBar.bar);
	background = App->gui->GetRectFromAtlas(lifeBar.background);
	height = lifeBar.background.h;
	width = lifeBar.background.w;

	this->isInWorld = isInWorld;
	priority = PriorityDraw_IMAGE;
	
}

void UILifeBar::Update(float dt)
{
	int l;

	if (lifeBar.maxLife > 0) {

		l = lifeBar.life * 100 / lifeBar.maxLife; //Pass life to %
		bar.w = l * lifeBar.maxWidth / 100;//Pass % to width
	}
}

void UILifeBar::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (GetParent() != nullptr) {
		SDL_Rect daddy = GetParent()->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (isInWorld) {
		
		App->render->Blit(App->gui->GetAtlas(), GetLocalPos().x + lifeBar.lifeBarPosition.x, GetLocalPos().y + lifeBar.lifeBarPosition.y, &bar);
		App->render->Blit(App->gui->GetAtlas(), GetLocalPos().x, GetLocalPos().y, &background);
	}
	else if (!isInWorld) {
		App->render->Blit(App->gui->GetAtlas(), blit_pos.x + lifeBar.lifeBarPosition.x, blit_pos.y + lifeBar.lifeBarPosition.y, &bar);
		App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y, &background);
	}
	//else
	//	App->render->Blit(App->gui->GetAtlas(), GetLocalPos().x, GetLocalPos().y, &texArea);
	if (GetParent() != nullptr)
		App->render->ResetViewPort();
}

void UILifeBar::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 255, 20, 255, alpha, false);
}


void UILifeBar::SetLife(const int life)
{
	lifeBar.life = life;
}

void UILifeBar::SetMaxLife(const int maxLife)
{
	lifeBar.maxLife = maxLife;
}

void UILifeBar::IncreaseLife(const int life)
{
	lifeBar.life += life;
	if (lifeBar.life > background.w)
		lifeBar.life = background.w;
}

void UILifeBar::DecreaseLife(const int life)
{
	lifeBar.life -= life;
	if (lifeBar.life < 0)
		lifeBar.life = 0;
}

int UILifeBar::GetLife() const
{
	return lifeBar.life;
}