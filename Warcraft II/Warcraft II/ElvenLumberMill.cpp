#include "ElvenLumberMill.h"

ElvenLumberMill::ElvenLumberMill(fPoint pos, iPoint size, int life, const ElvenLumberMillInfo& elvenLumberMillInfo) :StaticEntity(pos, size, life), elvenLumberMillInfo(elvenLumberMillInfo)
{
	texArea = &elvenLumberMillInfo.completeTexArea;
}


void ElvenLumberMill::Move(float dt)
{

}

// Animations
void ElvenLumberMill::LoadAnimationsSpeed()
{

}
void ElvenLumberMill::UpdateAnimations(float dt)
{

}