#ifndef __GryphonRider_H__
#define __GryphonRider_H__

#include "DynamicEntity.h"

struct GryphonRiderInfo
{

};

class GryphonRider :public DynamicEntity
{
public:

	GryphonRider(fPoint pos, iPoint size, int life, float speed, const GryphonRiderInfo& gryphonRiderInfo);
	~GryphonRider() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GryphonRiderInfo gryphonRiderInfo;
};

#endif //__GryphonRider_H__