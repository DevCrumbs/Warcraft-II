#include "p2Log.h"

#include "j1App.h"

#include "UIElement.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Window.h"

UIElement::UIElement(iPoint local_pos, UIElement* parent, j1Module* listener) : local_pos(local_pos), parent(parent), listener(listener)
{
	uint width = 0, height = 0, scale = 0;

	App->win->GetWindowSize(width, height);
	scale = App->win->GetScale();
}

UIElement::~UIElement() {}

void UIElement::Update(float dt) {}

void UIElement::UpdateDragging(float dt)
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	uint scale = App->win->GetScale();

	if (parent != nullptr) {
		if (parent->drag) {
			local_pos.x = parent->GetLocalPos().x - (mouse_pos.x * scale - mouse_click_pos.x);
			local_pos.y = parent->GetLocalPos().y - (mouse_pos.y * scale - mouse_click_pos.y);
		}
		else {
			local_pos.x = mouse_pos.x * scale - mouse_click_pos.x;
			local_pos.y = mouse_pos.y * scale - mouse_click_pos.y;
		}
	}
	else {
		local_pos.x = mouse_pos.x * scale - mouse_click_pos.x;
		local_pos.y = mouse_pos.y * scale - mouse_click_pos.y;
	}
}

void UIElement::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (parent != nullptr) {
		SDL_Rect daddy = parent->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (tex_area.w != 0)
		App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y, &tex_area);

	if (App->gui->debug_draw)
		DebugDraw(blit_pos);

	App->render->ResetViewPort();
}

void UIElement::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 255, 255, 255, alpha, false);
}

void UIElement::HandleInput() {}

bool UIElement::HasToBeRemoved() const
{
	return to_remove;
}

UIElement_TYPE UIElement::GetType() const
{
	return type;
}

bool UIElement::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	return x > GetScreenPos().x / scale && x < GetScreenPos().x / scale + GetLocalRect().w && y > GetScreenPos().y / scale && y < GetScreenPos().y / scale + GetLocalRect().h;
}

void UIElement::SetOrientation()
{
	uint scale = App->win->GetScale();

	switch (horizontal) {
	case UIElement_HORIZONTAL_POS::LEFT_:
		break;
	case UIElement_HORIZONTAL_POS::RIGHT_:
		local_pos.x -= width * scale;
		break;
	case UIElement_HORIZONTAL_POS::CENTER_:
		local_pos.x -= (width / 2) * scale;
		break;
	}

	switch (vertical) {
	case UIElement_VERTICAL_POS::TOP_:
		break;
	case UIElement_VERTICAL_POS::BOTTOM_:
		local_pos.y -= height * scale;
		break;
	case UIElement_VERTICAL_POS::MIDDLE_:
		local_pos.y -= (height / 2) * scale;
		break;
	}
}

//---------------------------------------------------------------

SDL_Rect UIElement::GetScreenRect() const
{
	return { GetScreenPos().x, GetScreenPos().y, width, height };
}

SDL_Rect UIElement::GetLocalRect() const
{
	return { GetLocalPos().x, GetLocalPos().y, width, height };
}

iPoint UIElement::GetScreenPos() const
{
	iPoint screen_pos;

	if (parent != nullptr) {
		screen_pos.x = parent->GetScreenPos().x + local_pos.x;
		screen_pos.y = parent->GetScreenPos().y + local_pos.y;
	}
	else {
		screen_pos.x = local_pos.x;
		screen_pos.y = local_pos.y;
	}

	return screen_pos;
}

iPoint UIElement::GetLocalPos() const
{
	return local_pos;
}

void UIElement::SetLocalPos(iPoint local_pos)
{
	this->local_pos = local_pos;
}

void UIElement::IncreasePos(iPoint add_local_pos)
{
	local_pos.x += add_local_pos.x;
	local_pos.y += add_local_pos.y;
}

void UIElement::DecreasePos(iPoint add_local_pos)
{
	local_pos.x -= add_local_pos.x;
	local_pos.y -= add_local_pos.y;
}

void UIElement::SetInteraction(bool interactive)
{
	this->interactive = interactive;
}

UIElement* UIElement::GetParent() const
{
	return parent;
}