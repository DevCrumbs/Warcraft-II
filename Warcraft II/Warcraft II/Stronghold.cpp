#include "Stronghold.h"

Stronghold::Stronghold(fPoint pos, iPoint size, int currLife, uint maxLife, const StrongholdInfo& strongholdInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), strongholdInfo(strongholdInfo)
{
	texArea = &strongholdInfo.completeTexArea;
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
