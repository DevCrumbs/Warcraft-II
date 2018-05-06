#ifndef __Turalyon_H__
#define __Turalyon_H__

#include "DynamicEntity.h"

struct TuralyonInfo
{
	UnitInfo unitInfo;

	Animation idle;
};

class Turalyon :public DynamicEntity
{
public:

	Turalyon(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const TuralyonInfo& khadgarInfo, j1Module* listener);
	~Turalyon() {};

	void Move(float dt);
	void Draw(SDL_Texture* sprites);
	void DebugDrawSelected();

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

	// Prisoner rescue
	bool IsUnitRescuingPrisoner() const;
	void SetUnitRescuePrisoner(bool isUnitRescuingPrisoner);

	bool IsRescued() const;
	void SetRescued(bool isRescued);

private:

	EntitiesEvent entityEvent = EntitiesEvent_NONE;
	TuralyonInfo turalyonInfo;
	float idleSpeed = 0.0f;

	bool isUnitRescuingPrisoner = false;
	bool isRescued = false;
};

#endif //__Turalyon_H__