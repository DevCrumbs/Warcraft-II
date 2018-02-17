#include "UIButton.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Window.h"

UIButton::UIButton(iPoint localPos, UIElement* parent, UIButton_Info& info, j1Module* listener) : UIElement(localPos, parent, listener), button(info)
{
	type = UIE_TYPE::UIE_TYPE_BUTTON;

	normalTexArea = App->gui->GetRectFromAtlas(button.normalTexArea);
	hoverTexArea = App->gui->GetRectFromAtlas(button.hoverTexArea);
	pressedTexArea = App->gui->GetRectFromAtlas(button.pressedTexArea);

	draggable = button.draggable;
	interactive = button.interactive;
	horizontal = button.horizontalOrientation;
	vertical = button.verticalOrientation;

	if (button.isChecked)
		texArea = pressedTexArea;
	else
		texArea = normalTexArea;

	width = texArea.w;
	height = texArea.h;

	SetOrientation();
}

void UIButton::Update(float dt)
{
	if (listener != nullptr && interactive)
		HandleInput();
}

void UIButton::DebugDraw(iPoint blitPos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blitPos.x, blitPos.y, width, height };
	App->render->DrawQuad(quad, 5, 12, 255, alpha, false);
}

void UIButton::HandleInput()
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (UIevent) {

	case UI_EVENT_NONE:
		if (MouseHover() || tab) {
			nextEvent = false;
			UIevent = UI_EVENT_MOUSE_ENTER;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_ENTER:

		if (!MouseHover() && !tab) {
			LOG("MOUSE LEAVE");
			nextEvent = false;
			UIevent = UI_EVENT_MOUSE_LEAVE;
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)) {
			nextEvent = false;
			LOG("MOUSE L CLICK START");
			ChangeSprite(pressedTexArea);
			UIevent = UI_EVENT_MOUSE_LEFT_CLICK;

			if (button.checkbox)
				button.isChecked = !button.isChecked;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)) {
			nextEvent = false;
			LOG("MOUSE R CLICK START");
			ChangeSprite(pressedTexArea);

			mouseClickPos.x = mouse_pos.x * App->win->GetScale() - GetLocalPos().x;
			mouseClickPos.y = mouse_pos.y * App->win->GetScale() - GetLocalPos().y;

			if (draggable) {
				drag = true;
				//App->gui->drag_to_true = true;
			}

			UIevent = UI_EVENT_MOUSE_RIGHT_CLICK;

			if (button.checkbox)
				button.isChecked = !button.isChecked;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}

		if (!nextEvent) {
			LOG("MOUSE ENTER");
			if (!button.isChecked)
				ChangeSprite(hoverTexArea);
			nextEvent = true;
		}

		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:

		if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP)) {
			LOG("MOUSE R CLICK FINISH");

			if (draggable) {
				drag = false;
				//App->gui->drag_to_false = true;
			}
			UIevent = UI_EVENT_MOUSE_RIGHT_UP;
			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}

		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (!MouseHover() && !tab) {
			LOG("MOUSE LEAVE");
			UIevent = UI_EVENT_MOUSE_LEAVE;
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP)) {
			LOG("MOUSE L CLICK FINISH");
			UIevent = UI_EVENT_MOUSE_LEFT_UP;
			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}

		break;
	case UI_EVENT_MOUSE_LEAVE:
		if (!button.isChecked)
			ChangeSprite(normalTexArea);
		listener->OnUIEvent((UIElement*)this, UIevent);
		UIevent = UI_EVENT_NONE;
		break;
	}
}

//---------------------------------------------------------------

void UIButton::ChangeSprite(SDL_Rect texArea)
{
	this->texArea = texArea;
}

SDL_Rect UIButton::GetHoverSprite() const
{
	return hoverTexArea;
}

SDL_Rect UIButton::GetPressedSprite() const
{
	return pressedTexArea;
}

SDL_Rect UIButton::GetNormalSprite() const
{
	return normalTexArea;
}

UI_EVENT UIButton::GetActualEvent() const
{
	return UIevent;
}

bool UIButton::SlideTransition(float dt, int endPosY, float speed, bool bounce, float bounceInterval, float bounceSpeed, bool down)
{
	bool ret = false;

	iPoint pos = GetLocalPos();

	if (down) {
		if (pos.y + height >= (int)endPosY - height / 2) {
			if (bounce && !startBouncing)
				startBouncing = true;
			else if (!bounce)
				ret = true;
		}
		else if (!startBouncing) {
			if ((int)(speed * dt) >= 1)
				IncreasePos({ 0,(int)(speed * dt) });
			else
				IncreasePos({ 0, 1 });
		}
	}
	else {
		if (pos.y + height <= (int)endPosY - height / 2) {
			if (bounce && !startBouncing)
				startBouncing = true;
			else if (!bounce)
				ret = true;
		}
		else if (!startBouncing) {
			if ((int)(speed * dt) >= 1)
				DecreasePos({ 0,(int)(speed * dt) });
			else
				DecreasePos({ 0, 1 });
		}
	}

	if (startBouncing) {
		if (Bounce(dt, bounceInterval, bounceSpeed, down)) {
			ret = true;
		}
	}

	return ret;
}

bool UIButton::Bounce(float dt, float bounceInterval, float bounceSpeed, bool down)
{
	bool ret = false;

	iPoint pos = GetLocalPos();

	if (reset) {
		InitializeBounce(bounceInterval, down);
		reset = false;
	}

	if (bounceValue <= bounceSpeed)
		ret = true;

	if (firstBounce) {
		if (pos.y >= startPos.y + bounceValue) {
			bounceValue -= bounceSpeed;
			firstBounce = false;
		}
		else {
			if ((int)(bounceValue * 10.0f * dt) >= 1)
				IncreasePos({ 0, (int)(bounceValue * 10.0f * dt) });
			else
				IncreasePos({ 0, 1 });
		}
	}
	else {
		if (pos.y <= startPos.y - bounceValue) {
			bounceValue -= bounceSpeed;
			firstBounce = true;
		}
		else {
			if ((int)(bounceValue * 10.0f * dt) >= 1)
				DecreasePos({ 0, (int)(bounceValue * 10.0f * dt) });
			else
				DecreasePos({ 0, 1 });
		}
	}

	return ret;
}

void UIButton::InitializeBounce(float bounceInterval, bool down)
{
	bounceValue = bounceInterval;
	startPos = GetLocalPos();

	if (!down)
		firstBounce = false;
}
