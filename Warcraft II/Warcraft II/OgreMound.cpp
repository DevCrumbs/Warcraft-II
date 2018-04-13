#include "OgreMound.h"
#include "j1Collision.h"

OgreMound::OgreMound(fPoint pos, iPoint size, int currLife, uint maxLife, const OgreMoundInfo& ogreMoundInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), ogreMoundInfo(ogreMoundInfo)
{
	texArea = &ogreMoundInfo.completeTexArea;

	CreateEntityCollider(EntitySide_Enemy);
	entityCollider->isTrigger = true;
}

void OgreMound::Move(float dt) {


}

// Animations
void OgreMound::LoadAnimationsSpeed()
{

}
void OgreMound::UpdateAnimations(float dt)
{

}


