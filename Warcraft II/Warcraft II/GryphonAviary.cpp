// Creates the Gryphon Aviary

#include "GryphonAviary.h"

GryphonAviary::GryphonAviary(fPoint pos, iPoint size, int maxLife, const GryphonAviaryInfo& gryphonAviaryInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), gryphonAviaryInfo(gryphonAviaryInfo)
{
	texArea = &gryphonAviaryInfo.completeTexArea;
	currentLife = maxLife;

}

void GryphonAviary::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void GryphonAviary::LoadAnimationsSpeed()
{

}
void GryphonAviary::UpdateAnimations(float dt)
{

}