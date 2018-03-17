#include "GryphonRider.h"

GryphonRider::GryphonRider(fPoint pos, iPoint size, int maxLife, float speed, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, size, maxLife, speed, listener), gryphonRiderInfo(gryphonRiderInfo)
{
	currentLife = maxLife;

}

void GryphonRider::Move(float dt)
{

}

// Animations
void GryphonRider::LoadAnimationsSpeed()
{

}
void GryphonRider::UpdateAnimations(float dt)
{

}