#include "GryphonRider.h"

GryphonRider::GryphonRider(fPoint pos, iPoint size, int currLife, uint maxLife, const UnitInfo& unitInfo, const GryphonRiderInfo& gryphonRiderInfo, j1Module* listener) :DynamicEntity(pos, size, currLife, maxLife, unitInfo, listener), gryphonRiderInfo(gryphonRiderInfo)
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