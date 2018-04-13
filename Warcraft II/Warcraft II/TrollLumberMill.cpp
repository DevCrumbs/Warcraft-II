#include "TrollLumberMill.h"
#include "j1Collision.h"

TrollLumberMill::TrollLumberMill(fPoint pos, iPoint size, int currLife, uint maxLife, const TrollLumberMillInfo& trollLumberMillInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), trollLumberMillInfo(trollLumberMillInfo)
{
	texArea = &trollLumberMillInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void TrollLumberMill::Move(float dt) {


}

// Animations
void TrollLumberMill::LoadAnimationsSpeed()
{

}
void TrollLumberMill::UpdateAnimations(float dt)
{

}


