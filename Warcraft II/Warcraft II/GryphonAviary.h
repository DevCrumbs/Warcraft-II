#ifndef __GryphonAviary_H__
#define __GryphonAviary_H__

#include "StaticEntity.h"

struct GryphonAviaryInfo
{

};

class GryphonAviary :public StaticEntity
{
public:

	GryphonAviary(fPoint pos, iPoint size, int life, const GryphonAviaryInfo& gryphonAviaryInfo);
	~GryphonAviary() {};

	void Move(float dt);

	// Animations
	void LoadAnimationsSpeed();
	void UpdateAnimations(float dt);

private:

	GryphonAviaryInfo gryphonAviaryInfo;
};

#endif //__GryphonAviary_H__