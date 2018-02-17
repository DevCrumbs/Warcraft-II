#include "UIButton.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Window.h"

UIButton::UIButton(iPoint local_pos, UIElement* parent, UIButton_Info& info, j1Module* listener) : UIElement(local_pos, parent, listener), button(info)
{
	type = UIElement_TYPE::BUTTON_;

	normal_tex_area = App->gui->GetRectFromAtlas(button.normal_tex_area);
	hover_tex_area = App->gui->GetRectFromAtlas(button.hover_tex_area);
	pressed_tex_area = App->gui->GetRectFromAtlas(button.pressed_tex_area);

	draggable = button.draggable;
	interactive = button.interactive;
	horizontal = button.horizontal_orientation;
	vertical = button.vertical_orientation;

	if (button.checkbox_checked)
		tex_area = pressed_tex_area;
	else
		tex_area = normal_tex_area;

	width = tex_area.w;
	height = tex_area.h;

	SetOrientation();
}

void UIButton::Update(float dt)
{
	if (listener != nullptr && interactive)
		HandleInput();
}

void UIButton::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 5, 12, 255, alpha, false);
}

void UIButton::HandleInput()
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (UIevent) {

	case UIEvents::NO_EVENT_:
		if (MouseHover() || tab) {
			next_event = false;
			UIevent = UIEvents::MOUSE_ENTER_;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		break;
	case UIEvents::MOUSE_ENTER_:

		if (!MouseHover() && !tab) {
			LOG("MOUSE LEAVE");
			next_event = false;
			UIevent = UIEvents::MOUSE_LEAVE_;
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)) {
			next_event = false;
			LOG("MOUSE L CLICK START");
			ChangeSprite(pressed_tex_area);
			UIevent = UIEvents::MOUSE_LEFT_CLICK_;

			if (button.checkbox)
				button.checkbox_checked = !button.checkbox_checked;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)) {
			next_event = false;
			LOG("MOUSE R CLICK START");
			ChangeSprite(pressed_tex_area);

			mouse_click_pos.x = mouse_pos.x * App->win->GetScale() - GetLocalPos().x;
			mouse_click_pos.y = mouse_pos.y * App->win->GetScale() - GetLocalPos().y;

			if (draggable) {
				drag = true;
				//App->gui->drag_to_true = true;
			}

			UIevent = UIEvents::MOUSE_RIGHT_CLICK_;

			if (button.checkbox)
				button.checkbox_checked = !button.checkbox_checked;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}

		if (!next_event) {
			LOG("MOUSE ENTER");
			if (!button.checkbox_checked)
				ChangeSprite(hover_tex_area);
			next_event = true;
		}

		break;
	case UIEvents::MOUSE_RIGHT_CLICK_:

		if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP)) {
			LOG("MOUSE R CLICK FINISH");

			if (draggable) {
				drag = false;
				//App->gui->drag_to_false = true;
			}
			UIevent = UIEvents::MOUSE_RIGHT_UP_;
			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UIEvents::MOUSE_ENTER_;
			break;
		}

		break;
	case UIEvents::MOUSE_LEFT_CLICK_:

		if (!MouseHover() && !tab) {
			LOG("MOUSE LEAVE");
			UIevent = UIEvents::MOUSE_LEAVE_;
			break;
		}
		else if ((!tab && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) || (tab && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP)) {
			LOG("MOUSE L CLICK FINISH");
			UIevent = UIEvents::MOUSE_LEFT_UP_;
			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UIEvents::MOUSE_ENTER_;
			break;
		}

		break;
	case UIEvents::MOUSE_LEAVE_:
		if (!button.checkbox_checked)
			ChangeSprite(normal_tex_area);
		listener->OnUIEvent((UIElement*)this, UIevent);
		UIevent = NO_EVENT_;
		break;
	}
}

//---------------------------------------------------------------

void UIButton::ChangeSprite(SDL_Rect tex_area)
{
	this->tex_area = tex_area;
}

SDL_Rect UIButton::GetHoverSprite() const
{
	return hover_tex_area;
}

SDL_Rect UIButton::GetPressedSprite() const
{
	return pressed_tex_area;
}

SDL_Rect UIButton::GetNormalSprite() const
{
	return normal_tex_area;
}

UIEvents UIButton::GetActualEvent() const
{
	return UIevent;
}

bool UIButton::SlideTransition(float dt, int end_pos_y, float speed, bool bounce, float bounce_interval, float bounce_speed, bool down)
{
	bool ret = false;

	iPoint pos = GetLocalPos();

	if (down) {
		if (pos.y + height >= (int)end_pos_y - height / 2) {
			if (bounce && !start_bouncing)
				start_bouncing = true;
			else if (!bounce)
				ret = true;
		}
		else if (!start_bouncing) {
			if ((int)(speed * dt) >= 1)
				IncreasePos({ 0,(int)(speed * dt) });
			else
				IncreasePos({ 0, 1 });
		}
	}
	else {
		if (pos.y + height <= (int)end_pos_y - height / 2) {
			if (bounce && !start_bouncing)
				start_bouncing = true;
			else if (!bounce)
				ret = true;
		}
		else if (!start_bouncing) {
			if ((int)(speed * dt) >= 1)
				DecreasePos({ 0,(int)(speed * dt) });
			else
				DecreasePos({ 0, 1 });
		}
	}

	if (start_bouncing) {
		if (Bounce(dt, bounce_interval, bounce_speed, down)) {
			ret = true;
		}
	}

	return ret;
}

bool UIButton::Bounce(float dt, float bounce_interval, float bounce_speed, bool down)
{
	bool ret = false;

	iPoint pos = GetLocalPos();

	if (reset) {
		InitializeBounce(bounce_interval, down);
		reset = false;
	}

	if (bounce_value <= bounce_speed)
		ret = true;

	if (first_bounce) {
		if (pos.y >= startPos.y + bounce_value) {
			bounce_value -= bounce_speed;
			first_bounce = false;
		}
		else {
			if ((int)(bounce_value * 10.0f * dt) >= 1)
				IncreasePos({ 0, (int)(bounce_value * 10.0f * dt) });
			else
				IncreasePos({ 0, 1 });
		}
	}
	else {
		if (pos.y <= startPos.y - bounce_value) {
			bounce_value -= bounce_speed;
			first_bounce = true;
		}
		else {
			if ((int)(bounce_value * 10.0f * dt) >= 1)
				DecreasePos({ 0, (int)(bounce_value * 10.0f * dt) });
			else
				DecreasePos({ 0, 1 });
		}
	}

	return ret;
}

void UIButton::InitializeBounce(float bounce_interval, bool down)
{
	bounce_value = bounce_interval;
	startPos = GetLocalPos();

	if (!down)
		first_bounce = false;
}
