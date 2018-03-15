#ifndef __ElvenLumberMill_H__
#define __ElvenLumberMill_H__

#include "StaticEntity.h"

struct ElvenLumberMillInfo
{
	SDL_Rect completeTexArea = { 0,0,0,0 };
	SDL_Rect inProgressTexArea = { 0,0,0,0 };
};

class ElvenLumberMill : public StaticEntity
{
public:
	ElvenLumberMill(fPoint pos, iPoint size, int life, const ElvenLumberMillInfo& elvenLumberMillInfo);
	~ElvenLumberMill() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	ElvenLumberMillInfo elvenLumberMillInfo;
};

#endif