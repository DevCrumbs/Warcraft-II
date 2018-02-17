#include "UICursor.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Window.h"

UICursor::UICursor(iPoint localPos, UIElement* parent, UICursor_Info& info, j1Module* listener) : UIElement(localPos, parent, listener), cursor(info)
{
	type = UIE_TYPE::UIE_TYPE_CURSOR;

	default = App->gui->GetRectFromAtlas(cursor.default);
	onClick = App->gui->GetRectFromAtlas(cursor.onClick);

	texArea = default;
	width = texArea.w;
	height = texArea.h;

	SDL_ShowCursor(0);
}

void UICursor::Update(float dt)
{
	SetMousePosition();

	if (listener != nullptr)
		HandleInput();
}

void UICursor::Draw() const {}

void  UICursor::DrawAbove() const
{
	iPoint blitPos;
	int scale = App->win->GetScale();
	blitPos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blitPos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (texArea.w != 0)
		App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y, &texArea);
	else
		App->render->Blit(App->gui->GetAtlas(), blitPos.x, blitPos.y);

	if (App->gui->isDebug)
		DebugDraw(blitPos);
}

void UICursor::HandleInput()
{
	switch (UIevent)
	{
	case UI_EVENT_NONE:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {
			UIevent = UI_EVENT_MOUSE_LEFT_CLICK;
			texArea = onClick;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {
			UIevent = UI_EVENT_MOUSE_RIGHT_CLICK;
			texArea = onClick;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) {
			UIevent = UI_EVENT_NONE;
			texArea = default;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) {
			UIevent = UI_EVENT_NONE;
			texArea = default;
			listener->OnUIEvent(this, UIevent);
			break;
		}
		break;
	}
}

//---------------------------------------------------------------

void UICursor::SetMousePosition()
{
	int scale = App->win->GetScale();
	iPoint mouse_position;
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);
	mouse_position.x *= scale;
	mouse_position.y *= scale;
	SetLocalPos(mouse_position);
}
