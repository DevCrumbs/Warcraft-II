#include "UISlider.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Input.h"
#include "p2Log.h"

UISlider::UISlider(iPoint local_pos, UIElement* parent, UISlider_Info& info, j1Module* listener, bool isInWorld) : UIElement(local_pos, parent, listener, isInWorld), slider(info)
{
	type = UIE_TYPE_SLIDER;

	tex_area = App->gui->GetRectFromAtlas(slider.tex_area);
	button_slider_area = App->gui->GetRectFromAtlas(slider.button_slider_area);

	width = tex_area.w;
	height = tex_area.h;
	start_pos = slider.sliderButtonPos;

	priority = PriorityDraw_SLIDER;
	SetOrientation();
}

void UISlider::Update(float dt)
{
	if (interactive) {
		int scale = App->win->GetScale();
		int mouseX = 0;
		int mouseY = 0;
		App->input->GetMousePosition(mouseX, mouseY);

		if (App->input->GetMouseButtonDown((SDL_BUTTON_LEFT) == KEY_DOWN)
			&& mouseX > GetScreenPos().x / scale + slider.offset 
			&& mouseX < (GetScreenPos().x / scale + tex_area.w - button_slider_area.w - slider.offset) 
			&& mouseY >  GetScreenPos().y / scale && mouseY < GetScreenPos().y / scale + tex_area.h)

			slider.sliderButtonPos.x = mouseX - GetScreenPos().x / scale;


		if (listener != nullptr)
			HandleInput();
	}
}

void UISlider::Draw() const
{
	iPoint blit_pos;
	int scale = App->win->GetScale();
	blit_pos.x = (GetScreenPos().x - App->render->camera.x) / scale;
	blit_pos.y = (GetScreenPos().y - App->render->camera.y) / scale;

	App->render->Blit(App->gui->GetAtlas(), blit_pos.x, blit_pos.y, &tex_area);
	App->render->Blit(App->gui->GetAtlas(), blit_pos.x + slider.sliderButtonPos.x, blit_pos.y, &button_slider_area);

}

void UISlider::DebugDraw(iPoint blit_pos) const
{
	Uint8 alpha = 80;

	SDL_Rect quad = { blit_pos.x, blit_pos.y, width, height };
	App->render->DrawQuad(quad, 150, 0, 40, alpha, false);
}

void UISlider::HandleInput()
{
	iPoint mouse_pos;
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	switch (UIevent) {

	case UI_EVENT_NONE:
		if (MouseHover()) {
			next_event = false;
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}
		break;
	case UI_EVENT_MOUSE_ENTER:

		if (!MouseHover()) {
			LOG("MOUSE LEAVE");
			next_event = false;
			UIevent = UI_EVENT_MOUSE_LEAVE;
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == SDL_PRESSED) {
			next_event = false;
			LOG("MOUSE L CLICK START");
			UIevent = UI_EVENT_MOUSE_LEFT_CLICK;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_PRESSED) {
			next_event = false;
			LOG("MOUSE R CLICK START");

			//mouse_click_pos.x = mouse_pos.x * App->win->GetScale() - GetLocalPos().x;
			//mouse_click_pos.y = mouse_pos.y * App->win->GetScale() - GetLocalPos().y;

			UIevent = UI_EVENT_MOUSE_RIGHT_CLICK;
			listener->OnUIEvent((UIElement*)this, UIevent);
			break;
		}

		if (!next_event) {
			LOG("MOUSE ENTER");
			listener->OnUIEvent((UIElement*)this, UIevent);
			next_event = true;
		}

		break;
	case UI_EVENT_MOUSE_RIGHT_CLICK:

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == SDL_RELEASED) {
			LOG("MOUSE R CLICK FINISH");

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
			UIevent = UI_EVENT_MOUSE_LEFT_UP;
			listener->OnUIEvent((UIElement*)this, UIevent);
			UIevent = UI_EVENT_MOUSE_ENTER;
			break;
		}

		break;
	case UI_EVENT_MOUSE_LEAVE:
		listener->OnUIEvent((UIElement*)this, UIevent);
		UIevent = UI_EVENT_NONE;
		break;
	default:
		UIevent = UI_EVENT_MOUSE_LEFT_CLICK;
		listener->OnUIEvent((UIElement*)this, UIevent);

		break;
	}

}


float UISlider::GetRelativePosition() {
	float ret = 0;

	int relative_x = slider.sliderButtonPos.x /*- start_pos.x*/;

	ret = (float)relative_x / (tex_area.w - button_slider_area.w);

	return ret;
}

void UISlider::SetRelativePos(float x) {

	float new_x = x * (tex_area.w - slider.button_slider_area.w);

	slider.sliderButtonPos.x = new_x;
}
//---------------------------------------------------------------

void UISlider::SetNewRect(SDL_Rect& new_rect)
{
	tex_area = new_rect;
}

SDL_Rect UISlider::GetRect()
{
	return tex_area;
}

uint UISlider::GetPercent()
{
	return 100 * (slider.sliderButtonPos.x + slider.buggy_offset - slider.offset) / (tex_area.w - slider.offset);
}
