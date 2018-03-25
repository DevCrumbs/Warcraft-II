#ifndef __GryphonRider_H__
#define __GryphonRider_H__

#include "DynamicEntity.h"

struct GryphonRiderInfo
{
	iPoint size{ 0,0 };
	uint life = 0u;
	float speed = 0.0f;
};

class GryphonRider :public DynamicEntity
{
public:

	GryphonRider(fPoint pos, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener);
	~GryphonRider() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GryphonRiderInfo gryphonRiderInfo;
};

#endif //__GryphonRider_H__