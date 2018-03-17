#ifndef __Barracks_H__
#define __Barracks_H__

#include "StaticEntity.h"

enum BarracksType 
{
	BarracksType_Barracks,
	BarracksType_Barracks2
};

struct BarracksInfo
{
	BarracksType barracksType = BarracksType_Barracks;
	SDL_Rect barracksCompleteTexArea = { 0,0,0,0 };
	SDL_Rect barracks2CompleteTexArea = { 0,0,0,0 };
	int barracks1MaxLife = 0;
	int barracks2MaxLife = 0;
};

class Barracks :public StaticEntity
{
public:

	Barracks(fPoint pos, iPoint size, int maxLife, const BarracksInfo& barracksInfo, j1Module* listener);
	~Barracks() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	BarracksInfo barracksInfo;
};

#endif //__Barracks_H__