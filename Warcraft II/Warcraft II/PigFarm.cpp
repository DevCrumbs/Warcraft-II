#include "PigFarm.h"

PigFarm::PigFarm(fPoint pos, iPoint size, int currLife, uint maxLife, const PigFarmInfo& pigFarmInfo, j1Module* listener) :StaticEntity(pos, size, currLife, maxLife, listener), pigFarmInfo(pigFarmInfo)
{
	texArea = &pigFarmInfo.completeTexArea;
}

void PigFarm::Move(float dt) {


}

// Animations
void PigFarm::LoadAnimationsSpeed()
{

}
void PigFarm::UpdateAnimations(float dt)
{

}
