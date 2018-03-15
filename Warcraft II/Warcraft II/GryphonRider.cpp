#include "GryphonRider.h"

GryphonRider::GryphonRider(fPoint pos, iPoint size, int life, float speed, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, size, life, speed, listener), gryphonRiderInfo(gryphonRiderInfo)
{

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