#ifndef __Alleria_H__
#define __Alleria_H__

#include "DynamicEntity.h"

struct AlleriaInfo
{
	UnitInfo unitInfo;

	Animation idle;
};

class Alleria :public DynamicEntity
{
public:

	Alleria(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const AlleriaInfo& alleriaInfo, j1Module* listener);
	~Alleria() {};

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
	AlleriaInfo alleriaInfo;
	float idleSpeed = 0.0f;

	bool isUnitRescuingPrisoner = false;
	bool isRescued = false;
};

#endif //__Alleria_H__