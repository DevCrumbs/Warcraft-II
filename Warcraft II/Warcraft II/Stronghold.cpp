#include "Stronghold.h"
#include "j1Collision.h"

Stronghold::Stronghold(fPoint pos, iPoint size, int currLife, uint maxLife, const StrongholdInfo& strongholdInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), strongholdInfo(strongholdInfo)
{
	texArea = &strongholdInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void Stronghold::Move(float dt) {


}

// Animations
void Stronghold::LoadAnimationsSpeed()
{

}
void Stronghold::UpdateAnimations(float dt)
{

}
