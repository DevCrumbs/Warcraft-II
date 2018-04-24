#ifndef __j1GUI_H__
#define __j1GUI_H__

#include <map>
#include <list>
#include <queue>
#include <functional>

#include "j1Module.h"
#include "UIElement.h"
#include "NTree.h"
#include "Animation.h"

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


struct UIImage_Info;
struct UILabel_Info;
struct UIButton_Info;
struct UICursor_Info;
struct UILifeBar_Info;
struct UISlider_Info;
struct UIMinimap_Info;
class UIImage;
class UILabel;
class UIButton;
class UICursor;
class UILifeBar;
class UIInputText;
class UISlider;
class UIMinimap;
// ---------------------------------------------------

struct compareUIPriority {
	bool operator()(const UIElement* infoA, const UIElement* infoB)
	{
		if (infoA != nullptr && infoB != nullptr)
			return infoA->GetPriorityDraw() > infoB->GetPriorityDraw();
	}
};
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

	// Gui creation functions
	UIImage* CreateUIImage(iPoint localPos, UIImage_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UILabel* CreateUILabel(iPoint localPos, UILabel_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UISlider* CreateUISlider(iPoint localPos, UISlider_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UIButton* CreateUIButton(iPoint localPos, UIButton_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr, bool isInWorld = false);
	UILifeBar* CreateUILifeBar(iPoint localPos, UILifeBar_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr, bool isInWorld = false);
	UIInputText* CreateUIInputText(iPoint localPos, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UICursor* CreateUICursor(UICursor_Info& info, j1Module* listener = nullptr, UIElement* parent = nullptr);
	UIMinimap * CreateUIMinimap(UIMinimap_Info & info, j1Module * listener = nullptr, UIElement * parent = nullptr);

	bool DestroyElement(UIElement** elem);
	bool RemoveElem(UIElement ** elem);
	bool ClearMapTextures();

	const SDL_Texture* GetAtlas() const;
	SDL_Rect GetRectFromAtlas(SDL_Rect rect);

	void SetTextureAlphaMod(float alpha);
	float IncreaseDecreaseAlpha(float from, float to, float seconds);
	void ResetAlpha();

public:

	std::list<UIElement*> addedElementUI;

	//NTree<UIElement*>* UIElementsTree; Don't delete yet
	bool isDebug = false;
	Animation parchmentAnim;
	SDL_Rect parchmentArea{ 0,0,0,0 };

	//artifacts
	Animation scepterAnim, bookAnim, skullAnim, eyeAnim;

	SDL_Rect scepterText{ 0,0,0,0 };
	SDL_Rect bookText{ 0,0,0,0 };
	SDL_Rect skullText{ 0,0,0,0 };
	SDL_Rect eyeText{ 0,0,0,0 };


private:

	string atlasFileName;
	const SDL_Texture* atlas = nullptr;

	list<UIElement*> UIElementsList;
	priority_queue<UIElement*, vector<UIElement*>, compareUIPriority> drawOrder;

	// Alpha parameters
	float totalTime = 0.0f;
	float startTime = 0.0f;
	bool reset = true;

};

#endif //__j1GUI_H__
