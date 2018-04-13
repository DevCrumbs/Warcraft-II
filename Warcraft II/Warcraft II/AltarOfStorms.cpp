#include "AltarOfStorms.h"
#include "j1Collision.h"

AltarOfStorms::AltarOfStorms(fPoint pos, iPoint size, int currLife, uint maxLife, const AltarOfStormsInfo& altarOfStormsInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), altarOfStormsInfo(altarOfStormsInfo)
{
	texArea = &altarOfStormsInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
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
