#include "ElvenLumberMill.h"

ElvenLumberMill::ElvenLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const ElvenLumberMillInfo& elvenLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), elvenLumberMillInfo(elvenLumberMillInfo)
{
	texArea = &elvenLumberMillInfo.constructionPlanks1;
	this->constructionTimer.Start();
}


void ElvenLumberMill::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);

	UpdateAnimations(dt);

	if (constructionTimer.Read() >= (constructionTime * 1000))
		isBuilt = true;
}

// Animations
void ElvenLumberMill::LoadAnimationsSpeed()
{

}
void ElvenLumberMill::UpdateAnimations(float dt)
{
	if (constructionTimer.Read() >= (constructionTime / 3) * 1000)
		texArea = &elvenLumberMillInfo.constructionPlanks2;

	if (constructionTimer.Read() >= (constructionTime / 3 * 2) * 1000)
		texArea = &elvenLumberMillInfo.inProgressTexArea;

	if (constructionTimer.Read() >= constructionTime * 1000)
		texArea = &elvenLumberMillInfo.completeTexArea;
}