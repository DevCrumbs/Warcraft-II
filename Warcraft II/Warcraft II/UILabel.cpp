#include "UILabel.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Textures.h"

UILabel::UILabel(iPoint localPos, UIElement* parent, UILabel_Info& info, j1Module* listener) : UIElement(localPos, parent, listener), label(info)
{
	type = UIE_TYPE::UIE_TYPE_LABEL;

	draggable = label.draggable;
	interactive = label.interactive;
	horizontal = label.horizontalOrientation;
	vertical = label.verticalOrientation;
	font = App->gui->GetFont(label.fontName);
	color = label.normalColor;

	tex = App->font->Print(label.text.data(), color, font, (Uint32)label.textWrapLength);
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
	iPoint blitPos;
	int scale = App->win->GetScale();
	blitPos.x = (GetLocalPos().x - App->render->camera.x) / scale;
	blitPos.y = (GetLocalPos().y - App->render->camera.y) / scale;

	if (GetParent() != nullptr) {
		SDL_Rect daddy = GetParent()->GetScreenRect();
		App->render->SetViewPort({ daddy.x,daddy.y,daddy.w * scale,daddy.h * scale });
	}

	if (texArea.w > 0)
		App->render->Blit(tex, blitPos.x, blitPos.y, &texArea);
	else
		App->render->Blit(tex, blitPos.x, blitPos.y);

	if (App->gui->isDebug)
		DebugDraw(blitPos);

	App->render->ResetViewPort();
}

void UILabel::DebugDraw(iPoint blitPos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blitPos.x, blitPos.y, width, height };
	App->render->DrawQuad(quad, 10, 30, 5, alpha, false);
}

bool UILabel::MouseHover() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	uint scale = App->win->GetScale();

	if (!label.interactionFromFather)
		return x > GetScreenPos().x / scale && x < GetScreenPos().x / scale + GetLocalRect().w && 
		       y > GetScreenPos().y / scale && y < GetScreenPos().y / scale + GetLocalRect().h;
	else
		return x > GetParent()->GetScreenPos().x / scale && x < GetParent()->GetScreenPos().x / scale + GetParent()->GetLocalRect().w &&
		       y > GetParent()->GetScreenPos().y / scale && y < GetParent()->GetScreenPos().y / scale + GetParent()->GetLocalRect().h;
}

void UILabel::HandleInput()
{
	iPoint mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);

	switch (UIevent) {

	case UI_EVENT_NONE:
		if (MouseHover()) {
			nextEvent = false;
			UIevent = UI_EVENT_MOUSE_ENTER;
			SetColor(label.hoverColor);
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		break;
	case UI_EVENT_MOUSE_ENTER:

		if (!MouseHover()) {
			LOG("MOUSE LEAVE");
			nextEvent = false;
			UIevent = UI_EVENT_MOUSE_LEAVE;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {
			nextEvent = false;
			LOG("MOUSE L CLICK START");
			SetColor(label.pressedColor);
			UIevent = UI_EVENT_MOUSE_LEFT_CLICK;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {
			nextEvent = false;
			LOG("MOUSE R CLICK START");
			SetColor(label.pressedColor);

			mouseClickPos.x = mousePos.x * App->win->GetScale() - GetLocalPos().x;
			mouseClickPos.y = mousePos.y * App->win->GetScale() - GetLocalPos().y;

			if (draggable) {
				drag = true;
				//App->gui->drag_to_true = true;
			}

			UIevent = UI_EVENT_MOUSE_RIGHT_CLICK;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}

		if (!nextEvent) {
			LOG("MOUSE ENTER");
			nextEvent = true;
		}

		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) {
			LOG("MOUSE R CLICK FINISH");

			if (draggable) {
				drag = false;
				//App->gui->drag_to_false = true;
			}

			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}

		break;
	case UI_EVENT_MOUSE_LEFT_CLICK:

		if (!MouseHover()) {
			LOG("MOUSE LEAVE");
			UIevent = UI_EVENT_MOUSE_LEAVE;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_RELEASED) {
			LOG("MOUSE L CLICK FINISH");
			// Uncomment next line and create a new UIEvent if you want to execute a function when mouse button is released
			//listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}

		break;
	case UI_EVENT_MOUSE_LEAVE:
		SetColor(label.normalColor);
		listener->OnUIEvent((UIElement*)this, UIevent);
		UIevent = UI_EVENT_NONE;
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
		label.normalColor = color;
	else if (hover)
		label.hoverColor = color;
	else if (pressed)
		label.pressedColor = color;
}

SDL_Color UILabel::GetColor(bool normal, bool hover, bool pressed)
{
	if (normal)
		return label.normalColor;
	else if (hover)
		return label.hoverColor;
	else if (pressed)
		return label.pressedColor;
	else
		return color;
}

bool UILabel::IntermitentFade(float seconds, bool loop, bool halfLoop)
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
	if (!isInvisible) {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha2 });

		if (alpha2 == 0.0f) {
			if (loop) {
				alpha = 255.0f;
				isInvisible = true;
				ResetFade();
			}
			ret = true;
		}
	}
	else {
		SetColor({ GetColor().r,GetColor().g,GetColor().b,(Uint8)alpha });

		if (alpha == 255.0f) {
			if (!halfLoop) {
				alpha2 = 0.0f;
				isInvisible = false;
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

void UILabel::RandomAlphaPainting(float dt, SDL_Color color, int baseAlpha, int minAlpha, int maxAlpha, float speed)
{
	if (timer >= speed) {
		int random = rand() % 2;

		int min = minAlpha + rand() % (baseAlpha - minAlpha + 1);
		int max = minAlpha + rand() % (maxAlpha - minAlpha + 1);

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

bool UILabel::SlideTransition(float dt, int endPosY, float speed, bool bounce, float bounceInterval, float bounceSpeed, bool down)
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

bool UILabel::Bounce(float dt, float bounceInterval, float bounceSpeed, bool down)
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

void UILabel::InitializeBounce(float bounceInterval, bool down)
{
	bounceValue = bounceInterval;
	startPos = GetLocalPos();

	if (!down)
		firstBounce = false;
}
