#ifndef __GryphonRider_H__
#define __GryphonRider_H__

#include "DynamicEntity.h"

struct GryphonRiderInfo
{
	int currLife = 0;
	uint maxLife = 0;
};

class GryphonRider :public DynamicEntity
{
public:

	GryphonRider(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener);
	~GryphonRider() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GryphonRiderInfo gryphonRiderInfo;
};

#endif //__GryphonRider_H__