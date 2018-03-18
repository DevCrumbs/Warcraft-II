#include "ElvenLumberMill.h"

ElvenLumberMill::ElvenLumberMill(fPoint pos, iPoint size, int life, const ElvenLumberMillInfo& elvenLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), elvenLumberMillInfo(elvenLumberMillInfo)
{
	texArea = &constructionPlanks.constructionPlanksBig1;
	this->constructionTimer.Start();
}


void ElvenLumberMill::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);
}

// Animations
void ElvenLumberMill::LoadAnimationsSpeed()
{

}
void ElvenLumberMill::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &constructionPlanks.constructionPlanksBig2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &elvenLumberMillInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &elvenLumberMillInfo.completeTexArea;
}