#include "UILabel.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Textures.h"

UILabel::UILabel(iPoint local_pos, UIElement* parent, UILabel_Info& info, j1Module* listener) : UIElement(local_pos, parent, listener), label(info)
{
	type = UIElement_TYPE::LABEL_;

	draggable = label.draggable;
	interactive = label.interactive;
	horizontal = label.horizontal_orientation;
	vertical = label.vertical_orientation;
	font = App->gui->GetFont(label.font_name);
	color = label.normal_color;

	tex = App->font->Print(label.text.data(), color, font, (Uint32)label.text_wrap_length);
	App->font->CalcSize(label.text.data(), width, height, font);

	SetOrientation();
}

void UILabel::Update(float dt)
{
	if (listener != nullptr && interactive)
		HandleInput();
}

UILabel::~UILabel()
{
	App->tex->UnLoad((SDL_Texture*)tex);
}

void UILabel::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (GetParent() != nullptr) {
		SDL_Rect daddy = GetParent()->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (tex_area.w != 0)
		App->render->Blit(tex, blit_pos.x, blit_pos.y, &tex_area);
	else
		App->render->Blit(tex, blit_pos.x, blit_pos.y);

	if (App->gui->debug_draw)
		DebugDraw(blit_pos);

	App->render->ResetViewPort();
}

void UILabel::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 10, 30, 5, alpha, false);
}

bool UILabel::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	if (!label.interaction_from_father)
		return x > GetScreenPos().x / scale && x < GetScreenPos().x / scale + GetLocalRect().w && y > GetScreenPos().y / scale && y < GetScreenPos().y / scale + GetLocalRect().h;
	else
		return x > GetParent()->GetScreenPos().x / scale && x < GetParent()->GetScreenPos().x / scale + GetParent()->GetLocalRect().w && y > GetParent()->GetScreenPos().y / scale && y < GetParent()->GetScreenPos().y / scale + GetParent()->GetLocalRect().h;
}

void UILabel::HandleInput()
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (UIevent) {

	case UIEvents::NO_EVENT_:
		if (MouseHover()) {
			next_event = false;
			UIevent = UIEvents::MOUSE_ENTER_;
			SetColor(label.hover_color);
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		break;
	case UIEvents::MOUSE_ENTER_:

		if (!MouseHover()) {
			LOG("MOUSE LEAVE");
			next_event = false;
			UIevent = UIEvents::MOUSE_LEAVE_;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {
			next_event = false;
			LOG("MOUSE L CLICK START");
			SetColor(label.pressed_color);
			UIevent = UIEvents::MOUSE_LEFT_CLICK_;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {
			next_event = false;
			LOG("MOUSE R CLICK START");
			SetColor(label.pressed_color);

			mouse_click_pos.x = mouse_pos.x * App->win->GetScale() - GetLocalPos().x;
			mouse_click_pos.y = mouse_pos.y * App->win->GetScale() - GetLocalPos().y;

			if (draggable) {
				drag = true;
				//App->gui->drag_to_true = true;
			}

			UIevent = UIEvents::MOUSE_RIGHT_CLICK_;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}

		if (!next_event) {
			LOG("MOUSE ENTER");
			next_event = true;
		}

		break;
	case UIEvents::MOUSE_RIGHT_CLICK_:

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) {
			LOG("MOUSE R CLICK FINISH");

			if (draggable) {
				drag = false;
				//App->gui->drag_to_false = true;
			}

			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UIEvents::MOUSE_ENTER_;
			break;
		}

		break;
	case UIEvents::MOUSE_LEFT_CLICK_:

		if (!MouseHover()) {
			LOG("MOUSE LEAVE");
			UIevent = UIEvents::MOUSE_LEAVE_;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) {
			LOG("MOUSE L CLICK FINISH");
			// Uncomment next line and create a new UIEvent if you want to execute a function when mouse button is released
			//listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = MOUSE_ENTER_;
			break;
		}

		break;
	case UIEvents::MOUSE_LEAVE_:
		SetColor(label.normal_color);
		listener->OnUIEvent((UIElement*)this, UIevent);
		UIevent = UIEvents::NO_EVENT_;
		break;
	}
}

//---------------------------------------------------------------

void UILabel::SetText(string text)
{
	App->tex->UnLoad((SDL_Texture*)tex);
	tex = App->font->Print(text.data(), color, font);
}

void UILabel::SetColor(SDL_Color color, bool normal, bool hover, bool pressed)
{
	this->color = color;
	App->tex->UnLoad((SDL_Texture*)tex);
	tex = App->font->Print(label.text.data(), color, font);

	if (normal)
		label.normal_color = color;
	else if (hover)
		label.hover_color = color;
	else if (pressed)
		label.pressed_color = color;
}

SDL_Color UILabel::GetColor(bool normal, bool hover, bool pressed)
{
	if (normal)
		return label.normal_color;
	else if (hover)
		return label.hover_color;
	else if (pressed)
		return label.pressed_color;
	else
		return color;
}

bool UILabel::IntermitentFade(float seconds, bool loop, bool half_loop)
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

	float alpha = 255.0f * normalized;
	float alpha2 = 255.0f * normalized2;

	// Color change
	if (!is_invisible) {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha2 });

		if (alpha2 == 0.0f) {
			if (loop) {
				alpha = 255.0f;
				is_invisible = true;
				ResetFade();
			}
			ret = true;
		}
	}
	else {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha });

		if (alpha == 255.0f) {
			if (!half_loop) {
				alpha2 = 0.0f;
				is_invisible = false;
				ResetFade();
			}
			else
				ret = true;
		}
	}

	return ret;
}

bool UILabel::FromAlphaToAlphaFade(float from, float to, float seconds)
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

void UILabel::ResetFade()
{
	reset = true;
}

void UILabel::RandomAlphaPainting(float dt, SDL_Color color, int base_alpha, int min_alpha, int max_alpha, float speed)
{
	if (timer >= speed) {
		int random = rand() % 2;

		int min = min_alpha + rand() % (base_alpha - min_alpha + 1);
		int max = min_alpha + rand() % (max_alpha - min_alpha + 1);

		if (min >= 0 && min <= 255 && max >= 0 && max <= 255) {
			switch (random) {
			case 0:
				SetColor({ color.r, color.g, color.b, (Uint8)min });
				break;
			case 1:
				SetColor({ color.r, color.g, color.b, (Uint8)max });
				break;
			default:
				break;
			}
		}
		timer = 0.0f;
	}

	timer += 1.0f * dt;
}

bool UILabel::SlideTransition(float dt, int end_pos_y, float speed, bool bounce, float bounce_interval, float bounce_speed, bool down)
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

bool UILabel::Bounce(float dt, float bounce_interval, float bounce_speed, bool down)
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
		if (pos.y >= start_pos.y + bounce_value) {
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
		if (pos.y <= start_pos.y - bounce_value) {
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

void UILabel::InitializeBounce(float bounce_interval, bool down)
{
	bounce_value = bounce_interval;
	start_pos = GetLocalPos();

	if (!down)
		first_bounce = false;
}
