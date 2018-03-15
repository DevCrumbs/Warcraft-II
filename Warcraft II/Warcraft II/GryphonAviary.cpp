// Creates the Gryphon Aviary

#include "GryphonAviary.h"

GryphonAviary::GryphonAviary(fPoint pos, iPoint size, int life, const GryphonAviaryInfo& gryphonAviaryInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), gryphonAviaryInfo(gryphonAviaryInfo)
{
	texArea = &gryphonAviaryInfo.completeTexArea;
}

void GryphonAviary::Move(float dt)
{

}

// Animations
void GryphonAviary::LoadAnimationsSpeed()
{

}
void GryphonAviary::UpdateAnimations(float dt)
{

}