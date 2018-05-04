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
#include "j1Particles.h"

#include "UIImage.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UICursor.h"
#include "UIInputText.h"
#include "UILifeBar.h"
#include "UISlider.h"
#include "UIMinimap.h"

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

	pugi::xml_node parchment = conf.child("parchment");
	parchmentAnim.speed = parchment.attribute("speed").as_float();
	parchmentAnim.loop = parchment.attribute("loop").as_bool();
	for (parchment = parchment.child("frame"); parchment; parchment = parchment.next_sibling("frame")) {
		parchmentAnim.PushBack({ parchment.attribute("x").as_int(), parchment.attribute("y").as_int(), parchment.attribute("w").as_int(), parchment.attribute("h").as_int() });
		parchmentArea = { parchment.attribute("x").as_int(), parchment.attribute("y").as_int(), parchment.attribute("w").as_int(), parchment.attribute("h").as_int() };
	}


	pugi::xml_node artifacts = conf.child("artifacts");

	pugi::xml_node book = artifacts.child("book");
	bookAnim.speed = artifacts.attribute("speed").as_float();
	for (book = book.child("frame"); book; book = book.next_sibling("frame")) {
		bookAnim.PushBack({ book.attribute("x").as_int(), book.attribute("y").as_int(), book.attribute("w").as_int(), book.attribute("h").as_int() });
		bookText = { book.attribute("x").as_int(), book.attribute("y").as_int(), book.attribute("w").as_int(), book.attribute("h").as_int() };
	}
	
	pugi::xml_node eye = artifacts.child("eye");
	eyeAnim.speed = artifacts.attribute("speed").as_float();
	for (eye = eye.child("frame"); eye; eye = eye.next_sibling("frame")) {
		eyeAnim.PushBack({ eye.attribute("x").as_int(), eye.attribute("y").as_int(), eye.attribute("w").as_int(), eye.attribute("h").as_int() });
		eyeText = { eye.attribute("x").as_int(), eye.attribute("y").as_int(), eye.attribute("w").as_int(), eye.attribute("h").as_int() };
	}

	pugi::xml_node scepter = artifacts.child("scepter");
	scepterAnim.speed = artifacts.attribute("speed").as_float();
	for (scepter = scepter.child("frame"); scepter; scepter = scepter.next_sibling("frame")) {
		scepterAnim.PushBack({ scepter.attribute("x").as_int(), scepter.attribute("y").as_int(), scepter.attribute("w").as_int(), scepter.attribute("h").as_int() });
		scepterText = { scepter.attribute("x").as_int(), scepter.attribute("y").as_int(), scepter.attribute("w").as_int(), scepter.attribute("h").as_int() };
	}

	pugi::xml_node skull = artifacts.child("skull");
	skullAnim.speed = artifacts.attribute("speed").as_float();
	for (skull = skull.child("frame"); skull; skull = skull.next_sibling("frame")) {
		skullAnim.PushBack({ skull.attribute("x").as_int(), skull.attribute("y").as_int(), skull.attribute("w").as_int(), skull.attribute("h").as_int() });
		skullText = { skull.attribute("x").as_int(), skull.attribute("y").as_int(), skull.attribute("w").as_int(), skull.attribute("h").as_int() };
	}

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	bool ret = true;

	// Load textures
	atlas = App->tex->Load(atlasFileName.data());

	// Load fonts

	return ret;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	list<UIElement*>::const_iterator add = addedElementUI.begin();

	while (add != addedElementUI.end()) {
	
		UIElementsList.push_back(*add);
		add++;
	}
	addedElementUI.clear();
	
	return ret;
}

bool j1Gui::Update(float dt)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Azure);

	bool ret = true;

	// Update UI elements
	list<UIElement*>::const_iterator UI_elem_it = UIElementsList.begin();

	while (UI_elem_it != UIElementsList.end())
	{
		(*UI_elem_it)->Update(dt);

		UI_elem_it++;
	}

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	bool ret = true;

	// Remove UI elements
	list<UIElement*>::const_iterator it = UIElementsList.begin();

	while (it != UIElementsList.end()) {

		if ((*it)->HasToBeRemoved()) {

			delete *it;
			UIElementsList.erase(it);

			it = UIElementsList.begin();
			continue;
		}

		it++;
	}

	// Blit UI elements
	/// Move UI elements to the priority queue
	for (it = UIElementsList.begin(); it != UIElementsList.end(); it++)
			drawOrder.push(*it);

	/// Blit UI elements using the priority queue
	UIElement* info = nullptr;
	while (!drawOrder.empty())
	{
		info = drawOrder.top();

		// Only blit elements with isBlit == true
		if (info->isBlit && info->isActive) {

			if (info->GetPriorityDraw() != PriorityDraw_LIFEBAR_INGAME)
				info->Draw();
			else if (App->render->IsInScreen(info->GetLocalRect())) {
				info->Draw();
			}
		}
		drawOrder.pop();
	}

	return ret;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	bool ret = true;

	LOG("Freeing GUI");
	isGuiCleanUp = true;

	// Clear UI_elements list (active elements)
	list<UIElement*>::const_iterator elem = UIElementsList.begin();

	while (elem != UIElementsList.end()) {

		delete *elem;
		elem++;
	}
	UIElementsList.clear();
		
	// Clear UI_elements list (active elements)
	list<UIElement*>::const_iterator add = addedElementUI.begin();

	while (add != addedElementUI.end()) {

		delete *add;
		add++;
	}
	addedElementUI.clear();

	// Remove textures
	ClearMapTextures();

	return ret;
}

UIImage* j1Gui::CreateUIImage(iPoint localPos, UIImage_Info& info, j1Module* listener, UIElement* parent)
{
	UIImage* image = new UIImage(localPos, parent, info, listener);

	addedElementUI.push_back((UIElement*)image);

	return image;
}

UILabel* j1Gui::CreateUILabel(iPoint localPos, UILabel_Info& info, j1Module* listener, UIElement* parent)
{
	UILabel* label = new UILabel(localPos, parent, info, listener);

	addedElementUI.push_back((UIElement*)label);

	return label;
}

UISlider* j1Gui::CreateUISlider(iPoint localPos, UISlider_Info& info, j1Module* listener, UIElement* parent)
{
	UISlider* slider = new UISlider(localPos, parent, info, listener);

	addedElementUI.push_back((UIElement*)slider);

	return slider;
}

UIButton* j1Gui::CreateUIButton(iPoint localPos, UIButton_Info& info, j1Module* listener, UIElement* parent, bool isInWorld)
{
	UIButton* button = new UIButton(localPos, parent, info, listener, isInWorld);

	addedElementUI.push_back((UIElement*)button);

	return button;
}

UILifeBar* j1Gui::CreateUILifeBar(iPoint localPos, UILifeBar_Info& info, j1Module* listener, UIElement* parent, bool isInWorld)
{
	UILifeBar* lifeBar = new UILifeBar(localPos, parent, info, listener, isInWorld);

	addedElementUI.push_back((UIElement*)lifeBar);

	return lifeBar;
}

UIInputText* j1Gui::CreateUIInputText(iPoint localPos, j1Module* listener, UIElement* parent)
{
	UIInputText* inputText = new UIInputText(localPos, parent, listener);

	addedElementUI.push_back((UIElement*)inputText);

	return inputText;
}

UICursor* j1Gui::CreateUICursor(UICursor_Info& info, j1Module* listener, UIElement* parent)
{
	iPoint localPos = { 0,0 };

	UICursor* cursor = new UICursor(localPos, parent, info, listener);

	addedElementUI.push_back((UIElement*)cursor);

	return cursor;
}

UIMinimap* j1Gui::CreateUIMinimap(UIMinimap_Info& info, j1Module* listener, UIElement* parent)
{
	iPoint localPos = { 0,0 };

	UIMinimap* minimap = new UIMinimap(localPos, parent, info, listener);

	addedElementUI.push_back((UIElement*)minimap);

	return minimap;
}


bool j1Gui::DestroyElement(UIElement** elem)
{
	bool ret = false;

	if (*elem != nullptr) {

		delete *elem;
		UIElementsList.remove(*elem);
		*elem = nullptr;
	}

	return ret;
}

bool j1Gui::RemoveElem(UIElement** elem)
{
	bool ret = false;

	if (*elem != nullptr) {

		(*elem)->toRemove = true;
		*elem = nullptr;
	}

	return ret;
}

bool j1Gui::ClearUI()
{
	bool ret = true;

	UIElement* cursor = nullptr;

	for (list<UIElement*>::iterator iterator = UIElementsList.begin(); iterator != UIElementsList.end(); ++iterator)
	{
		if ((*iterator)->type != UIE_TYPE_CURSOR)
			delete *iterator;
		else cursor = *iterator;
	}
	UIElementsList.clear();

	if (cursor != nullptr)
		UIElementsList.push_back(cursor);

	return ret;
}


bool j1Gui::ClearMapTextures()
{
	bool ret = true;

	App->tex->UnLoad((SDL_Texture*)atlas);

	return ret;
}

const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

SDL_Rect j1Gui::GetRectFromAtlas(SDL_Rect rect)
{
	return rect;
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

bool j1Gui::IsMouseOnUI()
{
	bool ret = false;
	iPoint mouse{ 0,0 };
	App->input->GetMousePosition(mouse.x, mouse.y);
	SDL_Rect mouseRect{ mouse.x,mouse.y,1,1 };

	for (list<UIElement*>::iterator iterator = UIElementsList.begin(); iterator != UIElementsList.end(); ++iterator)
	{
		if ((*iterator)->type != UIE_TYPE_CURSOR && (*iterator)->type != UIE_TYPE_LIFE_BAR)
		{
			if ((*iterator)->isActive)
			{
				SDL_Rect elementRect = (*iterator)->GetScreenRect();
				if (elementRect.x != 0 && elementRect.y != 0 && elementRect.w != App->win->width && elementRect.h != App->win->height)
					if (RectIntersect(&mouseRect, &elementRect))
					{
						ret = true;
						break;
					}
			}
		}
	}

	return ret;
}