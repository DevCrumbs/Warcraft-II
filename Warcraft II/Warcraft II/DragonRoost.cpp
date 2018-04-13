#include "DragonRoost.h"
#include "j1Collision.h"

DragonRoost::DragonRoost(fPoint pos, iPoint size, int currLife, uint maxLife, const DragonRoostInfo& dragonRoostInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), dragonRoostInfo(dragonRoostInfo)
{
	texArea = &dragonRoostInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void DragonRoost::Move(float dt) {


}

// Animations
void DragonRoost::LoadAnimationsSpeed()
{

}
void DragonRoost::UpdateAnimations(float dt)
{

}

