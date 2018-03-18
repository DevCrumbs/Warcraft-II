// Creates the Gryphon Aviary

#include "GryphonAviary.h"

GryphonAviary::GryphonAviary(fPoint pos, iPoint size, int maxLife, const GryphonAviaryInfo& gryphonAviaryInfo, j1Module* listener) :StaticEntity(pos, size, maxLife, listener), gryphonAviaryInfo(gryphonAviaryInfo)
{
	texArea = &constructionPlanks.constructionPlanksBig1;
	currentLife = maxLife;
	this->constructionTimer.Start();
}

void GryphonAviary::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
	UpdateAnimations(dt);
}

// Animations
void GryphonAviary::LoadAnimationsSpeed()
{

}
void GryphonAviary::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksBig2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &gryphonAviaryInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &gryphonAviaryInfo.completeTexArea;
}