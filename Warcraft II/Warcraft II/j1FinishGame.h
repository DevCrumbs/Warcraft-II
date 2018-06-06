#ifndef __j1MODULEFINISHGAME_H__
#define __j1MODULEFINISHGAME_H__

#include "j1Module.h"

#include "Animation.h"
//#include <list>
#include <vector>

#include "SDL\include\SDL_rect.h"

struct UILabel;
struct UIImage;
struct UIButton; 
struct ArtifactsCollection;

enum Artifacts
{
	Artifact_BOOK,
	Artifact_EYE,
	Artifact_SKULL,
	Artifact_SCEPTER
};
class j1FinishGame : public j1Module
{
public:
	j1FinishGame();
	~j1FinishGame();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool CleanUp();


private:

	void LoadSceneOne(bool isWin);
	void ArtifactWon(uint time);
	void LevelWon(Artifacts artifact);
	void AddArtifact(ArtifactsCollection &artifactStruct, Artifacts artifact);
	void SaveArtifactConfig(pugi::xml_node & config);
	void SaveArtifact(ArtifactsCollection & artifactStruct, pugi::xml_node & node);
	void DeleteScene();

	void OnUIEvent(UIElement* UIelem, UI_EVENT UIevent);

private:

	vector<UILabel*> labelVector;
	vector<UIImage*> imageVector;
	UIButton* returnButt = nullptr;
	
	//Background
	UIImage* background = nullptr;
	SDL_Rect winBG = { 0,0,0,0 };
	SDL_Rect loseBG = { 0,0,0,0 };
	//Flag
	UIImage* flag = nullptr;
	Animation winFlagAnimation;
	Animation loseFlagAnimation;


	SDL_Rect screen;

	string bgTexName;
	SDL_Texture* bg;

	//Values
	uint roomsExploredCont = 0u;

	//Music paths
	string victoryMusicPath;
	string defeatMusicPath;


};

#endif //__j1MODULEFINISHGAME_H__