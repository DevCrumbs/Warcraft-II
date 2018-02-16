#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "UIElement.h"
#include "NTree.h"

#include <map>
#include <list>
using namespace std;

#define CURSOR_WIDTH 2

#define Black_ { 0,0,0,255 }
#define White_ { 255,255,255,255 }
#define LightGrey_ { 231,231,231,255 }
#define Purple_ { 47,33,56,255 }
#define Pink_ { 183,116,141,255 }
#define LightPink_ { 255,247,226,255 }
#define WarmYellow_ { 255,193,82,255 }
#define BloodyRed_ { 85,5,0,255 }

using namespace std;

struct _TTF_Font;

enum Font_Names {
	DEFAULT_,
	MSMINCHO_,
	ZELDA_,
	MAX_FONTS_
};

enum UIElement_Rect {
	NO_ELEMENT_RECT_,
	MM_OPT_1_NORMAL_,
	MM_OPT_2_NORMAL_,
	MM_OPT_3_NORMAL_,
	MM_OPT_4_NORMAL_,
	MM_OPT_5_NORMAL_,

	MM_OPT_1_HOVER_,
	MM_OPT_2_HOVER_,
	MM_OPT_3_HOVER_,
	MM_OPT_4_HOVER_,
	MM_OPT_5_HOVER_,

	MAX_RECTS_
};

struct UIImage_Info;
struct UILabel_Info;
struct UIButton_Info;
struct UICursor_Info;
struct UIImage;
struct UILabel;
struct UIButton;
struct UICursor;

// ---------------------------------------------------

class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Blit(float dt) const;

	// Gui creation functions
	UIImage* CreateUIImage(iPoint local_pos, UIImage_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UILabel* CreateUILabel(iPoint local_pos, UILabel_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UIButton* CreateUIButton(iPoint local_pos, UIButton_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UICursor* CreateUICursor(UICursor_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);

	bool DestroyElement(UIElement* elem);
	bool ClearAllUI();
	bool ClearMapTextures();

	void SetUpDraggingChildren(UIElement* elem, bool dragging);
	void SetUpDraggingNode(bool drag);

	_TTF_Font* GetFont(Font_Names font_name);

	const SDL_Texture* GetAtlas() const;
	SDL_Rect GetRectFromAtlas(UIElement_Rect rect);

	void SetTextureAlphaMod(float alpha);
	float IncreaseDecreaseAlpha(float from, float to, float seconds);
	void ResetAlpha();

private:
	string atlas_file_name;
	const SDL_Texture* atlas;

	list<UIElement*> UI_elements_list;

	map<UIElement_Rect, SDL_Rect> UI_elements_rects;
	map<Font_Names, _TTF_Font*> map_fonts;

	// Alpha parameters
	float totalTime = 0.0f;
	float startTime = 0.0f;
	bool reset = true;

public:
	NTree<UIElement*>* UI_elements_tree;
	bool debug_draw = false;
};

#endif //__j1GUI_H__
