#include "UIInputText.h"
#include "UIElement.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "UIButton.h"


UIInputText::UIInputText(iPoint localPos, UIElement* parent, j1Module* listener, bool isInWorld) : UIElement (localPos, parent, listener, isInWorld){

	type = UIE_TYPE_INPUT_TEXT;
	original_pos = localPos;
	r = { 0,0,0,0 };

}

UIInputText::~UIInputText()
{
	isInputText = false;

	App->tex->UnLoad(texture);
	texture = nullptr;
}


void UIInputText::Update(float dt) {
	
	if (isInputText) {	
		App->render->DrawQuad({ original_pos.x + r.w ,original_pos.y,3,13 }, 255, 255, 255, 255);
		if (App->input->isPresed) {
			text += App->input->newLetter;
			App->font->CalcSize(text.data(), r.w, r.h);
			texture = App->font->Print(text.data());
			App->input->isPresed = false;
		}

		/*
		if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
			if (!text.empty()) {
				text.pop_back();
				App->font->CalcSize(text.data(), r.w, r.h);
				texture = App->font->Print(text.data());
			}
		}*/
		App->render->Blit(texture, original_pos.x, original_pos.y, &r);
	}
}

void UIInputText::ChangeInputState() {
	isInputText = !isInputText;
	if (isInputText)
		SDL_StartTextInput();

	else
		SDL_StopTextInput();
}

void UIInputText::CleanText() {

	text.clear();
	App->font->CalcSize(text.data(), r.w, r.h);
	texture = App->font->Print(text.data());

}
