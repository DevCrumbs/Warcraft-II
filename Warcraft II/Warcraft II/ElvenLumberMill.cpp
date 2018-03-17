#include "ElvenLumberMill.h"

ElvenLumberMill::ElvenLumberMill(fPoint pos, iPoint size, int life, const ElvenLumberMillInfo& elvenLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, life, listener), elvenLumberMillInfo(elvenLumberMillInfo)
{
	texArea = &elvenLumberMillInfo.completeTexArea;
}


void ElvenLumberMill::Move(float dt)
{
	if (listener != nullptr)
		HandleInput(EntityEvent);
}

// Animations
void ElvenLumberMill::LoadAnimationsSpeed()
{

}
void ElvenLumberMill::UpdateAnimations(float dt)
{

}