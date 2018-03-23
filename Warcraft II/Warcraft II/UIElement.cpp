#include "p2Log.h"

#include "j1App.h"

#include "UIElement.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Window.h"

UIElement::UIElement(iPoint localPos, UIElement* parent, j1Module* listener) : localPos(localPos), parent(parent), listener(listener)
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
			localPos.x = parent->GetLocalPos().x - (mouse_pos.x * scale - mouseClickPos.x);
			localPos.y = parent->GetLocalPos().y - (mouse_pos.y * scale - mouseClickPos.y);
		}
		else {
			localPos.x = mouse_pos.x * scale - mouseClickPos.x;
			localPos.y = mouse_pos.y * scale - mouseClickPos.y;
		}
	}
	else {
		localPos.x = mouse_pos.x * scale - mouseClickPos.x;
		localPos.y = mouse_pos.y * scale - mouseClickPos.y;
	}
}

void UIElement::Draw() const
{
	iPoint blitPos;
	int scale = App->win->GetScale();
	blitPos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blitPos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (parent != nullptr) {
		SDL_Rect daddy = parent->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (texArea.w != 0)
		App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y, &texArea);

	if (App->gui->isDebug)
		DebugDraw(blitPos);

	App->render->ResetViewPort();
}

void UIElement::DebugDraw(iPoint blitPos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blitPos.x, blitPos.y, width, height };
	App->render->DrawQuad(quad, 255, 255, 255, alpha, false);
}

void UIElement::HandleInput() {}

bool UIElement::HasToBeRemoved() const
{
	return toRemove;
}

UIE_TYPE UIElement::GetType() const
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
	case UIE_HORIZONTAL_POS::HORIZONTAL_POS_LEFT:
		break;
	case UIE_HORIZONTAL_POS::HORIZONTAL_POS_RIGHT:
		localPos.x -= width * scale;
		break;
	case UIE_HORIZONTAL_POS::HORIZONTAL_POS_CENTER:
		localPos.x -= (width / 2) * scale;
		break;
	}

	switch (vertical) {
	case UIE_VERTICAL_POS::VERTICAL_POS_TOP:
		break;
	case UIE_VERTICAL_POS::VERTICAL_POS_BOTTOM:
		localPos.y -= height * scale;
		break;
	case UIE_VERTICAL_POS::VERTICAL_POS_CENTER:
		localPos.y -= (height / 2) * scale;
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
		screen_pos.x = parent->GetScreenPos().x + localPos.x;
		screen_pos.y = parent->GetScreenPos().y + localPos.y;
	}
	else {
		screen_pos.x = localPos.x /*+ App->render->camera.x*/;
		screen_pos.y = localPos.y /*+ App->render->camera.y*/;
	}

	return screen_pos;
}

iPoint UIElement::GetLocalPos() const
{
	return localPos;
}

void UIElement::SetLocalPos(iPoint localPos)
{
	this->localPos = localPos;
}

void UIElement::IncreasePos(iPoint add_localPos)
{
	localPos.x += add_localPos.x;
	localPos.y += add_localPos.y;
}

void UIElement::DecreasePos(iPoint add_localPos)
{
	localPos.x -= add_localPos.x;
	localPos.y -= add_localPos.y;
}

void UIElement::SetInteraction(bool interactive)
{
	this->interactive = interactive;
}

UIElement* UIElement::GetParent() const
{
	return parent;
}