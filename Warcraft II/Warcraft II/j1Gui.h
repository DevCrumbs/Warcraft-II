#ifndef __j1GUI_H__
#define __j1GUI_H__

#include <map>
#include <list>

#include "j1Module.h"
#include "UIElement.h"
#include "NTree.h"

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

enum FONT_NAME {
	FONT_NAME_DEFAULT,
	FONT_NAME_MSMINCHO,
	FONT_NAME_ZELDA,
	FONT_NAME_MAX_FONTS
};

enum UIE_RECT {

	NO_ELEMENT_RECT,
	MM_OPT_1_NORMAL,
	MM_OPT_2_NORMAL,
	MM_OPT_3_NORMAL,
	MM_OPT_4_NORMAL,
	MM_OPT_5_NORMAL,

	MM_OPT_1_HOVER,
	MM_OPT_2_HOVER,
	MM_OPT_3_HOVER,
	MM_OPT_4_HOVER,
	MM_OPT_5_HOVER,

	MAX_RECTS
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
	UIImage* CreateUIImage(iPoint localPos, UIImage_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UILabel* CreateUILabel(iPoint localPos, UILabel_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UIButton* CreateUIButton(iPoint localPos, UIButton_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UICursor* CreateUICursor(UICursor_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);

	bool DestroyElement(UIElement* elem);
	bool ClearAllUI();
	bool ClearMapTextures();

	void SetUpDraggingChildren(UIElement* elem, bool dragging);
	void SetUpDraggingNode(bool drag);

	_TTF_Font* GetFont(FONT_NAME fontName);

	const SDL_Texture* GetAtlas() const;
	SDL_Rect GetRectFromAtlas(UIE_RECT rect);

	void SetTextureAlphaMod(float alpha);
	float IncreaseDecreaseAlpha(float from, float to, float seconds);
	void ResetAlpha();

private:

	string atlasFileName;
	const SDL_Texture* atlas = nullptr;

	list<UIElement*> UIElementsList;

	map<UIE_RECT, SDL_Rect> UIElementsRects;
	map<FONT_NAME, _TTF_Font*> mapFonts;

	// Alpha parameters
	float totalTime = 0.0f;
	float startTime = 0.0f;
	bool reset = true;

public:
	NTree<UIElement*>* UIElementsTree;
	bool isDebug = false;
};

#endif //__j1GUI_H__
