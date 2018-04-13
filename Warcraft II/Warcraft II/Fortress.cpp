#include "Fortress.h"
#include "j1Collision.h"

Fortress::Fortress(fPoint pos, iPoint size, int currLife, uint maxLife, const FortressInfo& fortressInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), fortressInfo(fortressInfo)
{
	texArea = &fortressInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void Fortress::Move(float dt) {


}

// Animations
void Fortress::LoadAnimationsSpeed()
{

}
void Fortress::UpdateAnimations(float dt)
{

}

