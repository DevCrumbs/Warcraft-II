#ifndef __ChickenFarm_H__
#define __ChickenFarm_H__

#include "SDL\include\SDL.h"

#include "Defs.h"
#include "StaticEntity.h"


struct ChickenFarmInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class ChickenFarm :public StaticEntity
{
public:

	ChickenFarm(fPoint pos, iPoint size, int life, const ChickenFarmInfo& chickenFarmInfo, j1Module* listener = nullptr);
	~ChickenFarm() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

	void HandleInput();
	bool MouseHover() const;

	bool nextEvent = false;

private:

	ChickenFarmInfo chickenFarmInfo;

	EntitiesEvent EntityEvent = EntitiesEvent_Created;
};

#endif //__ChickenFarm_H__