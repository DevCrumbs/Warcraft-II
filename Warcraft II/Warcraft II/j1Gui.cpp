#include "Brofiler\Brofiler.h"

#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"

#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"

#include "UIImage.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UICursor.h"


j1Gui::j1Gui() : j1Module()
{
	name.assign("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlasFileName = conf.child("atlas").attribute("file").as_string("");

	pugi::xml_node rects = conf.child("UI_elements_rects");
	pugi::xml_node node;
	uint i = 0;
	UIElementsRects[(UIE_RECT)i] = { 0,0,0,0 };

	for (node = rects.child("rect"); node; node = node.next_sibling("rect")) {
		UIElementsRects[(UIE_RECT)++i] = { node.attribute("x").as_int(), node.attribute("y").as_int(), node.attribute("w").as_int(), node.attribute("h").as_int() };
	}
	//UIElementsRects

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	bool ret = true;

	// Load textures
	atlas = App->tex->Load(atlasFileName.data());

	// Load fonts
	mapFonts[FONT_NAME_MSMINCHO] = App->font->Load("Assets/Fonts/MSMINCHO.TTF");
	mapFonts[FONT_NAME_ZELDA] = App->font->Load("Assets/Fonts/Zelda.ttf", 60);

	UIElementsTree = new NTree<UIElement*>((UIElement*)App->win->window);

	return ret;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	UIElementsList.clear();

	///UIElementsTree->recursivePreOrderList(UIElementsTree->getRoot(), &UIElementsList);

	return ret;
}

bool j1Gui::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Azure);

	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		isDebug = !isDebug;

	list<UIElement*>::const_iterator UI_elem_it = UIElementsList.begin();

	while (UI_elem_it != UIElementsList.end()) {
		(*UI_elem_it)->Update(dt);

		/*
		if (iterator->data->drag && update_drag)
			iterator->data->UpdateDragging(dt);
		*/

		UI_elem_it++;
	}

	UI_elem_it = UIElementsList.begin();

	return ret;
}

bool j1Gui::Blit(float dt) const
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Azure);

	bool ret = true;

	list<UIElement*>::const_iterator iterator = UIElementsList.begin();

	while (iterator != UIElementsList.end()) {
		(*iterator)->Draw();
		iterator++;
	}

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	bool ret = true;

	list<UIElement*>::const_iterator iterator = UIElementsList.begin();

	while (iterator != UIElementsList.end()) {

		if ((*iterator)->HasToBeRemoved()) {
			delete *iterator;
			UIElementsList.remove(*iterator);
		}

		iterator++;
	}

	return ret;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	bool ret = true;

	LOG("Freeing GUI");

	if (ClearAllUI()) {
		UIElementsTree->clear();

		// Clear UI_elements list (active elements)
		list<UIElement*>::const_iterator iterator = UIElementsList.begin();

		while (iterator != UIElementsList.end()) {
			delete *iterator;
			iterator++;
		}
		UIElementsList.clear();
	}

	// Remove textures
	ClearMapTextures();

	return ret;
}

UIImage* j1Gui::CreateUIImage(iPoint localPos, UIImage_Info& info, j1Module* listener, UIElement* parent)
{
	UIImage* image = new UIImage(localPos, parent, info, listener);

	if (parent == nullptr)
		parent = (UIElement*)App->win->window;

	UIElementsTree->insertChild((UIElement*)image, parent);

	return image;
}

UILabel* j1Gui::CreateUILabel(iPoint localPos, UILabel_Info& info, j1Module* listener, UIElement* parent)
{
	UILabel* label = new UILabel(localPos, parent, info, listener);

	if (parent == nullptr)
		parent = (UIElement*)App->win->window;

	UIElementsTree->insertChild((UIElement*)label, parent);

	return label;
}

UIButton* j1Gui::CreateUIButton(iPoint localPos, UIButton_Info& info, j1Module* listener, UIElement* parent)
{
	UIButton* button = new UIButton(localPos, parent, info, listener);

	if (parent == nullptr)
		parent = (UIElement*)App->win->window;

	UIElementsTree->insertChild((UIElement*)button, parent);

	return button;
}

UICursor* j1Gui::CreateUICursor(UICursor_Info& info, j1Module* listener, UIElement* parent)
{
	iPoint localPos = { 0,0 };

	UICursor* cursor = new UICursor(localPos, parent, info, listener);

	if (parent == nullptr)
		parent = (UIElement*)App->win->window;

	UIElementsTree->insertChild((UIElement*)cursor, parent);

	return cursor;
}

bool j1Gui::DestroyElement(UIElement* elem)
{
	bool ret = false;

	UIElementsTree->remove(elem);
	elem->toRemove = true;

	return ret;
}

bool j1Gui::ClearAllUI()
{
	bool ret = false;

	// Clear UI_elements tree
	UIElementsTree->getRoot()->removeChildren();

	// Delete trans pointer to cursor?
	/*
	App->trans->CreateCursor();
	*/

	list<UIElement*>::const_iterator UI_elem_it = UIElementsList.begin();

	while (UI_elem_it != UIElementsList.end()) {

		/*
		if (iterator->data != (UIElement*)App->trans->l_level_name && iterator->data != (UIElement*)App->trans->l_cats_picked
			&& iterator->data != (UIElement*)App->trans->l_level_cats_picked && iterator->data != (UIElement*)App->trans->l_level_cats_picked2
			&& iterator->data != (UIElement*)App->trans->l_score_text && iterator->data != (UIElement*)App->trans->l_you
			&& iterator->data != (UIElement*)App->trans->l_died && iterator->data != (UIElement*)App->trans->black_screen_image)
			iterator->data->toRemove = true;
		*/

		UI_elem_it++;
	}

	return ret;
}

bool j1Gui::ClearMapTextures()
{
	bool ret = true;

	App->tex->UnLoad((SDL_Texture*)atlas);

	return ret;
}

_TTF_Font* j1Gui::GetFont(FONT_NAME fontName)
{
	return mapFonts[fontName];
}

const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

SDL_Rect j1Gui::GetRectFromAtlas(UIE_RECT rect)
{
	return UIElementsRects[rect];
}

void j1Gui::SetTextureAlphaMod(float alpha)
{
	SDL_SetTextureAlphaMod((SDL_Texture*)atlas, (Uint8)alpha);
}

void j1Gui::ResetAlpha()
{
	reset = true;
}

float j1Gui::IncreaseDecreaseAlpha(float from, float to, float seconds)
{
	float calculatedAlpha = 0.0f;

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
	if (from > to)
		calculatedAlpha = alpha2;
	else
		calculatedAlpha = alpha;

	return calculatedAlpha;
}

void j1Gui::SetUpDraggingChildren(UIElement* elem, bool drag)
{
	// List including toDrag element and all of its children
	list<UIElement*> toDrag;
	///UIElementsTree->recursivePreOrderList(UIElementsTree->search(elem), &toDrag);

	// Don't drag elements which are not in the previous list
	list<UIElement*>::const_iterator iterator = UIElementsList.begin();

	while (iterator != UIElementsList.end() && iterator != toDrag.begin()) {
		(*iterator)->drag = false;
		iterator++;
	}

	/*
	update_drag = drag;
	*/
}

void j1Gui::SetUpDraggingNode(bool drag)
{
	UIElement* draggingNode = nullptr;
	int lowest_level = UIElementsTree->getNumLevels(UIElementsTree->getRoot()) + 1;

	// If several elements are clicked at once, select the element in the lowest level in the tree
	list<UIElement*>::const_iterator iterator = UIElementsList.begin();

	while (iterator != UIElementsList.end()) {

		if ((*iterator)->drag == drag) {
			int level = UIElementsTree->getNumLevels(UIElementsTree->search(*iterator));

			if (level <= lowest_level) {
				draggingNode = *iterator;
			}
		}

		iterator++;
	}

	SetUpDraggingChildren(draggingNode, drag);
}