#include "GryphonRider.h"

GryphonRider::GryphonRider(fPoint pos, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, gryphonRiderInfo.size, gryphonRiderInfo.life, gryphonRiderInfo.speed, listener), gryphonRiderInfo(gryphonRiderInfo)
{
	currentLife = gryphonRiderInfo.life;

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