#include "AltarOfStorms.h"

AltarOfStorms::AltarOfStorms(fPoint pos, iPoint size, int currLife, uint maxLife, const AltarOfStormsInfo& altarOfStormsInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), altarOfStormsInfo(altarOfStormsInfo)
{
	texArea = &altarOfStormsInfo.completeTexArea;
}

void AltarOfStorms::Move(float dt) {


}

// Animations
void AltarOfStorms::LoadAnimationsSpeed()
{

}
void AltarOfStorms::UpdateAnimations(float dt)
{

}


