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

	this->isInWorld = isInWorld;
	
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
	blit_pos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (GetParent() != nullptr) {
		SDL_Rect daddy = GetParent()->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (isInWorld) {
		
		App->render->Blit(App->gui->GetAtlas(), GetLocalPos().x + life_bar.lifeBarPosition.x, GetLocalPos().y + life_bar.lifeBarPosition.y, &bar);
		App->render->Blit(App->gui->GetAtlas(), GetLocalPos().x, GetLocalPos().y, &background);
	}
	else if (!isInWorld) {
		App->render->Blit(App->gui->GetAtlas(), blit_pos.x + life_bar.lifeBarPosition.x, blit_pos.y + life_bar.lifeBarPosition.y, &bar);
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
	life_bar.life = life;
}

void UILifeBar::IncreaseLife(const int life)
{
	life_bar.life += life;
	if (life_bar.life > background.w)
		life_bar.life = background.w;
}

void UILifeBar::DecreaseLife(const int life)
{
	life_bar.life -= life;
	if (life_bar.life < 0)
		life_bar.life = 0;
}

int UILifeBar::GetLife() const
{
	return life_bar.life;
}

void UILifeBar::SetBar(SDL_Rect bar)
{
	this->bar = App->gui->GetRectFromAtlas(bar);
}
